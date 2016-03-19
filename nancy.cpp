#include <yaml-cpp/yaml.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <twitcurl.h>
#include <unistd.h>
#include <verbly.h>

std::string capitalize(std::string input)
{
  std::string result;
  bool capnext = true;
  
  for (auto ch : input)
  {
    if (capnext)
    {
      result += toupper(ch);
      capnext = false;
    } else {
      result += ch;
    }
    
    if ((ch == ' ') || (ch == '-'))
    {
      capnext = true;
    }
  }
  
  return result;
}

int main(int argc, char** argv)
{
  srand(time(NULL));
  
  YAML::Node config = YAML::LoadFile("config.yml");
  
  // Forms
  std::vector<std::string> forms;
  std::ifstream formfile("forms.txt");
  if (formfile.is_open())
  {
    while (!formfile.eof())
    {
      std::string l;
      getline(formfile, l);
      if (l.back() == '\r')
      {
        l.pop_back();
      }
      
      forms.push_back(l);
    }
    
    formfile.close();
  }
  
  if (forms.size() == 0)
  {
    std::cout << "No forms found... check forms.txt." << std::endl;
    
    return 2;
  }
  
  // verbly
  verbly::data database("data.sqlite3");
  
  // Twitter  
  twitCurl twitter;
  twitter.getOAuth().setConsumerKey(config["consumer_key"].as<std::string>());
  twitter.getOAuth().setConsumerSecret(config["consumer_secret"].as<std::string>());
  twitter.getOAuth().setOAuthTokenKey(config["access_key"].as<std::string>());
  twitter.getOAuth().setOAuthTokenSecret(config["access_secret"].as<std::string>());
  
  for (;;)
  {
    std::cout << "Generating tweet" << std::endl;
    
    std::string form = forms[rand() % forms.size()];
    
    // Adjectives
    int i;
    while ((i = form.find("{adj}")) != std::string::npos)
    {
      verbly::adjective adj = database.adjectives().random(true).limit(1).run().front();
      form.replace(i, 5, capitalize(adj.base_form()));
    }
    
    // Nouns
    while ((i = form.find("{noun}")) != std::string::npos)
    {
      verbly::noun n = database.nouns().is_not_proper(true).random(true).limit(1).run().front();
      form.replace(i, 6, capitalize(n.singular_form()));
    }
    
    if (form.size() > 140)
    {
      continue;
    }
        
    std::string replyMsg;
    if (twitter.statusUpdate(form))
    {
      twitter.getLastWebResponse(replyMsg);
      std::cout << "Twitter message: " << replyMsg << std::endl;
    } else {
      twitter.getLastCurlError(replyMsg);
      std::cout << "Curl error: " << replyMsg << std::endl;
    }
    
    std::cout << "Waiting" << std::endl;
    sleep(60 * 60 * 3);
  }
}

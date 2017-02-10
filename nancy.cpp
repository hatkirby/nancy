#include <yaml-cpp/yaml.h>
#include <verbly.h>
#include <twitter.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <chrono>
#include <random>
#include <thread>

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
  if (argc != 2)
  {
    std::cout << "usage: nancy [configfile]" << std::endl;
    return -1;
  }

  std::string configfile(argv[1]);
  YAML::Node config = YAML::LoadFile(configfile);

  std::random_device random_device;
  std::mt19937 random_engine{random_device()};
  
  // Forms
  std::vector<std::string> forms;
  {
    std::ifstream formfile(config["forms"].as<std::string>());
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
    }
  }
  
  if (forms.size() == 0)
  {
    std::cout << "No forms found... check forms file." << std::endl;
    
    return 2;
  }
  
  // verbly
  verbly::database database(config["verbly_datafile"].as<std::string>());
  
  // Twitter
  twitter::auth auth;
  auth.setConsumerKey(config["consumer_key"].as<std::string>());
  auth.setConsumerSecret(config["consumer_secret"].as<std::string>());
  auth.setAccessKey(config["access_key"].as<std::string>());
  auth.setAccessSecret(config["access_secret"].as<std::string>());
  
  twitter::client client(auth);
  
  for (;;)
  {
    std::cout << "Generating tweet..." << std::endl;
    
    int form_i = std::uniform_int_distribution<int>(0, forms.size()-1)(random_engine);
    std::string form = forms[form_i];
    
    // Adjectives
    int i;
    while ((i = form.find("{adj}")) != std::string::npos)
    {
      verbly::word adj = database.words(verbly::notion::partOfSpeech == verbly::part_of_speech::adjective).first();

      form.replace(i, 5, capitalize(adj.getBaseForm().getText()));
    }
    
    // Nouns
    while ((i = form.find("{noun}")) != std::string::npos)
    {
      verbly::word n = database.words(
        (verbly::notion::partOfSpeech == verbly::part_of_speech::noun)
        && (verbly::form::proper == false)
        && !(verbly::word::usageDomains %= (verbly::notion::wnid == 106718862)) // Blacklist ethnic slurs
      ).first();

      form.replace(i, 6, capitalize(n.getBaseForm().getText()));
    }
    
    if (form.size() > 140)
    {
      continue;
    }
    
    try
    {
      client.updateStatus(form);
      
      std::cout << "Tweeted!" << std::endl;
    } catch (const twitter::twitter_error& e)
    {
      std::cout << "Twitter error: " << e.what() << std::endl;
    }
    
    std::cout << "Waiting..." << std::endl;
    
    std::this_thread::sleep_for(std::chrono::hours(1));
  }
}

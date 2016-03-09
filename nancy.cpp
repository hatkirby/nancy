#include <yaml-cpp/yaml.h>
#include <iostream>
#include <mysql/mysql.h>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <twitcurl.h>
#include <unistd.h>

int db_error(MYSQL* driver, const char* error)
{
  std::cout << error << ": " << mysql_error(driver) << std::endl;
  return 1;
}

int main(int argc, char** argv)
{
  srand(time(NULL));
  
  YAML::Node config = YAML::LoadFile("config.yml");
  const char* host = config["host"].as<std::string>().c_str();
  const char* user = config["user"].as<std::string>().c_str();
  const char* pass = config["pass"].as<std::string>().c_str();
  const char* db = config["db"].as<std::string>().c_str();
  
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
  
  // WordNet data
  MYSQL* driver = mysql_init(NULL);
  if (!mysql_real_connect(driver, host, user, pass, db, 0, NULL, 0))
  {
    return db_error(driver, "Error connecting to database");
  }
  
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
      const char* getword = "SELECT word FROM wn_synset WHERE ss_type = 'a' OR ss_type = 's' ORDER BY RAND() LIMIT 1";
      if (mysql_query(driver, getword)) return db_error(driver, "Query failed");
      MYSQL_RES* getword2 = mysql_use_result(driver); if (getword2 == NULL) return db_error(driver, "Query failed");
      MYSQL_ROW getword3 = mysql_fetch_row(getword2); if (getword3 == NULL) return db_error(driver, "Query failed");
      std::string adj {getword3[0]};
      mysql_free_result(getword2);
      
      adj[0] = toupper(adj[0]);
      
      int j;
      while ((j = adj.find("_")) != std::string::npos)
      {
        adj[j] = ' ';
        adj[j+1] = toupper(adj[j+1]);
      }
      
      if (adj[adj.size()-1] == ')')
      {
        adj.resize(adj.size()-3);
      }
      
      form.replace(i, 5, adj);
    }
    
    // Nouns
    while ((i = form.find("{noun}")) != std::string::npos)
    {
      const char* getword = "SELECT word FROM wn_synset WHERE ss_type = 'n' ORDER BY RAND() LIMIT 1";
      if (mysql_query(driver, getword)) return db_error(driver, "Query failed");
      MYSQL_RES* getword2 = mysql_use_result(driver); if (getword2 == NULL) return db_error(driver, "Query failed");
      MYSQL_ROW getword3 = mysql_fetch_row(getword2); if (getword3 == NULL) return db_error(driver, "Query failed");
      std::string noun {getword3[0]};
      mysql_free_result(getword2);
      
      noun[0] = toupper(noun[0]);
      
      int j;
      while ((j = noun.find("_")) != std::string::npos)
      {
        noun[j] = ' ';
        noun[j+1] = toupper(noun[j+1]);
      }
      
      form.replace(i, 6, noun);
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
  
  mysql_close(driver);
}

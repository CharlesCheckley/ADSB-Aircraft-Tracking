#include "getData.h"
#include <iostream>
#include <curl/curl.h>
#include <string>
#include <fstream>


static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

json getData() 
{

  //uncomment below and comment the rest to use a sample json file
  //std::ifstream ifs("sample.json");
  //json j = json::parse(ifs);

  CURL *curl;
  CURLcode res;
  std::string readBuffer;
  curl = curl_easy_init();
      if(curl) {
            curl_easy_setopt(curl, CURLOPT_URL, "http://192.168.0.122/tar1090/data/aircraft.json");

            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
            res = curl_easy_perform(curl);
            curl_easy_cleanup(curl);
      }
    json j = json::parse(readBuffer);


  return j; 

}   


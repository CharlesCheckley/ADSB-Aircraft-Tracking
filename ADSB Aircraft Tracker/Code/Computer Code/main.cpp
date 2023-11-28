#include <iostream>
using namespace std;
#include <stdio.h>// printf 
#include "targeting_calculator.h"
#include "getData.h"
#include <time.h>



class Aircraft: public location {
    public: 
        string hex;  
        string flight; 
        float distance; //distance to the tracker 
        time_t lastSeen; 
};


class Tracker: public location { 
  float lat = 0;
  float lon = 0; 
  float elv = 0;  
}; 

Tracker tracker; 


int main() {
  tracker.locationToPoint(true);
  vector<Aircraft> list;


  while(true){
    //get the data
    json j = getData(); 
    
    //convert data to objects
    Aircraft *a1; 
    for(json& o : j["aircraft"]) {
      bool knownAircraft = false; 
      if (o["type"] == "adsb_icao") {  
        if (o.contains(string{"lat"}) && o.contains(string{"lon"}) && o.contains(string{"alt_geom"})){
          //todo check if object already exists for aircraft of this hex code
          vector<Aircraft>::iterator it;
          for ( it = list.begin(); it != list.end(); ++it ) {
            if (it->hex == o["hex"]){
              cout << "hex match";
              it->flight = o["flight"]; 
              it->lat = o["lat"]; 
              it->lon = o["lon"]; 
              it->elv = o["alt_geom"];
              it->locationToPoint(true); 
              it->distance = calcDistance(tracker, *it);
              knownAircraft = true; 
              break; 
            }
          }
          if (knownAircraft == false) {
            a1 = new Aircraft; 
            a1->hex = o["hex"]; 
            a1->flight = o["flight"]; 
            a1->lat = o["lat"]; 
            a1->lon = o["lon"]; 
            a1->elv = o["alt_geom"];
            a1->locationToPoint(true); 
            a1->distance = calcDistance(tracker, *a1);
            list.push_back(*a1);
          }
        }
      }
    }

    //current shortest distance
    float shortestDistance = 999999999; 
    Aircraft *ptrCurrentAircraft; 

    //iterate through all current aircraft objects
    vector<Aircraft>::iterator it;
    for ( it = list.begin(); it != list.end(); ++it ) {
      cout << it->flight << "\n\n"; 
      cout << it->distance << "\n\n";
      //if it is the closest aircraft set it as the current aircraft
      if (it->distance < shortestDistance){
        shortestDistance = it->distance; 
        ptrCurrentAircraft = &*it;
      };
      
    };
    cout << "current closest aircraft \n\n";
    cout << ptrCurrentAircraft->flight << "\n\n";  
    cout << "distance of" << ptrCurrentAircraft-> distance<< "\n\n";

    //distance check that the chosen aircraft is close enough to track
    if (ptrCurrentAircraft->distance < 100000000){
      cout << "starting tracking \n\n"; 
      auto [azimuth, altitude] = targetCalculate(*ptrCurrentAircraft, tracker, true); 
      ptrCurrentAircraft->lastSeen = time(NULL); 
      cout << azimuth << "\n\n";
      cout << altitude <<  " this is the angle above the horizon of B as seen from A \n\n"; 
      cout << ptrCurrentAircraft-> lastSeen; 
    } else {
      cout << "all aircraft are too far away to capture pictures \n\n"; 
    }; 
  }

    return 0; 
}



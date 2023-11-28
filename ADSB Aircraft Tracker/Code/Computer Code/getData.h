#ifndef getData_H
#define getData_H
#include "nlohmann/json.hpp"

//fetches the ADS-B data from a local dump1090/ readsb instance
using json = nlohmann::json;
using namespace std;
json getData(); 
#endif
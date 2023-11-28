#ifndef targeting_calculator_H
#define targeting_calculator_H

using namespace std;
#include <tuple>
#include <math.h> //sqrt


float  earthRadiusInMeters(float latitudeRadians); 
double geocentricLatitude(double lat); 

//class 
class location{
    public:
        float lat = 0;
        float lon = 0; 
        float elv = 0;
        float radius = 0; //oblation corrected radius of the earth for object's lattitude
        
        //object coordinates calculated by locationToPoint function
        double x =0;
        double y =0;
        double z =0;

        double nx; 
        double ny; 
        double nz; 
    
        void locationToPoint(bool oblate);  
}; 


//functions
float calcDistance (location &tracker, location &aircraft); 
location rotateGlobe(location b, location a, bool oblate); 
tuple<double, double, double> normalizeVectorDiff(location b, location a); 
tuple<double, double> targetCalculate(location &b, location &a, bool oblate); //returns azimuth and altitude angle

#endif 
#include "targeting_calculator.h"
#include <iostream>


//adapted from http://cosinekitty.com/compass.html 


float  earthRadiusInMeters(float latitudeRadians) //latitude is geodetic (same as GPS )
        {
            float a = 6378137.0;  // equatorial radius in meters
            float b = 6356752.3;  // polar radius in meters
            double cosLat = cos(latitudeRadians);
            double sinLat = sin(latitudeRadians);
            double t1 = a * a * cosLat;
            double t2 = b * b * sinLat;
            double t3 = a * cosLat;
            double t4 = b * sinLat;
            return sqrt((t1*t1 + t2*t2) / (t3*t3 + t4*t4));
        };

double geocentricLatitude(double lat) {
    // Convert geodetic latitude 'lat' to a geocentric latitude 'clat'.
    // Geodetic latitude is the latitude as given by GPS.
    // Geocentric latitude is the angle measured from center of Earth between a point and the equator.
    // https://en.wikipedia.org/wiki/Latitude#Geocentric_latitude
    double e2 = 0.00669437999014;
    double clat = atan((1.0 - e2) * tan(lat));
    return clat;
};

void location::locationToPoint(bool oblate) {  
            double rlat = lat * M_PI / 180.0; //convert a latitude from degrees to radians
            double rlon = lon * M_PI / 180.0; //convert a longitude from degrees to radians
            radius = oblate ? earthRadiusInMeters(rlat) : 6371009; //earth's radius, can be corrected for oblation
            double clat = oblate ? geocentricLatitude(rlat) : rlat; //calculate geocentric latitude, can account for oblation

            double cosLon = cos(rlon); 
            double sinLon = sin(rlon); 
            double cosLat = cos(clat); 
            double sinLat = sin(clat); 

            //standard conversion for geocentric coordinates
            x = radius * cosLon * cosLat;
            y = radius * sinLon * cosLat;
            z = radius * sinLat;


            // We used geocentric latitude to calculate (x,y,z) on the Earth's ellipsoid.
            // Now we use geodetic latitude to calculate normal vector from the surface, to correct for elevation.
            double cosGlat = cos(rlat); 
            double sinGlat = sin(rlat); 

            nx = cosGlat * cosLon;
            ny = cosGlat * sinLon;
            nz = sinGlat;

            x += elv * nx;
            y += elv * ny;
            z += elv * nz;

          
    };

float calcDistance (location &a, location &b) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    float dz = a.z - b.z;
    return sqrt(dx*dx + dy*dy + dz*dz);
};



    //trick to work out the azimuth angle
location rotateGlobe(location b, location a, bool oblate) {
    // Get modified coordinates of 'b' by rotating the globe so that 'a' is at lat=0, lon=0.
    b.lon = b.lon - a.lon; 
    b.locationToPoint(oblate);


    // Rotate brp cartesian coordinates around the z-axis by a.lon degrees,
    // then around the y-axis by a.lat degrees.
    // Though we are decreasing by a.lat degrees, as seen above the y-axis,
    // this is a positive (counterclockwise) rotation (if B's longitude is east of A's).
    // However, from this point of view the x-axis is pointing left.
    // So we will look the other way making the x-axis pointing right, the z-axis
    // pointing up, and the rotation treated as negative.

    double alat = -a.lat * M_PI / 180.0;
    if (oblate) {
        alat = geocentricLatitude(alat);
    };
    double acos = cos(alat);
    double asin = sin(alat);


    location l; 
    l.x = (b.x * acos) - (b.z * asin);
    l.y = b.y;
    l.z = (b.x * asin) + (b.z * acos);
    l.radius = b.radius; 
    return l;
    
}; 

tuple<double, double, double> normalizeVectorDiff(location b, location a) {
// Calculate norm(b-a), where norm divides a vector by its length to produce a unit vector.
    double dx = b.x - a.x;
    double dy = b.y - a.y;
    double dz = b.z - a.z;
    double dist2 = dx*dx + dy*dy + dz*dz;
    
    double dist = sqrt(dist2);
    return { dx/dist, dy/dist, dz/dist };   
};

tuple<double, double> targetCalculate(location &b, location &a, bool oblate ) {
    a.locationToPoint(oblate);
    b.locationToPoint(oblate);
  


    // Let's use a trick to calculate azimuth:
    // Rotate the globe so that point A looks like latitude 0, longitude 0.
    //point B becomes point B rotated br
    // We keep the actual radii calculated based on the oblate geoid,
    // but use angles based on subtraction.
    // Point A will be at x=radius, y=0, z=0.
    // Vector difference B-A will have dz = N/S component, dy = E/W component.                
    location br; 
    br = rotateGlobe (b, a, oblate);
    double azimuth; 
    if (br.z*br.z + br.y*br.y > 1.0e-6) {
        double theta = atan2(br.z, br.y) * 180.0 / M_PI;
        azimuth = 90.0 - theta;
        if (azimuth < 0.0) {
            azimuth += 360.0;
        };
        if (azimuth > 360.0) {
            azimuth -= 360.0;
        };
    } else {
        azimuth = 0;  
    };
    //missing if statment to check for null bp data
    auto [bmax, bmay, bmaz] = normalizeVectorDiff(b, a); 
    // Calculate altitude, which is the angle above the horizon of B as seen from A.
    // Almost always, B will actually be below the horizon, so the altitude will be negative.
    // The dot product of bma and norm = cos(zenith_angle), and zenith_angle = (90 deg) - altitude.
    // So altitude = 90 - acos(dotprod).
    double altitude = 90.0 - (180.0 / M_PI)*acos(bmax*a.nx + bmay*a.ny + bmaz*a.nz);
    return {azimuth, altitude}; 

};




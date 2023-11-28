#ifndef Tracker_Control_H
#define Tracker_Control_H


//tilt stepper driver setup
const int tilt_stepper_MS2_PIN = 7; 
const int tilt_stepper_MS1_PIN = 8;
const int tilt_stepper_EN_PIN  = 9; // Enable
const int tilt_stepper_DIR_PIN  = 2; // Direction
const int tilt_stepper_STEP_PIN  = 3; // Step
const int tilt_stepper_DRIVER_ADDRESS = 0b11; // TMC2209 Driver address according to MS1 and MS2

//pan stepper driver setup
//MS1 and MS2 are not defined. They default to low giving address of 0b00
const int pan_stepper_EN_PIN  = 12; // Enable
const int pan_stepper_DIR_PIN  = 10; // Direction
const int pan_stepper_STEP_PIN  = 11; // Step
const int pan_stepper_DRIVER_ADDRESS = 0b00; // TMC2209 Driver address according to MS1 and MS2


//common variables for both drivers
#define R_SENSE 0.11f // Match to your driver // SilentStepStick series use 0.11
const int MICROSTEPS = 16; 
const float TILT_PULLEY_RATIO = 3.75; 
const int STEPS_PER_REV = 200; 
const int PAN_GEAR_RATIO = 8.470; 


//function declarations 
void GOTO( float targetAzimuth, float targetTiltAngle); 
void autoMode(); 
void calibrate(); 
void processCommand(); 
void parseData(); 
void recvUserInput(); 
void trackerSetup(); 
void trackerLoop(); 


#endif


#include <iostream>
#include <math.h>       /* fmod */
#include "TrackerControl.h"
#include <TMCStepper.h>
#include <AccelStepper.h>
// #include "targeting_calculator.h"

//serial connection to drivers using TMCstepper library
  UART mySerial (digitalPinToPinName(4), digitalPinToPinName(5), NC, NC);  // create a hardware serial port named mySerial with RX: pin 13 and TX: pin 8
  #define SERIAL_PORT mySerial // TMC2208/TMC2224 HardwareSerial port
  TMC2209Stepper tilt_driver(&SERIAL_PORT, R_SENSE, tilt_stepper_DRIVER_ADDRESS); // Hardware Serial0
  TMC2209Stepper pan_driver(&SERIAL_PORT, R_SENSE, pan_stepper_DRIVER_ADDRESS); // Hardware Serial0

  //defining drivers for accel stepper
  AccelStepper tilt_stepper = AccelStepper(tilt_stepper.DRIVER, tilt_stepper_STEP_PIN, tilt_stepper_DIR_PIN);
  AccelStepper pan_stepper = AccelStepper(pan_stepper.DRIVER, pan_stepper_STEP_PIN, pan_stepper_DIR_PIN);


  //handles user serial input
  const byte numChars = 33;
  char receivedChars[numChars];   // an array to store the received data
  char tempChars[numChars];        // temporary array for use when parsing
  boolean newData = false;

  // variables to hold the parsed data
  char messageFromPC[numChars] = {0};
  float firstNoFromPC; //set to impossible values
  float secondNoFromPC; //set to impossible values
  float currentAzimuth;
  float currentTiltAngle;
  int Microsteps = 16; 

void trackerSetup() {
  //sets the address of the tilt stepper driver to 0b11
  //pan stepper driver's address defaults to 0b00
  pinMode(tilt_stepper_MS2_PIN, OUTPUT);
  digitalWrite(tilt_stepper_MS2_PIN, HIGH); //set MS2 pin high
  pinMode(tilt_stepper_MS1_PIN, OUTPUT);
  digitalWrite(tilt_stepper_MS1_PIN, HIGH); //set MS1 pin high
  //setting MS1 snd MS2 low sets UART address to 0

  delay(5000); //delay to allow uploading of new scripts before serial port starts
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Native USB only
  }

  SERIAL_PORT.begin(115200);
  tilt_driver.begin();
  pan_driver.begin();
  Serial.print("\nTesting tilt driver connection...");
  uint8_t result = tilt_driver.test_connection();
  if (result) {
    Serial.println("failed!");
    Serial.print("Likely cause: ");
    switch (result) {
      case 1: Serial.println("loose connection"); break;
      case 2: Serial.println("no power"); break;
    }
    Serial.println("Fix the problem and reset board.");
    // We need this delay or messages above don't get fully printed out
    delay(100);
    //abort();
  } else {
    Serial.println("Tilt Driver connection succesfull!");
  }
  Serial.print("\nTesting pan driver connection...");
  result = pan_driver.test_connection();
  if (result) {
    Serial.println("failed!");
    Serial.print("Likely cause: ");
    switch (result) {
      case 1: Serial.println("loose connection"); break;
      case 2: Serial.println("no power"); break;
    }
    Serial.println("Fix the problem and reset board.");
    // We need this delay or messages above don't get fully printed out
    delay(100);
    //abort();
  } else {
    Serial.println("Pan driver connection succesfull!");
  }

  // Initiate pins and registeries
  tilt_driver.toff(5);                 // Enables driver in software

  tilt_driver.rms_current(900);    // Set stepper current to 600mA. The command is the same as command TMC2130.setCurrent(600, 0.11, 0.5);
  tilt_driver.microsteps(Microsteps );
  tilt_stepper.setMaxSpeed(1800); // 100mm/s @ 80 steps/mm
  tilt_stepper.setAcceleration(900); // 2000mm/s^2
  tilt_stepper.setEnablePin(tilt_stepper_EN_PIN);
  tilt_stepper.setPinsInverted(false, false, true);
  tilt_stepper.enableOutputs();

  pan_driver.toff(5);                 // Enables driver in software
  pan_driver.rms_current(900);    // Set stepper current to 600mA. The command is the same as command TMC2130.setCurrent(600, 0.11, 0.5);
  pan_driver.microsteps(Microsteps);
  pan_stepper.setMaxSpeed(2700); // 100mm/s @ 80 steps/mm
  pan_stepper.setAcceleration(900); // 2000mm/s^2
  pan_stepper.setEnablePin(pan_stepper_EN_PIN);
  pan_stepper.setPinsInverted(false, false, true);
  pan_stepper.enableOutputs();
}


void trackerLoop() {
  recvUserInput();
  if (newData == true) {
    strcpy(tempChars, receivedChars);
    // this temporary copy is necessary to protect the original data
    //   because strtok() used in parseData() replaces the commas with \0
    processCommand();
    newData = false;
  }

  tilt_stepper.run();
  pan_stepper.run();
  if (tilt_stepper.currentPosition() >= 12000 ||  tilt_stepper.currentPosition() <= -12000 ) {
    Serial.print("Tilt 0! ");
    tilt_stepper.setCurrentPosition(0);
  }
  if (pan_stepper.currentPosition() >= 27104 ||  pan_stepper.currentPosition() <= -27104 ) {
    Serial.print("Pan 0! ");
    pan_stepper.setCurrentPosition(0);
  }
  currentAzimuth = pan_stepper.currentPosition() / 75.28888888;
  currentTiltAngle = tilt_stepper.currentPosition() / 33.333333;
}


void recvUserInput() {
  static byte ndx = 0;
  char endMarker = '\n';
  char rc;

  while (Serial.available() > 0 && newData == false) {
    rc = Serial.read();

    if (rc != endMarker) {
      receivedChars[ndx] = rc;
      ndx++;
      if (ndx >= numChars) {
        ndx = numChars - 1;
      }
    }
    else {
      receivedChars[ndx] = '\0'; // terminate the string
      ndx = 0;
      newData = true;
    }
  }
}


void parseData() {      // split the data into its parts
  char * strtokIndx; // this is used by strtok() as an index
  strtokIndx = strtok(tempChars, " ");     // get the first part - the string
  strcpy(messageFromPC, strtokIndx); // copy it to messageFromPC
  strtokIndx = strtok(NULL, " "); // this continues where the previous call left off
  firstNoFromPC = atof(strtokIndx);     // convert this part to an float
  strtokIndx = strtok(NULL, " ");
  secondNoFromPC = atof(strtokIndx);     // convert this part to a float


  Serial.print("Message ");
  Serial.println(messageFromPC);
  Serial.print("first number ");
  Serial.println(firstNoFromPC);
  Serial.print("second number ");
  Serial.println(secondNoFromPC);

}

void processCommand() {
  if (newData == true) {
    // Serial.print("Ready for command ... ");
    if (strcasecmp(receivedChars, "stop") == 0) // If strcmp returns 0, the strings match
    {
      Serial.println("STOP!");
      tilt_stepper.stop();
      pan_stepper.stop();
    }
    else if (strcasecmp(receivedChars, "calibrate") == 0) // If strcmp returns 0, the strings match
    {
      Serial.println("start calibrate");

      calibrate();
    }
    else if (strcasecmp(receivedChars, "get position") == 0) // If strcmp returns 0, the strings match
    {
      Serial.println("tilt motor position: ");
      Serial.println(tilt_stepper.currentPosition());
      Serial.println("\n pan motor position: ");
      Serial.println(pan_stepper.currentPosition());
    }
    else if (strcasecmp(receivedChars, "get dposition") == 0) // If strcmp returns 0, the strings match
    {
      Serial.println("tilt motor position: ");
      Serial.println(currentAzimuth);
      Serial.println("\n pan motor position: ");
      Serial.println(currentTiltAngle);
    }
    else if (strncmp(receivedChars, "GOTO", 4) == 0) // If strcmp returns 0, the strings match
    {
      Serial.println("going to azimuth / elevation");
      parseData();
      if (1 == 1) {
        //if (firstNoFromPC <= 360 && firstNoFromPC >= 0 && secondNoFromPC <= 90 && secondNoFromPC >= -10) {
        GOTO(firstNoFromPC, secondNoFromPC);
      }  else {
        Serial.println("invalid input");
      }
    }
    else if (strncmp(receivedChars, "STEP", 4) == 0) // If strcmp returns 0, the strings match
    {
      Serial.println("Stepping");
      parseData();
      if (fmod(firstNoFromPC, 1) == 0.0 && fmod(secondNoFromPC, 1) == 0.0) { //checks for integer values
        pan_stepper.move(firstNoFromPC);
        tilt_stepper.move(secondNoFromPC);
      }  else {
        Serial.println("invalid input");
      }
    }
    else if (strncmp(receivedChars, "MICROSTEPS", 10) == 0) // If strcmp returns 0, the strings match
    {
      Serial.println("setting microsteps");
      parseData();
      if (fmod(firstNoFromPC, 1) == 0.0 && fmod(secondNoFromPC, 1) == 0.0) { //checks for integer values
          pan_driver.microsteps(Microsteps);
          tilt_driver.microsteps(Microsteps);
          Microsteps = firstNoFromPC; 
      }  else {
        Serial.println("invalid input");
      }
    }
    
  }
}

void GOTO( float targetAzimuth, float targetTiltAngle) {

  //take two angles
  //find the difference between them
  //pan axis
  float panAngleToMove = targetAzimuth - currentAzimuth;
  panAngleToMove = fmod((panAngleToMove + 360), 360);
  if (panAngleToMove > 180) {
    panAngleToMove -= 360;
  }
  int panSteps = panAngleToMove * PAN_GEAR_RATIO * STEPS_PER_REV * Microsteps / 360 ;
  pan_stepper.move(panSteps);

  //tilt axis
  float tiltAngleToMove = targetTiltAngle - currentTiltAngle;
  tiltAngleToMove = fmod((tiltAngleToMove + 360), 360);
  if (tiltAngleToMove > 180) {
    tiltAngleToMove -= 360;
  }
  int tiltSteps = tiltAngleToMove * TILT_PULLEY_RATIO * STEPS_PER_REV * Microsteps / 360 ;
  tilt_stepper.move(tiltSteps);

}

void calibrate() {
  Serial.println("Calibrating");
  tilt_stepper.setCurrentPosition(0);
  pan_stepper.setCurrentPosition(0);
}

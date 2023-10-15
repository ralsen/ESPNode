/*
  ToF.ino

  ToF -> time of Flight Sensor (VL53L1X from ST-Microelectronics)

  !!! for history see settings.h !!!

  ToDos:    ???

  hints:    ???
*/

#include <Arduino.h>
#include "settings.h"
#include "Config.h"
#include  "log.h"


#if (H_TOF == H_TRUE)
 #include "ToF.h"
 #include "SparkFun_VL53L1X.h" //Click here to get the library: http://librarymanager/All#SparkFun_VL53L1X
 //#include <Wire.h>

 //Optional interrupt and shutdown pins.
 #define SHUTDOWN_PIN 2
 #define INTERRUPT_PIN 3

extern class log_CL logit;

 SFEVL53L1X distanceSensor;
 //Uncomment the following line to use the optional shutdown and interrupt pins.
 //SFEVL53L1X distanceSensor(Wire, SHUTDOWN_PIN, INTERRUPT_PIN);

int ToFPresent;

int SetupToF(){
  DBGF("SetupToF()");

  Wire.begin();

  if (distanceSensor.begin() != 0) //Begin returns 0 on a good init
  {
    Serial.println("Sensor failed to initialisation. Please check wiring ...");
    logit.entry("ToF-Sensor failed !!!");
    ToFPresent = TOF_FALSE;
  }
  else{
    Serial.println("ToF-Sensor detected!");
    logit.entry("ToF-Sensor detected.");
    ToFPresent = TOF_TRUE;
  }
  return ToFPresent;
}

int ToFRange;

void ToFDistance(void){
  if(!ToFPresent){
    ToFRange = -1;
    return;
  }
  DBGF("!!! I S R !!! ToFDistance()");
  distanceSensor.startRanging(); //Write configuration bytes to initiate measurement
  while (!distanceSensor.checkForDataReady())
  {
    delay(1);
  }
  int distance = distanceSensor.getDistance(); //Get the result of the measurement from the sensor
  distanceSensor.clearInterrupt();
  distanceSensor.stopRanging();
  sysData.CntMeasCyc++;
  ToFRange = distance;
}

#endif //(H_TOF == H_TRUE)

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


#if (S_TOF == true)
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
    DBGLN("Sensor failed to initialisation. Please check wiring ...");
    #if(S_FS == true)
    logit.entry("ToF-Sensor failed !!!");
    #endif
    ToFPresent = TOF_FALSE;
  }
  else{
    DBGLN("ToF-Sensor detected!");
    #if(S_FS == true)
    logit.entry("ToF-Sensor detected.");
    #endif
    ToFPresent = TOF_TRUE;
  }
  return ToFPresent;
}

int ToFRange;

void ToFDistance(void){
  int distance = 1234;

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
  Serial.println("");
  Serial.println("getDistance");
  distance = distanceSensor.getDistance(); //Get the result of the measurement from the sensor
  Serial.println("clearInterrupt");
  distanceSensor.clearInterrupt();
  Serial.println("stopRanging");
  distanceSensor.stopRanging();
  sysData.CntMeasCyc++;
  ToFRange = distance;
}

#endif //(S_TOF == true)

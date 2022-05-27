/*
  DS1820.ino

  all functions for the DS1820. the functions handle up to ONE_WIRE_MAX_DEV DS1820.
  here weÂ´re doing all the stuff to communicate with the DS1820, calculate the Temp-value
  put the results into an array, handle the DS1820-Addresses etc.

  !!! for history see end of file !!!

  todos:    done: measurement frequency and transmit frequency has do be defined (default values and configuration values in webserver)

  hints:    ???
*/

#if (H_DS1820 == H_TRUE)

#include  "Settings.h"
#include  <OneWire.h>
#include  "DS1820.h"
#include  <string.h>

OneWire           oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);
int               numberOfDevices; //Number of temperature devices found
DeviceAddress     devAddr[ONE_WIRE_MAX_DEV];  //An array device temperature sensors
float             tempDev[ONE_WIRE_MAX_DEV]; //Saving the last measurement of temperature
float             tempDevLast[ONE_WIRE_MAX_DEV]; //Previous temperature measurement


//------------------------------------------
//Convert device id to String
String GetAddressToString(DeviceAddress deviceAddress){
  String str = "";
  DBGF( "GetAddressToString(DeviceAddress deviceAddress)");
  for (uint8_t i = 0; i < 8; i++){
    if( deviceAddress[i] < 16 ) str += String(0, HEX);
    str += String(deviceAddress[i], HEX);
  }
  return str;
}

//Setting the temperature sensor
void SetupDS18B20(){
  DS18B20.begin();

  DBGF("SetupDS18B20()");
  DBGL("Parasite power is: ");
  if( DS18B20.isParasitePowerMode() ){
    DBGLN("ON");
  }else{
    DBGLN("OFF");
  }

  numberOfDevices = DS18B20.getDeviceCount();

//  # if (H_DBG == H_TRUE)
    numberOfDevices = 2;
//  # endif

  DBGL( "DS1820 count: " );
  DBGLN( numberOfDevices );

  DS18B20.requestTemperatures();

  // Loop through each device, print out address
  for(int i=0;i<numberOfDevices; i++){
    // Search the wire for address
    if( DS18B20.getAddress(devAddr[i], i) ){
      DBGL("Found device ");
      DBGL(i);
      DBGL(" with address: " + GetAddressToString(devAddr[i]));
      DBGLN();
    }else{
      DBGL("Found ghost device at ");
      DBGL(i);
      DBGL(" but could not detect address. Check power and cabling");
    }

    //Get resolution of DS18b20
    DBGL("Resolution: ");
    DBGL(DS18B20.getResolution( devAddr[i] ));
    DBGLN();

    //Read temperature from DS18b20
    float tempC = DS18B20.getTempC( devAddr[i] );
    tempC *= 1;
    DBGL("Temp C: ");
    DBGLN(tempC);
  }
}

// interruptservice !!!
//Loop measuring the temperature
void DS1820_Measuring(void){
  if( sysData.mode == MODE_STA) {
    DBGF("!!! I S R !!! DS1820_Measuring()");
      for(int i=0; i<numberOfDevices; i++){
        float tempC = DS18B20.getTempC( devAddr[i] ); //Measuring temperature in Celsius
        tempDev[i] = tempC; //Save the measured value to the array
      }
      DS18B20.setWaitForConversion(false); //No waiting for measurement
      DS18B20.requestTemperatures(); //Initiate the temperature measurement
      sysData.CntMeasCyc++;
    }
}

#endif //(H_DS1820 == H_TRUE)

/*
  History:
  --------------------- V1.4
  14.01.21  Chip-ID is not longer used because its not realy a UID
  --------------------- V1.4

  07.05.20  V1.30b: measuring only in MODE_STA
  16.03.19  added chipID to devicename in URL for unique identification on server when

  05.03.19  V1.03: config or DS1820 added (Meascyx, Transcyc and pagereload)
            count the messages to server, the pagereloads (good and bad trys)

  03.03.19  V1.02: renamed TempLoop to MeasuringLoop which runs from now in sec_ISR.
  23.02.19  first Version wirh new Version management. Released for Sonoff S20 and Sonoff Basic
  19.01.19  V0.04 first version to re released
            debuging and error-check for all versions is to be done

  16.12.18  extract the routines from the main-File
*/

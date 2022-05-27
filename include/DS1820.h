/*
  DS1820.h

  !!! for history see end of file !!!

  all functions for the DS1820. the functions handle up to ONE_WIRE_MAX_DEV DS1820.
  here weÂ´re doing all the stuff to communicate with the DS1820, calculate the Temp-value
  put the results into an array, handle the DS1820-Addresses etc.

  ToDos:    -

  hints:    -
*/

#if (H_DS1820 == H_TRUE)
# ifndef _DS1820_H_
# define _DS1820_H_

#include    <OneWire.h>
#include    <DallasTemperature.h>

# define     ONE_WIRE_BUS      H_DS1820_PIN    //Pin to which is attached a temperature sensor
# define     ONE_WIRE_MAX_DEV  H_DS180_MAX_DEV //The maximum number of devices

  extern int  numberOfDevices;
  extern DeviceAddress     devAddr[];
  void        DS1820_Measuring(void);
  String      GetAddressToString(DeviceAddress deviceAddress);
  void        SetupDS18B20(void);
  void        TempLoop(long now);

#  endif // _DS1820_H_
#endif //(H_DS1820 == H_TRUE)

/*
  History:
  20.04.20  V1.30 from now we use platformio instead of the Arduiono stuff
  23.02.19  first Version wirh new Version management. Released for Sonoff S20 and Sonoff Basic
  19.01.19  V0.04 first version to re released
            debuging and error-check for all versions is to be done

  16.12.18  extract the routines from the main-File

  todos:
*/

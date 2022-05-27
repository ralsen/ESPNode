/*
  WiFi.h

  !!! for history see end of file !!!

  ToDos:    -

  hints:    -
*/

#ifndef _WIFI_H_
#define _WIFI_H_

#define SCAN_PERIOD     5000      // for what???
#define MAX_WIFI_TRY    20        // max count of tries
#define DELAY_WIFI_TRY  500       // ms between two tries

int  WiFiStartClient(void);
void WiFiStartAP(void);

#endif // _WIFI_H_

/*
  history:
  --------------------- V2.0a
  21.11.21  lot of changes for 2.xx, made for arduino IDE and 
            many changes to improve stability, performance etc.

  20.04.20  V1.30 from now we use platformio instead of the Arduiono stuff
  23.02.19  first Version wirh new Version management. Released for Sonoff S20 and Sonoff Basic
  19.01.19  V0.04 first version to re released
            debuging and error-check for all versions is to be done

 */

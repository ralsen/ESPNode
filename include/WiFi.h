/*
  WiFi.h

  !!! for history see settings.h !!!

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

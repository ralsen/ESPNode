/*
  WiFi.h

  !!! for history see settings.h !!!

  ToDos:    -

  hints:    -
*/

#ifndef _WIFI_S_
#define _WIFI_S_

#define SCAN_PERIOD     5000      // for what???
#define MAX_WIFI_TRY    20        // max count of tries
#define DELAY_WIFI_TRY  100       // ms between two tries

int  WiFiStartClient(void);
void WiFiStartAP(void);

#endif // _WIFI_S_

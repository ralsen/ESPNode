/*
  WebServer.h

  !!! for history see end of file !!!

  ToDos:    -

  hints:    -
*/

#ifndef _WEBSERVER_H_
#define _WEBSERVER_H_

#include  <ESP8266WebServer.h>

// -----------------------------------------
// Server stuff
#define MyServerPort  80

#define W_TEXT_PLAIN      "text/plain"
#define W_TEXT_HTML       "text/html"

#define W_URL_ROOT        "/"
#define W_URL_ON          "on"
#define W_URL_OFF         "off"
#define W_URL_FIX_IP      "fixip"
#define W_URL_SCAN        "scan"
#define W_URL_LED         "led"
#define W_URL_INFO        "info"
#define W_URL_RESET       "reset"
#define W_URL_APMODE      "ap-mode"
#define W_URL_STAMODE     "sta-mode"
#define W_URL_HOSTNAME    "hostname"
#define W_URL_STATUS      "status"
#define W_URL_DEFAULT     "default"
#define W_URL_ERASE       "erase"
#define W_URL_SERVER      "server"
#define W_URL_SERVICE     "service"
#define W_URL_NETWORK     "network"
#define W_URL_CONFIG      "config"
#define W_URL_SHOWLOG     "showlog"
#define W_URL_SHOWDIR     "showdir"
#define W_URL_MEASCYC     "meascyc"
#define W_URL_TRANSCYC    "transcyc"
#define W_URL_PAGERELOAD  "pagereload"

extern ESP8266WebServer server;

void WebServerStart(void);
void handleStatus();
String BuildURL (void);

#if (H_DS1820 == H_TRUE)
  String      BuildDS1820URL (void);
  String      BuildDS1820HTML(void);
#elif (H_SWITCH == H_TRUE)
  String      BuildSwitchURL(void);
#endif

#endif // _WEBSERVER_H_

/*
  history:
  --------------------- V2.0a
  21.11.21  lot of changes for 2.xx, made for arduino IDE and 
            many changes to improve stability, performance etc.
  --------------------- V1.4
  18.01.21  FNC_TYPE isnt displayed in headline of status page anymore
  18.01.21  Version is only displayed in hardline og status page
  18.01.21  first messge is send after going into MODE_STA
  17.01.21  ToF sensor added
  17.01.21  if cfg.TransmitCycle is 0 nothing is send to the Server
            MAC-Address is used for identification and is displayed during
            boot. MACAddress is appended to the device name instead of the
            Chip-Number because this number is not realy unique, espressif
            stated
  --------------------- V1.4

  20.04.20  V1.30 from now we use platformio instead of the Arduiono stuff
  23.02.19  first Version wirh new Version management. Released for Sonoff S20 and Sonoff Basic
  19.01.19  V0.04 first version to re released
            debuging and error-check for all versions is to be done

 */

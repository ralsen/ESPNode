/*
  WebServer.h

  !!! for history see settings.h !!!

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


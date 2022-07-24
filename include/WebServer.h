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

extern ESP8266WebServer server;

void initHTML(void);
void WebServerStart(void);
void handleStatus();
String BuildURL (void);
String readHTML (String);

#if (H_DS1820 == H_TRUE)
  String      BuildDS1820URL (void);
  String      BuildDS1820HTML(void);
#elif (H_SWITCH == H_TRUE)
  String      BuildSwitchURL(void);
#endif

#endif // _WEBSERVER_H_


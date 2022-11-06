/*
  WebServer.h

  !!! for history see settings.h !!!

  ToDos:    -

  hints:    -
*/
#include <Arduino.h>
#include "settings.h"
#include "Config.h"

#ifndef _WEBSERVER_H_
#define _WEBSERVER_H_


// -----------------------------------------
// Server stuff
#define MyServerPort  80

#define W_TEXT_PLAIN      "text/plain"
#define W_TEXT_HTML       "text/html"

//FOL extern ESP8266WebServer server;

void initHTML(void);
void WebServerStart(void);
void handleStatus(void);
void handleInfo(void);
void handleServer(void);
void handleService(void);
void handleReset(void);
void handleLED(void);
void handleSetDefault(void);
void handleNetPage(void);
void handleConfPage(void);;
void handleShowLog(void);
void handleShowDir(void);
void handlePagereload(void);
void handleMeasCyc(void);
void handleTransCyc(void);
void handleFavicon(void);
void handleNotFound(void);
void handleHostName(void);
void handleUpdate(void);
void startWebServer(void);

void buildInfoPage(void);
String buildConfPage(String);
String buildNetworkPage(String);
String BuildURL (void);
String readHTML (String);
String buildAppPage(String);
String buildMainPage(String);
String buildURL (void);
String buildDS1820Page(void);
String buildDS1820URL (void);
String buildSwitchPage(void);
String buildSwitchURL(void);

int checkInput(String, String);
bool string_isNumber(String);

#if (H_DS1820 == H_TRUE)
  String      BuildDS1820URL (void);
  String      BuildDS1820HTML(void);
#elif (H_SWITCH == H_TRUE)
  String      BuildSwitchURL(void);
#endif

#if (H_TOF == H_TRUE)
String buildToFPage(void);
String buildToFURL (void);
#endif

#endif // _WEBSERVER_H_


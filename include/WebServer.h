/*
  WebServer.h

  !!! for history see settings.h !!!

  ToDos:    -

  hints:    -
*/
#include <Arduino.h>
#include "settings.h"
#include "Config.h"

#ifndef _WEBSERVER_S_
#define _WEBSERVER_S_


// -----------------------------------------
// Server stuff
#define MyServerPort  80

#define W_TEXT_PLAIN      "text/plain"
#define W_TEXT_HTML       "text/html"

void initHTML(void);
void WebServerStart(void);
void handleStatus(void);
void handleServer(void);
void handlePort(void);
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
void handleUpdate_part1(void);
void handleUpdate_part2(void);
void handleHostName(void);
void handleUpdate(void);
void startWebServer(void);
void handleUpdate(void);
void handleScan(void);
void ScanStart(void);
void ScanEnd(void);

void buildInfoPage(void);
String buildConfPage(String);
String buildNetworkPage(String);
String buildAppPage(String);
String buildMainPage(String);
String buildDict (void);
String buildTypeDict(void);
String buildTypePage(void);
String buildFullName(void);

int checkInput(String, String);
bool string_isNumber(String);

#if (S_TOF == true)
String buildToFPage(void);
String buildToFDict (void);
#endif

#endif // _WEBSERVER_S_


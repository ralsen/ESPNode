/*
  WebServer.ino

  !!! for history see end of file !!!

  ToDos:    ???

  hints:    ???
*/


#include  "settings.h"
#include  "Config.h"
#include  "WebServer.H"
#include  "timer.h"
#include  "log.h"
#include  "ToF.h"
#include  <string>
#include  <iostream>
#include  <SPI.h>
#include "LittleFS.h"

// -----------------------------------------
// Server stuff
//------------------------------------------
//HTTP

extern class log_CL logit;

ESP8266WebServer server(MyServerPort);

const char H_HOME_PAGE[] PROGMEM = {
  //  this is the page frame for all webpages
  //  all pages should be generated by replacing the {pagename}-fields width
  //  the webpages for the particular pages
  "<!DOCTYPE html>\r\n"
  "<html lang=\"de\">\r\n"
  "<head>\r\n"
  "<meta charset=\"utf-8\">\r\n"
  "<TITLE>{title}</TITLE>\r\n"
  "<meta name=\"viewport\" content=\"width=device-width, initial-scale=0.5\">\r\n"
  "<style>\r\n"
  "body {\r\n"
    "  font-family: Verdana, sans-serif;\r\n"
    "  font-size: 87.5%;\r\n"
    "  background-color: #ffe9b4;\r\n"
    "  color: black;\r\n"
    "  padding: 0;\r\n"
    "  margin:0;\r\n"
  "}\r\n"

  "div1 {\r\n"
    "  width: 100%;\r\n"
    "  color: black;\r\n"
    "  padding: 10px;\r\n"
    "  position: absolute;\r\n"
    "  top: 60px;\r\n"
    "  left: 150px;\r\n"
  "}\r\n"

  ".vertical-menu a {\r\n"
    "  background-color: #eee;\r\n"
    "  color: black;\r\n"
    "  display: block;\r\n"
    "  padding: 12px;\r\n"
    "  text-decoration: none;\r\n"
  "}\r\n"

  ".vertical-menu {\r\n"
    "  width: 100px;\r\n"
    "  background-color: #e9b4;\r\n" //menuefarbe ???
  "}\r\n"

  ".vertical-menu a:hover {\r\n"
    "  background-color: #4CAFf0;\r\n"
  "}\r\n"

  ".vertical-menu a.active {\r\n"
    "  background-color: #4CAFf0;\r\n"
    "  color: white;\r\n"
  "}\r\n"

  "</style>\r\n"
  "</head>\r\n"
  "<body>\r\n"

  "<h1>{hostname}</h1>\r\n"
  "<div1>{content}</div1>\r\n"
  "<div class=\"vertical-menu\">\r\n"
  "  <a href=" W_URL_INFO      ">Info</a><br>\r\n"
  "{appmenu}"
  "{mainpage}"
  "{networkpage}"
  "{confpage}"
  "</div>\r\n"
  "</body>\r\n"
  "</html>\r\n"};

const char H_APPMENU[]  PROGMEM = {
  "  <a href=" W_URL_STATUS    ">Status</a><br>\r\n"
  "{appmenu}"};

const char H_STATUSMENU[]  PROGMEM = {
#if (H_SWITCH == H_TRUE)
  "  <a href=" W_URL_ON          ">Ein</a><br>\r\n"
  "  <a href=" W_URL_OFF         ">Aus</a><br>\r\n"
#else
  ""
#endif  
   };

const char H_INFOMENU[] PROGMEM = {
  "  <a href=" W_URL_NETWORK     ">Network</a>\r\n "
  "  <a href=" W_URL_CONFIG      ">Config</a><br>\r\n "
  "  <a href=" W_URL_SHOWLOG     ">show log</a>\r\n "
  "  <a href=" W_URL_SHOWDIR     ">show Dir</a>\r\n "};

const char H_NETWORKMENU[] PROGMEM = {
  "  <a href=" W_URL_SCAN        ">Scan</a>\r\n "
  "  <a href=" W_URL_STAMODE     ">STA-Mode</a>\r\n"
  "  <a href=" W_URL_APMODE      ">AP-Mode</a>\r\n"
  "  <a href=" W_URL_FIX_IP      ">Fixed IP</a></p>\r\n"};

const char H_CONFMENU[] PROGMEM = {
#if (H_DS1820 == H_TRUE)||(H_TOF == H_TRUE)
  "  <a href=" W_URL_MEASCYC     ">MeasCyc</a>\r\n "
#endif
  "  <a href=" W_URL_TRANSCYC    ">TransCyc</a>\r\n "
  "  <a href=" W_URL_PAGERELOAD  ">Pagereload</a>\r\n "
  "  <a href=" W_URL_SERVER      ">Server</a>\r\n "
  "  <a href=" W_URL_SERVICE     ">Service</a>\r\n "
  "  <a href=" W_URL_HOSTNAME    ">Name</a>\r\n"
  "  <a href=" W_URL_LED         ">LED</a>\r\n"
  "  <a href=" W_URL_DEFAULT     ">set defaults</a>\r\n"
  "  <a href=" W_URL_RESET       ">Reset</a></p>\r\n"
  "  <form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>"};


const char H_RADIO_WIFI_START[] PROGMEM = {
  "<form>\r\n"
  "<p>mit welchem Netzwerk soll eine Verbindung hergestellt werden?</p>\r\n"};

const char H_RADIO_WIFI_LINE[] PROGMEM = {
  "<input type=\"radio\" name=\"SSID\" value=\"{SSID}\">  {CRYPT}{SSID}   ->   (Signal: {RSSI})\r\n"};

const char H_RADIO_WIFI_END[] PROGMEM = {
    "<br><br>\r\n"
    "<input type=\"password\" name=\"pass\"> </label>\r\n"
    "<input type=\"submit\" value=\"senden\">\r\n"
    "</form>\r\n"
  };


const char H_RADIO_LED_START[] PROGMEM = {
    "<form>\r\n"
    "<p> soll die LED ein- oder ausgeschaltet sein?</p>\r\n"
    "<input type=\"radio\" name=\"Led\" value=\"On\" {ontext}>  On\r\n"
    "<input type=\"radio\" name=\"Led\" value=\"Off\" {offtext}>  Off\r\n"};

const char H_RADIO_LED_END[] PROGMEM = {
    "<br><br>\r\n"
    "<input type=\"submit\" value=\"senden\">\r\n"
    "</form>\r\n"
  };

const char H_NAME[] PROGMEM = {
    "<form>\r\n"
    "<p> {nametext}</p>\r\n"
    "<input type=\"text\" name=\"{urltext}\"> </label>\r\n"
    "<input type=\"submit\" value=\"senden\">\r\n"
    "</form>\r\n"
  };

void startWebServer(){
  DBGF("startWebServer()")
  server.on((W_URL_ROOT), [](){
    handleInfo();
    sysData.CntPageDelivered++;
  });

# if (H_SWITCH == H_TRUE)
  server.on(("/" W_URL_ON), [](){
    DBGF("HandleOn()");
    DIG_WRITE( H_LED_PIN, LOW);
    if( DIG_READ( H_RELAY_PIN) == LOW )
      cycles++;
    DIG_WRITE(H_RELAY_PIN, HIGH);
    handleStatus();
    sysData.TransmitCycle = 0; // send status immediately
    delay(1000);
  });
  server.on(("/" W_URL_OFF), [](){
    DBGF("HandleOff()");
    DIG_WRITE( H_LED_PIN, HIGH);
    if( DIG_READ( H_RELAY_PIN) == HIGH )
      cycles++;
    DIG_WRITE( H_RELAY_PIN, LOW);
    handleStatus();
    sysData.TransmitCycle = 0; // send status immediately
    delay(1000);
  });
# endif

  server.on(F("/" W_URL_STATUS), handleStatus);
  server.on(F("/" W_URL_SERVER), handleServer);
  server.on(F("/" W_URL_SERVICE), handleService);
  server.on(F("/" W_URL_SCAN), handleScan);
  server.on(F("/" W_URL_INFO), handleInfo);
  server.on(F("/"), handleInfo);
  server.on(F("/" W_URL_RESET), handleReset);
  server.on(F("/" W_URL_APMODE), handleAPMode);
  server.on(F("/" W_URL_STAMODE), handleSTAMode);
  server.on(F("/" W_URL_HOSTNAME), handleHostName);
  server.on(F("/" W_URL_LED), handleLED);
  server.on(F("/" W_URL_DEFAULT), handleSetDefault);
  server.on(F("/" W_URL_NETWORK), handleNetPage);
  server.on(F("/" W_URL_CONFIG), handleConfPage);
  server.on(F("/" W_URL_SHOWLOG), handleShowLog);
  server.on(F("/" W_URL_SHOWDIR), handleShowDir);
  server.on(F("/" W_URL_PAGERELOAD), handlePagereload);
  server.on(F("/" W_URL_MEASCYC), handleMeasCyc);
  server.on(F("/" W_URL_TRANSCYC), handleTransCyc);
  server.on(F("/favicon.ico"), handleFavicon);
  server.onNotFound( handleNotFound );
  server.on(F("/update"), HTTP_POST, []() {
    DBGF("handleUpdate_part1()");
    server.sendHeader(F("Connection"), "close");
    sysData.CntPageDelivered++;
    server.send(200, F(W_TEXT_PLAIN), (Update.hasError()) ? F("FAIL") : F("OK"));
    ESP.restart();
  }, []() {
    //DBGF("handleUpdate_part2()");
    char buf[80];
    HTTPUpload& upload = server.upload();
    yield();
    if (upload.status == UPLOAD_FILE_START) {
      DBGLN("UPLOAD_FILE_START");
      Serial.setDebugOutput(true);
      WiFiUDP::stopAll();
      Serial.printf(("Update: %s\n"), upload.filename.c_str());
      uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
      DBGL("maxSketchSpace: ");
      sprintf(buf, "Flash size: 0x%x; needed: 0x%x Byte; row needed 0x%x", ESP.getFlashChipSize(), maxSketchSpace, ESP.getFreeSketchSpace());
      DBGLN(buf);
      sprintf(buf, "\r\n act. Sketchsize: 0x%x", ESP.getSketchSize());
      DBGLN(buf);
      sprintf(buf, "\r\n act. Chipsize: 0x%x", ESP.getFlashChipSize());
      DBGLN(buf);
      if (!Update.begin(maxSketchSpace+0x1000)) { //start with max available size
        DBGLN("could not begin");
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      DBG(".");
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        DBGLN("Update.write");
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      DBGLN("\r\nUPLOAD_FILE_END");
      if (Update.end(true)) { //true to set the size to the current progress
        DBGLN ("Update OK");
        Serial.printf("Update Success: %u, 0x%x\nRebooting...\n", upload.totalSize, upload.totalSize);
      } else {
        DBGLN ("Update failed");
        Update.printError(Serial);
      }
      Serial.setDebugOutput(false);
    }
    yield();
  });

  server.begin();
}

/*  all webserver handlers
 */
void handleSetDefault(){
  DBGF(server.uri());
  sysData.CntPageDelivered++;
  server.send(200, F(W_TEXT_HTML), buildConfPage( F("set every thing to defaults")));
  SetToDefault();
//  DEAD
  delay(10000);
  ESP.restart();
}

void handleNetPage(){
  DBGF(server.uri());
  String WebPage = buildNetworkPage(F(""));
  sysData.CntPageDelivered++;
  server.send(200, F(W_TEXT_HTML), WebPage);
}

void handleConfPage(){
  DBGF(server.uri());
  sysData.CntPageDelivered++;
  server.send(200, F(W_TEXT_HTML), buildConfPage(F("")));
}

void handleServer() {
  int x;

  DBGF(server.uri());
  x = checkInput(F("an welchen Server soll gesendet werden?"), F(W_URL_SERVER));
  if( x > 0 ){
    strncpy( cfgData.server, server.arg(server.args()-1).c_str(), x);
    cfgData.server[x] = 0;
    SaveConfig();
  }
}

void handleStatus(){
  String  message;
  
  DBGF(server.uri());

#if (H_DS1820 == H_TRUE)
  message = F("Number of devices: ");
  message += numberOfDevices;
  message += F("\r\n<br>");

  message += F("<table border='1'>\r\n");
  message += F("<tr><td>Device ID</td><td>Temperature</td></tr>\r\n");
  message += buildDS1820Page();
  message += F("</table>\r\n");
  sysData.CntPageDelivered++;
  server.send(200, F(W_TEXT_HTML), buildAppPage(message));
  sysData.CntPageDelivered++;

#elif (H_TOF == H_TRUE)
  message = F("<h1>Distance: </h1>");

  //message += F("\r\n<br>");
  message += buildToFPage();
  server.send(200, F(W_TEXT_HTML), buildAppPage(message));
  sysData.CntPageDelivered++;
#elif (H_SWITCH == H_TRUE)
  message = F("");
  String WebPage;
  char cyc[10];

  message += buildSwitchPage();
  WebPage = buildAppPage(message);
  itoa (cfgData.PageReload, cyc, 10);
  DBGL("Pagereload: ");
  DBGL ( cyc );
  DBGNL(" sec.");
  WebPage += F("<meta http-equiv=\"refresh\" content=\"");
  WebPage += String( cyc );
  WebPage += F("\">");
  sysData.CntPageDelivered++;
  server.send(200, F(W_TEXT_HTML), WebPage );
  sysData.CntPageDelivered++;
# endif
}

void handleFavicon(){
  DBGF(server.uri());
  server.send(200, F(W_TEXT_HTML), "");
}
  
void handleInfo(){
  DBGF(server.uri());
  buildInfoPage();
}

void handleShowLog(){
  DBGF(server.uri());
  String str = "Content of log:<br><br>";
  
  str += logit.show();
  str.replace("\r\n", "<br>");
  server.send(200, F(W_TEXT_HTML), buildMainPage(str));
}

void handleShowDir(){
  DBGF(server.uri());
  String str = "Directory: <br><br>";
  Dir dir = LittleFS.openDir("/");
  while (dir.next()) {
    str += "File: ";
    str += dir.fileName();
    str += " - Size: ";
    str += dir.fileSize();
    str += "<br>";
  }
  if (LittleFS.exists(LOGFILE)) {
    str += "<br>LOGFILE exists";
  }
  else {
    str += "<br>NO LOGFILE";
  }
  server.send(200, F(W_TEXT_HTML), buildMainPage(str));
}

void handleNotFound(){
  DBGF(server.uri());
  String message = F("404: Not Found");
  message += F("\nURI: ");
  message += server.uri();
  message += F("\nMethod: ");
  message += (server.method() == HTTP_GET)?F("GET"):F("POST");
  message += F("\nArguments: ");
  message += server.args();
  message += F("\n");
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + F(": ") + server.arg(i) + F("\n");
  }
  DBGL(message)
  sysData.CntPageDelivered++;
  server.send(404, F(W_TEXT_HTML), message);
}

void handleService(){
  int x;

  DBGF(server.uri());
  x = checkInput(F("wie lautet der Service?"), F(W_URL_SERVICE));
  if( x ){
    strncpy( cfgData.service, server.arg(server.args()-1).c_str(), x);
    cfgData.service[x] = 0;
    SaveConfig();
  }
}

void handleLED(){
  String output;

  DBGF(server.uri());
  output = FPSTR(H_RADIO_LED_START);
  if(cfgData.LED == H_TRUE){
    output.replace(F("{ontext}"), F("checked=\"checked\""));
    output.replace(F("{offtext}"), F(""));
  }
  else{
    output.replace(F("{ontext}"), F(""));
    output.replace(F("{offtext}"), F("checked=\"checked\""));
  }
  output += FPSTR(H_RADIO_LED_END);
  sysData.CntPageDelivered++;
  server.send(200, F(W_TEXT_HTML), buildConfPage (output));
  DBGF("LED ist jetzt: ");
  DBGF(cfgData.LED);
//  DBGF(WebPage)

  DBGF(server.args());
  //x = server.arg(server.args()-1);
  DBGF(server.arg(server.args()-1));
  if(server.args() == 1){
    server.arg(server.args()-1) == "On" ? cfgData.LED = H_TRUE : cfgData.LED = H_FALSE;
    DBGF(cfgData.LED);
    SaveConfig();
    output = F("Yippie");
    sysData.CntPageDelivered++;
    server.send(200, F(W_TEXT_HTML), buildConfPage (output));
  }
}

void handleHostName(){
  int x;

  DBGF(server.uri());
  x = checkInput(F("wie soll das Ding denn heissen?"), F(W_URL_HOSTNAME));
  if( x ){
    strncpy( cfgData.hostname, server.arg(server.args()-1).c_str(), x);
    cfgData.hostname[x] = 0;
    SaveConfig();
    handleReset();
  }
}

void handleMeasCyc(){
  int x;
  String buf;

  DBGF(server.uri());
  x = checkInput(F("wie oft soll gemessen werden?"), F(W_URL_MEASCYC));
  DBGF(x)
  if( x ){
    buf = server.arg(server.args()-1);
    DBGF(buf)
    if (string_isNumber(buf))
    {
      cfgData.MeasuringCycle = atol(buf.c_str());
      SaveConfig();
      sysData.MeasuringCycle = cfgData.MeasuringCycle;
    }
    else DBGF("is not a number")
  }
}

void handleTransCyc(){
  int x;
  String buf;
  
  DBGF(server.uri());
  x = checkInput(F("wie oft soll zum Server gesendet werden?"), F(W_URL_TRANSCYC));
  if( x ){
    buf = server.arg(server.args()-1);
    if (string_isNumber(buf))
    {
      cfgData.TransmitCycle = atol(buf.c_str());
      SaveConfig();
      sysData.TransmitCycle = cfgData.TransmitCycle;
      if(!sysData.TransmitCycle) LEDControl(BLKMODEFLASH, BLKFLASHOFF);
    }
  }
}

void handlePagereload(){
  int x;
  String  buf;

  DBGF(server.uri());
  x = checkInput("wie oft soll die Seite neu geladen werden?", W_URL_PAGERELOAD);
  if( x ){
    buf = server.arg(server.args()-1);
//FOL    buf[x]=0;
    if (string_isNumber(buf))
    {
      cfgData.PageReload = atol(buf.c_str());
      SaveConfig();
    }
  }
}

void handleSTAMode(){
  DBGF(server.uri());
  sysData.CntPageDelivered++;
  server.send(200, F(W_TEXT_HTML), buildNetworkPage( F("switching to STA-Mode")));
  delay(1000);
  sysData.mode = MODE_CHG_TO_STA;
}

void handleAPMode(){
  DBGF(server.uri());
  String message= F("look me up under 192.168.4.1 (my name is: ");
  message += (String)cfgData.APname;
  message += F(")");
  buildNetworkPage(message);
  sysData.CntPageDelivered++;
  server.send(200, F(W_TEXT_HTML), buildNetworkPage(message));
  delay(1000);
  sysData.mode = MODE_CHG_TO_AP;
}

void handleReset(){
  DBGF(server.uri());
  sysData.CntPageDelivered++;
  server.send(200, F(W_TEXT_HTML), buildConfPage (F("restart in a few seconds<br> please reload page")));
  delay(1000);
  ESP.restart();
//  DEAD
//  delay(10000);

}

void handleScan(){
  String message;
  int x;

  DBGF(server.uri());

  if( server.args() == 0 ){
    ScanStart();
  }
  else
    if( server.args() == 2 ){
      ScanEnd();
// string laenge von SSID und password ueberpruefen
      x = strlen(server.arg(server.args()-2).c_str());
      strncpy( cfgData.SSID, server.arg(server.args()-2).c_str(), x);
      cfgData.SSID[x] = 0;
      x = strlen(server.arg(server.args()-1).c_str());
      strncpy( cfgData.password, server.arg(server.args()-1).c_str(), x);
      cfgData.password[x] = 0;
      SaveConfig();
      handleSTAMode();
      startWebServer();
    }
    else {
      ScanStart();
    }
}

void ScanEnd(){
  DBGF("ScanEnd()")
  // Fehlerhandling wenn parameter falsch und handling wenn alles ok
  sysData.CntPageDelivered++;
  server.send(200, F(W_TEXT_HTML), buildNetworkPage(F("")));
}

void ScanStart(){
  DBGF("ScanStart()")
  //printUrlArg();
  String output = F("");
  String  sbuf;

  delay(100);
  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  if (n == 0)
    output += F("no networks found");
  else
  {
    DBGL(n);
    DBG((" networks found"));
    Serial.println("");
    output += FPSTR(H_RADIO_WIFI_START);

    for (int i = 0; i < n; ++i)
    {
      // Print SSID and RSSI for each network found
      output += F("<br>");
      DBGLN(i + 1);
      DBG(": ");
      DBG((WiFi.encryptionType(i) == ENC_TYPE_NONE)?"  ":"* ");
      DBG(WiFi.SSID(i));
      output += FPSTR(H_RADIO_WIFI_LINE);
      if( WiFi.encryptionType(i) == ENC_TYPE_NONE )
        output.replace( F("{CRYPT}"),F(" - "));
      else
        output.replace( F("{CRYPT}"),F(" * "));
      output.replace(F("{SSID}"), WiFi.SSID(i));
      sbuf = String(WiFi.RSSI(i));
      output.replace(F("{RSSI}"), (String)(sbuf));
      DBG(" (");
      DBG(WiFi.RSSI(i));
      DBG(")");
      Serial.println("");
      delay(10);
    }
    output += F("<h6>");
    output += FPSTR(H_RADIO_WIFI_END);
    output += F("</h6>");
  }
  sysData.CntPageDelivered++;
  server.send(200, F(W_TEXT_HTML), buildNetworkPage(output));
}

/*  all webpage building routines
 *
 */
 String buildPageFrame(String content) {
   // building the frame for all Pages
   // setup the pages with the content thats the same for all pages
   DBGF("buildPageFrame(String content)")
   String WebPage = FPSTR(H_HOME_PAGE);
   WebPage.replace(F("{hostname}"), (String)cfgData.hostname);
   WebPage.replace(F("{title}"), (String)cfgData.hostname);
   WebPage.replace(F("{content}"), content);
   return WebPage;
 }

String buildNetworkPage(String content){
  DBGF("buildNetworkPage(String content)")
  String WebPage = buildPageFrame(content);
  WebPage.replace(F("{appmenu}"), F(""));
  WebPage.replace(F("{mainpage}"), F(""));
  WebPage.replace(F("{networkpage}"), FPSTR(H_NETWORKMENU));
  WebPage.replace(F("{confpage}"), F(""));
  sysData.APTimeout = cfgData.APTimeout;
  return WebPage;
}

String buildConfPage(String content){
  DBGF("buildConfPage(String content)")
  String WebPage = buildPageFrame(content);
  WebPage.replace(F("{appmenu}"), F(""));
  WebPage.replace(F("{mainpage}"), F(""));
  WebPage.replace(F("{networkpage}"), F(""));
  WebPage.replace(F("{confpage}"), FPSTR(H_CONFMENU));
  sysData.APTimeout = cfgData.APTimeout;
  return WebPage;
}

String buildMainPage(String content){
  DBGF("buildMainPage(String content)")
  String  WebPage = buildPageFrame(content);
  WebPage.replace(F("{appmenu}"), FPSTR(H_APPMENU));
  WebPage.replace(F("{appmenu}"), F(""));
  WebPage.replace(F("{mainpage}"), FPSTR(H_INFOMENU));
  WebPage.replace(F("{networkpage}"), F(""));
  WebPage.replace(F("{confpage}"), F(""));
  sysData.APTimeout = cfgData.APTimeout;
  return WebPage;
}

String buildAppPage(String content){
  DBGF("buildAppPage(String str)")
  String WebPage = buildPageFrame(content);
  WebPage.replace(F("{appmenu}"), FPSTR(H_APPMENU));
  WebPage.replace(F("{appmenu}"), FPSTR(H_STATUSMENU));
  WebPage.replace(F("{mainpage}"), F(""));
  WebPage.replace(F("{networkpage}"), F(""));
  WebPage.replace(F("{confpage}"), F(""));
  sysData.APTimeout = cfgData.APTimeout;
  return WebPage;
}

String buildURL (){
  String url;

  DBGF("buildURL()");
  url = String(F("?Host=")) + (String)cfgData.hostname + String(F("_")) + (String)cfgData.MACAddress;
  url.replace (F(":"), F("_"));
  url += String(F("&IP=")) + WiFi.localIP().toString();
  url += String(F("&Type=")) + String(F(FNC_TYPE));
  url += String(F("&Version=")) + String(F(VERNR));
  url += String(F("&Hardw=")) + String(F(DEV_TYPE));
  url += String(F("&Network=")) + String(cfgData.SSID);
  url += String(F("&MAC=")) + String(cfgData.MACAddress);
  url += String(F("&APName=")) + String(cfgData.APname);
  url += String(F("&Hash=0x")) + String(cfgData.hash, HEX);
  url += String(F("&Size=0x")) + String(sizeof(cfgData));
  url += String(F("&TransCyc=")) + String(cfgData.TransmitCycle);
  url += String(F("&MeasCyc=")) + String(cfgData.MeasuringCycle);
  url += String(F("&PageReload=")) + String(cfgData.PageReload);
  url += String(F("&Service=")) + String(cfgData.service);
  url += String(F("&ServerIP=")) + String(cfgData.server);
  url += String(F("&uptime=")) + String(uptime);
  url += String(F("&delivPages=")) + String(sysData.CntPageDelivered);
  url += String(F("&goodTrans=")) + String(sysData.CntGoodTrans);
  url += String(F("&badTrans=")) + String(sysData.CntBadTrans);
  url += String(F("&LED=")) + String(cfgData.LED);

#if (H_DS1820 == H_TRUE)
  return url + buildDS1820URL();
#elif (H_SWITCH == H_TRUE)
  return url + buildSwitchURL();
#elif (H_TOF == H_TRUE)
  return url + buildToFURL();
#else
#error "keine Hardwarefunktion definiert"
#endif
}

#if (H_DS1820 == H_TRUE)
String buildDS1820URL (){
  char buf[10];
  String url;

  DBGF("buildDS1820URL()");
  for(int i=0;i<numberOfDevices;i++){
    url += ("&Adress_") + String(i) + F("=");
    url += GetAddressToString( devAddr[i] );
    url += ("&Value_") + String(i) + F("=");
    dtostrf(tempDev[i], 2, 2, buf);
    url += buf;
  }
  return url;
}

String buildDS1820Page(){
  String message="";
  char temperatureString[10];

  DBGF("buildDS1820Page()");
  for(int i=0; i<numberOfDevices; i++){
    dtostrf(tempDev[i], 2, 2, temperatureString);
    DBGL( "Sending temperature: " );
    DBGLN( temperatureString );

    message += F("<tr><td>");
    message += GetAddressToString( devAddr[i] );
    message += F("</td>\r\n");
    message += F("<td>");
    message += temperatureString;
    message += F("</td></tr>\r\n");
    message += F("\r\n");
  }
  return message;
}

#elif (H_TOF == H_TRUE)
String buildToFURL (){
  String url = "";

  DBGF("buildToFURL()");
  url += "&distance=" + String(ToFRange);
  return url;
}

String buildToFPage(){
  String message="";

  DBGF("buildToFPage()");
  message = F("<meta http-equiv=\"refresh\" content=\"1\">"); //"; URL=http://192.168.1.38/tof>");
  message += "<h1 style=\"font-size:128px\">"+String(ToFRange)+"</h1>";
  return message;
}

#elif (H_SWITCH == H_TRUE)
String buildSwitchURL(){
  String url;

  DBGF("buildSwitchURL()");
  url += F("&ontime=") + String(ontime);
  url += F("&offtime=") + String(offtime);
  url += F("&cycles=") + String(cycles);
  url += F("&status=");
  if (DIG_READ(H_RELAY_PIN))
    url += F("AN");
  else
    url += F("AUS");

  return url;
}

String buildSwitchPage(){
  String message;
  char cyc[10];

  DBGF("buildSwitchHTML()");

  message = F("");
  message += F("<h3>Schalter ist: ");
  if (DIG_READ(H_RELAY_PIN))
    message += F("AN</h3>");
  else
    message += F("AUS</h3>");

  itoa (cfgData.PageReload, cyc, 10);
  DBGL("Pagereload: ");
  DBGL ( cyc );
  DBGNL(" sec.");
  message += F("<meta http-equiv=\"refresh\" content=\"");
  message += String( cyc );
  message += F(";URL=192.168.1.4/switch");
  message += F("\">");
  return message;
}

#endif

void buildInfoPage(){
  String  output;
  
  DBGF("buildInfoPage()")

  output = F("</h3>");
  output += Version + F("\r\n\r\n<br><br>")+ F("\r\n<br>Type: ") + F(FNC_TYPE) + F("\r\n<br>Hardw: ") + F(DEV_TYPE);
  output += F("\r\n<br>MAC-Address: ") + (String)cfgData.MACAddress;
  output += F("\r\n<br>Network: ") + (String)cfgData.SSID;
  output += F("\r\n<br>Network-IP: ") + WiFi.localIP().toString();
  output += F("\r\n<br>Devicename: ") + (String)cfgData.hostname;
  output += F("\r\n<br>AP-Name: ") + (String)cfgData.APname;
  output += F("\r\n<br>Hash: 0x") + String(cfgData.hash, HEX);
  output += F("\r\n<br>cfg-Size: 0x") + String(sizeof(cfgData), HEX);
  output += F("\r\n<br>");

  output += F("\r\n<br>uptime: ");
  output += String(uptime/86400) + F(" days - ") + String((uptime/3600)%24) + F(" hours - ") + String((uptime/60)%60) + F(" minutes - ") + String(uptime%60) + F(" seconds");
#if (H_DS1820 == H_TRUE) || (H_TOF == H_TRUE)
  output += F("\r\n<br>Measuring cycle: ") + String(cfgData.MeasuringCycle) + F(" s (remainig: ") + String(sysData.MeasuringCycle) + F(" s)");
#endif
  output += F("\r\n<br>Transmit cycle: ") + String(cfgData.TransmitCycle) + F(" s (remaining: ") + String(sysData.TransmitCycle) + F(" s)");
  output += F("\r\n<br>PageReload cycle: ") + String(cfgData.PageReload) + F(" s");
  output += F("\r\n<br>Service: ")+ (String)cfgData.service;
  output += F("\r\n<br>Server: ")+ (String)cfgData.server;
  output += F("\r\n<br>LED: ");
  cfgData.LED == H_TRUE ? output += F("on ") : output += F("off ");
  output += F("\r\n\r\n<br><br>good Transmissions: ") + String(sysData.CntGoodTrans);
  output += F("\r\n<br>bad Transmissions: ") + String(sysData.CntBadTrans);
  output += F("\r\n<br>Pages delivered: ") + String(sysData.CntPageDelivered);
#if (H_DS1820 == H_TRUE) || (H_TOF == H_TRUE)
  output += F("\r\n<br>Measurements: ") + String(sysData.CntMeasCyc);
#endif

#if (H_RELAY == H_TRUE)
  output += F("\r\n<br>ontime: ");
  output += String(ontime/86400) + F(" days - ") + String((ontime/3600)%24) + F(" hours - ") + String((ontime/60)%60) + F(" minutes - ") + String(ontime%60) + F(" seconds");
  output += F("\r\n<br>offtime: ");
  output += String(offtime/86400) + F(" days - ") + String((offtime/3600)%24) + F(" hours - ") + String((offtime/60)%60) + F(" minutes - ") + String(offtime%60) + F(" seconds");
  output += F("\r\n<br>Cycles:                 ") + String(cycles);
#endif
  output += F("</h6>");
  sysData.CntPageDelivered++;
  server.send(200, F(W_TEXT_HTML), buildMainPage(output));
}

/*  ---------------------------------------------------------------------------------------------------------

    some tool routines

    --------------------------------------------------------------------------------------------------------*/
// !!! is urltext really needed ???
int checkInput(String text, String URLText){
  String message="";
  String output;
  int x;

  DBGF("checkInput()")

  output = FPSTR(H_NAME);
  output.replace(F("{nametext}"), text);
  output.replace(F("{urltext}"), URLText);
  DBGF(server.args())

  if( server.args() == 0 ){
    DBGF("server.args=0")
    sysData.CntPageDelivered++;
    server.send(200, F(W_TEXT_HTML), buildConfPage (output));
    return 0;
  }
  else
    if( server.arg(server.args()-1).length() > 0){
      DBGF("message is:")
      sysData.CntPageDelivered++;
      server.send(200, F(W_TEXT_HTML), buildConfPage(F("")));
      x = server.arg(server.args()-1).substring(0, 30).length();
      return x < MAXCHAR ? x : MAXCHAR;  // cut everything whats longer than MAXCHAR
    }
    else {
      DBGF("server.args=leer")

      message = F("<p><font color=\"red\">keine Angabe ist keine gute Idee !!!</font></p>");
      message += output;
      sysData.CntPageDelivered++;
      server.send(200, F(W_TEXT_HTML), buildConfPage(message));
      return 0;
    }
  return 0;
}

//FOL was ist der Unterschied zwischen _Bool und bool?
// gibt es eine Funktion fü Strings
bool string_isNumber(String str) {
  char  buf[80];
  char *ptr;

  ptr = buf;
  str.toCharArray(buf, 80);
  while(*ptr) {
    if(!isdigit(*ptr)) {
      return false;
    }
    ptr++;
  }
  return H_TRUE;
}

/*
  history:
  --------------------- V2.1c
  26.05.22  LED Radio-buttons with defaults ("checked=\"checked\"")
  --------------------- V2.0a
  21.11.21  lot of changes for 2.xx, made for arduino IDE and 
            many changes to improve stability, performance etc.
            all Webpages build with String / no longer use of sprintf
  --------------------- V1.4
  19.01.21  ToF pages added
  --------------------- V1.4

  07.05.20  V1.30b: Switches sending uptime twice, removed
  06.05.20  V1.30a: new WiFi-state machine, sending a message after boot
  20.04.20  V1.30 from now we use platformio instead of the Arduiono stuff
            a lot of pointer castings changed
            string from checkinput is terminated with 0
            URL contains more information
  11.07.19  V1.24: switches dont send cyclic
  08.07.19  V1.23: some changes on the status-site
  05.03.19  V1.03: config or DS1820 added (Meascyx, Transcyc and pagereload)
            count the messages to server, the pagereloads (good and bad trys)
  03.03.19  V1.02: many changes on Webdesign and website structure
  02.03.19  V1.01 Website optimization (code and design)
            Server- and Servicename input added
  23.02.19  first Version wirh new Version management. Released for Sonoff S20 and Sonoff Basic
  19.02.19  V0.06 OTA implemented
            some little changes in Website source code and design
  19.01.19  V0.04 first version to re released
            debuging and error-check for all versions is to be done

  29.12.18  set deafults and some tiny stuff

  02.10.18  version 1.17
            device type added
  31.05.18  version 1.15
            cycles added
  17.05.18  version 1.13
            lifetimer added

  30.03.18  Version1.12
            scan displays RSSI and Crypt value in scan mode
            in status chip-id, mac and ip added
  24.03.18  version 1.11
            all PROGMEM-HTML strings changed for better readability in source code and on
            receivers

  03.01.18  version 1.10
            improvments website
            Name and network changing works
  02.03.18  Version 1.09
            network config added to webpage
  01.03.18  version 1.08
            new Webpage
  29.02.18  version 1.07
            improvements (Radio buttons for Scan, hostname can be changed etc.
  27.02.18  Version 1.06
            radio buttons testing
            STA-MODE-button added
            #defines for text added
  23.02.18  version 1.05
            Webserver scan Wifi

 todos:
  22.02.19  v0.07 crashes during scan with Advanced IP Scanner (or takes a long time until its working again)
            mesurement and transmit cycles for i.e. DS1820
            eraseconfig
  04.03.19  counting messages during uptime/lifetime good and bad ones and displaying them on Status-website

*/

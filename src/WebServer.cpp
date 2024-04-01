/*
  WebServer.ino

  HTTP-Version 
  !!! for history see settings.h !!!

  ToDos:    ???

  hints:    ???
*/

#include <Arduino.h>
#include "settings.h"
#include "Config.h"

#include "WebServer.h"
#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include <string.h>
#include "WiFi.h"
//##include <iostream>
//#include <WiFi.h>

#include "timer.h"
#include "log.h"
#include "ToF.h"
#include "html.h"
#include "DS1820.h"
//##include <ESP8266HTTPUpdateServer.h>

//FOL extern const String  Version;
//const String  Version = "hier muss ich noch was tun!!!";
extern String Version;

// -----------------------------------------
// Server stuff
//------------------------------------------
//HTTP

 
extern class log_CL logit;

ESP8266WebServer server(MyServerPort);

//#ESP8266HTTPUpdateServer httpUpdater;

String HomeHTML;
String AppMenueHTML;
String StatusMenueSwitchHTML;
String InfoMenueHTML;
String ConfMenueHTML;
String ConfMenueMeasHTML;
String RadioWifiStartHTML;
String RadioWifiLinetHTML;
String RadioWifiEndHTML;
String RadioLEDStartHTML;
String RadioLEDEndHTML;
String NameHTML;
String q = "\"";
String qd = "\":\"";
String qc = "\",\"";

void initHTML(){
  HomeHTML = FPSTR(H_HOME_PAGE);
  AppMenueHTML = FPSTR(H_APPMENU);
  StatusMenueSwitchHTML = FPSTR(H_STATUSMENU);
  InfoMenueHTML = FPSTR(H_INFOMENU);
  ConfMenueHTML = FPSTR(H_CONFMENU);
  ConfMenueMeasHTML = FPSTR(H_CONFMENUMEAS);
  RadioWifiStartHTML = FPSTR(H_RADIO_WIFI_START);
  RadioWifiLinetHTML = FPSTR(H_RADIO_WIFI_LINE);
  RadioWifiEndHTML = FPSTR(H_RADIO_WIFI_END);
  RadioLEDStartHTML = FPSTR(H_RADIO_LED_START);
  RadioLEDEndHTML = FPSTR(H_RADIO_LED_END);
  NameHTML = FPSTR(H_NAME);
}

String readHTML(String fname){
  File fhtml = LittleFS.open(fname, "r");
  String fstr="";
  while(fhtml.available()){
    fstr += (char)(fhtml.read());
  }
  fhtml.close();
  return fstr;
}

void startWebServer(){
  initHTML();
  DBGF("startWebServer()")
  server.on(("/"), [](){
    handleInfo();
  });

# if (H_SWITCH == H_TRUE)
  server.on(("/" "on"), [](){
    DBGF("HandleOn()");
    DIG_WRITE( H_LED_PIN, LOW);
    if( DIG_READ( H_RELAY_PIN) == LOW )
      sysData.cycles++;
    DIG_WRITE(H_RELAY_PIN, HIGH);
    handleStatus();
    sysData.TransmitCycle = 0; // send status immediately
    delay(1000);
  });
  server.on(("/" "off"), [](){
    DBGF("HandleOff()");
    DIG_WRITE( H_LED_PIN, HIGH);
    if( DIG_READ( H_RELAY_PIN) == HIGH )
      sysData.cycles++;
    DIG_WRITE( H_RELAY_PIN, LOW);
    handleStatus();
    sysData.TransmitCycle = 0; // send status immediately
    delay(1000);
  });
# endif

  server.on("/status", handleStatus);
  server.on("/server", handleServer);
  server.on("/port", handlePort);
  server.on("/info", handleInfo);
  server.on("/", handleInfo);
  server.on("/reset", handleReset);
  server.on("/hostname", handleHostName);
  server.on("/led", handleLED);
  server.on("/default", handleSetDefault);
  server.on("/config", handleConfPage);
  server.on("/showlog", handleShowLog);
  server.on("/showdir", handleShowDir);
  server.on("/pagereload", handlePagereload);
  server.on("/meascyc", handleMeasCyc);
  server.on("/transcyc", handleTransCyc);
  server.on("/scan", handleScan);
  server.on("/favicon.ico", handleFavicon);
  server.onNotFound( handleNotFound );
    server.on("/update", HTTP_POST, handleUpdate_part1, handleUpdate_part2);
  server.begin();
  DBGF("startWebServer() done.")
}

void handleUpdate_part1() {
    DBGF("handleUpdate_part1()");
    server.sendHeader("Connection", "close");
    sysData.CntPageDelivered++;
    server.send(200, W_TEXT_PLAIN, (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
}

void handleUpdate_part2() {
    //DBGF("handleUpdate_part2()");
    char buf[80];
    HTTPUpload& upload = server.upload();
    yield();
    if (upload.status == UPLOAD_FILE_START) {
      DBGLN("UPLOAD_FILE_START");
      Serial.setDebugOutput(true);
      //WiFiUDP::stopAll();
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
}

void handleUpdate(){
    DBGF("handleUpdate");
    Serial.println("jetzt mach ich ein Update!!!");
    
//    httpUpdater.setup(&server);
//    httpServer.begin();
    Serial.println("habe fertig!!!");
    server.send(404, W_TEXT_HTML, "habe fertich");
}
/*  all webserver handlers
 */
void handleSetDefault(){
  DBGF(server.uri());
  server.send(200, W_TEXT_HTML, buildConfPage("set every thing to defaults"));
  sysData.CntPageDelivered++;
  SetToDefault();
//  DEAD
  delay(10000);
  ESP.restart();
}

void handleConfPage(){
  DBGF(server.uri());
  server.send(200, W_TEXT_HTML, buildConfPage(""));
  sysData.CntPageDelivered++;
}

void handleServer() {
  int x;

  DBGF(server.uri());
  x = checkInput("an welchen Server soll gesendet werden?", "server");
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
  message = "Number of devices: ";
  message += numberOfDevices;
  message += "<br>";

  message += "<table border='1'>";
  message += "<tr><td>Device ID</td><td>Temperature</td></tr>";
  message += buildDS1820Page();
  message += "</table>";
  server.send(200, W_TEXT_HTML, buildAppPage(message));
  sysData.CntPageDelivered++;
  
#elif (H_TOF == H_TRUE)
  message = "<h1>Distance: </h1>";

  //message += "<br>";
  message += buildToFPage();
  server.send(200, W_TEXT_HTML, buildAppPage(message));
  sysData.CntPageDelivered++;
#elif (H_SWITCH == H_TRUE)
  message = "";
  String WebPage;
  char cyc[10];

  message += buildSwitchPage();
  WebPage = buildAppPage(message);
  itoa (cfgData.PageReload, cyc, 10);
  DBGL("Pagereload: ");
  DBGL ( cyc );
  DBGNL(" sec.");
  WebPage += "<meta http-equiv=\"refresh\" content=\"";
  WebPage += String( cyc );
  WebPage += "\">";
  server.send(200, W_TEXT_HTML, WebPage );
  sysData.CntPageDelivered++;
# endif
}

void handleFavicon(){
  DBGF(server.uri());
  server.send(200, W_TEXT_HTML, "");
}
  
void handleInfo(){
  DBGF(server.uri());
  buildInfoPage();
}

void handleShowLog(){
  DBGF(server.uri());
  String str = "Content of log:<br><br>";
  
  str += logit.show();
  str.replace("\n", "<br>");
  //Serial.println(str);
  server.send(200, W_TEXT_HTML, buildMainPage(str));
  sysData.CntPageDelivered++;
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
  server.send(200, W_TEXT_HTML, buildMainPage(str));
  sysData.CntPageDelivered++;
}

void handleNotFound(){
  DBGF(server.uri());
  String message = "404: Not Found";
  message += "\nURI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  DBGL(message)
  server.send(404, W_TEXT_HTML, message);
  sysData.CntPageDelivered++;
};

void handlePort(){
  int x;

  DBGF(server.uri());
  x = checkInput("wie lautet der Port?", "port");
  if( x ){
    strncpy( cfgData.port, server.arg(server.args()-1).c_str(), x);
    cfgData.port[x] = 0;
    SaveConfig();
  }
}

void handleLED(){
  String output;

  DBGF(server.uri());
  output = RadioLEDStartHTML;
  if(cfgData.LED == H_TRUE){
    output.replace("{ontext}", "checked=\"checked\"");
    output.replace("{offtext}", "");
  }
  else{
    output.replace("{ontext}", "");
    output.replace("{offtext}", "checked=\"checked\"");
  }
  output += RadioLEDEndHTML;
  server.send(200, W_TEXT_HTML, buildConfPage (output));
  sysData.CntPageDelivered++;
  DBGF("LED ist jetzt: ");
  DBGF(cfgData.LED);
//  DBGF(WebPage)

  DBGF(server.args());
  //x = server.arg(server.args()-1);
  DBGF(server.arg(server.args()-1));
  if(server.args() == 1){
    server.arg(server.args()-1) == "On" ? cfgData.LED = H_TRUE : cfgData.LED = H_FALSE;
    DBGF("so ist die LED jetzt")
    DBGF(cfgData.LED);
    SaveConfig();
    output = "Yippie";
    server.send(200, W_TEXT_HTML, buildConfPage (""));
    sysData.CntPageDelivered++;
  }
}

void handleHostName(){
  int x;

  DBGF(server.uri());
  x = checkInput("wie soll das Ding denn heissen?", "hostname");
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
  x = checkInput("wie oft soll gemessen werden?", "meascyc");
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
    else Serial.println("is not a number");
  }
}

void handleTransCyc(){
  int x;
  String buf;
  
  DBGF(server.uri());
  x = checkInput("wie oft soll zum Server gesendet werden?", "transcyc");
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
  x = checkInput("wie oft soll die Seite neu geladen werden?", "pagereload");
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

void handleReset(){
  DBGF(server.uri());
  server.send(200, W_TEXT_HTML, buildConfPage ("restart in a few seconds<br> please reload page"));
  sysData.CntPageDelivered++;
  delay(1000);
  ESP.restart();
//  DEAD
//  delay(10000);

}

void handleScan(){
  String message;
  int x;

  DBGF("handleScan()")

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
      //handleSTAMode();
      //WebServerStart();
    }
    else {
      ScanStart();
    }
}

void ScanEnd(){
  DBGF("ScanEnd()")
  
  // Fehlerhandling wenn parameter falsch und handling wenn alles ok
  server.send(200, W_TEXT_HTML, buildConfPage(""));
  sysData.CntPageDelivered++;
}

void ScanStart(){
  DBGF("ScanStart()")
  //printUrlArg();
  String output = "";
  char buf[80];

  delay(100);
  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  if (n == 0)
    output += "no networks found";
  else
  {
    DBGL(n);
    DBG((" networks found"));
    Serial.println("");
    output += H_RADIO_WIFI_START;

    for (int i = 0; i < n; ++i)
    {
      // Print SSID and RSSI for each network found
      output += "<br>";
      DBGLN(i + 1);
      DBG(": ");
      DBG((WiFi.encryptionType(i) == ENC_TYPE_NONE)?"  ":"* ");
      DBG(WiFi.SSID(i));
      output += H_RADIO_WIFI_LINE;
      if( WiFi.encryptionType(i) == ENC_TYPE_NONE )
        output.replace("{CRYPT}", " - ");
      else
        output.replace("{CRYPT}", " * ");
      output.replace("{SSID}", WiFi.SSID(i));
      ltoa(WiFi.RSSI(i), buf, 10);
      output.replace("{RSSI}", (String)(buf));
      DBG(" (");
      DBG(WiFi.RSSI(i));
      DBG(")");
      Serial.println("");
      delay(10);
    }
    output += "<h6>";
    output += H_RADIO_WIFI_END;
    output += "</h6>";
  }
  server.send(200, W_TEXT_HTML, buildConfPage(output));
  sysData.CntPageDelivered++;
}

/*  all webpage building routines
 *
 */
 String buildPageFrame(String content) {
   // building the frame for all Pages
   // setup the pages with the content thats the same for all pages
   DBGF("buildPageFrame(String content)")

   String WebPage = HomeHTML;
   WebPage.replace("{hostname}", String(cfgData.hostname));
   //WebPage.replace("{hostname}", content);
   WebPage.replace("{title}", String(cfgData.hostname));
   WebPage.replace("{content}", content);
   return WebPage;
 }

String buildConfPage(String content){
  DBGF("buildConfPage(String content)")
  String WebPage = buildPageFrame(content);
  WebPage.replace("{appmenu}", "");
  WebPage.replace("{mainpage}", "");
  #if (H_DS1820 == H_TRUE)||(H_TOF == H_TRUE)
  WebPage.replace("{confpage}", ConfMenueMeasHTML);
  #else
  WebPage.replace("{confpage}", ConfMenueHTML);
  #endif
  return WebPage;
}

String buildMainPage(String content){
  DBGF("buildMainPage(String content)")
  String  WebPage = buildPageFrame(content);
  WebPage.replace("{appmenu}", AppMenueHTML);
  WebPage.replace("{appmenu}", "");
  WebPage.replace("{mainpage}", InfoMenueHTML);
  WebPage.replace("{confpage}", "");
  WebPage.replace("{content}", content);
  return WebPage;
}

String buildAppPage(String content){
  DBGF("buildAppPage(String str)")
  String WebPage = buildPageFrame(content);
  WebPage.replace("{appmenu}", AppMenueHTML);
  #if(H_SWITCH == H_TRUE)
  WebPage.replace("{appmenu}", StatusMenueSwitchHTML);
  #else
  WebPage.replace("{appmenu}", "");
  #endif
  WebPage.replace("{mainpage}", "");
  WebPage.replace("{confpage}", "");
  return WebPage;
}

String buildDict (){  // 21KB
  DBGF("buildDict()");
  String httpRequestData = "{" + q;
  String FullName=String(cfgData.hostname) + "_" + String(cfgData.MACAddress);
  FullName.replace (":", "_");
  httpRequestData += "name" + qd + FullName + qc;
  httpRequestData += "IP" + qd + WiFi.localIP().toString() + qc;
  httpRequestData += "Version" + qd + VERNR + qc;
  httpRequestData += "Hardware" + qd + DEV_TYPE + qc;
  httpRequestData += "Network" + qd + String(WiFi.SSID()) + qc;
  httpRequestData += "APName" + qd + String(cfgData.APname) + qc;
  httpRequestData += "MAC" + qd + String(cfgData.MACAddress) + qc;
  httpRequestData += "TransmitCycle" + qd + String(cfgData.TransmitCycle) + qc;
  httpRequestData += "MeasuringCycle" + qd + String(cfgData.MeasuringCycle) + qc;
  httpRequestData += "Hash" + qd + String(cfgData.hash, HEX) + qc;
  httpRequestData += "Size" + qd + String(sizeof(cfgData)) + qc;
  httpRequestData += "TransmitCycle" + qd + String(cfgData.TransmitCycle) + qc;
  httpRequestData += "MeasuringCycle" + qd + String(cfgData.MeasuringCycle) + qc;
  httpRequestData += "PageReload" + qd + String(cfgData.PageReload) + qc;
  httpRequestData += "Server" + qd + String(cfgData.server) + qc;
  httpRequestData += "Port" + qd + String(cfgData.port) + qc;
  httpRequestData += "uptime" + qd + String(sysData.uptime) + qc;
  httpRequestData += "delivPages" + qd + String(sysData.CntPageDelivered) + qc;
  httpRequestData += "goodTrans" + qd + String(sysData.CntGoodTrans) + qc;
  httpRequestData += "badTrans" + qd + String(sysData.CntBadTrans) + qc;
  httpRequestData += "LED" + qd + String(cfgData.LED) + qc;
  httpRequestData += "WiFi" + qd + String(WiFi.RSSI()) + qc;
#if (H_DS1820 == H_TRUE)
  httpRequestData += buildDS1820Dict();
#elif (H_SWITCH == H_TRUE)
  httpRequestData += buildSwitchDict();
#elif (H_TOF == H_TRUE)
  httpRequestData += buildToFDict();
#else
#error "keine Hardwarefunktion definiert"
#endif
 return httpRequestData + "}";
}

#if (H_DS1820 == H_TRUE)
String buildDS1820Dict (){
  char buf[10];
  String url;
  String httpRequestData = "Type" + qd + FNC_TYPE + "-" + String(numberOfDevices) + qc;
  DBGF("buildDS1820Dict()");
  for(int i=0;i<numberOfDevices;i++){
    httpRequestData += "Adress_" + String(i) + qd + GetAddressToString( devAddr[i]) + qc;
    dtostrf(tempDev[i], 2, 2, buf);
    httpRequestData += "Value_" + String(i) + qd + buf + qc;
  }
  return httpRequestData.substring(0, httpRequestData.length() - 2);
}

String buildDS1820Page(){
  String message="";
  char temperatureString[10];

  DBGF("buildDS1820Page()");
  for(int i=0; i<numberOfDevices; i++){
    dtostrf(tempDev[i], 2, 2, temperatureString);
    DBGL( "Sending temperature: " );
    DBGLN( temperatureString );

    message += "<tr><td>";
    message += GetAddressToString( devAddr[i] );
    message += "</td>";
    message += "<td>";
    message += temperatureString;
    message += "</td></tr>";
    message += "";
  }
  return message;
}

#elif (H_TOF == H_TRUE)
String buildToFDict (){
  DBGF("buildToFDict()");
  String httpRequestData = "Type" + qd + FNC_TYPE + qc + "distance" + qd + String(ToFRange) + q;
  return httpRequestData;
}

String buildToFPage(){
  String message="";

  DBGF("buildToFPage()");
  message = "<meta http-equiv=\"refresh\" content=\"1\">"; //"; URL=http://192.168.1.38/tof>");
  message += "<h1 style=\"font-size:128px\">"+String(ToFRange)+"</h1>";
  return message;
}

#elif (H_SWITCH == H_TRUE)
String buildSwitchDict(){
  DBGF("buildSwitchDict()");
  String httpRequestData = "Type" + qd + FNC_TYPE + qc;
  httpRequestData += "ontime" + qd + String(sysData.ontime) + qc;
  httpRequestData += "offtime" + qd + String(sysData.offtime) + qc;
  httpRequestData += "cycles" + qd + String(sysData.cycles) + qc;
  httpRequestData += "status" + qd;
  if (DIG_READ(H_RELAY_PIN))
    httpRequestData += "AN";
  else
    httpRequestData += "AUS";
  httpRequestData += q;
  return httpRequestData;
}

String buildSwitchPage(){
  String message;
  char cyc[10];

  DBGF("buildSwitchHTML()");

  message = "";
  message += "<h3>Schalter ist: ";
  if (DIG_READ(H_RELAY_PIN))
    message += "AN</h3>";
  else
    message += "AUS</h3>";

  itoa (cfgData.PageReload, cyc, 10);
  DBGL("Pagereload: ");
  DBGL ( cyc );
  DBGNL(" sec.");
  message += "<meta http-equiv=\"refresh\" content=\"";
  message += String( cyc );
  message += ";URL=192.168.1.4/switch";
  message += "\">";
  return message;
}

#endif

void buildInfoPage(){
  String  output;

  DBGF("buildInfoPage()")

  output = "</h3>";
  output += Version + "<br><br>"+ "<br>Type: " + FNC_TYPE + "<br>Hardw: " + DEV_TYPE;
  output += "<br>Chip-ID: 0x" + String(cfgData.ChipID);
  output += "<br>MAC-Address: " + String(cfgData.MACAddress);
  output += "<br>Network: " + String(WiFi.SSID());
  output += "<br>Network-IP: " + WiFi.localIP().toString();
  output += "<br>Devicename: " + String(cfgData.hostname);
  output += "<br>AP-Name: " + String(cfgData.APname);
  output += "<br>cfg-Size: 0x" + String(sizeof(cfgData), HEX);
  output += "<br>Hash: 0x" + String(cfgData.hash, HEX);
  output += "<br>";

  output += "<br>Display: ";
  output += (H_TFT_18 == H_TRUE) ? "True" : "False";
  output += "<br>";

  output += "<br>uptime: ";
  output += String(sysData.uptime/86400) + " days - " + String((sysData.uptime/3600)%24) + " hours - " + String((sysData.uptime/60)%60) + " minutes - " + String(sysData.uptime%60) + " seconds";
#if (H_DS1820 == H_TRUE) || (H_TOF == H_TRUE)
  output += "<br>Measuring cycle: " + String(cfgData.MeasuringCycle) + " s (remainig: " + String(sysData.MeasuringCycle) + " s)";
#endif
  output += "<br>Transmit cycle: " + String(cfgData.TransmitCycle) + " s (remaining: " + String(sysData.TransmitCycle) + " s)";
  output += "<br>PageReload cycle: " + String(cfgData.PageReload) + " s";
  output += "<br>Server: " + String(cfgData.server);
  output += "<br>Port: " + String(cfgData.port);
  output += "<br>LED: ";
  cfgData.LED == H_TRUE ? output += "on " : output += "off ";
  output += "<br>Signal strength: ";
  output += String(WiFi.RSSI());
  output += "<br><br>good Transmissions: " + String(sysData.CntGoodTrans);
  output += "<br>bad Transmissions: " + String(sysData.CntBadTrans);
  output += "<br>Pages delivered: " + String(sysData.CntPageDelivered);
#if (H_DS1820 == H_TRUE) || (H_TOF == H_TRUE)
  output += "<br>Measurements: " + String(sysData.CntMeasCyc);
#endif

#if (H_RELAY == H_TRUE)
  output += "<br>ontime: ";
  output += String(sysData.ontime/86400) + " days - " + String((sysData.ontime/3600)%24) + " hours - " + String((sysData.ontime/60)%60) + " minutes - " + String(sysData.ontime%60) + " seconds";
  output += "<br>offtime: ";
  output += String(sysData.offtime/86400) + " days - " + String((sysData.offtime/3600)%24) + " hours - " + String((sysData.offtime/60)%60) + " minutes - " + String(sysData.offtime%60) + " seconds";
  output += "<br>Cycles:                 " + String(sysData.cycles);
#endif
  output += "</h6>";
  server.send(200, W_TEXT_HTML, buildMainPage(output));
  sysData.CntPageDelivered++;
}

/*  ---------------------------------------------------------------------------------------------------------

    some tool routines

    --------------------------------------------------------------------------------------------------------*/
// !!! is urltext really needed ???
int checkInput(String text, String URLText){
  String message="";
  String output;
  int x;

  DBGF("checkInput()");
  output = NameHTML;
  output.replace("{nametext}", text);
  output.replace("{urltext}", URLText);
  DBGF(server.args())

  if( server.args() == 0 ){
    DBGF("server.args=0")
    server.send(200, W_TEXT_HTML, buildConfPage (output));
    sysData.CntPageDelivered++;
    return 0;
  }
  else
    if( server.arg(server.args()-1).length() > 0){
      DBGF("message is:")
      server.send(200, W_TEXT_HTML, buildConfPage(""));
      sysData.CntPageDelivered++;
      x = server.arg(server.args()-1).substring(0, 30).length();
      return x < MAXCHAR ? x : MAXCHAR;  // cut everything whats longer than MAXCHAR
    }
    else {
      DBGF("server.args=leer")

      message = "<p><font color=\"red\">keine Angabe ist keine gute Idee !!!</font></p>";
      message += output;
      server.send(200, W_TEXT_HTML, buildConfPage(message));
      sysData.CntPageDelivered++;
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

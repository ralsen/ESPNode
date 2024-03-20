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
String q = String FPSTR("\"");
String qd = String FPSTR("\":\"");
String qc = String FPSTR("\",\"");

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
    sysData.CntPageDelivered++;
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

  server.on(F("/" "status"), handleStatus);
  server.on(F("/" "server"), handleServer);
  server.on(F("/" "port"), handlePort);
  server.on(F("/" "info"), handleInfo);
  server.on(F("/"), handleInfo);
  server.on(F("/" "reset"), handleReset);
  server.on(F("/" "hostname"), handleHostName);
  server.on(F("/" "led"), handleLED);
  server.on(F("/" "default"), handleSetDefault);
  server.on(F("/" "config"), handleConfPage);
  server.on(F("/" "showlog"), handleShowLog);
  server.on(F("/" "showdir"), handleShowDir);
  server.on(F("/" "pagereload"), handlePagereload);
  server.on(F("/" "meascyc"), handleMeasCyc);
  server.on(F("/" "transcyc"), handleTransCyc);
  server.on(F("/" "scan"), handleScan);
  server.on(F("/favicon.ico"), handleFavicon);
  server.onNotFound( handleNotFound );
  server.on(F("/updatex"), handleUpdate);
  //#httpUpdater.setup(&server);
  server.begin();
  DBGF("startWebServer() done.")
}

void handleUpdate(){
    DBGF("handleUpdate");
    Serial.println("jetzt mach ich ein Update!!!");
//    httpUpdater.setup(&server);
//    httpServer.begin();
    Serial.println("habe fertig!!!");
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

void handleConfPage(){
  DBGF(server.uri());
  sysData.CntPageDelivered++;
  server.send(200, F(W_TEXT_HTML), buildConfPage(F("")));
}

void handleServer() {
  int x;

  DBGF(server.uri());
  x = checkInput(F("an welchen Server soll gesendet werden?"), F("server"));
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
  message += F("<br>");

  message += F("<table border='1'>");
  message += F("<tr><td>Device ID</td><td>Temperature</td></tr>");
  message += buildDS1820Page();
  message += F("</table>");
  sysData.CntPageDelivered++;
  server.send(200, F(W_TEXT_HTML), buildAppPage(message));
  sysData.CntPageDelivered++;

#elif (H_TOF == H_TRUE)
  message = F("<h1>Distance: </h1>");

  //message += F("<br>");
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
  str.replace("\n", "<br>");
  //Serial.println(str);
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
};

void handlePort(){
  int x;

  DBGF(server.uri());
  x = checkInput(F("wie lautet der Port?"), F("port"));
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
    output.replace(F("{ontext}"), F("checked=\"checked\""));
    output.replace(F("{offtext}"), F(""));
  }
  else{
    output.replace(F("{ontext}"), F(""));
    output.replace(F("{offtext}"), F("checked=\"checked\""));
  }
  output += RadioLEDEndHTML;
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
    DBGF("so ist die LED jetzt")
    DBGF(cfgData.LED);
    SaveConfig();
    output = F("Yippie");
    sysData.CntPageDelivered++;
    server.send(200, F(W_TEXT_HTML), buildConfPage (F("")));
  }
}

void handleHostName(){
  int x;

  DBGF(server.uri());
  x = checkInput(F("wie soll das Ding denn heissen?"), F("hostname"));
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
  x = checkInput(F("wie oft soll zum Server gesendet werden?"), F("transcyc"));
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
  sysData.CntPageDelivered++;
  server.send(200, F(W_TEXT_HTML), buildConfPage(F("")));
}

void ScanStart(){
  DBGF("ScanStart()")
  //printUrlArg();
  String output = F("");
  char buf[80];

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
      ltoa(WiFi.RSSI(i), buf, 10);
      output.replace(F("{RSSI}"), (String)(buf));
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
  server.send(200, F(W_TEXT_HTML), buildConfPage(output));
}

/*  all webpage building routines
 *
 */
 String buildPageFrame(String content) {
   // building the frame for all Pages
   // setup the pages with the content thats the same for all pages
   DBGF("buildPageFrame(String content)")

   String WebPage = HomeHTML;
   WebPage.replace(F("{hostname}"), String(cfgData.hostname));
   //WebPage.replace(F("{hostname}"), content);
   WebPage.replace(F("{title}"), String(cfgData.hostname));
   WebPage.replace(F("{content}"), content);
   return WebPage;
 }

String buildConfPage(String content){
  DBGF("buildConfPage(String content)")
  String WebPage = buildPageFrame(content);
  WebPage.replace(F("{appmenu}"), F(""));
  WebPage.replace(F("{mainpage}"), F(""));
  #if (H_DS1820 == H_TRUE)||(H_TOF == H_TRUE)
  WebPage.replace(F("{confpage}"), ConfMenueMeasHTML);
  #else
  WebPage.replace(F("{confpage}"), ConfMenueHTML);
  #endif
  return WebPage;
}

String buildMainPage(String content){
  DBGF("buildMainPage(String content)")
  String  WebPage = buildPageFrame(content);
  WebPage.replace(F("{appmenu}"), AppMenueHTML);
  WebPage.replace(F("{appmenu}"), F(""));
  WebPage.replace(F("{mainpage}"), InfoMenueHTML);
  WebPage.replace(F("{confpage}"), F(""));
  WebPage.replace(F("{content}"), content);
  return WebPage;
}

String buildAppPage(String content){
  DBGF("buildAppPage(String str)")
  String WebPage = buildPageFrame(content);
  WebPage.replace(F("{appmenu}"), AppMenueHTML);
  #if(H_SWITCH == H_TRUE)
  WebPage.replace(F("{appmenu}"), StatusMenueSwitchHTML);
  #else
  WebPage.replace(F("{appmenu}"), "");
  #endif
  WebPage.replace(F("{mainpage}"), F(""));
  WebPage.replace(F("{confpage}"), F(""));
  return WebPage;
}

String buildDict (){
  DBGF("buildDict()");
  String httpRequestData = String(F("{")) + q;
  String FullName=String(cfgData.hostname) + "_" + String(cfgData.MACAddress);
/*villeicht sowas machen:
typedef struct {
  char* key;
  int value;
} DictEntry;
*/
  FullName.replace (F(":"), F("_"));
  httpRequestData += String F(("name")) + qd + FullName + qc;
  httpRequestData += String F(("IP")) + qd + WiFi.localIP().toString() + qc;
  httpRequestData += String F(("Version")) + qd + String(F(VERNR)) + qc;
  httpRequestData += String F(("Hardware")) + qd + String(F(DEV_TYPE)) + qc;
  httpRequestData += String F(("Network")) + qd + String(WiFi.SSID()) + qc;
  httpRequestData += String F(("APName")) + qd + String(cfgData.APname) + qc;
  httpRequestData += String F(("MAC")) + qd + String(cfgData.MACAddress) + qc;
  httpRequestData += String F(("TransmitCycle")) + qd + String(cfgData.TransmitCycle) + qc;
  httpRequestData += String F(("MeasuringCycle")) + qd + String(cfgData.MeasuringCycle) + qc;
  httpRequestData += String F(("Hash")) + qd + String(cfgData.hash, HEX) + qc;
  httpRequestData += String F(("Size")) + qd + String(sizeof(cfgData)) + qc;
  httpRequestData += String F(("TransmitCycle")) + qd + String(cfgData.TransmitCycle) + qc;
  httpRequestData += String F(("MeasuringCycle")) + qd + String(cfgData.MeasuringCycle) + qc;
  httpRequestData += String F(("PageReload")) + qd + String(cfgData.PageReload) + qc;
  httpRequestData += String F(("Server")) + qd + String(cfgData.server) + qc;
  httpRequestData += String F(("Port")) + qd + String(cfgData.port) + qc;
  httpRequestData += String F(("uptime")) + qd + String(sysData.uptime) + qc;
  httpRequestData += String F(("delivPages")) + qd + String(sysData.CntPageDelivered) + qc;
  httpRequestData += String F(("goodTrans")) + qd + String(sysData.CntGoodTrans) + qc;
  httpRequestData += String F(("badTrans")) + qd + String(sysData.CntBadTrans) + qc;
  httpRequestData += String F(("LED")) + qd + String(cfgData.LED) + qc;
  httpRequestData += String F(("WiFi")) + qd + String(WiFi.RSSI()) + qc;
#if (H_DS1820 == H_TRUE)
  httpRequestData += buildDS1820Dict();
#elif (H_SWITCH == H_TRUE)
  httpRequestData += buildSwitchDict();
#elif (H_TOF == H_TRUE)
  httpRequestData += buildToFDict();
#else
#error "keine Hardwarefunktion definiert"
#endif
 return httpRequestData + String F(("}"));
}

#if (H_DS1820 == H_TRUE)
String buildDS1820Dict (){
  char buf[10];
  String url;
  String httpRequestData = String F(("Type")) + qd + String(F(FNC_TYPE)) + String(F("-")) + String(numberOfDevices) + qc;
  DBGF("buildDS1820Dict()");
  for(int i=0;i<numberOfDevices;i++){
    httpRequestData += String F(("Adress_")) + String(i) + qd + GetAddressToString( devAddr[i]) + qc;
    dtostrf(tempDev[i], 2, 2, buf);
    httpRequestData += String F(("Value_")) + String(i) + qd + buf + qc;
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

    message += F("<tr><td>");
    message += GetAddressToString( devAddr[i] );
    message += F("</td>");
    message += F("<td>");
    message += temperatureString;
    message += F("</td></tr>");
    message += F("");
  }
  return message;
}

#elif (H_TOF == H_TRUE)
String buildToFDict (){
  DBGF("buildToFDict()");
  String httpRequestData = String F(("Type")) + qd + String(F(FNC_TYPE)) + qc + String (F("distance")) + qd + String(ToFRange) + q;
  return httpRequestData;
}

String buildToFPage(){
  String message="";

  DBGF("buildToFPage()");
  message = F("<meta http-equiv=\"refresh\" content=\"1\">"); //"; URL=http://192.168.1.38/tof>");
  message += "<h1 style=\"font-size:128px\">"+String(ToFRange)+"</h1>";
  return message;
}

#elif (H_SWITCH == H_TRUE)
String buildSwitchDict(){
  DBGF("buildSwitchDict()");
  String httpRequestData = String F(("Type")) + qd + String(F(FNC_TYPE)) + qc;
  httpRequestData += String (F("ontime")) + qd + String(sysData.ontime) + qc;
  httpRequestData += String (F("offtime")) + qd + String(sysData.offtime) + qc;
  httpRequestData += String (F("cycles")) + qd + String(sysData.cycles) + qc;
  httpRequestData += String (F("status") + qd);
  if (DIG_READ(H_RELAY_PIN))
    httpRequestData += String(F("AN"));
  else
    httpRequestData += String (F("AUS"));
  httpRequestData += q;
  return httpRequestData;
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
  output += Version + F("<br><br>")+ F("<br>Type: ") + F(FNC_TYPE) + F("<br>Hardw: ") + F(DEV_TYPE);
  output += F("<br>Chip-ID: 0x") + String(cfgData.ChipID);
  output += F("<br>MAC-Address: ") + String(cfgData.MACAddress);
  output += F("<br>Network: ") + String(WiFi.SSID());
  output += F("<br>Network-IP: ") + WiFi.localIP().toString();
  output += F("<br>Devicename: ") + String(cfgData.hostname);
  output += F("<br>AP-Name: ") + String(cfgData.APname);
  output += F("<br>cfg-Size: 0x") + String(sizeof(cfgData), HEX);
  output += F("<br>Hash: 0x") + String(cfgData.hash, HEX);
  output += F("<br>");

  output += F("<br>Display: ");
  output += (H_TFT_18 == H_TRUE) ? F("True") : F("False");
  output += F("<br>");

  output += F("<br>uptime: ");
  output += String(sysData.uptime/86400) + F(" days - ") + String((sysData.uptime/3600)%24) + F(" hours - ") + String((sysData.uptime/60)%60) + F(" minutes - ") + String(sysData.uptime%60) + F(" seconds");
#if (H_DS1820 == H_TRUE) || (H_TOF == H_TRUE)
  output += F("<br>Measuring cycle: ") + String(cfgData.MeasuringCycle) + F(" s (remainig: ") + String(sysData.MeasuringCycle) + F(" s)");
#endif
  output += F("<br>Transmit cycle: ") + String(cfgData.TransmitCycle) + F(" s (remaining: ") + String(sysData.TransmitCycle) + F(" s)");
  output += F("<br>PageReload cycle: ") + String(cfgData.PageReload) + F(" s");
  output += F("<br>Server: ")+ String(cfgData.server);
  output += F("<br>Port: ")+ String(cfgData.port);
  output += F("<br>LED: ");
  cfgData.LED == H_TRUE ? output += F("on ") : output += F("off ");
  output += F("<br>Signal strength: ");
  output += String(WiFi.RSSI());
  output += F("<br><br>good Transmissions: ") + String(sysData.CntGoodTrans);
  output += F("<br>bad Transmissions: ") + String(sysData.CntBadTrans);
  output += F("<br>Pages delivered: ") + String(sysData.CntPageDelivered);
#if (H_DS1820 == H_TRUE) || (H_TOF == H_TRUE)
  output += F("<br>Measurements: ") + String(sysData.CntMeasCyc);
#endif

#if (H_RELAY == H_TRUE)
  output += F("<br>ontime: ");
  output += String(sysData.ontime/86400) + F(" days - ") + String((sysData.ontime/3600)%24) + F(" hours - ") + String((sysData.ontime/60)%60) + F(" minutes - ") + String(sysData.ontime%60) + F(" seconds");
  output += F("<br>offtime: ");
  output += String(sysData.offtime/86400) + F(" days - ") + String((sysData.offtime/3600)%24) + F(" hours - ") + String((sysData.offtime/60)%60) + F(" minutes - ") + String(sysData.offtime%60) + F(" seconds");
  output += F("<br>Cycles:                 ") + String(sysData.cycles);
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

  DBGF("checkInput()");
  output = NameHTML;
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

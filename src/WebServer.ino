/*
  WebServer.ino

  !!! for history see settings.h !!!

  ToDos:    ???

  hints:    ???
*/


#include  "settings.h"
#include  "Config.h"
//#include  "WebServer.H"
//#include  "timer.h"
#include  "log.h"
#include  "ToF.h"
#include  "html.h"
//#include  <string>
//#include  <iostream>

// -----------------------------------------
// Server stuff
//------------------------------------------
//HTTP

extern class log_CL logit;

ESP8266WebServer server(MyServerPort);

String HomeHTML;
String AppMenueHTML;
String StatusMenueSwitchHTML;
String InfoMenueHTML;
String NetWorkMenueHTML;
String ConfMenueHTML;
String ConfMenueMeasHTML;
String RadioWifiStartHTML;
String RadioWifiLinetHTML;
String RadioWifiEndHTML;
String RadioLEDStartHTML;
String RadioLEDEndHTML;
String NameHTML;

void initHTML(){
  HomeHTML = FPSTR(H_HOME_PAGE);
  AppMenueHTML = FPSTR(H_APPMENU);
  StatusMenueSwitchHTML = FPSTR(H_STATUSMENU);
  InfoMenueHTML = FPSTR(H_INFOMENU);
  NetWorkMenueHTML = FPSTR(H_NETWORKMENU);
  ConfMenueHTML = FPSTR(H_CONFMENU);
  ConfMenueMeasHTML = FPSTR(H_CONFMENUMEAS);
  RadioWifiStartHTML = FPSTR(H_RADIO_WIFI_START);
  RadioWifiLinetHTML = FPSTR(H_RADIO_WIFI_LINE);
  RadioWifiEndHTML = FPSTR(H_RADIO_WIFI_END);
  RadioLEDStartHTML = FPSTR(H_RADIO_LED_START);
  RadioLEDEndHTML = FPSTR(H_RADIO_LED_END);
  NameHTML = FPSTR(H_NAME);

/*  HomeHTML = readHTML("Home.html");
  AppMenueHTML = readHTML("AppMenue.html");
  StatusMenueSwitchHTML = readHTML("StatusMenueSwitch.html");
  InfoMenueHTML = readHTML("InfoMenue.html");
  NetWorkMenueHTML = readHTML("NetWorkMenue.html");
  ConfMenueHTML = readHTML("ConfMenue.html");
  ConfMenueMeasHTML = readHTML("ConfMenueMeas.html");
  RadioWifiStartHTML = readHTML("RadioWifiStart.html");
  RadioWifiLinetHTML = readHTML("RadioWifiLine.html");
  RadioWifiEndHTML = readHTML("RadioWifiEnd.html");
  RadioLEDStartHTML = readHTML("RadioLEDStart.html");
  RadioLEDEndHTML = readHTML("RadioLEDEnd.html");
  NameHTML = readHTML("Name.html");*/
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
      cycles++;
    DIG_WRITE(H_RELAY_PIN, HIGH);
    handleStatus();
    sysData.TransmitCycle = 0; // send status immediately
    delay(1000);
  });
  server.on(("/" "off"), [](){
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

  server.on(F("/" "status"), handleStatus);
  server.on(F("/" "server"), handleServer);
  server.on(F("/" "service"), handleService);
  server.on(F("/" "scan"), handleScan);
  server.on(F("/" "info"), handleInfo);
  server.on(F("/"), handleInfo);
  server.on(F("/" "reset"), handleReset);
  server.on(F("/" "ap-mode"), handleAPMode);
  server.on(F("/" "sta-mode"), handleSTAMode);
  server.on(F("/" "hostname"), handleHostName);
  server.on(F("/" "led"), handleLED);
  server.on(F("/" "default"), handleSetDefault);
  server.on(F("/" "network"), handleNetPage);
  server.on(F("/" "config"), handleConfPage);
  server.on(F("/" "showlog"), handleShowLog);
  server.on(F("/" "showdir"), handleShowDir);
  server.on(F("/" "pagereload"), handlePagereload);
  server.on(F("/" "meascyc"), handleMeasCyc);
  server.on(F("/" "transcyc"), handleTransCyc);
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
  x = checkInput(F("wie lautet der Service?"), F("service"));
  if( x ){
    strncpy( cfgData.service, server.arg(server.args()-1).c_str(), x);
    cfgData.service[x] = 0;
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
  message += String(cfgData.APname);
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
    output += RadioWifiStartHTML;

    for (int i = 0; i < n; ++i)
    {
      // Print SSID and RSSI for each network found
      output += F("<br>");
      DBGLN(i + 1);
      DBG(": ");
      DBG((WiFi.encryptionType(i) == ENC_TYPE_NONE)?"  ":"* ");
      DBG(WiFi.SSID(i));
      output += RadioWifiLinetHTML;
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
    output += RadioWifiEndHTML;
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
   DBGF(strlen(content.c_str()))
//   DBGF(content)

   String WebPage = HomeHTML;
   WebPage.replace(F("{hostname}"), (String)cfgData.hostname);
   WebPage.replace(F("{title}"), (String)cfgData.hostname);
   WebPage.replace(F("{content}"), content);
//   DBGF(WebPage)
   return WebPage;
 }

String buildNetworkPage(String content){
  DBGF("buildNetworkPage(String content)")
  String WebPage = buildPageFrame(content);
  WebPage.replace(F("{appmenu}"), F(""));
  WebPage.replace(F("{mainpage}"), F(""));
  WebPage.replace(F("{networkpage}"), NetWorkMenueHTML);
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
  #if (H_DS1820 == H_TRUE)||(H_TOF == H_TRUE)
  WebPage.replace(F("{confpage}"), ConfMenueMeasHTML);
  #else
  WebPage.replace(F("{confpage}"), ConfMenueHTML);
  #endif
  sysData.APTimeout = cfgData.APTimeout;
  return WebPage;
}

String buildMainPage(String content){
  DBGF("buildMainPage(String content)")
  DBGF(strlen(content.c_str()))
  //DBGF(content)
  String  WebPage = buildPageFrame(content);
  WebPage.replace(F("{appmenu}"), AppMenueHTML);
  WebPage.replace(F("{appmenu}"), F(""));
  WebPage.replace(F("{mainpage}"), InfoMenueHTML);
  WebPage.replace(F("{networkpage}"), F(""));
  WebPage.replace(F("{confpage}"), F(""));
  sysData.APTimeout = cfgData.APTimeout;
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
  output += F("\r\n<br>Display: ");
  output += (H_TFT_18 == H_TRUE) ? F("True") : F("False");
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

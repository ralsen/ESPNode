/*
Arduino

  HOMA

  universal system for Sonoffs, NodeMCCUs and other ESP8266 devices.

  !!! for history see settings.h !!!

  ToDos:    ???

  hints:    ???
*/

#include <Arduino.h>
#include "Settings.h"
#include "Config.h"

void transmitData(void);

#include "timer.h"
#include "WebServer.h"
#include  "WiFi.h"
#include <string.h>

#include <ArduinoOTA.h>
#include "LittleFS.h"
#include <SPI.h>
#include "log.h"
#include <Ticker.h>
#include <time.h>


#include  "WebServer.h"
#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
//#include <WiFiManager.h>         // https://github.com/tzapu/WiFiManager


#if(S_TFT_18 == true)
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include "tft.h"
#endif


#if (S_TOF == true)
#include  "ToF.h"
#endif

#if (S_DS1820 == true)
#include  "DS1820.h"
#include  <DallasTemperature.h>
#endif

const String  MyName  = {"\r\n*************************************************************************************\r\n"
                             "*******************************     E S P N o d e      ******************************\r\n"
                             "*************************************************************************************"
                        };


String Version = "\r\n-----> V" VERNR " vom " __DATE__ " " __TIME__ " " RELEASE " <-----\r\n";

extern ESP8266WebServer server;

#if(S_FS == true)
log_CL logit(LOGFILE, 11);
TimeDB TimeServ(MY_NTP_SERVER, MY_TZ);
#endif

//------------------------------------------
void setup() { //KB38
  //Setup Serial port speed
  Serial.begin(115200);
  DBGF("setup()")
  //Serial.setDebugOutput(true);
  DIG_MODE(S_LED_PIN, OUTPUT)
  //DIG_MODE(D0, OUTPUT);
  DIG_MODE(S_RELAY_PIN, OUTPUT)
  sysData.uptime = 0;
#if (S_RELAY == true)
  sysData.ontime = sysData.offtime = sysData.cycles = 0;
#endif

  Serial.println(MyName);
  Serial.println(Version);

  //FOL here Init_key and below TISms_key???
  Init_Key();
  DBGLN("Key service started!");
  
  TIms_LED.attach_ms(10, TISms_LED);
  DBGLN("LED Key 10ms Timer services started!");

  #if(S_TFT_18 == true)
  tft_hello();
  #endif

  if (!TestHashConfig()) {
    LEDControl(BLKMODEON, BLKALLERT);
  #if(S_TFT_18 == true)
    tft_println("Hash FAILED !!!");
  #endif
    Serial.println(" Hash FAILED !!!\r\n loading default configuration");
    SetToDefault();
    ESP.eraseConfig();
    ESP.restart();
  }

  String FullName=String(cfgData.hostname) + "_" + String(cfgData.MACAddress);
  FullName.replace (":", "_");
  Serial.println("");
  Serial.print("Hello from device: ");
  Serial.println(FullName);
  Serial.print("Hardware:          ");
  Serial.println(DEV_TYPE);
  Serial.print("Function:          ");
  Serial.println(FNC_TYPE);

  #if(S_FS == true)
  LittleFS.begin();
  #endif

    //Setup DS18b20 temperature sensor
#if (S_DS1820 == true)
  DBGLN("intialisiere die DS1820")
  SetupDS18B20();
#endif
#if (S_TOF == true)
  SetupToF();
#endif

#if(S_FS == true)
  String str = "Directory: \n\r";
  Dir dir = LittleFS.openDir("/");
  while (dir.next()) {
    str += "File: ";
    str += dir.fileName();
    str += " - Size: ";
    str += dir.fileSize();
    str += "\r\n";
  }
  DBG(str);
  if (LittleFS.exists(LOGFILE)) {
    DBGLN("LOGFILE exists");
  }
  else {
    DBGLN("NO LOGFILE");
  }
#endif

  DBGLN("---> " + WiFi.macAddress());
  
  TIms_DspTimeout.attach_ms(10, TISms_DspTimeout);
  TIms_Key.attach_ms(10, TISms_Key);

  TIs_Uptime.attach(1, TISs_Uptime);
  TIs_TransmitCycle.attach(1, TISs_TransmitCycle);
  TIs_MeasuringCycle.attach(1, TISs_MeasuringCycle);
  
  sysData.MeasuringCycle = 0;
  sysData.TransmitCycle = 3;

  #if(S_TFT_18 == true)
  tft_info();
  #endif
  #if (S_RELAY == true)
  TIs_Relais.attach(1, TISs_Relais);
  #endif

  LEDControl(BLKMODEOFF, -1);
  delay(1000);
  
  Serial.println("connecting ...");
  if (!WiFiStartClient())
    WiFiStartAP();//KB24
  MDNS.addService("http", "tcp", 80);

  if (MDNS.begin(FullName)) {  //Start mDNS
    Serial.print("MDNS started with name: ");
    Serial.println(FullName);
  } 
  else Serial.println("schiete kein MDNS");

  startWebServer();

  Serial.println("\r\neverything is initialized, let's go ahead now  ...\r\n");  
#if(S_FS == true)  
  logit.entry("System initialized ...");
#endif
}


// -----------------------------------------------------------------------------

void loop() {
  #if(S_TFT_18 == true)
  int x, y;
  #endif
  // beides wird inTasmota nicht gemacht, aber mdns.advertise, mdns.addserver u.ae.
  MDNS.update(); //KB13
  server.handleClient(); //KB14
  if ((!sysData.TransmitCycle) && (WiFi.getMode() != WIFI_AP)){
      transmitData();
  }
  
  #if (S_RELAY == true)
    if (key) {
      DBGLN("KEY");
      DIG_WRITE(S_RELAY_PIN, !DIG_READ(S_RELAY_PIN));
      DBGL("Relay switched\r\n");
      sysData.cycles++;
      sysData.TransmitCycle = 0; // send status immediately
    }
  #endif

  if(!sysData.DspTimeout){
    sysData.DspTimeout = 100;
    #if(S_TFT_18 == true)
    tft_display2Temps((int)(tempDev[1]), (int)(tempDev[0]));
    #endif
  }
}

// -----------------------------------------------------------------------------

void transmitData() { //KB12
  // !!!hier muss noch zwischen Mess+Transmitzyklen unterschieden werden!!!
  DBGF("transmitData()");
  String header;
  String serverName = "http://" + String(cfgData.server) + ":" + String(cfgData.port); //"http://192.168.1.53:8080/";
  //serverName += "/";

  LEDControl (BLKMODEON, BLKALLERT);

  //Check WiFi connection status
  WiFiClient client;
  HTTPClient http;
  
  http.begin(client, serverName);
  http.addHeader("Content-Type", "application/json");
  String httpRequestData =  buildDict();
  DBGL("sending to: ");
  DBGNL(serverName);
  DBGLN(httpRequestData);
  int httpResponseCode = http.POST(httpRequestData);
  if (httpResponseCode == 301){
    sysData.CntGoodTrans++;
  }
  else {
    sysData.CntBadTrans++;
    #if(S_FS == true)
    logit.entry("server send failed...");
    #endif
  }

  DBGLN(http.getString());
  Serial.print("HTTP Response code: ");
  Serial.println(httpResponseCode);
    
  // Free resources
  http.end();
  //Serial.print(TimeDB.showTime());
  // Auswertung was der Server gemeldet hat und entsprechend handeln
  DBGL("\r\n------------------------------------------------------------------------------------\r\n");
  sysData.TransmitCycle = cfgData.TransmitCycle;
  LEDControl(BLKMODEOFF, -1);

}

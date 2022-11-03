
/*
Arduino

  HOMA

  universal system for Sonoffs, NodeMCCUs and other ESP8266 devices.

  !!! for history see settings.h !!!

  ToDos:    ???

  hints:    ???
*/

#include <Arduino.h>
#include  "Settings.h"
#include  "Config.h"

void DoNormStuff(void);

#include  "timer.h"
#include  "WebServer.h"
//FOL #include  "WiFi.h"
#include  <string>

#include <ArduinoOTA.h>
#include "LittleFS.h"
//FOL das waere cool ---> #include <ESPFtpServer.h>
#include <SPI.h>
#include  "log.h"
#include <ESP8266mDNS.h>
#include <Ticker.h>
#include <time.h>


#include <ESP8266WiFi.h>
#include <DNSServer.h>
//FOL #include <ESP8266WebServer.h>
#include <WiFiManager.h>         // https://github.com/tzapu/WiFiManager


#if(H_TFT_18 == H_TRUE)
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include "tft.h"
#endif


#if (H_TOF == H_TRUE)
#include  "ToF.h"MY_NTP_SERVER
#endif

#if (H_DS1820 == H_TRUE)
#include  "DS1820.h"
#include  <DallasTemperature.h>
#endif

const String  MyName  = {"\r\n*************************************************************************************\r\n"
                             "*******************************     E S P N o d e      ******************************\r\n"
                             "*************************************************************************************"
                        };


String Version = "\r\n-----> V" VERNR " vom " __DATE__ " " __TIME__ " " RELEASE " <-----\r\n";
extern long Intervall;
extern long uptime;
extern ESP8266WebServer server;

log_CL logit(LOGFILE, 11);
TimeDB TimeServ(MY_NTP_SERVER, MY_TZ);
WiFiManager wm;

//------------------------------------------
void setup() {
  //Setup Serial port speed
  Serial.begin(115200);
  DBGF("setup()")
  DIG_MODE(H_LED_PIN, OUTPUT)
  DIG_MODE(H_RELAY_PIN, OUTPUT)
  Intervall = uptime = 0;
  //while (1);
    Serial.println("Hurra ich lebe !!!\r\n");
#if (H_RELAY == H_TRUE)
  ontime = offtime = cycles = 0;
#endif

  String FullName=(String)cfgData.hostname + "_" + (String)cfgData.MACAddress;
  Serial.println( MyName );
  Serial.println( Version );

  Init_Key();
  Serial.println("Key service started!");

  TIms_LED.attach_ms(10, TISms_LED);
  TIms_DspTimeout.attach_ms(10, TISms_DspTimeout);
  TIms_Key.attach_ms(10, TISms_Key);
  Serial.println("10ms Timer services started!");

  #if(H_TFT_18 == H_TRUE)
  tft_hello();
  #endif

  // if many systems on the some time try to connect, some are not scheduled from DHCP
  // don´t no why but this delay depending on the ChipID my help
  /*Serial.print( "waiting " );
    Serial.print(ESP.getChipId() % 10000);
    Serial.println( "ms to subscribe to WiFi.\r\n" );
    delay (ESP.getChipId() % 10000);*/

  //Setup WIFI
  // do default configuration if conf not valid
  //EraseConfig();
  if (!TestHashConfig()) {
    LEDControl(BLKMODEON, BLKALLERT);
  #if(H_TFT_18 == H_TRUE)
    tft_println("Hash FAILED !!!");
  #endif
    Serial.println( " Hash FAILED !!!" );
    Serial.print ("load default configuration, size is: ");
    Serial.println(sizeof(cfgData));
    SetToDefault();
    sysData.status = STATUS_HASH_ERR;
    ESP.eraseConfig();
    ESP.restart();
  }
  else {
    Serial.printf(("Hash: 0x%lx"), cfgData.hash);
  #if(H_TFT_18 == H_TRUE)
    tft_println("Hash: ok");
  #endif
    Serial.println(" is ok");
    LEDControl(BLKMODEON, BLKWIFISTA);
    sysData.status = STATUS_OK;
  }
  Serial.println( "" );
  Serial.print( "Hello from device: " );
  Serial.println( cfgData.hostname );
  Serial.print( "Hardware:          ");
  Serial.println(DEV_TYPE);
  Serial.print("Function:          ");
  Serial.println(FNC_TYPE);
  Serial.print( "MAC-Adress:        " );
  Serial.println( cfgData.MACAddress);
  Serial.println( "" );

  #if(H_TFT_18 == H_TRUE)
    tft_info();
  #endif
  TIs_Uptime.attach(1, TISs_Uptime);
  TIs_TransmitCycle.attach(1, TISs_TransmitCycle);
  TIs_MeasuringCycle.attach(1, TISs_MeasuringCycle);
  #if (H_RELAY == H_TRUE)
  TIs_Relais.attach(1, TISs_Relais);
  #endif
  Serial.println("1s timer services started!");

  //Setup DS18b20 temperature sensor
#if (H_DS1820 == H_TRUE)
  DBGLN("intialiesiere die DS1820")
  SetupDS18B20();
#endif
#if (H_TOF == H_TRUE)
  SetupToF();
#endif

  LittleFS.begin();
  String str = "Directory: \n\r";
  Dir dir = LittleFS.openDir("/");
  while (dir.next()) {
    str += "File: ";
    str += dir.fileName();
    str += " - Size: ";
    str += dir.fileSize();
    str += "\r\n";
  }
  Serial.print(str);
  if (LittleFS.exists(LOGFILE)) {
    Serial.println("LOGFILE exists");
  }
  else {
    Serial.println("NO LOGFILE");
  }

  Serial.println("\r\neverything is initialized, let's go ahead and connect now ->\r\n");

  sysData.WifiRes = wm.autoConnect("AutoConnectAP");

  if(!sysData.WifiRes) {
        Serial.print("Failed to connect!!! -> Result: ");
        ESP.restart();
  } 
  else {
        //if you get here you have connected to the WiFi    
        Serial.print("connected to: ");
        //FOL Serial.print(WiFi.SSID());
        Serial.print(" -> Result: ");
  } 
  Serial.println(sysData.WifiRes);

  LEDControl(BLKMODEOFF, -1);
  delay(1000);
  logit.entry("starting WebServer...");
  //FOL server.begin();
  
  if (MDNS.begin(FullName)) {  //Start mDNS
    Serial.println("MDNS started");
  } 
  else Serial.println("schiete");

  MDNS.addService("http", "tcp", 80);
  startWebServer();
}


// -----------------------------------------------------------------------------

void loop() {
  #if(H_TFT_18 == H_TRUE)
  int x, y;
  #endif
  // beides wird inTasmota nicht gemacht, aber mdns.advertise, mdns.addserver u.ae.
  MDNS.update();
  server.handleClient();

  //logit.entry("...");
  //Serial.println(sysData.WifiRes);
  if ((cfgData.TransmitCycle) && (!sysData.TransmitCycle)){
      DoNormStuff();
  }
  if(!sysData.DspTimeout){
    sysData.DspTimeout = 100;
    #if(H_TFT_18 == H_TRUE)
    tft_display2Temps((int)(tempDev[1]), (int)(tempDev[0]));
    #endif
  }
}

// -----------------------------------------------------------------------------

void DoNormStuff() {
  // !!!hier muss noch zwischen Mess+Transmitzyklen unterschieden werden!!!
  DBGF( "############################ DoNormStuff() #######################################" );
  DBGF(cfgData.TransmitCycle);

  LEDControl (BLKMODEON, BLKALLERT);

  WiFiClient client;
  const int httpPort = MyServerPort;

  DBGL("\r\n------------------------------------------------------------------------------------\r\n");
  DBGL("Verbindungsaufbau zu Server: ");
  DBGLN(cfgData.server);
  DBGL("Port: ");
  DBGLN(httpPort);

  if (client.connect(cfgData.server, httpPort))
  {
    DBGLN("connected]");
    String url = cfgData.service; //Url wird zusammengebaut
    url += buildURL();

    DBGLN("sending this URL: ");
    DBGLN(cfgData.server + url);

    DBGLN("[Sending a request]");
    client.print(String("GET /") + url + " HTTP/1.1\r\n" +
                 "Host: " + cfgData.server + "\r\n" +
                 "Connection: close\r\n" +
                 "\r\n"
                );

    DBGLN("[Response:]");
    String line="";
    while (client.connected() || client.available())
    {
      if (client.available())
      {
        line += client.readStringUntil('\n');
        line += "\r\n";
      }
    }
    client.stop();
    DBGLN(line);
    DBGLN(line.indexOf("OK"));
    if (line.indexOf("OK") == -1){
      logit.entry("Serverfehler");
    }
    sysData.CntGoodTrans++;
    DBGLN("\n[Disconnected]");
  }
  else
  {
    DBGLN("[connection failed!]");
    logit.entry("[connection failed!]");
    client.stop();
    sysData.CntBadTrans++;
  }

  //Serial.print(TimeDB.showTime());
  // Auswertung was der Server gemeldet hat und entsprechend handeln
  DBGL("\r\n------------------------------------------------------------------------------------\r\n");
  sysData.TransmitCycle = cfgData.TransmitCycle;
  LEDControl(BLKMODEOFF, -1);
}


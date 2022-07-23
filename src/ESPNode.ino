
/*
Arduino

  HOMA

  universal system for Sonoffs, NodeMCCUs and other ESP8266 devices.

  !!! for history see settings.h !!!

  ToDos:    ???

  hints:    ???
*/

#include  "Settings.h"
#include  "Config.h"
#include  "timer.h"
#include  "WebServer.h"
#include  "WiFi.h"
#include  <string>

#include <ArduinoOTA.h>
#include "LittleFS.h"
#include <SPI.h>
#include  "log.h"
#include <ESP8266mDNS.h>
#include <Ticker.h>
#include <time.h>

#if(H_TFT_18 == H_TRUE)
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include "tft.h"
#endif

#if (H_TOF == H_TRUE)
#include  "ToF.h"
#endif

#if ((H_DS1820 == H_TRUE) || (H_TOF == H_TRUE)) //FOL warumist hier H_TOF drin???
#include  "DS1820.h"
#include  <DallasTemperature.h>
#endif

#include <DallasTemperature.h> //FOL muss da sein wenn H_DS1820==H_FALSE, sehr merkwuerdig

#define MY_NTP_SERVER "at.pool.ntp.org"
#define MY_TZ "CET-1CEST,M3.5.0,M10.5.0/3"   

const String  MyName  = {"\r\n*************************************************************************************\r\n"
                             "*******************************     E S P N o d e      ******************************\r\n"
                             "*************************************************************************************"
                        };


const String  Version = "\r\n-----> V" VERNR " vom " __DATE__ " " __TIME__ " " RELEASE " <-----\r\n";

log_CL logit(LOGFILE, 11);
TimeDB TimeDB(MY_NTP_SERVER, MY_TZ);

//------------------------------------------
void setup() {
  //Setup Serial port speed
  Serial.begin(115200);
  DBGF("setup()")
  DIG_MODE(H_LED_PIN, OUTPUT)
  DIG_MODE(H_RELAY_PIN, OUTPUT)
  Intervall = uptime = 0;
#if (H_RELAY == H_TRUE)
  ontime = offtime = cycles = 0;
#endif

  Serial.println( MyName );
  Serial.println( Version );

  Init_Key();
  Serial.println("Key service started!");

  CntmTicks.attach_ms(10, milli_ISR);
  Serial.println("10ms Timer service started!");

  #if(H_TFT_18 == H_TRUE)
  tft_hello();
  #endif

  LEDControl(BLKMODEON, BLKALLERT);

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
  #if(H_TFT_18 == H_TRUE)
    tft_println("Hash FAILED !!!");
  #endif
    Serial.println( " Hash FAILED !!!" );
    Serial.print ("load default configuration, size is: ");
    Serial.println(sizeof(cfgData));
    SetToDefault();
    sysData.mode = MODE_CHG_TO_AP;
    sysData.status = STATUS_HASH_ERR;
  }
  else {
    Serial.printf(("Hash: 0x%lx"), cfgData.hash);
  #if(H_TFT_18 == H_TRUE)
    tft_println("Hash: ok");
  #endif
    Serial.println(" is ok");
    sysData.mode = MODE_CHG_TO_STA;
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
  CntTicks.attach(1, sec_ISR);
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


  Serial.println("\r\neverything is initialized, let's go ahead now ->\r\n");

  delay(1000);
  logit.entry("starting ...");
}

int toggle = 0;

char buf[80];
// -----------------------------------------------------------------------------

void loop() {
  #if(H_TFT_18 == H_TRUE)
  int x, y;
  #endif
  // beides wird inTasmota nicht gemacht, aber mdns.advertise, mdns.addserver u.ae.
  MDNS.update();
  server.handleClient();
  //ArduinoOTA.handle(); 

  
  switch ( sysData.mode ) {
    case MODE_STA: {
        if ((cfgData.TransmitCycle) && (!sysData.TransmitCycle)){
           DoNormStuff();
        }
        if(!sysData.DspTimeout){
          sysData.DspTimeout = 100;
          #if(H_TFT_18 == H_TRUE)
          tft_display2Temps(47, 11);
          #endif
        }
#if (H_RELAY == H_TRUE)
        if ( key && ( toggle == 0 )) {
          DBGLN( "KEY" );
          DIG_WRITE( H_RELAY_PIN, !DIG_READ(H_RELAY_PIN));
          DBGL("Relay switched\r\n");
          cycles++;
          sysData.TransmitCycle = 0; // send status immediately
          toggle = 1;
        }
        else if ( !key) toggle = 0;
#endif
        break;
      }
    case MODE_AP: {
        if (!sysData.APTimeout) {
          DBGLN(" !!! Restarting now !!!");
          logit.entry(" !!! Restarting now !!!");
          ESP.restart();
        }
        #if(H_TFT_18 == H_TRUE)
        else{
        tft.setCursor(0,30);
        sprintf(buf,"leaving in: %lis", sysData.APTimeout);
        tft_print(buf);
        }
        #endif
        break;
      }
    case MODE_CHG_TO_AP: {
        WiFiStartAP();
        startWebServer();
        sysData.APTimeout = cfgData.APTimeout;
        sysData.mode = MODE_AP;
        logit.entry("switching into AP mode");
        break;
      }
    case MODE_CHG_TO_STA: {
        #if(H_TFT_18 == H_TRUE)
        tft.setTextColor(ST7735_WHITE, ST7735_BLUE);
        x = tft.getCursorX();
        y = tft.getCursorY();
        tft_print("wait for WLAN");
        #endif
        if (!WiFiStartClient()) {
          sysData.mode = MODE_CHG_TO_AP;
        }
        else {
          #if(H_TFT_18 == H_TRUE)
          tft.setCursor(x,y);
          tft_print("IP:");
          tft_println(WiFi.localIP().toString());
          #endif
          startWebServer();
          sysData.mode = MODE_STA;
          if (!cfgData.TransmitCycle) LEDControl(BLKMODEFLASH, BLKFLASHOFF);
          else LEDControl(BLKMODEOFF, -1);
          MDNS.addService("http", "tcp", 80);
          logit.entry("WebServer started");
          #if(H_TFT_18 == H_TRUE)
          tft_textWait(5);
          tft_init2Temps();
          #endif
        }
        sysData.MeasuringCycle = 0;
        if (sysData.TransmitCycle) {
          sysData.TransmitCycle = SEND_AFTER_BOOT_SEC;
          DBGL("sending first message in ");
          DBG(SEND_AFTER_BOOT_SEC);
          DBGNL(" seconds");
          logit.entry("sending first message");
        }
        break;

      }
  }
}


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

  // Serial.print(TimeDB.showTime());
  // Auswertung was der Server gemeldet hat und entsprechend handeln
  DBGL("\r\n------------------------------------------------------------------------------------\r\n");
  sysData.TransmitCycle = cfgData.TransmitCycle;
  LEDControl(BLKMODEOFF, -1);
}


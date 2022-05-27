
/*
Arduino

  HOMA

  universal system for Sonoffs, NodeMCCUs and other ESP8266 devices.

  !!! for history see end of file !!!

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

#if (H_TOF == H_TRUE)
#include  "ToF.h"
#endif
#if ((H_DS1820 == H_TRUE) || (H_TOF == H_TRUE))
#include  "DS1820.h"
#include  <DallasTemperature.h>
#endif

#include  <ESP8266mDNS.h>
#include  <Ticker.h>
#include  <DallasTemperature.h>
#include <time.h>

#define MY_NTP_SERVER "at.pool.ntp.org"
#define MY_TZ "CET-1CEST,M3.5.0,M10.5.0/3"   


const String  MyName  = {"\r\n*************************************************************************************\r\n"
                             "*******************************     E S P N o d e      ******************************\r\n"
                             "*************************************************************************************"
                        };
const String  Version = "\r\n-----> V" VERNR " vom " __DATE__ " " __TIME__ " " RELEASE " <-----\r\n";

log_CL logit(LOGFILE, 11);

time_t e_now;                         // this is the epoch
tm tm;                              // the structure tm holds time information in a more convient way

void showTime() {
  configTime(MY_TZ, MY_NTP_SERVER); // --> Here is the IMPORTANT ONE LINER needed in your sketch!

  time(&e_now);                       // read the current time
  localtime_r(&e_now, &tm);           // update the structure tm with the current time
  Serial.print("year:");
  Serial.print(tm.tm_year + 1900);  // years since 1900
  Serial.print("\tmonth:");
  Serial.print(tm.tm_mon + 1);      // January = 0 (!)
  Serial.print("\tday:");
  Serial.print(tm.tm_mday);         // day of month
  Serial.print("\thour:");
  Serial.print(tm.tm_hour);         // hours since midnight  0-23
  Serial.print("\tmin:");
  Serial.print(tm.tm_min);          // minutes after the hour  0-59
  Serial.print("\tsec:");
  Serial.print(tm.tm_sec);          // seconds after the minute  0-61*
  Serial.print("\twday");
  Serial.print(tm.tm_wday);         // days since Sunday 0-6
  if (tm.tm_isdst == 1)             // Daylight Saving Time flag
    Serial.print("\tDST");
  else
    Serial.print("\tstandard");
  Serial.println();
}

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
    Serial.println( " Hash FAILED !!!" );
    Serial.print ("load default configuration, size is: ");
    Serial.println(sizeof(cfgData));
    SetToDefault();
    sysData.mode = MODE_CHG_TO_AP;
    sysData.status = STATUS_HASH_ERR;
  }
  else {
    Serial.printf(("Hash: 0x%lx"), cfgData.hash);
    Serial.println(" is ok");
    sysData.mode = MODE_CHG_TO_STA;
    sysData.status = STATUS_OK;
  }
  Serial.println( "" );
  Serial.print( "Hello from device: " );
  Serial.println( cfgData.hostname );
  Serial.print( "Hardware:          ");
  Serial.println(DEV_TYPE);
  Serial.print( "MAC-Adress:        " );
  Serial.println( cfgData.MACAddress);
  Serial.println( "" );

  CntTicks.attach(1, sec_ISR);
  Serial.println("timer services started!");

  //Setup DS18b20 temperature sensor
#if (H_DS1820 == H_TRUE)
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

  Serial.println("everything is initialized, let's go ahead now ->\r\n");
  delay(1000);
}

int toggle = 0;


// -----------------------------------------------------------------------------

void loop() {

  // beides wird inTasmota nicht gemacht, aber mdns.advertise, mdns.addserver u.ae.
  MDNS.update();
  server.handleClient();
  //ArduinoOTA.handle(); 
  
  switch ( sysData.mode ) {
    case MODE_STA: {
        if ((cfgData.TransmitCycle) && (!sysData.TransmitCycle)) DoNormStuff();
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
          DBGLN(" !! Restarting now !!!");
          ESP.restart();
        }
        break;
      }
    case MODE_CHG_TO_AP: {
        WiFiStartAP();
        startWebServer();
        sysData.APTimeout = cfgData.APTimeout;
        sysData.mode = MODE_AP;
        break;
      }
    case MODE_CHG_TO_STA: {
        if (!WiFiStartClient()) {
          sysData.mode = MODE_CHG_TO_AP;
        }
        else {
          startWebServer();
          sysData.mode = MODE_STA;
          if (!cfgData.TransmitCycle) LEDControl(BLKMODEFLASH, BLKFLASHOFF);
          else LEDControl(BLKMODEOFF, -1);
          MDNS.addService("http", "tcp", 80);
        }
        sysData.MeasuringCycle = 0;
        if (sysData.TransmitCycle) {
          sysData.TransmitCycle = SEND_AFTER_BOOT_SEC;
          DBGL("sending first message in ");
          DBG(SEND_AFTER_BOOT_SEC);
          DBGNL(" seconds");
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

  showTime();

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

  // Auswertung was der Server gemeldet hat und entsprechend handeln
  DBGL("\r\n------------------------------------------------------------------------------------\r\n");
  sysData.TransmitCycle = cfgData.TransmitCycle;
  LEDControl(BLKMODEOFF, -1);
}

/*
  History:
  --------------------- V1.4
  19.01.21  prettier the hello message
  17.01.21  if cfgData.TransmitCycle is 0 nothing is send to the Server
            MAC-Address is used for identification and is displayed during
            boot. MACAddress is appended to the device name instead of the
            Chip-Number because this number is not realy unique, espressif
            stated
  --------------------- V1.4
  12.08.20  V1.30d: hash is displayed in hex on debug port
            Transcyc and MeasCyc is transmitted additionaly to the server
  14.05.20  V1.30C: bonjour service added
  06.05.20  V1.30a: new WiFi-state machine, sending a message after boot
  20.04.20  V1.30 from now we use platformio instead of the Arduiono stuff
            SEND_AFTER_BOOT_SEC
  18.06.19  switches status when their status every time when they are switched
  16.06.19  V1.20 switches send their status after TransmitCycle
  17.04.19  V1.10 using new routine for server communication in DoNormStuff
            -> https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/client-examples.html?highlight=client.available
  16.04.19  V1.06 found the error why the devices disconnected from WLAN
            in DoNormStuff was a Wfifi.disconnect() on communication errors
  31.03.19  error the brings the WiFi connection down found
  05.03.19  V1.03: config or DS1820 added (Meascyx, Transcyc and pagereload)
            count the messages to server, the pagereloads (good and bad trys)
            server and service now variable
  03.03.19  V1.02: renamed TempLoop to MeasuringLoop which runs from now in sec_ISR.
  02.03.19  V1.01 Website optimization (code and design)
            Serer- and Servicename input added
  23.02.19  first Version wirh new Version management. Released for Sonoff S20 and Sonoff Basic
  19.01.19  V0.04 first version to re released
            debuging and error-check for all versions is to be done

  22.12.18  Version 0.03  implementation of first version control functionality
  20.12.18  Version 0.02  cleaned up the WiFi and System.Mode switching
            debug enhancements
  16.12.18  Version 0.01  the DS1820-stuff from here to DS1820.ino/h
            AP-Mode dont work now
  28.11.18  Version 0.00  first version. sends a url-string to the php-server

  todos:
            done: zyklische Statusmeldung an Server senden, z.B. täglich
*/

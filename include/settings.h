/*
  settings.h

  !!! for history see end of file !!!

  ToDos:    -

  hints:    -
*/
#include <Arduino.h>

#ifndef __SETTINGS_H
#define __SETTINGS_H

 #if (S_DBG == true)
  #define RELEASE         "--- DEBUG ---"
#else
  #define RELEASE
#endif

/* ----------------------------------------------------------------------
;        Versionsnummern, Devicetypes
; -------------------------------------------------------------------- */

#if defined(SONOFF_BASIC)
# define DEV_TYPE       "SONOFF_BASIC"
# define FNC_TYPE       "Switch"
# define S_SWITCH        true
# define S_DS1820        false
# define S_PIR           false
# define S_BUTTON        true
# define S_LED           true
# define S_RELAY         true
# define S_TOF           false
# define S_TFT_18        false
# define S_FS            false

#elif defined(SONOFF_S20)
# define DEV_TYPE       "SONOFF_S20"
# define FNC_TYPE       "Switch"
# define S_SWITCH        true
# define S_DS1820        false
# define S_PIR           false
# define S_BUTTON        true
# define S_LED           true
# define S_RELAY         true
# define S_TOF           false
# define S_TFT_18        false
# define S_FS            false

#elif defined(SONOFF_S26)
# define DEV_TYPE       "SONOFF_S26"
# define FNC_TYPE       "Switch"
# define S_SWITCH        true
# define S_DS1820        false
# define S_PIR           false
# define S_BUTTON        true
# define S_LED           true
# define S_RELAY         true
# define S_TOF           false
# define S_TFT_18        false
# define S_FS            false

#elif defined(NODEMCU_SWITCH)
# define DEV_TYPE       "NODEMCU"
# define FNC_TYPE       "Switch"
# define S_SWITCH        true
# define S_DS1820        false
# define S_PIR           false
# define S_BUTTON        true
# define S_LED           true
# define S_RELAY         true
# define S_TOF           false
# define S_TFT_18        false
# define S_FS            false

#elif defined(NODEMCU_DS1820)
# define DEV_TYPE       "NODEMCU"
# define FNC_TYPE       "DS1820"
# define S_SWITCH        false
# define S_DS1820        true
# define S_PIR           false
# define S_BUTTON        true
# define S_LED           true
# define S_RELAY         false
# define S_TOF           false
# define S_TFT_18        false
# define S_FS            true

#elif defined(NODEMCU_DS1820_TFT_18)
# define DEV_TYPE       "NODEMCU"
# define FNC_TYPE       "DS1820"
# define S_SWITCH        false
# define S_DS1820        true
# define S_PIR           false
# define S_BUTTON        true
# define S_LED           true
# define S_RELAY         false
# define S_TOF           false
# define S_TFT_18        true
# define S_FS            true

#elif defined(D1MINI_DS1820)
# define DEV_TYPE       "D1MINI"
# define FNC_TYPE       "DS1820"
# define S_SWITCH        false
# define S_DS1820        true
# define S_PIR           false
# define S_BUTTON        true
# define S_LED           true
# define S_RELAY         false
# define S_TOF           false
# define S_TFT_18        false
# define S_FS            true
# define S_FS            true

#elif defined(D1MINI_DS1820_TFT_18)
# define DEV_TYPE       "D1MINI"
# define FNC_TYPE       "DS1820"
# define S_SWITCH        false
# define S_DS1820        true
# define S_PIR           false
# define S_BUTTON        true
# define S_LED           true
# define S_RELAY         false
# define S_TOF           false
# define S_TFT_18        true
# define S_FS            true

#elif defined(D1MINI_TOF)
# define DEV_TYPE       "D1MINI"
# define FNC_TYPE       "ToF"
# define S_SWITCH        false
# define S_DS1820        false
# define S_PIR           false
# define S_BUTTON        true
# define S_LED           true
# define S_RELAY         false
# define S_TOF           true
# define S_TFT_18        false
# define S_FS            true

#elif defined(NODEMCU_PIR)
# define DEV_TYPE       "NODEMCU"
# define FNC_TYPE       "PIR"
# define S_SWITCH        false
# define S_DS1820        false
# define S_PIR           true
# define S_BUTTON        true
# define S_LED           true
# define S_RELAY         false
# define S_TOF           false
# define S_TFT_18        false
# define S_FS            true

#else
#error "Hey! welche Version???"
#endif

// --------- some hardware stuff  ---------

#if (S_LED == true) || (S_RELAY == TRUE)
# define  DIG_WRITE(pin, val)   digitalWrite(pin, val)
# define  DIG_READ(pin)         digitalRead(pin)
# define  DIG_MODE(pin, inout)  pinMode(pin, inout);

# if defined(SONOFF_BASIC) || defined(SONOFF_S20) || defined(SONOFF_S26)
#   define S_LED_PIN      13
#   define S_RELAY_PIN    12
//# elif    (DEV_TYPE == "NODEMCU") || (DEV_TYPE == "D1MINI")
# elif    defined(NODEMCU_SWITCH) || defined(NODEMCU_DS1820) || defined(D1MINI_DS1820) || defined(D1MINI_TOF) || defined(D1MINI_DS1820_TFT_18) || defined(NODEMCU_DS1820_TFT_18)
#   define S_LED_PIN      2
#   define S_RELAY_PIN    D0
# else
#   error   "S_LED_PIN undefined"
# endif
#else     // if (S_LED == FALSE)
# define  S_LED_PIN
# define  S_RELAY_PIN
# define  DIG_WRITE(pin, val)
# define  DIG_READ(pin)
# define  DIG_MODE(pin, inout)
#endif



#if (S_DS1820 == true)
# define S_DS1820_PIN     D3
# define S_DS180_MAX_DEV  10
#endif


#if (S_BUTTON == true)
# define S_BUTTON_PIN     0
#endif
// --------- some hardware stuff  ---------


// --------- some software stuff  ---------
#define LOGFILE       "/status.log"
#define MAXCHAR       30
#define MAXLOGSIZE    6000  // bigger values than ~ 6600 causes error in string processing and sending to server. No solution yet look ->
                            // https://github.com/esp8266/Arduino/issues/3205

#define FOREVER         while(1)delay(1);
#define DEAD            while(1);

#define FALSE           0
#define TRUE            1

#if (S_DBG == true)
  #define DBG(x)            Serial.print(x);
  #define DBGF(x)           {Serial.print("F()-> "); Serial.print (__FILE__); Serial.print(" Line ");Serial.print (__LINE__); Serial.print(": "); Serial.println(x);}
  #define DBGLN(x)          {Serial.print(" ---> "); Serial.println(x);}
  #define DBGL(x)           {Serial.print(" ---> "); Serial.print(x);}
  #define DBGNL(x)          Serial.println(x);
#else
  #define DBG(x)
  #define DBGF(x)
  #define DBGL(x)
  #define DBGLN(x)
  #define DBGNL(x)
#endif


// these are the blink modes
#define BLKMODEOFF      0
#define BLKMODEON       1
#define BLKPRIOMODEON   2
#define BLKMODEFLASH    4
#define BLKPRIOMODEFLAH 8


// these are the blink modes timings
#define BLKALLERT       5
#define BLKCONF         150 //???
#define BLKNORM         250000
#define BLKWIFIAP       10
#define BLKWIFISTA      50
#define BLKFIRM         100 //???
#define BLKFLASHON      3
#define BLKFLASHOFF     500


// mode value for sysflag.mode
#define MODE_CHG_TO_STA 1
#define MODE_CHG_TO_AP  2
#define MODE_AP         4
#define MODE_STA        8

// values for sysflag.status
#define STATUS_ALLERT   1
#define STATUS_OK       2
#define STATUS_CONF_ERR 4
#define STATUS_HASS_ERR 8
#define STATUS_LED      16

#define CFGSTART              0         // startadress og cfg-block in EEPROM
#define DEFAULT_LED           true

#define DEFAULT_SSID          "janzneu"
#define DEFAULT_PASSWORD      "D1AFFE1234!"
//#define DEFAULT_SSID          "TK800"
//#define DEFAULT_PASSWORD      "Lanecharge"

#define DEFAULT_HOSTNAME      "No-Name"
#define DEFAULT_APNAME        "ESPnet"
#define DEFAULT_MEASCYCLE     150       // Measurement cycle in sec.
#define DEFAULT_PAGERELOAD    10        // page reload cycle in sec.

#if (S_SWITCH == false)
  #define DEFAULT_TRANSCYCLE    300     // transmit cycle to server in sec.
#else
  #define DEFAULT_TRANSCYCLE    3600
#endif

#define MY_NTP_SERVER "at.pool.ntp.org"
#define MY_TZ "CET-1CEST,M3.5.0,M10.5.0/3"

#define DEFAULT_PORT          "8080"
#define DEFAULT_SERVER        "192.168.2.87"
// --------- some software stuff  ---------


// -----------------------------------------------------
#endif /* #ifndef __SETTINGS_H */

/*
  history
  --------------------- V4.0a
  03.11.22  change to cpp-files instead ino. Networkpage removed (we use the WiFiManager), Platformio with new library management, lots of other minor things
  --------------------- V3.0a
  09.10.22  doing the Wifi-Stuff with the WifiManager from github
  --------------------- V2.3j
  08.09.22  removed some lines with TimeDB and log.entry because of problems and crashes
  --------------------- V2.3h
  10.08.22  new websites in seperate file (html.h)
  --------------------- V2.3g
  10.08.22  obviously we dont need the while loop with wifi.multirun
  --------------------- V2.3f
  10.08.22  S_RELAY is also working with new timer interrupt
  --------------------- V2.3e
  10.08.22  timer service routines are seperated for each service
  --------------------- V2.3d
  23.07.22  in some cases the blue-LED remains on when with TFT
            Display on Info Page
  --------------------- V2.3c
  23.07.22  first version with TFT and DS1820
  --------------------- V2.3b
  23.07.22  TFT code cleanup
  --------------------- V2.3a
  21.07.22  TFT in DS1820-App included
  --------------------- V2.2a
  10.07.22  TFT-Display and some changes for logging
  --------------------- V2.1c
  26.05.22  LED Radio-buttons with defaults ("checked=\"checked\"")
  --------------------- V2.0c
  26.11.21  "Info" and "Status" on all webpages in the same order
  --------------------- V2.0b
  25.11.21  some changes on webpage
  --------------------- V2.0a
  21.11.21  lot of changes for 2.xx, made for arduino IDE and 
            many changes to improve stability, performance etc.
  --------------------- V1.5k
  27.02.21  ToF-pagereload was constant set to 1 s. now its
            Pagereload
            default AP-SSID is now "ESPnet"
  --------------------- V1.5j
  27.02.21  average calculation for the ToF-Sensor
  --------------------- V1.5i
  26.02.21  ToF not present for new VL53L1X lib updated
            this Version has REALES-State
  --------------------- V1.5g
  25.02.21  using TOF- library "VL53L1X" instead of the old Sparcfun
  --------------------- V1.5g
  23.02.21  "mm" text un ToF-Web side added
  --------------------- V1.5f
  29.01.21  system crahed if the tof sensor failed.
            system checks if ToFPresent is TOF_TRUE or not
  --------------------- V1.5e
  29.01.21  LED on status page moved to upper information block
  --------------------- V1.5b
  29.01.21  LED is controlled via LEDControl()
  --------------------- V1.4x
  24.01.21  if TransCyc = 0 the LED shows a heartbeat
  24.01.21  the LED could be switch off on the Config-Pages
            but the LED is always during boot und WiFi-connect
  19.01.21  ToF pages added on webserver
  18.01.21  FNC_TYPE isnt displayed in headline of status page anymore
  18.01.21  Version is only displayed in hardline og status page
  18.01.21  first messge is send after going into MODE_STA
  17.01.21  ToF sensor added
  17.01.21  if cfg.TransmitCycle is 0 nothing is send to the Server
            MAC-Address is used for identification and is displayed during
            boot. MACAddress is appended to the device name instead of the
            Chip-Number because this number is not realy unique, espressif
            stated
  12.08.20  V1.30d: hash is displayed in hex on debug port
            Transcyc and MeasCyc is transmitted additionaly to the server
  14.05.20  V1.30C: bonjour service added
  07.05.20  V1.30b: measuring only in MODE_STA
  06.05.20  V1.30a: new WiFi-state machine, sending a message after boot
  20.04.20  V1.30 from now we use platformio instead of the Arduiono stuff
            therefore we dont need the defines NODEMCU_SWITCH and so an.
            this is now in with platformio.ini
  06.11.19  working on:
            after reboot send a message immendently
            send a long message with all infos that are needed on server
  11.07.19  V1.24: switches dont send cyclic
  08.07.19  V1.23: some changes on the status-site
  16.06.19  V1.20 switches send their status after TransmitCycle
  26.04.19  V1.11 removed an error on switching the LED. Sonoff-Basic has no
            LED in parallel with the relay so we have to use the same one for
            blinking and relais state. On Sonoff S20 the LED is dedicated for
            blinking and the relais state is indicated with the blue (sometimes red)
            LED parallel to the relays
  17.04.19  V1.10 using new routine for server communication in DoNormStuff
            -> https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/client-examples.html?highlight=client.available
  16.04.19  V1.1x should send status to the server in adjustable time
            Versions and Hardware sitch should be clarified
            why is there a delay after the server responce in DoNormStuff in Homa_xxx.ino?
  16.04.19  V1.06 found the error why the devices disconnected from WLAN
            in DoNormStuff was a Wfifi.disconnect() on communication errors
  05.03.19  V1.03: config or DS1820 added (Meascyx, Transcyc and pagereload)
            count the messages to server, the pagereloads (good and bad trys)
            server and service now variable
  03.03.19  V1.02: renamed TempLoop to MeasuringLoop which runs from now in sec_ISR.
                    renamed ISR_count to sec_ISR which contains now Measuring and transmit counters
  02.03.19  V1.01 Website optimization (code and design)
            Server- and Servicename input added
  23.02.19  first Version wirh new Version management. Released for Sonoff S20 and Sonoff Basic
  19.01.19  V0.04 first version to re released
            debuging and error-check for all versions is to be done

  ToDos:
            - statusmeldung an server senden
            - LED dauer an in blinken einbauen

  Errors:
            - warum werden 5 bewsiteausliefrungen gezaehlt wenn Seite vom iPhone aufgerufen wird
            - Warum regiert WLAN schlecht bis garnicht wenn DBG == S_FASLE
            - OTA funktioniert aof nicht  (vor allem SB und S20). Scheinbar hilft es machnmal das
              Board im bootloader zu straten und gleich wieder aus und einzuschalten
            - warum kommt 404: not found URI: /favicon.ico
*/

/* Problem:
    ich kann nicht fuer beliebige Boards beliebige z.B. Relay konfigurationen
    machen. Beim NodeMcu würde ich aber gerne alle Hardwarekombinationen machen koennen.
    Schon allien zu Testzwecken. Aber das Konzept ist aus erst rund wenn das geht. Und
    das war ja Sinn des Konzepts.
    Nochmal im alten HWDOS nachsehen. Ist im Quellcode nun Bezug auf Geraet oder auf S_-Schalter?
    */

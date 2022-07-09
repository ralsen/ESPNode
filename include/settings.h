/*
  settings.h

  !!! for history see end of file !!!

  ToDos:    -

  hints:    -
*/

#ifndef __SETTINGS_H
#define __SETTINGS_H

#define H_FALSE 0
#define H_TRUE  1

// #define H_DBG   H_TRUE -> in platformio

/* ----------------------------------------------------------------------
;        Versionsnummer and System
; -------------------------------------------------------------------- */

// -> in platformio
// #define VERNR   "2.0c"
// #define NODEMCU_DS1820
// #define D1MINI_DS1820
// #define D1MINI_TOF
// #define NODEMCU_SWITCH
// #define SONOFF_S20_SWITCH
// #define SONOFF_BASIC_SWITCH
// #define NODEMCU_PIR        // noch nicht existent
// <- in platformio 


 #if (H_DBG == H_TRUE)
  #define RELEASE         "NOT REALESED"
#else
  #define RELEASE
#endif

/* ----------------------------------------------------------------------
;        Versionsnummern, Devicetypes
; -------------------------------------------------------------------- */

#if defined(SONOFF_BASIC_SWITCH)
# define DEV_TYPE       "SONOFF_BASIC"
# define FNC_TYPE       "Switch"
# define H_SWITCH        H_TRUE
# define H_DS1820        H_FALSE
# define H_PIR           H_FALSE
# define H_BUTTON        H_TRUE
# define H_LED           H_TRUE
# define H_RELAY         H_TRUE
# define H_TOF           H_FALSE
# define H_TFT_18        H_FALSE

#elif defined(SONOFF_S20_SWITCH)
# define DEV_TYPE       "SONOFF_S20"
# define FNC_TYPE       "Switch"
# define H_SWITCH        H_TRUE
# define H_DS1820        H_FALSE
# define H_PIR           H_FALSE
# define H_BUTTON        H_TRUE
# define H_LED           H_TRUE
# define H_RELAY         H_TRUE
# define H_TOF           H_FALSE
# define H_TFT_18        H_FALSE

#elif defined(NODEMCU_SWITCH)
# define DEV_TYPE       "NODEMCU"
# define FNC_TYPE       "Switch"
# define H_SWITCH        H_TRUE
# define H_DS1820        H_FALSE
# define H_PIR           H_FALSE
# define H_BUTTON        H_TRUE
# define H_LED           H_TRUE
# define H_RELAY         H_TRUE
# define H_TOF           H_FALSE
# define H_TFT_18        H_FALSE

#elif defined(NODEMCU_DS1820)
# define DEV_TYPE       "NODEMCU"
# define FNC_TYPE       "DS1820"
# define H_SWITCH        H_FALSE
# define H_DS1820        H_TRUE
# define H_PIR           H_FALSE
# define H_BUTTON        H_TRUE
# define H_LED           H_TRUE
# define H_RELAY         H_FALSE
# define H_TOF           H_FALSE
# define H_TFT_18        H_FALSE

#elif defined(D1MINI_DS1820)
# define DEV_TYPE       "D1Mini"
# define FNC_TYPE       "DS1820"
# define H_SWITCH        H_FALSE
# define H_DS1820        H_TRUE
# define H_PIR           H_FALSE
# define H_BUTTON        H_TRUE
# define H_LED           H_TRUE
# define H_RELAY         H_FALSE
# define H_TOF           H_FALSE
# define H_TFT_18        H_TRUE

#elif defined(D1MINI_TOF)
# define DEV_TYPE       "D1Mini"
# define FNC_TYPE       "ToF"
# define H_SWITCH        H_FALSE
# define H_DS1820        H_FALSE
# define H_PIR           H_FALSE
# define H_BUTTON        H_TRUE
# define H_LED           H_TRUE
# define H_RELAY         H_FALSE
# define H_TOF           H_TRUE
# define H_TFT_18        H_FALSE

#elif defined(NODEMCU_PIR)
# define DEV_TYPE       "NODEMCU"
# define FNC_TYPE       "PIR"
# define H_SWITCH        H_FALSE
# define H_DS1820        H_FALSE
# define H_PIR           H_TRUE
# define H_BUTTON        H_TRUE
# define H_LED           H_TRUE
# define H_RELAY         H_FALSE
# define H_TOF           H_FALSE
# define H_TFT_18        H_FALSE

#else
#error "Hey! welche Version???"
#endif

// --------- some hardware stuff  ---------

#if (H_LED == H_TRUE) || (H_RELAY == TRUE)
# define  DIG_WRITE(pin, val)   digitalWrite(pin, val)
# define  DIG_READ(pin)         digitalRead(pin)
# define  DIG_MODE(pin, inout)  pinMode(pin, inout);

# if defined(SONOFF_BASIC_SWITCH) || defined(SONOFF_S20_SWITCH)
#   define H_LED_PIN      13
#   define H_RELAY_PIN    12
//# elif    (DEV_TYPE == "NODEMCU") || (DEV_TYPE == "D1MINI")
# elif    defined(NODEMCU_SWITCH) || defined(NODEMCU_DS1820) || defined(D1MINI_DS1820) || defined(D1MINI_TOF)
#   define H_LED_PIN      2
#   define H_RELAY_PIN    3
# else
#   error   "H_LED_PIN undefined"
# endif
#else     // if (H_LED == FALSE)
# define  H_LED_PIN
# define  H_RELAY_PIN
# define  DIG_WRITE(pin, val)
# define  DIG_READ(pin)
# define  DIG_MODE(pin, inout)
#endif



#if (H_DS1820 == H_TRUE)
# define H_DS1820_PIN     D0
# define H_DS180_MAX_DEV  10
#endif


#if (H_BUTTON == H_TRUE)
# define H_BUTTON_PIN     0
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

#if (H_DBG == H_TRUE)
  #define DBG(x)            Serial.print(x);
  #define DBGF(x)           {Serial.print( millis() ); Serial.print(" File: " ); Serial.print (__FILE__); Serial.print( " Line:");Serial.print (__LINE__); Serial.print( ": -> " ); Serial.print( x ); Serial.println( " <-" );}
  #define DBGLN(x)          {Serial.print( " ---> " ); Serial.println( x );}
  #define DBGL(x)           {Serial.print( " ---> " ); Serial.print( x );}
  #define DBGNL(x)          Serial.println( x );
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
#define STATUS_HASH_ERR 8
#define STATUS_LED      16

#define CFGSTART              0         // startadress og cfg-block in EEPROM
#define DEFAULT_LED           H_TRUE

#define DEFAULT_SSID          "unknown"
#define DEFAULT_PASSWORD      "unknown"

#define DEFAULT_HOSTNAME      "No-Name"
#define DEFAULT_APNAME        "ESPnet"
#define DEFAULT_MEASCYCLE     150       // Measurement cycle in sec.
#define DEFAULT_PAGERELOAD    10        // page reload cycle in sec.
#define DEFAULT_APTIMEOUT     300       // stay for this in AP-Mode before
                                        // retrying STA-Mode again

#if (H_SWITCH == H_FALSE)
  #define DEFAULT_TRANSCYCLE    300     // transmit cycle to server in sec.
#else
  #define DEFAULT_TRANSCYCLE    3600
#endif

#define DEFAULT_SERVICE       "/DummyServ.php/"
#define DEFAULT_SERVER        "192.168.1.6"
// --------- some software stuff  ---------


// -----------------------------------------------------
#endif /* #ifndef __SETTINGS_H */

/*
  history
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
  18.01.21  FNC_TYPE isnt displayed in headline of status page anymore
  18.01.21  Version is only displayed in hardline og status page
  18.01.21  first messge is send after going into MODE_STA
  17.01.21  ToF sensor added
  17.01.21  if cfg.TransmitCycle is 0 nothing is send to the Server
            MAC-Address is used for identification and is displayed during
            boot. MACAddress is appended to the device name instead of the
            Chip-Number because this number is not realy unique, espressif
            stated

  20.04.20  V1.30 from now we use platformio instead of the Arduiono stuff
            therefore we dont need the defines NODEMCU_SWITCH and so an.
            this is now in with platformio.ini
  06.11.19  working on:
            after reboot send a message immendently
            send a long message with all infos that are needed on server

  16.04.19  V1.1x should send status to the server in adjustable time
            Versions and Hardware sitch should be clarified
            why is there a delay after the server responce in DoNormStuff in Homa_xxx.ino?
  23.02.19  first Version wirh new Version management. Released for Sonoff S20 and Sonoff Basic
  19.01.19  V0.04 first version to re released
            debuging and error-check for all versions is to be done

  ToDos:
            - statusmeldung an server senden
            - LED dauer an in blinken einbauen

  Errors:
            - warum werden 5 bewsiteausliefrungen gezaehlt wenn Seite vom iPhone aufgerufen wird
            - Warum regiert WLAN schlecht bis garnicht wenn DBG == H_FASLE
            - OTA funktioniert aof nicht  (vor allem SB und S20). Scheinbar hilft es machnmal das
              Board im bootloader zu straten und gleich wieder aus und einzuschalten
            - warum kommt 404: not found URI: /favicon.ico
*/

/* Problem:
    ich kann nicht fuer beliebige Boards beliebige z.B. Relay konfigurationen
    machen. Beim NodeMcu würde ich aber gerne alle Hardwarekombinationen machen koennen.
    Schon allien zu Testzwecken. Aber das Konzept ist aus erst rund wenn das geht. Und
    das war ja Sinn des Konzepts.
    Nochmal im alten HWDOS nachsehen. Ist im Quellcode nun Bezug auf Geraet oder auf H_-Schalter?
    */

/*
  Config.h

  ToDos:    -
    - append DEV_TYPE to cfgData. If the DEV_TYPE is changed put the reqired defaults into cfgData
    - char arrays in cfgData should be changed into String

  hints:    -
*/
#include <Arduino.h>

#ifndef _CONFIG_S_
#define _CONFIG_S_


#include  "Settings.h"

typedef struct {
  int   vald;
  int   LED;                     // 0=no configuration, 1=valid configuration
//  int   dummy;                 // for hash testings
  char  SSID[MAXCHAR+4];         // SSID of WiFi
  char  password[MAXCHAR+4];     // Password of WiFi
  char  hostname[MAXCHAR+4];     // hostname
  char  APname[MAXCHAR+4];       // Access point name
  char  MACAddress[MAXCHAR+4];   // holds MAC
  char  ChipID[MAXCHAR+4];
  char  LocalIP[MAXCHAR+4];
  char  server[MAXCHAR+4];
  char  port[MAXCHAR+4];
  long  MeasuringCycle;
  long  TransmitCycle;
  long  PageReload;
  long  hash;                        // hash, must be the very last value
} cfgData_t;

typedef struct {
  long  blinkmode;
  long  blinktime;
  int   status;
  int   wifi;
  long  uptime;
  long  MeasuringCycle;
  long  TransmitCycle;
  long  CntGoodTrans;
  long  CntBadTrans;
  long  CntPageDelivered;
  long  CntMeasCyc;
  long  DspTimeout;
#if (S_RELAY == true)
  long  ontime;
  long  offtime;
  long  cycles;
#endif  
} sysData_t;

  extern  cfgData_t cfgData;
  extern  sysData_t sysData;

  void eraseConfig(void);
  void SaveConfig(void);
  void loadConfig(void);
  void SetToDefault(void);
  long CalcHashConfig(void);
  int TestHashConfig(void);
  
#define SEND_AFTER_BOOT_SEC   5

#endif //_CONFIG_S_

/*
  Config.h

  ToDos:    -
    - append DEV_TYPE to cfgData. If the DEV_TYPE is changed put the reqired defaults into cfgData
    - char arrays in cfgData should be changed into String

  hints:    -
*/

#ifndef _CONFIG_H_
#define _CONFIG_H_

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
  char  LocalIP[MAXCHAR+4];
  char  fixip[MAXCHAR+4];       // if fixed IP is used store it here
  char  server[MAXCHAR+4];
  char  service[MAXCHAR+4];
  long  MeasuringCycle;
  long  TransmitCycle;
  long  PageReload;
  long  APTimeout;
  long  hash;                        // hash, must be the very last value
} cfgData_t;

typedef struct {
  int   mode;
  long  blinkmode;
  long  blinktime;
  int   status;
  int   wifi;
  long  MeasuringCycle;
  long  MeasuringReloadCycle;
  long  TransmitCycle;
  long  CntGoodTrans;
  long  CntBadTrans;
  long  CntPageDelivered;
  long  CntMeasCyc;
  long  APTimeout;
  long  DspTimeout;
} sysData_t;

  extern  cfgData_t cfgData;
  extern  sysData_t sysData;

  void eraseConfig(void);
  void saveConfig(void);
  void loadConfig(void);

#define SEND_AFTER_BOOT_SEC   5

#endif //_CONFIG_H_

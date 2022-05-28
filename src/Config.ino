/*;lib_extra_dirs = d:\Projects\ESPNode\lib_TOF\
hallo
  Config.ino

  !!! for history see end of file !!!

  ToDos:

  hints:    ???
*/

#include  "Settings.h"
#include  "WebServer.H"
#include  "Config.h"

#include <EEPROM.h>

cfgData_t cfgData;
sysData_t sysData;

void EraseConfig() {
  // Reset EEPROM bytes to '0' for the length of the data structure
  DBGF( "EraseConfig()");
  EEPROM.begin(sizeof(cfgData));
  for (int i = CFGSTART; i < (int)sizeof(cfgData); i++) {
    EEPROM.write(i, 0);
  }
  delay(200);
  EEPROM.commit();
  EEPROM.end();
}

void SaveConfig() {
  // Save configuration from RAM into EEPROM
  cfgData.hash = CalcHashConfig();
  DBGF( "SaveConfig()");
  DBGLN("EEPROM.begin()");
  EEPROM.begin(sizeof(cfgData));
  DBGLN("EEPROM.put()");
  EEPROM.put( CFGSTART, cfgData );
  delay(200);
  DBGLN("EEPROM.commit()");
  EEPROM.commit(); // Only needed for ESP8266 to get data written
  DBGLN("EEPROM.end()");
  EEPROM.end();                         // Free RAM copy of structure
  delay(500);     // wird nach EEPROM.end scheinbar gebraucht sonst kommt der WTD
  DBGLN(cfgData.hash);
}

void LoadConfig() {
  // Loads configuration from EEPROM into RAM

  DBGF( "LoadConfig()")

  EEPROM.begin(sizeof(cfgData));
  EEPROM.get( CFGSTART, cfgData );
  EEPROM.end();
  sysData.MeasuringCycle = SEND_AFTER_BOOT_SEC; //cfgData.MeasuringCycle;
  sysData.TransmitCycle = SEND_AFTER_BOOT_SEC+1;
  sysData.APTimeout = DEFAULT_APTIMEOUT;
  sysData.blinkmode = BLKMODEOFF;
}

void SetToDefault(){
  DBGF( "SetToDefault()" );
  // CHN:
  EraseConfig();
  cfgData.LED = DEFAULT_LED;
  strcpy (cfgData.SSID, DEFAULT_SSID);
  strcpy (cfgData.password,DEFAULT_PASSWORD);
  strcpy (cfgData.hostname, DEFAULT_HOSTNAME);
  strcpy (cfgData.APname, DEFAULT_APNAME);
  strcpy (cfgData.MACAddress, (WiFi.macAddress()).c_str());
  cfgData.LocalIP[0]=0;
  cfgData.fixip[0]=0;
  strcpy (cfgData.server, DEFAULT_SERVER);
  strcpy (cfgData.service, DEFAULT_SERVICE);
  cfgData.MeasuringCycle = DEFAULT_MEASCYCLE;
  cfgData.TransmitCycle = DEFAULT_TRANSCYCLE;
  cfgData.PageReload  = DEFAULT_PAGERELOAD;
  cfgData.APTimeout = DEFAULT_APTIMEOUT;
  cfgData.hash = CalcHashConfig();
  SaveConfig();
}


long CalcHashConfig(){
  int i;
  long hash = 0;
  uint8_t *ptr=(uint8_t*)&cfgData;

  DBGF( "CalcHashConfig()")

  for (i=CFGSTART; i<((int)sizeof(cfgData)-(int)sizeof(cfgData.hash)); i++){
    hash += *ptr++ * i + 1;
  }
  return hash;
}

int TestHashConfig(){
  DBGF( "TestHashConfig()")
  LoadConfig();
  return ((CalcHashConfig()-cfgData.hash) ? 0:1);
}

/*
  history
  20.04.20  V1.30 from now we use platformio instead of the Arduiono stuff
            many changes for debugging and some type castings
            SEND_AFTER_BOOT implemented in sysData
            setToDefaults erases the EEPROM at first
  18.04.20  some debug output changed
            send a message after boot
  08.03.19  server and service now variable
  23.02.19  first Version wirh new Version management. Released for Sonoff S20 and Sonoff Basic
  19.01.19  V0.04 first version to re released
            debuging and error-check for all versions is to be done

  19.02.18  first version
            routines are from http://www.kriwanek.de/index.php/de/homeautomation/esp8266/364-eeprom-f%C3%BCr-parameter-verwenden

*/
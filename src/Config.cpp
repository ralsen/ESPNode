/*;lib_extra_dirs = d:\Projects\ESPNode\lib_TOF\
hallo
  Config.ino

  !!! for history see settings.h !!!

  ToDos:

  hints:    ???
*/

#include <Arduino.h>
#include  "Settings.h"
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
  sysData.blinkmode = BLKMODEOFF;
  sysData.DspTimeout = 100;
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
  //FOL strcpy (cfgData.MACAddress, (WiFi.macAddress()).c_str());
  cfgData.LocalIP[0]=0;
  cfgData.fixip[0]=0;
  strcpy (cfgData.server, DEFAULT_SERVER);
  strcpy (cfgData.service, DEFAULT_SERVICE);
  cfgData.MeasuringCycle = DEFAULT_MEASCYCLE;
  cfgData.TransmitCycle = DEFAULT_TRANSCYCLE;
  cfgData.PageReload  = DEFAULT_PAGERELOAD;
  //cfgData.hash = CalcHashConfig();
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

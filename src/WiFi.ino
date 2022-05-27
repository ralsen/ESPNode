/*
  WiFi.ino

  !!! for history see end of file !!!

  ToDos:    testing why the DNS isn´t working when changing the device name (needs a reset to work)

  hints:    ???
*/

#include  "Settings.h"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266WiFiMulti.h>
#include <stdio.h>
#include "WiFi.h"

ESP8266WiFiMulti wifiMulti;     // Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti'
// for AP-Mode see here
// https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/examples/WiFiAccessPoint/WiFiAccessPoint.ino

void WiFiStartAP(){
  DBGF( "WiFiStartAP()" )
  LEDControl(BLKMODEON, BLKWIFIAP);
  DBGLN( "Disconnect and WIFI_OFF" )
  WiFi.disconnect();
  delay(2*DELAY_WIFI_TRY);
  WiFi.mode(WIFI_OFF);
  delay(2*DELAY_WIFI_TRY);

  DBGLN( "Start WIFI in AP-Mode" )
  WiFi.mode(WIFI_AP);
  DBGLN("WiFi.mode is done");
  WiFi.softAP(cfgData.APname);
  delay(DELAY_WIFI_TRY);
  server.begin();
  DBGLN(WiFi.softAPIP());
}

int WiFiStartClient(){
  int err = 0;
  char wheel[] = {'-', '\\', '|', '/'};

  DBGF ("WiFiStartClient()" )
  LEDControl(BLKMODEON, BLKWIFISTA);
  DBGLN( "Disconnect and WIFI_OFF" )
  WiFi.disconnect();
  delay(2*DELAY_WIFI_TRY);
  WiFi.mode(WIFI_OFF);      // scheint wichtig zu sein wenn Modeis geswitched werden, Umschaltung funktioniert sonst nicht
  delay(2*DELAY_WIFI_TRY);

  DBGLN( "Start WIFI as client" )
  WiFi.mode(WIFI_STA);
  delay(DELAY_WIFI_TRY);
  String FullName=(String)cfgData.hostname + "_" + (String)cfgData.MACAddress;
  FullName.replace(":", "_");
  WiFi.hostname(FullName);

  WiFi.begin(cfgData.SSID, cfgData.password);
  wifiMulti.addAP(cfgData.SSID, cfgData.password);
  DBGLN( ("trying to connect to:  ") );
  DBGL( "SSID: " );
  DBGLN( cfgData.SSID );
  DBGL( "PASS: " );
  DBGLN( cfgData.password );
  while ( wifiMulti.run() != WL_CONNECTED ) {
    delay(DELAY_WIFI_TRY);
    Serial.print(wheel[err%4]);
    Serial.print("\b");
    err++;
    if( err > MAX_WIFI_TRY ){
      LEDControl(BLKMODEON, BLKALLERT);
      Serial.println( "" );
      WiFi.mode(WIFI_OFF);
      return false;
    }
  }

  Serial.println("");
  Serial.print("connected with: ");
  Serial.println(WiFi.SSID());
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.print("Signal Strength (RSSI): ");
  Serial.print(getWifiQuality());
  Serial.println("%");

  // CHN: Tasmota macht hier sowas mdns_begun = MDNS.begin(my_hostname) mdns.update wird dort nicht benutzt
  // vielleicht kommen die Netzwerkprobleme daher?


  DBGL("mDNS-Name is")
  DBGLN(FullName)
  DBGLN("\r\n")

  if (MDNS.begin(FullName)){
    DBGLN("mDNS responder started");
    MDNS.update();
  }
  else{
    DBGLN("mDNS responder failed");
  }

  LEDControl(BLKMODEOFF, -1);
  return true;
}

// converts the dBm to a range between 0 and 100%
int8_t getWifiQuality() {
  int32_t dbm = WiFi.RSSI();
  if (dbm <= -100) {
    return 0;
  } else if (dbm >= -50) {
    return 100;
  } else {
    return 2 * (dbm + 100);
  }
}

/*
  history:
  --------------------- V2.0a
  21.11.21  lot of changes for 2.xx, made for arduino IDE and 
            many changes to improve stability, performance etc.

  20.04.20  V1.30 from now we use platformio instead of the Arduiono stuff
            ESP8266_WiFiMulti is used
            Devicename in URL is now the name plus the Chip-ID
            changed the WiFi-login to mDNS
  23.02.19  first Version wirh new Version management. Released for Sonoff S20 and Sonoff Basic
  19.02.19  problem below solved
  16.02.19  DNS is working except when changing the device name and changes the name after a while
            to "unknownMACADRESSface"
  19.01.19  V0.04 first version to re released
            debuging and error-check for all versions is to be done

  23.02.18  version 1.05
            WifiScan removed here because it isn't a Mode change in sysData.Wifi
  22.02.18  version 1.04
            network scan added
  21.02.18  first version

 */

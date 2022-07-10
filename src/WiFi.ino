/*
  WiFi.ino

  !!! for history see settings.h !!!

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


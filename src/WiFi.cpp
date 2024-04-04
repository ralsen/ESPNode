/*
  WiFi.ino

  !!! for history see settings.h !!!

  ToDos:    testing why the DNS isn´t working when changing the device name (needs a reset to work)

  hints:    ???
*/
#include <Arduino.h>
#include "settings.h"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266WiFiMulti.h>
#include <stdio.h>
#include "WiFi.h"
#include "tft.h"
#include "timer.h"
#include "WebServer.h"

ESP8266WiFiMulti wifiMulti;     // Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti'
extern ESP8266WebServer server;

// for AP-Mode see here
// https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/examples/WiFiAccessPoint/WiFiAccessPoint.ino

void WiFiStartAP(){
  DBGF("WiFiStartAP()")
  #if(S_TFT_18 == true)
  tft_initR(INITR_BLACKTAB);
  tft_fillScreen(ST77XX_RED);
  tft.setTextColor(ST77XX_WHITE, ST77XX_RED);
  tft.setTextSize(1);
  tft_setCursor(0, 0);
  #endif
  LEDControl(BLKMODEON, BLKWIFIAP);
  DBGLN("Disconnect and WIFI_OFF")
  WiFi.disconnect();
  delay(2*DELAY_WIFI_TRY);
  WiFi.mode(WIFI_OFF);
  delay(2*DELAY_WIFI_TRY);

  DBGLN("Start WIFI in AP-Mode")
  #if(S_TFT_18 == true)
  tft_println("Start WIFI in AP-Mode");
  #endif
  WiFi.mode(WIFI_AP);
  DBGLN("WiFi.mode is done");
  #if(S_TFT_18 == true)
  tft_println("wifi.mode is done.");
  #endif
  WiFi.softAP(cfgData.APname);
  delay(DELAY_WIFI_TRY);
  #if(S_TFT_18 == true)
  tft_print("SSID: ");
  tft_println(String(cfgData.APname));
  #endif
  DBGLN(WiFi.softAPIP());
  //tft_println(WiFi.softAPIP());
}

int WiFiStartClient(){
  DBGF ("WiFiStartClient()")
  int err = 0;
  char wheel[] = {'-', '\\', '|', '/'};

  LEDControl(BLKMODEON, BLKWIFISTA);
  DBGLN("Disconnect and WIFI_OFF")
  WiFi.disconnect();
  delay(2*DELAY_WIFI_TRY);
  WiFi.mode(WIFI_OFF);      // scheint wichtig zu sein wenn Modeis geswitched werden, Umschaltung funktioniert sonst nicht
  delay(2*DELAY_WIFI_TRY);

  DBGLN("Start WIFI as client")
  WiFi.mode(WIFI_STA);
  delay(DELAY_WIFI_TRY);
  String FullName=(String)cfgData.hostname + "_" + (String)cfgData.MACAddress;
  FullName.replace(":", "_");
  WiFi.hostname(FullName);

  //WiFi.begin(cfgData.SSID, cfgData.password);
  wifiMulti.addAP(cfgData.SSID, cfgData.password);
  DBGLN(("trying to connect to:  "));
  DBGL("SSID: ");
  DBGNL(cfgData.SSID);
  DBGL("PASS: ");
  DBGNL(cfgData.password);
  while (wifiMulti.run() != WL_CONNECTED) {//WiFi.status() != WL_CONNECTED) { 
    delay(DELAY_WIFI_TRY);
    Serial.print(wheel[err%4]);
    Serial.print("\b");
    err++;
    Serial.println(err);
    if(err > MAX_WIFI_TRY){
      LEDControl(BLKMODEON, BLKALLERT);
      Serial.println("");
      WiFi.mode(WIFI_OFF);
      return false;
    }
  }

  Serial.print("connected with: ");
  Serial.println(WiFi.SSID());
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  DBGL("Signal Strength (RSSI): ");
  DBG(WiFi.RSSI());
  DBGNL(" dBm");
  
  // CHN: Tasmota macht hier sowas mdns_begun = MDNS.begin(my_hostname) mdns.update wird dort nicht benutzt
  // vielleicht kommen die Netzwerkprobleme daher?


  DBGL("mDNS-Name is: ")
  DBGNL(FullName)

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


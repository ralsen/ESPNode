/*
  timer.ino

  !!! for history see settings.h !!!

  ToDos:    ???

  hints:    ???
*/
#include <Arduino.h>
#include  "Settings.h"
#include  "Config.h"

#include  "timer.h"

#if(S_DS1820 == true)
  #include  "DS1820.h"
#endif

#if(S_TOF == true)
  #include  "ToF.h"
#endif

#include  "version.h"
#include  <Ticker.h>

#include  <string.h>

int key;            // holds the button codes for internal use
static int  oldKey;           // counts the ticks how long a key is pressed
static long blkcnt;
int LEDCrit;
volatile bool doMeasuring = true; // at first signalize that we have to measure


// count object for ISR
Ticker TIs_Uptime;
Ticker TIs_TransmitCycle;
Ticker TIs_MeasuringCycle;
Ticker TIms_DspTimeout;
Ticker TIms_Key;
Ticker TIms_LED;
#if (S_RELAY == true)
Ticker TIs_Relais;
#endif

/* ---------------------------------------------------------------
 *
 *  intialize the variables for the key handling
 *
 *  only one key is supported yet. for the 4 channel Sonoff
 *  there is to do more
 *
 * --------------------------------------------------------------- */
void Init_Key()
{
 // Setup the button with an internal pull-up :
  pinMode(S_BUTTON_PIN,INPUT_PULLUP);
  key = KEY_NO;           // no key is pressed
  blkcnt = BLKMODEOFF;
  oldKey = KEY_NO;
  LEDCrit = false;
}


void TISms_LED()
{
  if (blkcnt) blkcnt--;

  while (LEDCrit);

  if (!blkcnt){
    if(sysData.blinkmode == BLKMODEFLASH){
      sysData.blinktime == BLKFLASHOFF ? sysData.blinktime = BLKFLASHON : sysData.blinktime = BLKFLASHOFF;
    }

    if(sysData.blinkmode != BLKMODEOFF){
      blkcnt = sysData.blinktime;
      //if(!((sysData.WifiRes == MODE_STA) && (cfgData.LED == false)))
        DIG_WRITE (S_LED_PIN, !DIG_READ(S_LED_PIN));
    }
  }
}

void TISms_Key(){
  key = !DIG_READ(S_BUTTON_PIN);
  if (key == oldKey)
    key = KEY_NO;
  else
    oldKey = key;
}

void TISms_DspTimeout(){
  if(sysData.DspTimeout) sysData.DspTimeout--;
}

void TISs_Uptime(){
  sysData.uptime++;
}

void TISs_TransmitCycle(){
  if(sysData.TransmitCycle) sysData.TransmitCycle--;
}

//  ISR_count() counts the life tickers
# if (S_RELAY == true)
void TISs_Relais(){
  if(DIG_READ(S_RELAY_PIN))
    sysData.ontime++;
  else
    sysData.offtime++;
}
# endif

void TISs_MeasuringCycle(){
  if(sysData.MeasuringCycle){
    sysData.MeasuringCycle--;
  }
  else{
    sysData.MeasuringCycle = cfgData.MeasuringCycle;
    doMeasuring = true;
  }
}

void LEDControl(long mode, long time){
  //DBGF("LEDControl()");

  LEDCrit = true;
  
  if(!strcmp(DEV_TYPE, "NODEMCU"))
    DIG_WRITE (S_LED_PIN, HIGH);

  if(!strcmp(DEV_TYPE, "SONOFF_BASIC"))
    DIG_WRITE (S_LED_PIN, !DIG_READ(S_RELAY_PIN));

  if(!strcmp(DEV_TYPE, "SONOFF_S20"))
    DIG_WRITE (S_LED_PIN, HIGH);

  if(!strcmp(DEV_TYPE, "D1MINI"))
    DIG_WRITE (S_LED_PIN, HIGH);

  sysData.blinktime = time;
  if(cfgData.LED)
    sysData.blinkmode = mode;
  else sysData.blinkmode = BLKMODEOFF;

  LEDCrit = false;

}

TimeDB::TimeDB(String server, String zone)
{ //FOL
  configTime(MY_TZ, MY_NTP_SERVER);   // --> Here is the IMPORTANT ONE LINER needed in your sketch!
  time(&e_now);                       // read the current time
  localtime_r(&e_now, &tm_t);         // update the structure tm with the current time
  
}

String TimeDB::zeroPad(int number) {
  if (number < 10) {
    return "0" + String(number);
  } else {
    return String(number);
  }
}

String TimeDB::showTime(){
  String str;

  time(&e_now);                       // read the current time
  localtime_r(&e_now, &tm_t);           // update the structure tm with the current time
  str = ("year:");
  str += String(tm_t.tm_year + 1900);  // years since 1900
  str += "\tmonth:";
  str += String(tm_t.tm_mon + 1);      // January = 0 (!)
  str += "\tday:";
  str += String(tm_t.tm_mday);         // day of month
  str += "\thour:";
  str += String(tm_t.tm_hour);         // hours since midnight  0-23
  str += "\tmin:";
  str += String(tm_t.tm_min);          // minutes after the hour  0-59
  str += "\tsec:";
  str += String(tm_t.tm_sec);          // seconds after the minute  0-61*
  str += "\twday";
  str += String(tm_t.tm_wday);         // days since Sunday 0-6
  if (tm_t.tm_isdst == 1)             // Daylight Saving Time flag
    str += "\tDST";
  else
    str += "\tstandard";
  str += "\n\r";
  return str;
}


String TimeDB::getTimestr(){
  String str;

  time(&e_now);                       // read the current time
  localtime_r(&e_now, &tm_t);           // update the structure tm with the current time
  str = zeroPad(tm_t.tm_mday)+"."+zeroPad(tm_t.tm_mon + 1)+"."+zeroPad(tm_t.tm_year + 1900)+" - ";
  str+= zeroPad(tm_t.tm_hour)+":"+zeroPad(tm_t.tm_min)+":"+zeroPad(tm_t.tm_sec);
  return str;
}

/*
  timer.ino

  !!! for history see end of file !!!

  ToDos:    ???

  hints:    ???
*/

#include  "Settings.h"
#include  "timer.h"
#include  "DS1820.h"
#include  "version.h"
#include  "Config.h"
#include  <Ticker.h>

int key;            // holds the button codes for internal use
static int  tiki;           // counts the ticks how long a key is pressed
static long blkcnt;
int LEDCrit;

// count object for ISR
Ticker CntTicks;
Ticker CntmTicks;

long uptime;
#if (H_RELAY == H_TRUE)
long ontime;
long offtime;
long cycles;
#endif

long Intervall;

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
  pinMode(H_BUTTON_PIN,INPUT_PULLUP);
  sysData.mode = MODE_STA;       // normal mode at start
  key = KEY_NO;           // no key is pressed
  blkcnt = BLKMODEOFF;
  tiki = KEY_WAIT;
  LEDCrit = H_FALSE;
}


void milli_ISR()
{
  key = !DIG_READ(H_BUTTON_PIN);

  if( key ){
    if( tiki ) tiki--;
  }
  else {
    if( tiki == 0 ){
      if( (sysData.mode == MODE_AP) || (sysData.mode == MODE_STA) ){
        sysData.mode = (sysData.mode == MODE_AP) ? MODE_CHG_TO_STA:MODE_CHG_TO_AP;
      }
    }
    tiki = KEY_WAIT;
  }

  if (blkcnt) blkcnt--;

  while (LEDCrit);

  if (!blkcnt){
    if(sysData.blinkmode == BLKMODEFLASH){
      sysData.blinktime == BLKFLASHOFF ? sysData.blinktime = BLKFLASHON : sysData.blinktime = BLKFLASHOFF;
    }

    if( sysData.blinkmode != BLKMODEOFF ){
      blkcnt = sysData.blinktime;
      if(!((sysData.mode == MODE_STA) && (cfgData.LED == H_FALSE)))
        DIG_WRITE (H_LED_PIN, !DIG_READ(H_LED_PIN));
    }
  }
}



//  ISR_count() counts the life tickers
void sec_ISR()
{
# if (H_RELAY == H_TRUE)
  if( DIG_READ(H_RELAY_PIN) )
    ontime++;
  else
    offtime++;
# endif

/*DBGL("LED: ")
DBGF(cfgData.LED)
DBGL("blink: ")
DBGF(sysData.blinkmode)
DBGL("Mode: ")
DBGF(sysData.mode)
*/
  uptime++;
  if( sysData.APTimeout ) sysData.APTimeout--;

  if( sysData.TransmitCycle ) sysData.TransmitCycle--;

  if(sysData.mode == MODE_STA){
    if( sysData.MeasuringCycle )
      sysData.MeasuringCycle--;
      else{
        sysData.MeasuringCycle = cfgData.MeasuringCycle;
        #if( H_DS1820 == H_TRUE )
        DS1820_Measuring();
        #endif
        #if( H_TOF == H_TRUE)
        ToFDistance();
        #endif
      }
  }
}


void LEDControl(long mode, long time){
  DBGF( "LEDControl()" );

  LEDCrit = H_TRUE;
  #if defined(SONOFF_BASIC_SWITCH)
    DIG_WRITE (H_LED_PIN, !DIG_READ(H_RELAY_PIN));
  #elif defined(SONOFF_S20_SWITCH)
    DIG_WRITE (H_LED_PIN, HIGH);
  #elif defined (NODEMCU_DS1820)
    DIG_WRITE (H_LED_PIN, HIGH);
  #elif defined(D1MINI_ToF)
    DIG_WRITE (H_LED_PIN, HIGH);
  #elif defined(D1MINI_DS1820)
    DIG_WRITE (H_LED_PIN, HIGH);
  #endif

  sysData.blinktime = time;
  if(cfgData.LED)
    sysData.blinkmode = mode;
  else sysData.blinkmode = BLKMODEOFF;

  LEDCrit = H_FALSE;

}

TimeDB::TimeDB(String server, String zone)
{
  Serial.print("getting time from: ");
  Serial.print(server);
  Serial.print(" / Timezone: ");
  Serial.println(zone);
  configTime(MY_TZ, MY_NTP_SERVER); // --> Here is the IMPORTANT ONE LINER needed in your sketch!
  time(&e_now);                       // read the current time
  localtime_r(&e_now, &tm_t);           // update the structure tm with the current time
}


time_t TimeDB::getTime(){
// get the time from whereever you want and pit it in here
  Serial.println("gettime in TimeDB");
  return 0;
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
  str += tm_t.tm_year + 1900;  // years since 1900
  str += "\tmonth:";
  str += tm_t.tm_mon + 1;      // January = 0 (!)
  str += "\tday:";
  str += tm_t.tm_mday;         // day of month
  str += "\thour:";
  str += tm_t.tm_hour;         // hours since midnight  0-23
  str += "\tmin:";
  str += tm_t.tm_min;          // minutes after the hour  0-59
  str += "\tsec:";
  str += tm_t.tm_sec;          // seconds after the minute  0-61*
  str += "\twday";
  str += tm_t.tm_wday;         // days since Sunday 0-6
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
 
/*
  history
  --------------------- V2.0a
  21.11.21  lot of changes for 2.xx, made for arduino IDE and 
            many changes to improve stability, performance etc.
  --------------------- V1.4
  29.01.21  LEDControl() added
  19.01.21  D1MINI_ToF added
  --------------------- V1.4

  06.05.20  V1.30a: new WiFi-state machine, sending a message after boot
  20.04.20  V1.30 from now we use platformio instead of the Arduiono stuff
            sysData.TransmitCycle is always decremented
  26.04.19  V1.11 removed an error on switching the LED. Sonoff-Basic has no
            LED in parallel with the relay so we have to use the same one for
            blinking and relais state. On Sonoff S20 the LED is dedicated for
            blinking and the relais state is indicated with the blue (sometimes red)
            LED parallel to the relays
  03.03.19  V1.02: renamed TempLoop to MeasuringLoop which runs from now in sec_ISR.
                    renamed ISR_count to sec_ISR which contains now Measuring and transmit counters
  23.02.19  first Version wirh new Version management. Released for Sonoff S20 and Sonoff Basic
  19.01.19  V0.04 first version to re released
            debuging and error-check for all versions is to be done
  16.12.18  porting/modificatios to/for SensClient
  19.05.18  Version 1.14
            all lifwtimw stuff moved to timer.ino/timer.h
  17.05.18  version 1.13
            lifetimer added
  27.03.18  version 1.11
            Sonoff-Basic needs different LED control
  21.02.18  back to time control :-P. there are only 2 modes: AP and STA.
            thats easy toggle with key pressed time
  20.02.18  key detection via key count not time pressed
  16.02.18  first version
*/

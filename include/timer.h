/*
  timer.h

  !!! for history see end of file !!!

  ToDos:    -


  hints:    -
*/

#ifndef _TIMER_H_
#define _TIMER_H_

#include  <Ticker.h>
#include  <TimeLib.h> // https://github.com/PaulStoffregen/Time  

#define KEY_NO      0         // Key_codes stored in Button 0 = No key pressed
#define KEY_1       1         // 4 keys for the 4 channel sonoff
#define KEY_2       2
#define KEY_3       3
#define KEY_4       4

#define KEY_WAIT    200

extern int  key;
extern long uptime;
extern long Intervall;

#if (H_RELAY == H_TRUE)
extern long ontime;
extern long offtime;
extern long cycles;
#endif

extern  Ticker CntTicks;
extern  Ticker CntmTicks;

void milli_ISR(void);
void sec_ISR(void);
void Init_Key(void);
void LEDControl(long mode, long time);

class TimeDB{
  public:
    TimeDB(String apiKey);
    time_t getTime();
    String zeroPad(int number);
    String getTimestr();

  private:
    const char* servername = "api.timezonedb.com";  // remote server we will connect to
    String myApiKey;
    String myLat;
    String myLon;
}; 
#endif // _TIMER_H_

/*
  history:
  --------------------- V2.0a
  21.11.21  lot of changes for 2.xx, made for arduino IDE and 
            many changes to improve stability, performance etc.
  
  20.04.20  V1.30 from now we use platformio instead of the Arduiono stuff
  03.03.19  V1.02: renamed ISR_count to sec_ISR which contains now Measuring and transmit counters
  23.02.19  first Version wirh new Version management. Released for Sonoff S20 and Sonoff Basic
  19.01.19  V0.04 first version to re released
            debuging and error-check for all versions is to be done

  16.12.18  porting/modificatios to/for SensClient
  19.05.18  Version 1.14
            all lifwtimw stuff moved to timer.ino/timer.h
  16.02.18  first version

 */

/*
  timer.h

  !!! for history see settings.h !!!

  ToDos:    -


  hints:    -
*/

#ifndef _TIMER_H_
#define _TIMER_H_

#include <time.h>
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
    TimeDB(String server, String zone);
    time_t getTime();
    String zeroPad(int number);
    String getTimestr();
    String showTime();

  private:
    time_t e_now;                         // this is the epoch
    tm tm_t;                              // the structure tm holds time information in a more convient way
}; 
#endif // _TIMER_H_

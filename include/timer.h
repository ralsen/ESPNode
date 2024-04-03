/*
  timer.h

  !!! for history see settings.h !!!

  ToDos:    -


  hints:    -
*/
#include <Arduino.h>

#ifndef _TIMER_S_
#define _TIMER_S_

#include <time.h>
#include  <Ticker.h>
//#include  <TimeLib.h> // https://github.com/PaulStoffregen/Time  

#define KEY_NO      0         // Key_codes stored in Button 0 = No key pressed
#define KEY_1       1         // 4 keys for the 4 channel sonoff
#define KEY_2       2
#define KEY_3       3
#define KEY_4       4

#define KEY_WAIT    200

void TISms_LED(void);
void TISms_DspTimeout(void);
void TISms_Key(void);
void TISs_Uptime(void);
void TISs_TransmitCycle(void);
void TISs_MeasuringCycle(void);
void TISs_Relais(void);

extern int  key;

//FOL Reihenfolge von Prototyping, externe variablen etc. klaeren
void LEDControl(long, long);

extern  Ticker TIs_Uptime;
extern  Ticker TIs_TransmitCycle;
extern  Ticker TIs_MeasuringCycle;
extern  Ticker TIms_DspTimeout;
extern  Ticker TIms_Key;
extern  Ticker TIms_LED;
#if (S_RELAY == S_TRUE)
extern  Ticker TIs_Relais;
#endif

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
#endif // _TIMER_S_

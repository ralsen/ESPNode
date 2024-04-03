/*
  ToFh

  !!! for history see settings.h !!!

  ToDos:    -

  hints:    -
*/
#include <Arduino.h>

#if (S_TOF == S_TRUE)
# ifndef _TOF_S_
# define _TOF_S_

# define TOF_FALSE    0
# define TOF_TRUE     0x5a5a
# define TOF_AVR_CNT  10
# define TOF_INTERVAL 5

  extern int  ToFPresent;
  extern int  ToFRange;
  extern int  ToFAvr;

  int   SetupToF(void);
  void  ToFDistance(void);

# endif // _TOF_S_
#endif //(S_TOF == S_TRUE)

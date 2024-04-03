/*
  log.h

    !!! for history see settings.h !!!

  ToDos:    -

  hints:    -
*/
#include <Arduino.h>
#include  "settings.h"

#ifndef _LOG_S_
#define _LOG_S_

//#include  "timer.h"
#if(S_FS == S_TRUE)
class log_CL{
  public:
    log_CL (String filename, int level);
    void entry (String str);
    String show(void);
    
  private:
    String  logfile;
    int level;
};
    
#endif //_CONFIG_S_
#endif // (S_FS == S_TRUE)
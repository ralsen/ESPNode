/*
  log.h

  !!! for history see end of file !!!

  ToDos:    -

  hints:    -
*/

#ifndef _LOG_H_
#define _LOG_H_

#include  "Settings.h"
#include  "timer.h"

class log_CL{
  public:
    log_CL (String filename, int level);
    void entry (String str);
    String show(void);
    
  private:
    String  logfile;
    int level;
};
    
#endif //_CONFIG_H_

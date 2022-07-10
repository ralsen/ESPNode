/*
  log.ino

  !!! for history see settings.h !!!

  ToDos:    ???

  hints:    pio run --target uploadfs
*/




#include  "settings.h"
#include  <string>
#include  "log.h"
#include  <SPI.h>
#include <LittleFS.h>
#include  "timer.h"

log_CL::log_CL(String filename, int level){
  DBGF("log_CL::log_CL()")
  logfile = filename;
  level = level;
  //return 1;
}

String log_CL::show(){
  DBGF("log_CL::show()")
  File flog = LittleFS.open(logfile, "r");
  String fstr="";
  while(flog.available()){
    fstr += (char)(flog.read());
  }
  flog.close();
  //DBGF(fstr)
  return fstr;
}

void log_CL::entry(String entry){
  DBGF("log_CL::entry(String entry)")
  File flog = LittleFS.open(logfile, "r");
  String fstr="";
  while(flog.available()){
    fstr += (char)(flog.read());
  }
  flog.close();
  flog = LittleFS.open(logfile, "w");
  fstr = TimeDB.getTimestr() + " --> " + entry + "\r\n" + fstr;
  flog.print(fstr.substring(0, MAXLOGSIZE));
  flog.close();
}
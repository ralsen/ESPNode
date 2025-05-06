/*
  log.ino

  !!! for history see settings.h !!!

  ToDos:    ???

  hints:    pio run --target uploadfs
*/


#include <Arduino.h>
#include "settings.h"
#include "Config.h"

#include <string>
#include "log.h"
#include <SPI.h>
#include <LittleFS.h>
#include "timer.h"

#if(S_FS == true)
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
  //Serial.println(fstr);
  return fstr;
}

extern TimeDB TimeServ;

void log_CL::entry(String entry){
  DBGF("log_CL::entry(String entry)")
  DBGLN(entry)
  File flog = LittleFS.open(logfile, "r");
  String fstr = "";
  String tstr = "";
  while(flog.available()){
    fstr += (char)(flog.read());
  }
  flog.close();
  flog = LittleFS.open(logfile, "w");
  tstr = TimeServ.getTimestr() + " :: " + entry;
  fstr = tstr + "\n" + fstr;
  //Serial.println(tstr);
  flog.print(fstr.substring(0, MAXLOGSIZE));
  flog.close();
}
#endif
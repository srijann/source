#ifndef __SPSLOG_H
#define __SPSLOG_H

#include "defs.h"
#include <time.h>

#define LOGFILENAME "SPSERROR.LOG"
#define NOLOGTHRESHOLD 20
#define ABORTTHRESHOLD 100

class SPSLogger {
  char* ProgramName;
  int ErrorCount;
  time_t ErrorTS;
  public:
  SPSLogger(char* ProgName);
  ~SPSLogger();
  void Log(char* FmtString,...);
  void IOError(char *Function,char *FileName=0);
  void Abort();
  bool Restart();
 };

#endif
#ifndef __RECFILTR_H
#define __RECFILTR_H

#include "kwrec.h"
#include "ipcmsg.h"
#define MAXACTIVITYRECORDS 20
#define MAXCHECKRECORDS 20

class AuditFilter {
  int  numRecs;                 // number of records
  ACTIVITY Recs[MAXACTIVITYRECORDS];
  public:
  AuditFilter();
  void Reset() { numRecs=0; };
  void Insert(AUDIT*,TINQ*);
  void PutMsg(IPCMessage&);
 };

class CheckFilter {
  int  numRecs;                 // number of records
  BADCHECK Recs[MAXCHECKRECORDS];
  char MothersName[26];
  public:
  CheckFilter();
  void Reset();
  void Insert(CHECK*,TINQ*);
  void PutMsg(IPCMessage&);
 };


#endif

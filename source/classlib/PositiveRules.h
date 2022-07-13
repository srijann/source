#ifndef __POSITIVERULES_H
#define __POSITIVERULES_H

#include "dbmanrec.h"
#include "checkauthfn.h"
#include "ipcmsg.h"
#include <time.h>

class PositiveRules : public DBManRecord {

  public:

  struct PositiveRulesRecord Data;

  // Init Empty
  PositiveRules();
  // Init from a Record
  PositiveRules(struct PositiveRulesRecord* Data);
  // Init from an IPC Message field
  PositiveRules(FLDNUM fn,IPCMessage* Msg);
  ~PositiveRules();

  virtual int DataLen() { return sizeof(Data); };
  virtual void* DataPtr() { return &Data; };
  virtual int ObjLen() { return sizeof(*this); };
  virtual void FormatNew(const char* ClassName);
  virtual int KeyType();
  virtual int RecType();
 };
#endif












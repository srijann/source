#ifndef __UNVFUNDSRULES_H
#define __UNVFUNDSRULES_H

#include "dbmanrec.h"
#include "checkauthfn.h"
#include "ipcmsg.h"
#include <time.h>

class UnvFundsRules : public DBManRecord {

  public:

  struct UnvFundsRulesRecord Data;

  // Init Empty
  UnvFundsRules();
  // Init from a Record
  UnvFundsRules(struct UnvFundsRulesRecord* Data);
  // Init from an IPC Message field
  UnvFundsRules(FLDNUM fn,IPCMessage* Msg);
  ~UnvFundsRules();

  void calcMinMaxScore(int& minScore,int& maxScore);

  virtual int DataLen() { return sizeof(Data); };
  virtual void* DataPtr() { return &Data; };
  virtual int ObjLen() { return sizeof(*this); };
  virtual void FormatNew(const char* ClassName);
  virtual int KeyType();
  virtual int RecType();
 };
#endif












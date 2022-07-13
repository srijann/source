#ifndef __PosResponse_H
#define __PosResponse_H

#include "dbmanrec.h"
#include "checkauthfn.h"
#include "ipcmsg.h"
#include <time.h>

class PosResponse : public DBManRecord {

  public:

  struct ResponseRecord Data;

  // Init Empty
  PosResponse();
  // Init from a Record
  PosResponse(struct PosResponseRecord* Data);
  // Init from an IPC Message field
  PosResponse(FLDNUM fn,IPCMessage* Msg);
  ~PosResponse();

  virtual int DataLen() { return sizeof(Data); };
  virtual void* DataPtr() { return &Data; };
  virtual int ObjLen() { return sizeof(*this); };
  virtual void FormatNew(const char* ClassName);
  virtual int KeyType();
  virtual int RecType();
 };
#endif












#ifndef __AUTHRULES_H
#define __AUTHRULES_H

#include "dbmanrec.h"
#include "checkauthfn.h"
#include "ipcmsg.h"
#include <time.h>


class AuthRules : public DBManRecord {

  void  TimeAdjust(AuthRules*,AuthRules*,uint);
  void  ProductAdjust(AuthRules*,AuthRules*,uint,uint);
  bool  isTimePeriod(WORD,WORD,unsigned char*,WORD,WORD);
  void* pickRules(uint,void*,void*,void*);
  BYTE  pickAdjustment(WORD,DWORD,struct DTDef*);
  void  Adjust(WORD&,BYTE);
  void  chooseLevel(BYTE& level,
                    BYTE globalLevel,
                    BYTE classLevel=0,
                    BYTE subClassLevel=0);

  public:

  struct AuthRulesRecord Data;

  // Init Empty
  AuthRules();
  // Init from a Record
  AuthRules(struct AuthRulesRecord* Data);
  // Init from an IPC Message field
  AuthRules(FLDNUM fn,IPCMessage* Msg);
  ~AuthRules();

  void ValidateLevels(const char* ValidLevels,char DefaultLevel);
  void Merge(AuthRules* globalRules,AuthRules* classRules,
             AuthRules* subclassRules,uint TimeZone,
             uint ProductGroup=0,uint ProductSubGroup=0,
             IPCMessage* Msg=0);
  bool ApplyProductAdjustments(IPCMessage* Msg);           

  void  FormatNew(const char* ClassName,const char* SubClassName);
  void  FormatNew(const char* ClassName);

  virtual int DataLen() { return sizeof(Data); };
  virtual void* DataPtr() { return &Data; };
  virtual int ObjLen() { return sizeof(*this); };
  virtual int KeyType();
  virtual int RecType();
 };
#endif












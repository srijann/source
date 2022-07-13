#ifndef __MERCHANTREC_H
#define __MERCHANTREC_H

#include "dbmanrec.h"
#include "checkauthfn.h"
#include "ipcmsg.h"
#include <time.h>

class MerchantRec : public DBManRecord {

  public:

  enum Enabler
   {
    MerchantService,
    MerchantSic,
    MerchantState,
    MerchantZip,
    MerchantTimeZone,
    PrivateLabelName,
    ProductClasses,
    FedRegions,
    GUAuthRules,
    GUUnvFundsRules,
    GUDataReq,
    GUPosFileRules,
    GURespMsg,
    VFYAuthRules,
    VFYUnvFundsRules,
    VFYDataReq,
    VFYPosFileRules,
    VFYRespMsg,
    ProductGroup,
    MessageNumber
   };

  struct MerchantRecord Data;

  // Init Empty
  MerchantRec();
  // Init from a Record
  MerchantRec(struct MerchantRecord* Data);
  // Init from an IPC Message field
  MerchantRec(FLDNUM fn,IPCMessage* Msg);
  ~MerchantRec();

  bool isFieldEnabled(int Field);
  bool  hasMessageNumber();
  DWORD getMessageNumber();
  void enableField(int Field,bool Enable);
  bool isRegionSelected(int Region);
  void selectRegion(int Region,bool Enable);
  int  numFedRegions() { return 12; };
  void Merge(MerchantRec*);

  virtual int DataLen() { return sizeof(Data); };
  virtual void* DataPtr() { return &Data; };
  virtual int ObjLen() { return sizeof(*this); };
  // Record has alternate key
  virtual void* AltKeyPtr() { return Data.MerchantName; };
  void FormatNew(const char* ID);
  virtual int KeyType();
  virtual int RecType();
  virtual int AltKeyType();
 };
#endif












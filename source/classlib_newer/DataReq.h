#ifndef __DATAREQ_H
#define __DATAREQ_H

#include "dbmanrec.h"
#include "checkauthfn.h"
#include "ipcmsg.h"
#include <time.h>

class DataRequirements : public DBManRecord {

  public:

  enum
   {
    Pos,
    Voice,
    Security
   };

   enum
    {
     CHECKTYPE,
     LICENSESTATE,
     LICENSE,
     DOB,
     CHECKNUMBER,
     MICR,
     NAME,
     ADDRESS,
     RESIDENCETYPE,
     CITY,
     CONSUMERSTATE,
     ZIPCODE,
     PHONE,
     SSN,
     PRODUCTCLASS,
     TRANSPORTMETHOD,
     BUSINESSNAME,
     PRIVATELABEL,
     TITLE,
     DOBIFMICRONLY,
     NAMEIFSOUNDEX,
     EMPLOYMENTDATA,
     SPOUSEDATA,
     FINANCEAMOUNT,
     LICENSEORMICR,
     EMPLOYERNAME=32,
     EMPLOYERADR,
     EMPLOYERPHONE,
     POSITION,
     YEARSEMPLOYED,
     SALARY,
     SPOUSENAME,
     SPOUSESSN,
     SPOUSEEMPLOYER,
     SPOUSEEMPLOYERADR,
     SPOUSEEMPLOYERPHONE,
     SPOUSEPOSITION,
     SPOUSEYEARSEMPLOYED,
     SPOUSESALARY
    };

  struct DataReqRecord Data;

  // Init Empty
  DataRequirements();
  // Init from a Record
  DataRequirements(struct DataReqRecord* Data);
  // Init from an IPC Message field
  DataRequirements(FLDNUM fn,IPCMessage* Msg);
  ~DataRequirements();

  bool isRequired(int Type,int Field,unsigned short Amount=0);
  void setRequired(int Type,int Field,bool Required);
  void setAmount(int Type,int Field,unsigned short Amount);
  unsigned short getAmount(int Type,int Field);

  virtual int DataLen() { return sizeof(Data); };
  virtual void* DataPtr() { return &Data; };
  virtual int ObjLen() { return sizeof(*this); };
  virtual void FormatNew(const char* ClassName);
  virtual int KeyType();
  virtual int RecType();
 };
#endif












#ifndef __DBMANRECORD_H
#define __DBMANRECORD_H

#include "SpsManDb.h"
#include "Dbdefs.h"

// All of the member functions are semi-pure virtuals, ie., you don't
// have to provide them in your derived classes but you will get a linker
// error if they are called because there is no code for them.

class DBManRecord {
  public:

   // These member functions are intended to be pure virtuals but
   // I haven't figured out how to declare pure virtuals yet.
   virtual int DataLen();
   virtual void* DataPtr();
   virtual int ObjLen();
   virtual void* KeyPtr();
   virtual void* AltKeyPtr();
   virtual int KeyType();
   virtual int RecType();
   virtual int AltKeyType();

   // Get the update timestamp
   virtual DBTIMESTAMP GetTimeStamp();
   // Put the update timestamp
   virtual void  PutTimeStamp(DBTIMESTAMP Ts=-1);
   // Compare timestamp
   virtual bool  SameTimeStamp(DBTIMESTAMP);
   virtual bool  SameTimeStamp(DBManRecord*);
   // Format a new record
   virtual void FormatNew(const char* ClassName);

   // Transfer the Data member into the object
   virtual void  TransferDataIn(void* Buf);
   // Transfer the Data member from the object
   virtual void  TransferDataOut(void* Buf);
 };
#endif












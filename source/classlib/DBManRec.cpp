//----------------------------------------------------------------------------
//  Project Classlib
//
//  Copyright © 1996. All Rights Reserved.
//
//  FILE:         DBManRec.cpp
//  AUTHOR:       Jack Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of DBManRecord (Base Class)
//
//  This is the bass class from which all of the other DBManager
//  records are derived. It provides some default behavior but most
//  of the member functions are intended to be overriden in the
//  derived class.
//----------------------------------------------------------------------------

#include "DBManRec.h"
#include <string.h>
#include <time.h>

//-----------------------------------------------------------
// Get the Timestamp (shows when this record was modified)
// The Timestamp is always at the front of the record so the
// timestamp functions do not need to be overridden.
//------------------------------------------------------------
DBTIMESTAMP DBManRecord::GetTimeStamp()
{
 DBTIMESTAMP Ts;
 memcpy(&Ts,DataPtr(),sizeof(Ts));
 return Ts;
}

//------------------
// Set the Timestamp
//------------------
void DBManRecord::PutTimeStamp(DBTIMESTAMP Ts)
{
 if ( Ts==(DBTIMESTAMP)-1 )
  Ts=(DBTIMESTAMP)time(NULL);
 memcpy(DataPtr(),&Ts,sizeof(DBTIMESTAMP));
}

//------------------------------------------------
// Compare two TimeStamps, given another Timestamp
//------------------------------------------------
bool DBManRecord::SameTimeStamp(DBTIMESTAMP Ts)
{
 DBTIMESTAMP TS=GetTimeStamp();
 return Ts==TS;
}

//----------------------------------------------------
// Compare Two TimeStamps, given another Record Object
//----------------------------------------------------
bool DBManRecord::SameTimeStamp(DBManRecord* DBRec)
{
 return SameTimeStamp(DBRec->GetTimeStamp());
}


//------------------------------
// Transfer Data into the object
//------------------------------
void DBManRecord::TransferDataIn(void* Buf)
{
 memcpy(DataPtr(),Buf,DataLen());
}

//--------------------------------
// Transfer Data out of the Object
//--------------------------------
void DBManRecord::TransferDataOut(void* Buf)
{
 memcpy(Buf,DataPtr(),DataLen());
}

//------------------------------------------------------------
// Default Formatter for new record. This is always overridden
//------------------------------------------------------------
void DBManRecord::FormatNew(const char* ClassName)
{
 char *Ptr=(char*)DataPtr();
 memset(Ptr,0,DataLen());
 memset(Ptr,0xff,sizeof(DBTIMESTAMP));
 strcpy(&Ptr[sizeof(DBTIMESTAMP)],ClassName);
}

//------------------------------------------------------------------
// These are supposed to be pure vituals, however, I couldn't figure
// out how to declare pure virtuals in Borland C++. If the derived
// class fails to override these, an exception will be thrown.
//-------------------------------------------------------------------
int DBManRecord::DataLen()
{
 throw;
}

void* DBManRecord::DataPtr()
{
 throw;
}

int DBManRecord::ObjLen()
{
 throw;
}

//-----------------------------------------------------------
// Get a Pointer to the Record's Key. This assumes it always
// comes immediately after the TimeStamp
//-----------------------------------------------------------
void* DBManRecord::KeyPtr()
{
 char* Ptr=(char*)DataPtr();
 // Assume key comes right after TimeStamp
 return( &Ptr[sizeof(DBTIMESTAMP)]);
}

//------------------------------------------------------------
// Get a pointer to the Alternate Key. This must be Overridden
//-------------------------------------------------------------
void* DBManRecord::AltKeyPtr()
{
 // By default, recods have only a primary key
 return NULL;
}

//------------------------------
// Always Override the following
//------------------------------
int DBManRecord::KeyType()
{
 return -1;
}

int DBManRecord::RecType()
{
 return -1;
}

int DBManRecord::AltKeyType()
{
 return -1;
}









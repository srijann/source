//----------------------------------------------------------------------------
//  Project Classlib
//
//  Copyright © 1996. All Rights Reserved.
//
//  FILE:         DataReq.cpp
//  AUTHOR:       Jack Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of DataRequirements (DBManRecord)
//
//  This is a Derived class of DBManRecord to handle the Data
//  Requirements Record.
//----------------------------------------------------------------------------

#include "Win4me.h"
#pragma hdrstop
#include <mem.h>
#include <string.h>
#include "DataReq.h"
#include "unsnum.h"

//----------------------------
// Make a new (empty) rule set
//----------------------------
DataRequirements::DataRequirements()
{
 memset(&Data,0,sizeof(Data));
}

//-----------------------------------------------
// Make a rule set, copying from another rule set
//-----------------------------------------------
DataRequirements::DataRequirements(struct DataReqRecord* Rec)
{
 memcpy(&Data,Rec,sizeof(Data));
}

//--------------------------------------------------
// Make a rule set, getting data from an IPC message
//--------------------------------------------------
DataRequirements::DataRequirements(FLDNUM fn,IPCMessage* Msg)
{
 int Len=sizeof(Data);

 if ( ! Msg->GetFld(fn,&Data,Len) )
  memset(&Data,0,sizeof(Data));
}

DataRequirements::~DataRequirements()
{
}


//----------------------------
// Test if a field is required
//----------------------------
bool DataRequirements::isRequired(int Type,int Field,unsigned short Amount)
{
 bool Required;

 if ( Field >= 32 )
  Required = (1<<(Field-32)) & Data.DataReq[Type].Bits[1];
 else
  Required = (1<<Field) & Data.DataReq[Type].Bits[0];

 if ( ! Required )
  return false;

 if ( Amount && ! isEmpty(Data.DataReq[Type].Amounts[Field]) )
  return ( Amount > Data.DataReq[Type].Amounts[Field] );
 else
  return true;
}

//-------------------------
// Set a Field Required bit
//-------------------------
void DataRequirements::setRequired(int Type,int Field,bool Required)
{
 unsigned long Mask;

 if ( Field >= 32 )
   Mask = 1 << (Field-32);
 else
   Mask = 1 << Field;

 if ( Required )
  Data.DataReq[Type].Bits[Field/32] |= Mask;
 else
  Data.DataReq[Type].Bits[Field/32] &= ~Mask;
}

//--------------------------
// Set an Amount for a Field
//--------------------------
void DataRequirements::setAmount(int Type,int Field,unsigned short Amount)
{
 Data.DataReq[Type].Amounts[Field]=Amount;
}

//--------------------------
// Get an Amount for a Field
//--------------------------
unsigned short DataRequirements::getAmount(int Type,int Field)
{
 return Data.DataReq[Type].Amounts[Field];
}

//--------------------
// Format a new record
//--------------------
void DataRequirements::FormatNew(const char* ClassName)
{
 int Type;
 memset(&Data,0xff,sizeof(Data));
 memset(Data.DataReqClassName,0,sizeof(Data.DataReqClassName));
 strcpy((char*)Data.DataReqClassName,ClassName);
 for (Type=0; Type<3; ++Type)
  Data.DataReq[Type].Bits[0] = Data.DataReq[Type].Bits[1] = 0;
}

int DataRequirements::KeyType()
{
 return DATAREQCLASSNAME;
}

int DataRequirements::RecType()
{
 return DATAREQRECORD;
}



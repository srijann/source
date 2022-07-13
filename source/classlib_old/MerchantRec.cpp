//----------------------------------------------------------------------------
//  Project Classlib
//
//  Copyright © 1996. All Rights Reserved.
//
//  FILE:         MerchantRec.cpp
//  AUTHOR:       Jack Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of MerchantRec (DBManRecord)
//
//  This is a Derived class of DBManRecord to handle the Merchant Record.
//----------------------------------------------------------------------------

#include "Win4me.h"
#pragma hdrstop
#include <mem.h>
#include <string.h>
#include "MerchantRec.h"
#include "TimeZoneInfo.h"
#include "CheckService.h"

//----------------------------
// Make a new (empty) rule set
//----------------------------
MerchantRec::MerchantRec()
{
 memset(&Data,0,sizeof(Data));
}

//-----------------------------------------------
// Make a rule set, copying from another rule set
//-----------------------------------------------
MerchantRec::MerchantRec(struct MerchantRecord* Rec)
{
 memcpy(&Data,Rec,sizeof(Data));
}

//--------------------------------------------------
// Make a rule set, getting data from an IPC message
//--------------------------------------------------
MerchantRec::MerchantRec(FLDNUM fn,IPCMessage* Msg)
{
 int Len=sizeof(Data);

 if ( ! Msg->GetFld(fn,&Data,Len) )
  memset(&Data,0,sizeof(Data));
}

MerchantRec::~MerchantRec()
{
}


//-------------------------------------
// Test if a field Enabler is on or off
//-------------------------------------
bool MerchantRec::isFieldEnabled(int Field)
{
 // If this is an account record, then all fields are always
 // enabled. Fields are only disabled in station records to
 // indicate that the Account default should be used.
 if ( strlen(Data.MerchantID) == 3 )
   return true;

 return (1 << Field) & Data.FieldEnablerBits;
}

//------------------------------
// Set a Field Enabler On or Off
//------------------------------
void MerchantRec::enableField(int Service,bool Enable)
{
 unsigned long Mask = 1 << Service;

 if ( Enable )
  Data.FieldEnablerBits |= Mask;
 else
  Data.FieldEnablerBits &= ~Mask;
}

//--------------------
// Format a New Record
//--------------------
void MerchantRec::FormatNew(const char* Number)
{
 char *global="DEFAULT";

 memset(&Data,0,sizeof(Data));
 Data.MerchantRecTStamp=(DBTIMESTAMP)-1;
 strcpy((char*)Data.MerchantID,Number);

 if ( strlen(Number) == 3 )
  {
   strcpy(Data.MerchantName,"NEW ACCOUNT");
   strcat(Data.MerchantName,Number);
   Data.FieldEnablerBits=(unsigned long)-1;
   strcpy(Data.Classes.Product,global);
   strcpy(Data.Classes.GUAuthRules,global);
   strcpy(Data.Classes.GUUnvFundsRules,global);
   strcpy(Data.Classes.GUDataReq,global);
   strcpy(Data.Classes.GUPosFileRules,global);
   strcpy(Data.Classes.GURespMsg,global);
   strcpy(Data.Classes.VFYAuthRules,global);
   strcpy(Data.Classes.VFYUnvFundsRules,global);
   strcpy(Data.Classes.VFYDataReq,global);
   strcpy(Data.Classes.VFYPosFileRules,global);
   strcpy(Data.Classes.VFYRespMsg,global);
   Data.MerchantService = CHECKSERVICE_GUARANTEE;
   Data.MerchantTimeZone=TimeZoneInfo::Pacific;
   strcpy(Data.MerchantState,"CA");
   strcpy(Data.MerchantZip,"90000");
   strcpy(Data.MerchantSic,"5651");
  }
 else
  {
   strcpy(Data.MerchantName,"NEW STA");
   strcat(Data.MerchantName,Number);
  }
}

//---------------------------------------------
// Test if a Federal reserver region is Enabled
//---------------------------------------------
bool MerchantRec::isRegionSelected(int Region)
{
 return (1<<Region) & Data.FedRegionBits;
}

//--------------------------------------------
// Test if a Federal Reserve Region is Enabled
//--------------------------------------------
void MerchantRec::selectRegion(int Region,bool Enable)
{
 unsigned short Mask = (unsigned short) (1 << Region);

 if ( Enable )
  Data.FedRegionBits |= Mask;
 else
  Data.FedRegionBits &= (unsigned short) ~Mask;
}

//--------------------------------------------
// Return the Key Type for the Database Server
//--------------------------------------------
int MerchantRec::KeyType()
{
 return MERCHANTID;
}

//-----------------------------------------------
// Return the Record Type for the Database Server
//-----------------------------------------------
int MerchantRec::RecType()
{
 return MERCHANTRECORD;
}

//------------------------------------------------------
// Return the Alternate Key Type for the database server
// (it's the Merhant Name
//------------------------------------------------------
int MerchantRec::AltKeyType()
{
 return MERCHANTNAME;
}

//----------------------------------------------
// Merge a Station record into an account record
//----------------------------------------------
#define MERGEINT(n) if ( stationRec->isFieldEnabled(n) )\
  Data.n=stationRec->Data.n
#define MERGESTR(n) if ( stationRec->isFieldEnabled(n) )\
  strcpy(Data.n,stationRec->Data.n)
#define MERGECLASS(n,Enabler) if ( stationRec->isFieldEnabled(Enabler) )\
  strcpy(Data.Classes.n,stationRec->Data.Classes.n)
void MerchantRec::Merge(MerchantRec* stationRec)
{
 if ( stationRec->isFieldEnabled(MerchantService) )
  {
   Data.MerchantService=stationRec->Data.MerchantService;
   Data.VerifyAmount=stationRec->Data.VerifyAmount;
   Data.PreauthDays=stationRec->Data.PreauthDays;
   Data.PreauthAmount=stationRec->Data.PreauthAmount;
   Data.HoldCheck1Percent=stationRec->Data.HoldCheck1Percent;
   Data.HoldCheckValuePercent=stationRec->Data.HoldCheckValuePercent;
   Data.HoldCheckAmount=stationRec->Data.HoldCheckAmount;
  }
 MERGESTR(MerchantSic);
 MERGESTR(MerchantState);
 MERGESTR(MerchantZip);
 MERGESTR(MessageNumber);
 MERGEINT(MerchantTimeZone);
 MERGEINT(ProductGroup);
 MERGECLASS(Product,ProductClasses);
 MERGECLASS(GUAuthRules,GUAuthRules);
 MERGECLASS(GUAuthRulesSub,GUAuthRules);
 MERGECLASS(GUUnvFundsRules,GUUnvFundsRules);
 MERGECLASS(GUDataReq,GUDataReq);
 MERGECLASS(GUPosFileRules,GUPosFileRules);
 MERGECLASS(GURespMsg,GURespMsg);
 MERGECLASS(VFYAuthRules,VFYAuthRules);
 MERGECLASS(VFYAuthRulesSub,VFYAuthRules);
 MERGECLASS(VFYUnvFundsRules,VFYUnvFundsRules);
 MERGECLASS(VFYDataReq,VFYDataReq);
 MERGECLASS(VFYPosFileRules,VFYPosFileRules);
 MERGECLASS(VFYRespMsg,VFYRespMsg);

 if ( stationRec->isFieldEnabled(FedRegions) )
  Data.FedRegionBits=stationRec->Data.FedRegionBits;

 strcpy(Data.MerchantID,stationRec->Data.MerchantID);
 strcpy(Data.MerchantName,stationRec->Data.MerchantName);
}

// Return if the record has a message number
bool MerchantRec::hasMessageNumber()
{
 return isdigit(Data.MessageNumber[0]);
}

// Return the message Number
DWORD MerchantRec::getMessageNumber()
{
 if ( ! hasMessageNumber() )
  return 0;
 return atoi(Data.MessageNumber); 
}


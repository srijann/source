//----------------------------------------------------------------------------
//  Project Classlib
//
//  Copyright © 1996. All Rights Reserved.
//
//  FILE:         UnvFundsRules.cpp
//  AUTHOR:       Jack Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of UnvFundsRules (DBManRecord)
//
//  This is a Derived class of DBManRecord to handle the Unverified Funds
//  Rules Record. (This is better known as the Transaction Scoring
//  Record).
//----------------------------------------------------------------------------


#include "Win4me.h"
#pragma hdrstop
#include <mem.h>
#include <string.h>
#include "UnvFundsRules.h"

//----------------------------
// Make a new (empty) rule set
//----------------------------
UnvFundsRules::UnvFundsRules()
{
 memset(&Data,0,sizeof(Data));
}

//-----------------------------------------------
// Make a rule set, copying from another rule set
//-----------------------------------------------
UnvFundsRules::UnvFundsRules(struct UnvFundsRulesRecord* Rec)
{
 memcpy(&Data,Rec,sizeof(Data));
}

//--------------------------------------------------
// Make a rule set, getting data from an IPC message
//--------------------------------------------------
UnvFundsRules::UnvFundsRules(FLDNUM fn,IPCMessage* Msg)
{
 int Len=sizeof(Data);

 if ( ! Msg->GetFld(fn,&Data,Len) )
  memset(&Data,0,sizeof(Data));
}

UnvFundsRules::~UnvFundsRules()
{
}

//----------------------------------------
// Format a new record with default values
//----------------------------------------
void UnvFundsRules::FormatNew(const char* ClassName)
{
 memset(&Data,0xff,sizeof(Data));
 memset(Data.UnvFundsRulesClassName,0,sizeof(Data.UnvFundsRulesClassName));
 strcpy((char*)Data.UnvFundsRulesClassName,ClassName);
  // Set all the points to zero
 memset(Data.AmountPoints,0,sizeof(Data.AmountPoints));
 memset(Data.CheckNumPoints,0,sizeof(Data.CheckNumPoints));
 memset(Data.AgePoints,0,sizeof(Data.AgePoints));
 memset(Data.TimePoints,0,sizeof(Data.TimePoints));
 memset(Data.DayOfWeekPoints,0,sizeof(Data.DayOfWeekPoints));
 memset(Data.SSNScorePoints,0,sizeof(Data.SSNScorePoints));
 memset(Data.YearsOnJobPoints,0,sizeof(Data.YearsOnJobPoints));
 memset(Data.VelocityTransPoints,0,sizeof(Data.VelocityTransPoints));
 memset(Data.VelocityAmountPoints,0,sizeof(Data.VelocityAmountPoints));
 Data.BankVerifyNGNoPoints=Data.BankVerifyNGYesPoints=0;
 Data.BankVerifyGoodNoPoints=0;
 memset(Data.BankVerifyAmountPoints,0,sizeof(Data.BankVerifyAmountPoints));
 Data.DeliveryYesPoints = Data.DeliveryNoPoints = 0;
 Data.LicenseInPoints = Data.LicenseOutPoints = 0;
 Data.FedRegionInPoints = Data.FedRegionOutPoints = 0;
 Data.ResidenceHousePoints = Data.ResidenceAptPoints = 0;
 Data.ConsumerPhoneGoodPoints = Data.ConsumerPhoneNoGoodPoints = 0;
 Data.EmployerPhoneGoodPoints = Data.EmployerPhoneNoGoodPoints = 0;
 Data.PositiveFileYesPoints = Data.PositiveFileNoPoints = 0;
}

//--------------------------------------------
// Return the key type for the Database Server
//--------------------------------------------
int UnvFundsRules::KeyType()
{
 return UNVFUNDSRULESCLASSNAME;
}

//-----------------------------------------------
// Return the record type for the Database Server
//-----------------------------------------------
int UnvFundsRules::RecType()
{
 return UNVFUNDSRULESRECORD;
}

#define DOROW(name,num,emptyval)\
 for (select=false, i=0; i<num; ++i)\
  if ( Data.name##Value[i] != emptyval )\
    if ( ! select )\
     {\
      Low = Hi = Data.name##Points[i];\
      select=true;\
     }\
    else\
     {\
      Low = min( Low, (signed char) Data.name##Points[i] );\
      Hi = max( Hi, (signed char) Data.name##Points[i] );\
     }\
 if ( select )\
  {\
   minScore += Low;\
   maxScore += Hi;\
  }

#define DOARRAY(name,num)\
 for (Low=Hi=Data.name##Points[0], i=1; i<num; ++i)\
  {\
   Low = min( Low, (signed char) Data.name##Points[i] );\
   Hi = max( Hi, (signed char) Data.name##Points[i] );\
  }\
 minScore += Low;\
 maxScore += Hi;

#define DOPAIR(name1,name2)\
  minScore += (signed char) min( (signed char) Data.name1##Points,\
                                 (signed char) Data.name2##Points);\
  maxScore += (signed char) max( (signed char) Data.name1##Points,\
                                 (signed char) Data.name2##Points)

//--------------------------------------------------
// Calculate the Minimum and Maximum possible scores
//--------------------------------------------------
void UnvFundsRules::calcMinMaxScore(int& minScore,int& maxScore)
{
 uint i;
 minScore = maxScore=0;
 signed char Low, Hi;
 bool select;

 DOROW(Amount,NUMAMOUNTROWS,ULONG_MAX);
 DOROW(CheckNum,NUMCHECKNUMROWS,ULONG_MAX);
 DOROW(Age,NUMAGEROWS,UCHAR_MAX);
 DOROW(Time,NUMTIMEOFDAYROWS,USHRT_MAX);
 DOARRAY(DayOfWeek,7);
 DOROW(SSNScore,NUMSSNSCOREROWS,USHRT_MAX);
 DOROW(YearsOnJob,NUMYEARSONJOBROWS,UCHAR_MAX);
 DOROW(VelocityTrans,NUMVELOCITYTRANSROWS,UCHAR_MAX);
 DOROW(VelocityAmount,NUMVELOCITYAMOUNTROWS,ULONG_MAX);
 DOPAIR(DeliveryYes,DeliveryNo);
 DOPAIR(LicenseIn,LicenseOut);
 DOPAIR(FedRegionIn,FedRegionOut);
 DOPAIR(ResidenceHouse,ResidenceApt);
 DOPAIR(ConsumerPhoneGood,ConsumerPhoneNoGood);
 DOPAIR(EmployerPhoneGood,EmployerPhoneNoGood);
 DOPAIR(PositiveFileYes,PositiveFileNo);

 DOPAIR(BankVerifyNGYes,BankVerifyNGNo);

 Hi=Low=Data.BankVerifyGoodNoPoints;

 for ( i=0; i<NUMBANKVERIFYAMOUNTROWS; ++i)
  if ( Data.BankVerifyAmountValue[i] != ULONG_MAX )
     {
      Low = min( Low, (signed char) Data.BankVerifyAmountPoints[i] );
      Hi = max( Hi, (signed char) Data.BankVerifyAmountPoints[i] );
     }

 minScore += Low;
 maxScore += Hi;
}





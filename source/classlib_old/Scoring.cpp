//----------------------------------------------------------------------------
//  Project ClassLib
//
//  Copyright © 1996. All Rights Reserved.
//
//  FILE:         CheckAuth.cpp
//  AUTHOR:       Jack Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of Check Auth
//
//  This is the Transaction scoring member function of the CheckAuth object.
//----------------------------------------------------------------------------

#include "win4me.h"
#include "CheckAuth.h"
#include "SPSManDB.h"
#include "UnsNum.h"
#include "Rcodes.h"
#include "BankVerify.h"

//----------------------------------------------------------------------
// Score the transaction using the unverified Funds (SCORING) rules.
// Return true if the score is less than the Transaction Cutoff, false
// if not.
//
// Also return the score, in case anyone cares
//----------------------------------------------------------------------

// A Literate Macro to help
#define SCOREFROMTABLE(Value,ValueArray,PointsArray,NumValues)\
 {\
  for (i=0; i < NumValues-1; ++i)\
   if ( isEmpty(scoringRules.Data.ValueArray[i]) )\
    {\
     i=NumValues-1;\
     break;\
    }\
   else\
     if (Value < scoringRules.Data.ValueArray[i])\
       break;\
  if ( ! isEmpty(scoringRules.Data.ValueArray[i]) )\
    Score += (signed char) scoringRules.Data.PointsArray[i];\
 }

bool CheckAuth::ScoreTransaction(int& Score)
{
 DWORD Amount;
 DWORD CheckNum;
 uint  Age;
 uint i;
 ushort SSNScore;
 time_t TimeStamp;
 BYTE YearsOnJob;
 struct tm *t;
 ACTIVITY* ActivityRecs;
 int      ActivityCount;
 BYTE Byte;
 char State[3];
 int  Len;
 uint p1,p2,p3,p4;

 Score = 0;

 // Can't score if I don't have the rules.
 if ( ! hasScoringRules )
  return false;

 // If I don't have any of the data elements that I need to figure the
 // score, then simply assume zero points.

 // Score AMOUNT
 Amount = CheckAmount();
 SCOREFROMTABLE(Amount,AmountValue,AmountPoints,NUMAMOUNTROWS);

 // Score Check Number
 if ( (CheckNum = CheckNumber()) != 0 )
   SCOREFROMTABLE(CheckNum,CheckNumValue,CheckNumPoints,NUMCHECKNUMROWS);

 // Score AGE
 if ( (Age=ConsumerAge()) != 0 )
   SCOREFROMTABLE(Age,AgeValue,AgePoints,NUMAGEROWS);

 // Score Time of Day and Day of Week
 TimeStamp=time(NULL);
 t=localtime(&TimeStamp);
 unsigned short mTime = (unsigned short) (t->tm_hour * 100 + t->tm_min);
 SCOREFROMTABLE(mTime,TimeValue,TimePoints,NUMTIMEOFDAYROWS);
 Score += scoringRules.Data.DayOfWeekPoints[t->tm_wday];

 // Score the SSN Score
 if ( GetFld(FN_SSNSCORE,SSNScore) )
  SCOREFROMTABLE(SSNScore,SSNScoreValue,SSNScorePoints,NUMSSNSCOREROWS);

 // Score Years on Job
 if ( GetFld(FN_YEARSEMPLOYED,YearsOnJob) )
  SCOREFROMTABLE(YearsOnJob,YearsOnJobValue,YearsOnJobPoints,NUMYEARSONJOBROWS);

 // Score Velocity (Prior Activity - number and $ of transactions)
 ActivityRecs = hasActivity(&ActivityCount);
 if ( ! isEmpty(scoringRules.Data.VelocityDays) &&
      ActivityCount != 0 )
  {
   unsigned char numTrans=0;
   unsigned long amountTrans=0;

   for (i=0; i<ActivityCount; ++i)
     if ( ISAPPROVALRCODE(ActivityRecs[i].Rcode) &&     // Is Approval
          ActivityRecs[i].Age < scoringRules.Data.VelocityDays )  // Is within days
      {
       ++numTrans;
       amountTrans += ActivityRecs[i].Amount;
      }
   SCOREFROMTABLE(numTrans,VelocityTransValue,VelocityTransPoints,
                    NUMVELOCITYTRANSROWS);
   SCOREFROMTABLE(amountTrans,VelocityAmountValue,VelocityAmountPoints,
                    NUMVELOCITYAMOUNTROWS);
  }


 // Score Prior Bank Verify No Good
 if ( ! isEmpty(scoringRules.Data.BankVerifyNGDays) &&
      ActivityCount != 0 )
  {
   for (i=0; i<ActivityCount; ++i)
     if ( ActivityRecs[i].Age < scoringRules.Data.BankVerifyNGDays &&
          memcmp(ActivityRecs[i].Output.BVStatus,
                  BVSTATUS_NOTENOUGHMONEY,
                  sizeof(ActivityRecs[i].Output.BVStatus)) == 0 )
       break;

   if ( i == ActivityCount )
    Score += (signed char) scoringRules.Data.BankVerifyNGNoPoints;
   else
    Score += (signed char) scoringRules.Data.BankVerifyNGYesPoints;
  }

 // Score Prior Bank Verify Good
 if ( ! isEmpty(scoringRules.Data.BankVerifyGoodDays) &&
      ActivityCount != 0 )
  {
   unsigned long Amount=0;

   for (i=0; i<ActivityCount; ++i)
     if ( ActivityRecs[i].Age < scoringRules.Data.BankVerifyGoodDays &&
          memcmp(ActivityRecs[i].Output.BVStatus,
                  BVSTATUS_VERIFIEDGOOD,
                  sizeof(ActivityRecs[i].Output.BVStatus)) == 0 )
       Amount += ActivityRecs[i].Amount;


   if ( Amount == 0 )
    Score += (signed char) scoringRules.Data.BankVerifyGoodNoPoints;
   else
    SCOREFROMTABLE(Amount,BankVerifyAmountValue,BankVerifyAmountPoints,
                      NUMBANKVERIFYAMOUNTROWS);

  }

 // Score Delivery Method
 if ( GetFld(FN_TRANSPORTMETHOD,Byte) )
  if ( Byte == 'C' ) // Carryout?
   Score += (signed char) scoringRules.Data.DeliveryNoPoints;
  else
   Score += (signed char) scoringRules.Data.DeliveryYesPoints;

 // Score License In Merchant's State
 Len = sizeof(State);
 if ( GetFld(FN_LICENSESTATE,State,Len) )
  if ( memcmp(State,merchantRec.Data.MerchantState,2) == 0 )
    Score += (signed char) scoringRules.Data.LicenseInPoints;
  else
    Score += (signed char) scoringRules.Data.LicenseOutPoints;

 // Score Bank in Merchant's Region
 if ( GetFld(FN_BANKNUMBER) )
  if ( isCheckInRegion() )
   Score += (signed char) scoringRules.Data.FedRegionInPoints;
  else
   Score += (signed char) scoringRules.Data.FedRegionOutPoints;

 // Score type of Residence
 if ( GetFld(FN_APARTMENT,Byte) )
  if ( Byte == 'Y' )
   Score += (signed char) scoringRules.Data.ResidenceAptPoints;
  else
   Score += (signed char) scoringRules.Data.ResidenceHousePoints;

 // Score Consumer Phone Number
 if ( GetFld(FN_PHONE) &&
      GetFld(FN_CONSUMERPHONEGOOD,Byte) )
  if ( Byte == 'Y' )
   Score += (signed char) scoringRules.Data.ConsumerPhoneGoodPoints;
  else
   Score += (signed char) scoringRules.Data.ConsumerPhoneNoGoodPoints;

 // Score Employer Phone Number
 if ( GetFld(FN_EMPLOYERPHONE) &&
      GetFld(FN_EMPLOYERPHONEGOOD,Byte) )
   if ( Byte == 'Y' )
    Score += (signed char) scoringRules.Data.EmployerPhoneGoodPoints;
   else
    Score += (signed char) scoringRules.Data.EmployerPhoneNoGoodPoints;

 // Score Positive File Status
 // getPosfileLimits tells us if the positive record has reached the
 // trigger date. We don't need any of the returned parameters.
 if ( hasPositiveFileRecord &&
      getPosfileLimits(p1,p2,p3,p4) )
  Score += (signed char) scoringRules.Data.PositiveFileYesPoints;
 else
  Score += (signed char) scoringRules.Data.PositiveFileNoPoints;

 // Put the Score into the Message and return whether it's over
 // the cutoff
 PutFld(FN_TRANSACTIONSCORE,(DWORD)Score);
 return Score >= scoringRules.Data.TransactionCutoffValue;
}
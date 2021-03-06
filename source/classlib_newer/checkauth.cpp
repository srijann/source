//----------------------------------------------------------------------------
//  Project ClassLib
//
//  Copyright ? 1996. All Rights Reserved.
//
//  FILE:         CheckAuth.cpp
//  AUTHOR:       Jack Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of Check Auth
//
//  CheckAuth is a derived class of IPCMessage. It contains the
//  Check Authorization logic.
//
// MAB030101 - When a transaction is submitted and treated as error,
//   if the check is run again, with error field corrected, the system
//   creates a check sequence error. Presumably, it is objecting to the
//   same check being used twice.
//
// MAB062899 - Removed that PostauthAmount must be < Preauth Amount
//   Not the way Lin wants it to work
//
// MAB091197A - modification to make the R8 rules about Federal
//   Reserve region give the right answer.  It seemed to be reversed.
//
// MAB091597A - If no checknumber on last check (0), but is number on
//   current check, do not make R4 rule exception for large gap
//
//
// JHE011698 - Modified to set IDTYPE flags in activity records depending on
//   whether or not the transaction was used to trigger a decline. Prior to
//   executing rules, the flags are shifted to lowercase. The rule functions
//   were modified to include a "flag_activity" parameter. The rule functions
//   are now called a second time with this parameter set to true to cause
//   the rule function to flag the relevant activity by shifting the IDTYPE
//   flags to uppercase.
//
// JHE061698 -    The above modification was deemed no longer desirable but the
//   code was left in place in case it is needed in the future. The VoiceAuth
//   program no longer cares about the case of the IDTYPES.
//
//   Modifed the R4 Check Sequence rule. See for details.
//
//   Modified the R3 Check Number and R1 Age rules to limit overrides.
//   See new function "maxOverrides" and rules for details.
//
//   Modifed getRcodes to not allow override an R4 Check Sequence Rcode
//
//   Fixed bug in GenerateApprovalNumber function which caused duplicate
//   hold check approvals to be generated.
//
//   Modified to include Phone number as key to positive file.
//
// JHE070898
//
//   Modified R4 Check sequence rule to detect duplicate sequence
//   numbers.
//
// JHE071798
//
//   Modified to not required DL if MICR only
//
// JHE121798
//
//   Modified R4 Check Sequence rule to ignore Error Transactions (ie.,
//   not approvals or declines).
//
// MAB120499 - R4 Check Number - Allow duplicate check number for certain Micr
//
// MAB120899 - R4 Check Number - Do not match on phone numbers
//
// MAB121099 - R2 Activity     - Do not match on phone numbers
//
// MAB122899 - Make a table of Micr's (M) that do not trigger errors for
//             account # and routing # activity. This is for bank checks
//             sent to people for their Credit Card accounts. Multiple
//             consumers would have the same Micr.
//----------------------------------------------------------------------------

#include "win4me.h"
#include <stdio.h>
#include <cstring.h>
#define NOFLDNAMES
#include "CheckAuth.h"
#include "CheckAuthFn.h"
#include "CheckService.h"
#include "SpsManDb.h"
#include "Rcodes.h"
#include "unsnum.h"
#include "DestName.h"
#include "TranType.h"
#include "splib.h"

//--------------------------------------------------------------------------
// MAGIC NUMBERS
//
// Hard-coding has been done at Lin's request. These macros are provided for
// ease of maintenance.
//---------------------------------------------------------------------------
#define OVERRIDE_PERIOD 7
#define OVERRIDE_AMOUNT 100
#define OVERRIDE_MAX_IF_UNDER_AMOUNT 2
#define OVERRIDE_MAX_IF_OVER_AMOUNT  1
#define R4_AGEOFF_PERIOD 10

#define VERSION_061000       0

// MAB122899
#define FILE_OPEN_ERROR                1001
#define NUM_BANK_NUMBERS_TO_CHECK_1      10
#define LEN_ACCT_BUF                     32
#define LENGTH_ERROR                   1003
#define ROUTE_NOT_FOUND                1011
#define FN(default) FLDNUM Fn=(fn==-1) ? FN_##default : (FLDNUM) fn
#define FN_(default) FLDNUM Fn_=(fn==-1) ? FN_##default : (FLDNUM) fn

//static int isThisACreditCardBankMicr(char* pszBankNumber,char *pszBankAccount);
//static int isValidTableBankNumber(char* pszBankNumber, char *pszBankAccount);

bool isCanadian(char* StateCode);
static void shiftCase(char* String,int Len,bool Upper=TRUE);

#define DEFAULT_TIMEOUT 7000

#if 1
void logg(int iLine, char *buf1, char *buf2, int i, long l);

void logg(int iLine, char *buf1, char *buf2, int i, long l)
{
   FILE  *fout;
   fout = fopen("err.log", "a");
   fprintf(fout,"@%d %s - %s. .%ld. .%d.\n", iLine, buf1, buf2, i, l);
   fclose(fout);
}
#endif

//------------------
// Class Constructor
//------------------
CheckAuth::CheckAuth() :
 hasScoringRules(false),
 hasDataReq(false),
 hasAuthRules(false),
 hasMerchantRec(false),
 NetTimeout(DEFAULT_TIMEOUT),
 NetUIRetry(false),
 OverrideFields(0)
{
 // Set up a couple of dummy strings
 emptyString[0]=0;
 strcpy(qmarkString,"?");
}

//------------------
// Class Destructor
//------------------
CheckAuth::~CheckAuth()
{
 // If I had an override Field array, delete it
 if ( NumOverrideFields )
  delete[] OverrideFields;
}

//------------------------------------------
// Determine if the Transaction is Guarantee
//------------------------------------------
bool CheckAuth::isGuarantee()
{
 DWORD Amount;
 struct MerchantRecord* Rec=(struct MerchantRecord*)
                                     GetFldPtr(FN_MERCHANTRECORD);
 if ( Rec==NULL )
  return false;

 switch( Rec->MerchantService )
  {
   // These services are considered Guarantee services
   case CHECKSERVICE_GUARANTEE:
   case CHECKSERVICE_PREAUTH:
   case CHECKSERVICE_HOLDCHECK:           return true;

   // This service is considered verify
   case CHECKSERVICE_VERIFY:              return false;

   // This service depends on the Amount
   case CHECKSERVICE_GUARANTEEVERIFY:
       // Consider it Guarantee if the Amount is not present
       // Editing should catch this in POS.
       Amount=CheckAmount();
       return Amount==0 || Amount > (DWORD) Rec->VerifyAmount;
  }

 return false;
}

//-----------------------------------
// Determine if transaction is verify
//-----------------------------------
bool CheckAuth::isVerify()
{
 return ! isGuarantee();
}

//-------------------------------------------------
// Determine if it's a Preauth Merchant Transaction
//-------------------------------------------------
bool CheckAuth::isPreauthMerchant()
{
 return merchantRec.Data.MerchantService==CHECKSERVICE_PREAUTH;
}

//---------------------------------------------------
// Determine if it's a HoldCheck Merchant Transaction
//---------------------------------------------------
bool CheckAuth::isHoldCheckMerchant()
{
 return merchantRec.Data.MerchantService==CHECKSERVICE_HOLDCHECK;
}

//------------------------------------------
// Get check amount in rounded whole dollars
//------------------------------------------
DWORD CheckAuth::CheckAmount()
{
 DWORD Amount;
 char sAmount[20];
 char *decPt;
 int   Len;

 // If already have rounded amount, just return it, otherwise
 // calculate it and store it for future use.
 if ( GetFld(FN_AMOUNTROUNDED,Amount) )
  return Amount;

 Len=sizeof(sAmount);
 memset(sAmount,0,sizeof(sAmount));
 if ( ! GetFld(FN_AMOUNT,Len,sAmount) )
  return 0;

 // See if it has a decimal point. If it has no decimal point, assume
 // that it's whole dollars. If it has a decimal point, round it up
 // to the nearest dollar.
 if ( (decPt=strchr(sAmount,'.')) == NULL )
   Amount = atoi(sAmount);
 else
  {
   Amount=atoi(sAmount) * 100 + atoi(&decPt[1]);
   // Round to nearest dollor
   Amount += 50;
   Amount /= 100;
  }

 PutFld(FN_AMOUNTROUNDED,Amount);
 return Amount;
}

//------------------------------------------------------
// Get A Pointer to a string containing the Merchant ID
//------------------------------------------------------
char* CheckAuth::MerchantID()
{
 char *Ptr;

 if ( ! hasMerchantRec )
  initClassMembers();
 if ( hasMerchantRec )
   return merchantRec.Data.MerchantID;
 if ( (Ptr=(char*)GetFldPtr(FN_MERCHID)) != NULL )
  return Ptr;
 else
  return qmarkString;
}

//-------------------------------------------------------
// Get A pointer to a string containing the Merchant Name
//-------------------------------------------------------
char* CheckAuth::MerchantName()
{
 if ( ! hasMerchantRec )
  {
   initClassMembers();
   if ( ! hasMerchantRec )
    return qmarkString;
  }

 return merchantRec.Data.MerchantName;
}

//----------------------------------------------------------
// Initialize the class member objects when a new message is
// received.
//----------------------------------------------------------
void CheckAuth::initClassMembers()
{
 int Len;
 struct MerchantRecord* mRec=(struct MerchantRecord*)
                                GetFldPtr(FN_MERCHANTRECORD);
 struct UnvFundsRulesRecord* sRec=(struct UnvFundsRulesRecord*)
                                GetFldPtr(FN_UNVFUNDSRULESRECORD);
 struct AuthRulesRecord* aRec=(struct AuthRulesRecord*)
                                GetFldPtr(FN_AUTHRULES);
 struct DataReqRecord* dRec=(struct DataReqRecord*)
                                GetFldPtr(FN_DATAREQRECORD);
 if ( mRec )
  {
   memcpy(&merchantRec.Data,mRec,sizeof(merchantRec.Data));
   hasMerchantRec=true;
  }
 else
  hasMerchantRec=false;

 if (sRec)
  {
   memcpy(&scoringRules.Data,sRec,sizeof(scoringRules.Data));
   hasScoringRules=true;
  }
 else
  hasScoringRules=false;

 if (aRec)
  {
   memcpy(&authRules.Data,aRec,sizeof(authRules.Data));
   hasAuthRules=true;
  }
 else
  hasAuthRules=false;

 if (dRec)
  {
   memcpy(&dataReq.Data,dRec,sizeof(dataReq.Data));
   hasDataReq=true;
  }
 else
  hasDataReq=false;

 Len=sizeof(PosFileRecord);
 memset(&PosFileRecord,0,sizeof(PosFileRecord));
 hasPositiveFileRecord=GetFld(FN_POSITIVEFILERECORD,&PosFileRecord,Len);

 // Get the current time
 if ( ! GetFld(FN_TIMESTAMP,(DWORD)NowTs) )
  {
   NowTs=time(NULL);
   PutFld(FN_TIMESTAMP,(DWORD)NowTs);
  }
 memcpy(&Now,localtime(&NowTs),sizeof(Now));

 NumOverrideFields=0;
}

//-------------------------------------------------------
// Copy in another IPC Message and call the init function
//-------------------------------------------------------
void CheckAuth::CopyMsg(IPCMessage* Msg)
{
 CopyMsgIn(Msg);
 initClassMembers();
}

//----------------------------------------------
// Get Product Information (ie., Group/SubGroup)
//
// Returns false if there is none.
//----------------------------------------------
bool CheckAuth::ProductInfo(uint& Group,uint& SubGroup)
{
 Group=0;
 SubGroup=0;

 if ( hasMerchantRec )
  Group=merchantRec.Data.ProductGroup;

 if ( ! GetFld(FN_PRODUCTCLASS,(DWORD)SubGroup) )
  return false;

 if ( Group == 0 || SubGroup == 0 )
  return false;

 return true;  
}

//--------------------------
// Reset (empty) the message
//--------------------------
void CheckAuth::Reset()
{
 ClearMsg();
 hasMerchantRec=hasAuthRules=hasScoringRules=hasDataReq;
}

//-------------------------
// Count up Prior Activity
//
// Provide Number of Days to look at, CountableAmount, and Countable Accum
// Limit. Number of Transactions and and Amount will be returned.
//
// Can also select by Account, Station, or Sic Code
//-------------------------
bool CheckAuth::CountActivity(uint Days,uint CountableAmount,
                              uint NonCountableAccumLimit,
                              uint& NumTrans,uint& Amount,
                              char* Account,char* Station,char* Sic,
                              bool flagActivity)
{
 int i;
 uint AmountUnderCountable=0;
 uint AmountOverCountable=0;
 uint TransUnderCountable=0;
 uint TransOverCountable=0;
 int ActivityCount;
 ACTIVITY* ActivityRecs;

 NumTrans=Amount=0;

 if ( (ActivityRecs=hasActivity(&ActivityCount)) == 0 )
  return false;

 for (i=0; i<ActivityCount; ++i)
  if ( ISAPPROVALRCODE(ActivityRecs[i].Rcode) &&     // Is Approval
       ActivityRecs[i].Age < (unsigned char) Days )  // Is within days
   {
    // Don't Count PREAUTH if there's a later approval by this
    // merchant
    if ( ActivityRecs[i].Rcode == RCODE_PREAUTHAPPROVAL )
     {
      int n;
      for (n=0; n<ActivityCount; ++n)
       if ( n != i &&
            ActivityRecs[i].Age >= ActivityRecs[n].Age &&
            ISAPPROVALRCODE(ActivityRecs[n].Rcode) &&
            memcmp(ActivityRecs[n].MerchID,
                   ActivityRecs[i].MerchID,
                   sizeof(ActivityRecs[n].MerchID)) == 0 )
       break;
      if ( n != ActivityCount )
       continue;
     }

    if ( Account &&
         memcmp(Account,ActivityRecs[i].MerchID,3) )
     continue;
    else
    if ( Station &&
         memcmp(Station,ActivityRecs[i].MerchID,sizeof(ActivityRecs[i].MerchID)) )
     continue;
    else
    if ( Sic &&
         memcmp(Sic,ActivityRecs[i].Sic,sizeof(ActivityRecs[i].Sic)) )
      continue;

    if ( flagActivity )
     shiftCase(ActivityRecs[i].Ids,sizeof(ActivityRecs[i].Ids));

    if ( ActivityRecs[i].Amount < CountableAmount )
     {
      AmountUnderCountable += ActivityRecs[i].Amount;
      ++TransUnderCountable;
     }
    else
     {
      AmountOverCountable += ActivityRecs[i].Amount;
      ++TransOverCountable;
     }
   }

 if ( ! TransOverCountable &&
      ! TransUnderCountable )
  {
   NumTrans=Amount=0;
   return false;
  }

 Amount = AmountUnderCountable + AmountOverCountable;
 if ( AmountUnderCountable > NonCountableAccumLimit )
  NumTrans = TransOverCountable + TransUnderCountable;
 else
  NumTrans = TransOverCountable;

 return true;
}

//-------------------
// Do the R1 Age rule
//-------------------
bool CheckAuth::doR1()
{
 int Age;
 uint Amount;

 // Insure rule is enabled and Age and Amount are present
 if ( authRules.Data.R1.R1Enable == ' ' ||
      (Age=ConsumerAge()) == 0  ||
      (Amount=CheckAmount()) == 0 )
  return false;

 // Don't do R1 rule if we have a check number unless the R3 rule
 // is disabled.
 if ( CheckNumber() != 0 &&
      authRules.Data.R3.R3Enable != ' ' )
  return false;

 // Do the Negative rules
 if ( (! isEmpty(authRules.Data.R1.R1ReferAge1) &&
       Age < authRules.Data.R1.R1ReferAge1 &&
       Amount > (DWORD) authRules.Data.R1.R1ReferAmt1 )
       ||
      (! isEmpty(authRules.Data.R1.R1ReferAge2) &&
       Age < authRules.Data.R1.R1ReferAge2 &&
       Amount > (DWORD) authRules.Data.R1.R1ReferAmt2 ) )
  {
   putRcode(RCODE_R1DECLINE);
   return true;
  }

 // Do the positive rules
 if ( (! isEmpty(authRules.Data.R1.R1OvrdAge1) &&
       Age > authRules.Data.R1.R1OvrdAge1 &&
       Amount < (DWORD) authRules.Data.R1.R1OvrdAmt1 )
       ||
      (! isEmpty(authRules.Data.R1.R1OvrdAge2) &&
       Age > authRules.Data.R1.R1OvrdAge2 &&
       Amount < (DWORD) authRules.Data.R1.R1OvrdAmt2 ) )
  {
   // JHE061698 Limit Overrides
   if ( ! maxOverrides() )
     putRcode(RCODE_R1APPROVAL);

   return false;
  }

 return false;
}

//---------------------------------
// Do the R2 Merchant Activity rule
//---------------------------------
bool CheckAuth::doR2()
{
 uint CountableAmount;
 uint NonCountableAccumLimit;
 uint Amount;
 uint NumTrans;
 uint TranAmount = CheckAmount();

 // MAB121099
 int ii, jj, iWasA_P, iWasA_M_Or_L, iItWasAPhone, iItWasAMicr;
 int ActivityCount;
 ACTIVITY* ActivityRecs;

 //logg(__LINE__, "ChAuth In doR2()","",0,0);

 // Insure rule is enabled and that there is activity
 if ( authRules.Data.R2.R2Enable == ' ' )
  return false;

 if ( authRules.Data.Countable.CountableEnable==' ' )
  CountableAmount=NonCountableAccumLimit=0;
 else
  {
   CountableAmount=authRules.Data.Countable.CountableAmt;
   NonCountableAccumLimit=authRules.Data.Countable.CountableAccum;
  }

#if 1
   // MAB121099 - Allow extra activity from same phone
   // If there is too much activity, if match was on phone only, Accept
   //   char   .Ids[4]

     // I think this is needed for initialization. The following is a given
   ActivityRecs=hasActivity(&ActivityCount);   // previous activity
   iItWasAPhone = 0;
   iWasA_M_Or_L = 0;
   iItWasAMicr = 0;   // MAB122899

   for (ii = 0; ii < ActivityCount; ++ii)
   {
    iWasA_P = 0;
    //logg(__LINE__, "ChAuth In doR2 Ids is",ActivityRecs[ii].Ids,0,ii);

    for(jj = 0; jj < 3; jj++)
     if((ActivityRecs[ii].Ids[jj] == 'P')||(ActivityRecs[ii].Ids[jj] == 'p'))
     {
        iWasA_P = 1;
        //  logg(__LINE__, "ChAuth In doR2 ItWasA_P","",0,0);
     }

    for(jj = 0; jj < 3; jj++)
     if((ActivityRecs[ii].Ids[jj] == 'M') || (ActivityRecs[ii].Ids[jj] == 'm'))
     {
        iWasA_M_Or_L = 1;
        iItWasAMicr  = 1;
        //logg(__LINE__, "ChAuth In doR2 ItWasA_ML","",0,0);
     }

    for(jj = 0; jj < 3; jj++)
     if((ActivityRecs[ii].Ids[jj] == 'L') || (ActivityRecs[ii].Ids[jj] == 'l')
       )
        iWasA_M_Or_L = 1;

    if((iWasA_M_Or_L == 0) && (iWasA_P == 1))
        iItWasAPhone = 1;

    //logg(__LINE__, "ChAuth In doR2 isPhone is","",iItWasAPhone,0);
    //logg(__LINE__, "ChAuth In doR2 ML and P is","",iWasA_M_Or_L,(long)iWasA_P);
   }
   //logg(__LINE__, "ChAuth In doR2 ML and P is","",iWasA_M_Or_L,(long)iWasA_P);

   if(iWasA_M_Or_L)
      iItWasAPhone = 0;

   //logg(__LINE__, "ChAuth In doR4 iItWasAPhone is","",iItWasAPhone,0);

// end MAB120899
#endif

// MAB122899

// end MAB122899

 if ( ! isEmpty(authRules.Data.R2.R2Sic1Trans) &&
      CountActivity(authRules.Data.R2.R2Sic1Days,
                    CountableAmount,NonCountableAccumLimit,
                    NumTrans,Amount,0,0,merchantRec.Data.MerchantSic) &&
      ( NumTrans + 1 > (uint) authRules.Data.R2.R2Sic1Trans ||
        Amount + TranAmount > (uint) authRules.Data.R2.R2Sic1Amt)
      &&
      (iItWasAPhone == 0)     // MAB120899
    )
  {
    // Do it again to flag the transactions
    CountActivity(authRules.Data.R2.R2Sic1Days,
                  CountableAmount,NonCountableAccumLimit,
                  NumTrans,Amount,0,0,merchantRec.Data.MerchantSic,true);
    putRcode(RCODE_R2DECLINE);
    return true;
  }

 if ( ! isEmpty(authRules.Data.R2.R2Sic2Trans) &&
      CountActivity(authRules.Data.R2.R2Sic2Days,
                    CountableAmount,NonCountableAccumLimit,
                    NumTrans,Amount,0,0,merchantRec.Data.MerchantSic) &&
      ( NumTrans + 1 > (uint) authRules.Data.R2.R2Sic2Trans ||
      Amount + TranAmount > (uint) authRules.Data.R2.R2Sic2Amt)
      &&
      (iItWasAPhone == 0)     // MAB120899
    )
  {

   CountActivity(authRules.Data.R2.R2Sic2Days,
                 CountableAmount,NonCountableAccumLimit,
                 NumTrans,Amount,0,0,merchantRec.Data.MerchantSic,true);
   putRcode(RCODE_R2DECLINE);
   return true;
  }

 if ( ! isEmpty(authRules.Data.R2.R2Acct1Trans) &&
     CountActivity(authRules.Data.R2.R2Acct1Days,
                   CountableAmount,NonCountableAccumLimit,
                   NumTrans,Amount,merchantRec.Data.MerchantID) &&
     ( NumTrans + 1 > (uint) authRules.Data.R2.R2Acct1Trans ||
     Amount + TranAmount > (uint) authRules.Data.R2.R2Acct1Amt)
     &&
     (iItWasAPhone == 0)     // MAB120899
    )
  {
   CountActivity(authRules.Data.R2.R2Acct1Days,
                 CountableAmount,NonCountableAccumLimit,
                 NumTrans,Amount,merchantRec.Data.MerchantID,0,0,true);
   putRcode(RCODE_R2DECLINE);
   return true;
  }

 if ( ! isEmpty(authRules.Data.R2.R2Acct2Trans) &&
     CountActivity(authRules.Data.R2.R2Acct2Days,
                   CountableAmount,NonCountableAccumLimit,
                   NumTrans,Amount,merchantRec.Data.MerchantID) &&
     ( NumTrans +1 > (uint) authRules.Data.R2.R2Acct2Trans ||
     Amount + TranAmount > (uint) authRules.Data.R2.R2Acct2Amt)
     &&
     (iItWasAPhone == 0)     // MAB120899
    )
  {
   CountActivity(authRules.Data.R2.R2Acct2Days,
                 CountableAmount,NonCountableAccumLimit,
                 NumTrans,Amount,merchantRec.Data.MerchantID,0,0,true);
   putRcode(RCODE_R2DECLINE);
   return true;
  }

 if ( ! isEmpty(authRules.Data.R2.R2Loc1Trans) &&
     CountActivity(authRules.Data.R2.R2Loc1Days,
                   CountableAmount,NonCountableAccumLimit,
                   NumTrans,Amount,0,merchantRec.Data.MerchantID) &&
     ( NumTrans + 1> (uint) authRules.Data.R2.R2Loc1Trans ||
     Amount + TranAmount > (uint) authRules.Data.R2.R2Loc1Amt)
     &&
     (iItWasAPhone == 0)     // MAB120899
    )
  {
   CountActivity(authRules.Data.R2.R2Loc1Days,
                 CountableAmount,NonCountableAccumLimit,
                 NumTrans,Amount,0,merchantRec.Data.MerchantID,0,true);
   putRcode(RCODE_R2DECLINE);
   return true;
  }

 if ( ! isEmpty(authRules.Data.R2.R2Loc2Trans) &&
     CountActivity(authRules.Data.R2.R2Loc2Days,
                   CountableAmount,NonCountableAccumLimit,
                   NumTrans,Amount,0,merchantRec.Data.MerchantID) &&
      ( NumTrans +1 > (uint) authRules.Data.R2.R2Loc2Trans ||
      Amount + TranAmount > (uint) authRules.Data.R2.R2Loc2Amt)
      &&
      (iItWasAPhone == 0)     // MAB120899
    )
  {
   CountActivity(authRules.Data.R2.R2Loc2Days,
                 CountableAmount,NonCountableAccumLimit,
                 NumTrans,Amount,0,merchantRec.Data.MerchantID,0,true);
   putRcode(RCODE_R2DECLINE);
   return true;
  }

 return false;
}

//----------------------------
// Do the R3 Check Number rule
//----------------------------
#define INRANGE(num,var) (num >= (uint) authRules.Data.R3.R3##var##From &&\
                              num <= (uint) authRules.Data.R3.R3##var##To)
bool CheckAuth::doR3()
{
 uint CheckNum;
 int Age=ConsumerAge();
 unsigned int Amount=CheckAmount();
 
 // Insure rule is enabled and that there is activity
 if ( authRules.Data.R3.R3Enable == ' ' ||
      (CheckNum=CheckNumber()) == 0 )
  return false;

 // Two sets of params. One with age and one without
 if ( ! Age )
  {
   // Neg rule without age
   if ( ( ! isEmpty(authRules.Data.R3.R3ReferNum1From) &&
          INRANGE(CheckNum,ReferNum1) &&
          INRANGE(Amount,ReferAmt1) ) ||

        ( ! isEmpty(authRules.Data.R3.R3ReferNum2From) &&
          INRANGE(CheckNum,ReferNum2) &&
          INRANGE(Amount,ReferAmt2) ) ||

        ( ! isEmpty(authRules.Data.R3.R3ReferNum3From) &&
          INRANGE(CheckNum,ReferNum3) &&
          INRANGE(Amount,ReferAmt3) ) ||

        ( ! isEmpty(authRules.Data.R3.R3ReferNum4From) &&
          INRANGE(CheckNum,ReferNum4) &&
          INRANGE(Amount,ReferAmt4) ) )
    {
     putRcode(RCODE_R3DECLINE);
     return true;
    }
   // Pos rule without age
   if ( ( ! isEmpty(authRules.Data.R3.R3OvrdNum1From) &&
          INRANGE(CheckNum,OvrdNum1) &&
          INRANGE(Amount,OvrdAmt1) ) ||

        ( ! isEmpty(authRules.Data.R3.R3OvrdNum2From) &&
          INRANGE(CheckNum,OvrdNum2) &&
          INRANGE(Amount,OvrdAmt2) ) ||

        ( ! isEmpty(authRules.Data.R3.R3OvrdNum3From) &&
          INRANGE(CheckNum,OvrdNum3) &&
          INRANGE(Amount,OvrdAmt3) ) ||

        ( ! isEmpty(authRules.Data.R3.R3OvrdNum4From) &&
          INRANGE(CheckNum,OvrdNum4) &&
          INRANGE(Amount,OvrdAmt4) ) )
    {
     // JHE061698 Limit Overrides
     if ( ! maxOverrides() )
       putRcode(RCODE_R3APPROVAL);

     return false;
    }
  }
 else
  {
   // Neg rule with age
   if ( ( ! isEmpty(authRules.Data.R3.R3AgeReferAge1) &&
          Age < authRules.Data.R3.R3AgeReferAge1 &&
          INRANGE(CheckNum,AgeReferNum1) &&
          INRANGE(Amount,AgeReferAmt1) ) ||

        ( ! isEmpty(authRules.Data.R3.R3AgeReferAge2) &&
          Age < authRules.Data.R3.R3AgeReferAge2 &&
          INRANGE(CheckNum,AgeReferNum2) &&
          INRANGE(Amount,AgeReferAmt2) ) ||

        ( ! isEmpty(authRules.Data.R3.R3AgeReferAge3) &&
          Age < authRules.Data.R3.R3AgeReferAge3 &&
          INRANGE(CheckNum,AgeReferNum3) &&
          INRANGE(Amount,AgeReferAmt3) ) ||

        ( ! isEmpty(authRules.Data.R3.R3AgeReferAge4) &&
          Age < authRules.Data.R3.R3AgeReferAge4 &&
          INRANGE(CheckNum,AgeReferNum4) &&
          INRANGE(Amount,AgeReferAmt4) ) )
    {
     putRcode(RCODE_R3DECLINE);
     return true;
    }
   // Pos rule with age
   if ( ( ! isEmpty(authRules.Data.R3.R3AgeOvrdAge1) &&
          Age > authRules.Data.R3.R3AgeOvrdAge1 &&
          INRANGE(CheckNum,AgeOvrdNum1) &&
          INRANGE(Amount,AgeOvrdAmt1) ) ||

        ( ! isEmpty(authRules.Data.R3.R3AgeOvrdAge2) &&
          Age > authRules.Data.R3.R3AgeOvrdAge2 &&
          INRANGE(CheckNum,AgeOvrdNum2) &&
          INRANGE(Amount,AgeOvrdAmt2) ) ||

        ( ! isEmpty(authRules.Data.R3.R3AgeOvrdAge3) &&
          Age > authRules.Data.R3.R3AgeOvrdAge3 &&
          INRANGE(CheckNum,AgeOvrdNum3) &&
          INRANGE(Amount,AgeOvrdAmt3) ) ||

        ( ! isEmpty(authRules.Data.R3.R3AgeOvrdAge4) &&
          Age > authRules.Data.R3.R3AgeOvrdAge4 &&
          INRANGE(CheckNum,AgeOvrdNum4) &&
          INRANGE(Amount,AgeOvrdAmt4) ) )
    {
     // JHE061698 Limit overrides
     if ( ! maxOverrides() )
       putRcode(RCODE_R3APPROVAL);
       
     return false;
    }
  }

 return false;
}

//------------------------------
// Do the R4 Check Sequence rule
//------------------------------
bool CheckAuth::doR4()
{
 uint CheckNum;  // current check number
 uint Gap;
 int  i, n;
 int ActivityCount;
 ACTIVITY* ActivityRecs;

 char MICR[20];
 int  Len=sizeof(MICR);

// MAB120899
 int jj, iWasA_P, iWasA_M_Or_L;

// MAB 122899
 int iItWasAMicr = 0;

 logg(__LINE__, "ChAuth Start doR4()","",0,0);

 if ( ! GetFld(FN_BANKNUMBER,Len,MICR) )
  return false;

 if ( authRules.Data.R4.R4Enable == ' ' ||              // disabled
      (CheckNum=CheckNumber()) == 0 ||                  // no current check #
      (ActivityRecs=hasActivity(&ActivityCount)) == 0  )// no previous activity
  return false;

 //------------------------------------------------------------------------
 // 6/16/98 - If there is a Check Sequence referral already on file without
 // a subsequent approval, then this customer will continue to get Check
 // sequence referrals until they call in and get an approval or the
 // transaction is 10 days old.

 //logg(__LINE__, "ChAuth In doR4","",0,0);
 for (i=0; i<ActivityCount; ++i)
   if ( ActivityRecs[i].Rcode == RCODE_R4DECLINE &&
        ActivityRecs[i].Age < R4_AGEOFF_PERIOD )
     {
      // Make sure there's an Approval on file for this check number
      for (n=0; n<ActivityCount; ++n)
       if ( ActivityRecs[n].Age <= ActivityRecs[i].Age &&
            ActivityRecs[n].CheckNumber == ActivityRecs[i].CheckNumber &&
            ISAPPROVALRCODE(ActivityRecs[n].Rcode) )
        break;
      // No approval .. Issue the R4
      if ( n==ActivityCount )
       {
        shiftCase(ActivityRecs[i].Ids,sizeof(ActivityRecs[i].Ids));
        putRcode(RCODE_R4DECLINE);
        logg(__LINE__, "ChAuth In doR4","",0,0);
        return true;
       }
     }

 // End 6/16/98
 //------------------------------------------------------------------------
 //logg(__LINE__, "ChAuth In doR4","",0,0);

// MAB120499 - R4 Check Number - Allow duplicate check number for certain Micr
  if ( ActivityRecs[0].CheckNumber == CheckNum )
   {
   if ( ! GetFld(FN_BANKNUMBER,Len,MICR) )
    ;
   else
    {
      if(!(memcmp(MICR, "122000247",9)))
        return false;
    }
   }
// end MAB120499
 logg(__LINE__, "ChAuth In doR4","",0,0);

 // JHE 070898 Look for a duplicate Check Number on file
 for (i=0; i<ActivityCount; ++i)
  if ( ActivityRecs[i].CheckNumber == CheckNum )
   {

    logg(__LINE__, "Chkuth In doR4","Duplicate",0,0);
    uint  ElapsedTime=0;

    if ( ActivityRecs[i].Age == 0 )
     {
      // Calc seconds since midnight and compute elapsed time in seconds
      uint mySecs = Now.tm_hour * 3600 + Now.tm_min * 60 + Now.tm_sec;
      if ( mySecs >= ActivityRecs[i].Time )
       ElapsedTime = mySecs - ActivityRecs[i].Time;
       logg(__LINE__, "Chkuth In doR4","Time",(long)ElapsedTime,0);
     }

//#### This sorta looks like it
// MAB030701
#if 0
    char szbuf[256];
    char szbuf1[16];
    char szbuf2[16];

    memcpy(szbuf1,merchantRec.Data.MerchantID,5);
    memcpy(szbuf1,ActivityRecs[i].MerchID,5);
    szbuf1[5] = '\0';
    szbuf2[5] = '\0';
    logg(__LINE__, "ChAuth In doR4","",0,0);

    sprintf(szbuf,"~%ld~~%ld~~%ld~~%s~~%s~~%ld~c#=%ld~",
                          (long)ActivityRecs[i].Age,
                          (long)CheckAmount(),
                          (long)ActivityRecs[i].Amount,
                          szbuf1, //merchantRec.Data.MerchantID,
                          szbuf2,//ActivityRecs[i].MerchID,
                          (long)ElapsedTime,
                          (long)ActivityRecs[n].CheckNumber);
    logg(__LINE__, "ChAuth In doR4", szbuf,0,0);
#endif

    // Generate the R4 unless it's the same Amount and Merchant Number and
    // within 5 minutes of the original.
    if ( ActivityRecs[i].Age != 0 ||
         ( CheckAmount() != ActivityRecs[i].Amount &&
           ActivityRecs[i].Amount != 0 ) ||
         memcmp(merchantRec.Data.MerchantID,ActivityRecs[i].MerchID,
                   sizeof(ActivityRecs[i].MerchID)) ||
         ElapsedTime > 5 * 60 ) // ??? Change this to more time??
     {
      shiftCase(ActivityRecs[i].Ids,sizeof(ActivityRecs[i].Ids));
      putRcode(RCODE_R4DECLINE);
      logg(__LINE__, "ChAuth In doR4, R4 time > 5 min", "This is the error",0,0);
      return true;
     }
     else
      logg(__LINE__, "ChAuth In doR4","time < 5 min",0,0);
   }

 logg(__LINE__, "ChAuth In doR4","",0,0);

 // Find the highest check number we have on file
 // n is # of record with highest check number

 // JHE121698 - now using only approvals and declines, not errors
 DWORD highCheckNumber=0;
 for (i=0; i<ActivityCount; ++i)                   // for each record
  if ( ! isEmpty(ActivityRecs[i].CheckNumber) &&
       ActivityRecs[i].CheckNumber > highCheckNumber &&
       ! RCInfo.IsErrorRcode(ActivityRecs[i].Rcode) )
   {
    n=i;
    highCheckNumber=ActivityRecs[i].CheckNumber;
   }

  // No check numbers to use?
  if ( highCheckNumber==0 )
   return false;

  // if the highest number is 0, skip it.
  //if( ActivityRecs[n].CheckNumber == 0)   /* MAB091597A */
  //{
   //  logg(__LINE__, "ChAuth In doR4", "checknumber is",0,ActivityRecs[n].CheckNumber);
  //   return false;
  //}
  

 // Check the backward gap
 if ( CheckNum < ActivityRecs[n].CheckNumber )
  {
   Gap = ActivityRecs[n].CheckNumber - CheckNum;
   if ( ! isEmpty(authRules.Data.R4.R4BackGap) &&
        Gap > (uint)  authRules.Data.R4.R4BackGap &&
        ActivityRecs[n].Age < authRules.Data.R4.R4BackDays )
    {

//   MAB120899
// If there is forward or back gap, if match was on phone only, Accept
//   char   .Ids[4]

     iWasA_P = 0;
     iWasA_M_Or_L = 0;

     logg(__LINE__, "ChAuth In doR4 Ids is",ActivityRecs[n].Ids,0,n);

     for(jj = 0; jj < 3; jj++)
      if((ActivityRecs[n].Ids[jj] == 'P') ||(ActivityRecs[n].Ids[jj] == 'p'))
         iWasA_P = 1;

    for(jj = 0; jj < 3; jj++)
     if((ActivityRecs[n].Ids[jj] == 'M') || (ActivityRecs[n].Ids[jj] == 'm'))
     {
        iWasA_M_Or_L = 1;
        iItWasAMicr  = 1;
        //logg(__LINE__, "ChAuth In doR2 ItWasA_ML","",0,0);
     }

    for(jj = 0; jj < 3; jj++)
     if((ActivityRecs[n].Ids[jj] == 'L') || (ActivityRecs[n].Ids[jj] == 'l')
       )
        iWasA_M_Or_L = 1;

     if(iWasA_M_Or_L == 0)
       if(iWasA_P == 1)
       {
         //logg(__LINE__, "ChAuth In doR4 ML and P is","",iWasA_M_Or_L,(long)iWasA_P);
         return false;
       }

// end MAB120899

     // Shift the Ids to uppercase to indicate that this is the
     // transaction which triggered the Rcode
     shiftCase(ActivityRecs[n].Ids,sizeof(ActivityRecs[n].Ids));
     putRcode(RCODE_R4DECLINE);
 logg(__LINE__, "ChAuth In doR4","",0,0);
     return true;
    }
   return false;
 } // end backwards gap

 // Check the forward gap
 Gap = CheckNum - ActivityRecs[n].CheckNumber;

 if ( ( ! isEmpty(authRules.Data.R4.R4FwdGap1) &&
        Gap > (uint) authRules.Data.R4.R4FwdGap1 &&
        ActivityRecs[n].Age < authRules.Data.R4.R4FwdDays1) ||

      ( ! isEmpty(authRules.Data.R4.R4FwdGap2) &&
        Gap > (uint) authRules.Data.R4.R4FwdGap2 &&
        ActivityRecs[n].Age < authRules.Data.R4.R4FwdDays2) ||

      ( ! isEmpty(authRules.Data.R4.R4FwdGap3) &&
        Gap > (uint) authRules.Data.R4.R4FwdGap3 &&
        ActivityRecs[n].Age < authRules.Data.R4.R4FwdDays3) )
  {

//   MAB120899
// If there is forward or back gap, if match was on phone only, Accept

     iWasA_P = 0;
     iWasA_M_Or_L = 0;

     logg(__LINE__, "ChAuth In doR4 Ids is",ActivityRecs[n].Ids,0,n);

     for(jj = 0; jj < 3; jj++)
      if((ActivityRecs[n].Ids[jj] == 'P') ||(ActivityRecs[n].Ids[jj] == 'p'))
         iWasA_P = 1;

    for(jj = 0; jj < 3; jj++)
     if((ActivityRecs[n].Ids[jj] == 'M') || (ActivityRecs[n].Ids[jj] == 'm'))
     {
        iWasA_M_Or_L = 1;
        iItWasAMicr  = 1;
        //logg(__LINE__, "ChAuth In doR2 ItWasA_ML","",0,0);
     }

    for(jj = 0; jj < 3; jj++)
     if((ActivityRecs[n].Ids[jj] == 'L') || (ActivityRecs[n].Ids[jj] == 'l')
       )
        iWasA_M_Or_L = 1;

     if(iWasA_M_Or_L == 0)
       if(iWasA_P == 1)
       {
         //logg(__LINE__, "ChAuth In doR4 ML and P is","",iWasA_M_Or_L,(long)iWasA_P);
         return false;
       }
// end MAB120899

   // Shift the Ids to uppercase to indicate that this is the
   // transaction which triggered the Rcode
   shiftCase(ActivityRecs[n].Ids,sizeof(ActivityRecs[n].Ids));
   putRcode(RCODE_R4DECLINE);
   logg(__LINE__, "ChAuth In doR4","",0,0);
   return true;
  }  // end forward gap
 logg(__LINE__, "ChAuth In doR4","",0,0);

 return false;
}

//----------------------------
// Do the R5 single check rule
//----------------------------
bool CheckAuth::doR5()
{
 if ( authRules.Data.R5.R5Enable==' ' ||
      isEmpty(authRules.Data.R5.R5Amt) )
  return false;

 if ( CheckAmount() > (DWORD) authRules.Data.R5.R5Amt )
  {
   putRcode(RCODE_R5DECLINE);
   return true;
  }

 return false;
}

//-------------------------------
// Do the R6 OUT of State ID rule
//-------------------------------
bool CheckAuth::doR6()
{
 char State[3];
 int  Len=sizeof(State);
 uint NumTrans;
 uint AccumAmount;
 uint TranAmount = CheckAmount();

 if ( authRules.Data.R6.R6Enable == ' ' ||
      isEmpty(authRules.Data.R6.R6Amt) ||
      ! GetFld(FN_LICENSESTATE,State,Len) ||
      memcmp(State,merchantRec.Data.MerchantState,2) == 0 )
  return false;

 if ( TranAmount > (DWORD) authRules.Data.R6.R6Amt )
  {
   putRcode(RCODE_R6DECLINE);
   return true;
  }

 if ( CountActivity(30,0,0,NumTrans,AccumAmount) &&
      AccumAmount + TranAmount > (DWORD) authRules.Data.R6.R6Accum )
  {
   putRcode(RCODE_R6DECLINE);
   return true;
  }

 return false;
}

//-------------------------------
// Do the R7 Period Activity rule
//-------------------------------
bool CheckAuth::doR7()
{
 uint CountableAmount;
 uint NonCountableAccumLimit;
 uint  NumTrans;
 uint  AccumAmount;
 uint  TranAmount = CheckAmount();

 if ( authRules.Data.R7.R7Enable == ' ' )
  return false;

 if ( authRules.Data.Countable.CountableEnable==' ' )
  CountableAmount=NonCountableAccumLimit=0;
 else
  {
   CountableAmount=authRules.Data.Countable.CountableAmt;
   NonCountableAccumLimit=authRules.Data.Countable.CountableAccum;
  }

 if ( ! isEmpty(authRules.Data.R7.R7DailyNum) &&
      CountActivity(1,CountableAmount,NonCountableAccumLimit,
                    NumTrans,AccumAmount) &&
     ( NumTrans + 1 > (uint) authRules.Data.R7.R7DailyNum ||
       AccumAmount + TranAmount > (uint) authRules.Data.R7.R7DailyAmt) )
  {
   // Do it again to flag the transactions
   CountActivity(1,CountableAmount,NonCountableAccumLimit,
                    NumTrans,AccumAmount,0,0,0,true);
   putRcode(RCODE_R7DECLINE);
   return true;
  }

 if ( ! isEmpty(authRules.Data.R7.R7Per1Days) &&
      CountActivity(authRules.Data.R7.R7Per1Days,
                    CountableAmount,NonCountableAccumLimit,
                    NumTrans,AccumAmount) &&
     ( NumTrans + 1 > (uint) authRules.Data.R7.R7Per1Num ||
       AccumAmount + TranAmount > (uint) authRules.Data.R7.R7Per1Amt) )
  {
   CountActivity(authRules.Data.R7.R7Per1Days,
                 CountableAmount,NonCountableAccumLimit,
                 NumTrans,AccumAmount,0,0,0,true);
   putRcode(RCODE_R7DECLINE);
   return true;
  }

 if ( ! isEmpty(authRules.Data.R7.R7Per2Days) &&
      CountActivity(authRules.Data.R7.R7Per2Days,
                    CountableAmount,NonCountableAccumLimit,
                    NumTrans,AccumAmount) &&
     ( NumTrans + 1 > (uint) authRules.Data.R7.R7Per2Num ||
       AccumAmount + TranAmount > (uint) authRules.Data.R7.R7Per2Amt) )
  {
   CountActivity(authRules.Data.R7.R7Per2Days,
                 CountableAmount,NonCountableAccumLimit,
                 NumTrans,AccumAmount,0,0,0,true);
   putRcode(RCODE_R7DECLINE);
   return true;
  }

 if ( ! isEmpty(authRules.Data.R7.R7Per3Days) &&
      CountActivity(authRules.Data.R7.R7Per3Days,
                    CountableAmount,NonCountableAccumLimit,
                    NumTrans,AccumAmount) &&
     ( NumTrans + 1 > (uint) authRules.Data.R7.R7Per3Num ||
       AccumAmount + TranAmount > (uint) authRules.Data.R7.R7Per3Amt) )
  {
   CountActivity(authRules.Data.R7.R7Per3Days,
                 CountableAmount,NonCountableAccumLimit,
                 NumTrans,AccumAmount,0,0,0,true);
   putRcode(RCODE_R7DECLINE);
   return true;
  }

 return false;
}

//------------------------------------------------
// Return if the Check is in the Merchant's Region
//------------------------------------------------
bool CheckAuth::isCheckInRegion()
{
 char MICR[20];
 int  Len=sizeof(MICR);
 uint region;

 if ( ! GetFld(FN_BANKNUMBER,Len,MICR) )
  return false;

 region = _atoi(MICR,2);

 // If the region is between 21 and 32 then consider it an S&L
 // Bank number and subtract 20 from it
 if ( region >= 21 &&
      region <= 32 )
  region -= 20;

 //logg(__LINE__, "ChAuth In isCheckInRegion", "region is", 0, (int)region);
 // Figure regions > 12 are canadian
 if ( region > 12 &&
      isCanadian(merchantRec.Data.MerchantState) )
  return false;  // No rules broken

 //logg(__LINE__, "ChAuth In isCheckInRegion", "region is not", 0, 0);
 //if (! merchantRec.isRegionSelected(region) )
 if ( merchantRec.isRegionSelected(region) )
 {
  //return false;  /* MAB 091197A */
  return true;
 }

 // return true;
 return false;     /* MAB 091197A */
}

//-----------------------------------
// Do the R8 out of Region Check rule
//-----------------------------------
bool CheckAuth::doR8()
{
 uint NumTrans;
 uint AccumAmount;
 uint TranAmount = CheckAmount();

// logg(__LINE__, "ChAuth In doR8", "", 0, 0);
 if ( authRules.Data.R8.R8Enable == ' ' ||
      isEmpty(authRules.Data.R8.R8Amt) ||
      ! GetFld(FN_BANKNUMBER) ||
      isCheckInRegion() )
  {
  return false;  // means no rules broken
  }

 if ( TranAmount > (uint) authRules.Data.R8.R8Amt )
  {
   putRcode(RCODE_R8DECLINE);
   return true;
  }

 if ( CountActivity(30,0,0,NumTrans,AccumAmount) &&
      AccumAmount + TranAmount > (uint) authRules.Data.R8.R8Accum )
  {
   putRcode(RCODE_R8DECLINE);
   return true;
  }
 return false;
}

//--------------------------------
// put an Rcode to the Rcode stack
//--------------------------------
void CheckAuth::putRcode(BYTE Rcode)
{
 BYTE rcodes[51];
 int  Count=sizeof(rcodes)-1;
 int  i;

 if ( ! GetFld(FN_RCODES,rcodes,Count) )
  Count=0;

 // Don't duplicate it
 for (i=0; i<Count; ++i)
  if ( rcodes[i]==Rcode )
   return;
 rcodes[Count]=Rcode;
 PutFld(FN_RCODES,rcodes,Count+1);
}

//-------------
// Do the Rules
//-------------
bool CheckAuth::doRules()
{
 bool okay=true;
// bool MicrAcctNumokay=true;

 // Explain what I'm doing here
 ACTIVITY* ActivityRecs;
 int ActivityCount;
 //logg(__LINE__, "ChAuth In doRules()","",0,0);
 if ( (ActivityRecs=hasActivity(&ActivityCount)) != 0 )
  {
   int i;
   for (i=0; i<ActivityCount; ++i)
    shiftCase(ActivityRecs[i].Ids,sizeof(ActivityRecs[i].Ids),false);
  }

 if ( ! doR1() ) okay = false;
 if ( ! doR2() ) okay = false;
 if ( ! doR3() ) okay = false;
 if ( ! doR4() ) okay = false;
 if ( ! doR5() ) okay = false;
 if ( ! doR6() ) okay = false;
 if ( ! doR7() ) okay = false;
 if ( ! doR8() ) okay = false;

 //logg(__LINE__, "ChAuth In doRules()","",0,0);

// if not okay, see if a positive override was generated
 if ( ! okay )
  {
   BYTE rcodes[51];
   int  Count=sizeof(rcodes)-1;
   int  i;
   //logg(__LINE__, "ChAuth In doRules()","",0,0);

   if (  GetFld(FN_RCODES,rcodes,Count) )
    for(i=0; i<Count; ++i)
     if ( ISAPPROVALRCODE(rcodes[i]) )
       okay = true; //~ return true; - // can still be rejected

   //~return false;
   if(! okay)
      return false;
  }
  //logg(__LINE__, "ChAuth In doRules()","",0,0);

 return okay;
}

//---------------------------------------
// Get the Age. Return 0 if not available
//---------------------------------------
int CheckAuth::ConsumerAge()
{
 int   Age;
 char  Dob[7];
 int   Len;
 int   YearOfBirth;

 // If already calculated and stored in message, use it.
 if ( GetFld(FN_CONSUMERAGE,(DWORD)Age) )
  return Age;

 // Get the date of birth. If I don't have one, see if I have a positive
 // file record with a DOB in it.
 Len=sizeof(Dob);
 if ( ! GetFld(FN_DOB,Len,Dob) ||
      strlen(Dob) != 6 )
  if ( hasPositiveFileRecord &&
       PosFileRecord.DateOfBirth )
   sprintf(Dob,"%06d",PosFileRecord.DateOfBirth);
  else
   return 0;

 // Calculate year of birth. Year is always last two digits of 6 digit
 // field.
 Dob[6]=0;
 YearOfBirth=atoi(&Dob[4]);

 // The tm_year is year-1900. It's supposed to go to 100 in the year
 // 2000.
 Age = Now.tm_year - YearOfBirth;

 // We figure if it comes out less than a person of checkwriting age
 // then the person must be a centenarian (a very old fart).
 if ( Age < 16 )
  Age += 100;

 // Put the age into the message as a single byte integer
 PutFld(FN_CONSUMERAGE,(BYTE)Age);
 return Age;
}

//---------------------
// Get the check number
//---------------------
DWORD CheckAuth::CheckNumber()
{
 DWORD Num;
 GetFld(FN_CHECKNUMBER,Num);
 return Num;
}

// Determine if a number is duplicated in an array
static bool duplicateNumber(WORD* numbers,WORD number,int n)
{
 register i;
 for (i=0; i<n; ++i)
  if ( numbers[i] == number )
   return true;

 return false;
}

//--------------------------------------------------------
// Generate an approval number and store it in the message
//
// This Approval number generation algorithm was intended to
// create an approval number derived from transaction elements.
//--------------------------------------------------------
WORD CheckAuth::GenerateApprovalNumber(BYTE Rcode)
{
 unsigned ApprovalNum;
 int Len;

 if ( Rcode==0 )
  GetFld(FN_RCODE,Rcode);

 // Build a string from which we will derive a hash value using
 // AMOUNT,DATE and MERCHANT NUMBER.
 char hashString[5+6+15+26+1];

 memset(hashString,0,sizeof(hashString));
 Len=6;
 GetFld(FN_MERCHID,Len,hashString);
 sprintf(&hashString[5],
         "%02d%02d%02d",Now.tm_mon+1,Now.tm_mday,Now.tm_year%100);
 Len=15;
 GetFld(FN_AMOUNT,Len,&hashString[5+6]);
 Len=25;
 if ( ! GetFld(FN_LICENSE,Len,&hashString[strlen(hashString)]) )
  {
   Len=25;
   GetFld(FN_BANKACCOUNT,Len,&hashString[strlen(hashString)]);
  }

 // If it's a Positive File Approval, then we need it to start with
 // a '9'. If it's not Positive, we want it to start with 0-8
 if ( Rcode == RCODE_POSFILEAPPROVAL )
  ApprovalNum=string(hashString).hash() % 1000 + 9000;
 else
   if ( (ApprovalNum=string(hashString).hash() % 10000) >= 9000 )
    ApprovalNum -= 9000;

 // If this is a hold check transaction then make sure I'm not reusing an
 // approval number
 if ( isHoldCheckMerchant() )
  {
   WORD AppNum[4];
   memset(AppNum,0,sizeof(AppNum));
   GetFld(FN_HOLDCHECKAPPROVALNUM1,AppNum[0]);
   GetFld(FN_HOLDCHECKAPPROVALNUM2,AppNum[1]);
   GetFld(FN_HOLDCHECKAPPROVALNUM3,AppNum[2]);
   GetFld(FN_HOLDCHECKAPPROVALNUM4,AppNum[3]);
   while (duplicateNumber(AppNum,(WORD)ApprovalNum,4))
     ++ApprovalNum;
  }

 PutFld(FN_APPROVALNUM,(WORD)ApprovalNum);
 return (WORD)ApprovalNum;
}

//-------------------------------
// Send message to referral queue
//-------------------------------
BYTE CheckAuth::QueueMsg()
{
 BYTE ReferralNum;

 SyncMsgBuf.CopyMsgIn(this);
 SyncMsgBuf.PutFld(FN_TRANTYPE,T_PUTREFERRAL);
 if ( ! SyncMsgBuf.SendRecv(REFERRALQUEUE_DEST,NetTimeout) ||
      ! SyncMsgBuf.GetFld(FN_REFERRALNUM,ReferralNum) )
  return 0;

 PutFld(FN_REFERRALNUM,ReferralNum);
 return ReferralNum;
}

//--------------------------------------------------------------
// Check for Rcodes. Return true or false to indicate if any are
// present. Return the decline and/or the override rcode
//--------------------------------------------------------------
bool CheckAuth::getRcodes(BYTE& Decline,BYTE& Approve)
{
 char Rcodes[25];
 int  Count;
 int  i;

 Decline=Approve=0;

 if ( ! GetFld(FN_RCODES,Rcodes,Count) )
   return false;

 for (i=0; i<Count; ++i)
   if ( ISAPPROVALRCODE(Rcodes[i]) )
    {
     if (Approve==0 )
      Approve=Rcodes[i];
    }
   else
    if ( Decline==0 )
     Decline=Rcodes[i];

 // JHE061698
 // Do not Override an R4 Check Sequence
 if ( Decline==RCODE_R4DECLINE )
   Approve=0;

 return true;
}

//------------------------------------------------------------------
// Return Bad Check Status. I don't get these unless it's a referral
// situation.
//------------------------------------------------------------------
bool CheckAuth::isBadChecks()
{
 return GetFld(FN_BADCHECKS);
}

//--------------------------
// Is it a Voice Transaction?
//--------------------------
bool CheckAuth::isVoice()
{
 BYTE SourceType;

 if ( ! GetFld(FN_SOURCETYPE,SourceType) )
  return false;
 else
  return SourceType == 'V';
}

//--------------------------------
// Get Reply Text for the Message.
// Rcode must be stored.
//--------------------------------
void CheckAuth::doPosReply(BYTE Rcode,const char* Text)
{
 char TextTemplate[100];
 char TextBuf[100];
 int  Len;
 char* macroPtr;

 PutFld(FN_RCODE,Rcode);

 // If caller didn't supply Text
 // Go get the response text from the Manager Database. We give him
 // the rcode and the class name and he gives us back the text response
 if ( Text )
  strcpy(TextTemplate,Text);
 else
  {
   SyncMsgBuf.ClearMsg();
   SyncMsgBuf.PutFld(FN_TRANTYPE,T_GETPOSRESPONSETEXT);
   SyncMsgBuf.PutFld(FN_RCODE,Rcode);
   SyncMsgBuf.PutFld(FN_DBKEY,
                      isGuarantee() ? merchantRec.Data.Classes.GURespMsg :
                                      merchantRec.Data.Classes.VFYRespMsg);

   Len=sizeof(TextTemplate);
   if ( ! SyncMsgBuf.SendRecv(MANAGERDB_DEST,NetTimeout) ||
        ! SyncMsgBuf.GetFld(FN_TEXTRESPONSE,Len,TextTemplate) )
    {
     PutFld(FN_RCODE,(BYTE)0);
     strcpy(TextTemplate,"CALL %R");
    }
  }

 // Do Macro Substitutions, if any
 if ( (macroPtr=strchr(TextTemplate,'%')) != NULL )
  {
   switch(macroPtr[1])
    {
     // Approval Code
     case 'A':
      macroPtr[0]=0;
      strcpy(TextBuf,TextTemplate);
      sprintf(&TextBuf[strlen(TextBuf)],"%04d",GenerateApprovalNumber());
      strcat(TextBuf,&macroPtr[2]);
      strcpy(TextTemplate,TextBuf);
      break;

     // Referral Code
     case 'R':
      macroPtr[0]=0;
      strcpy(TextBuf,TextTemplate);
      sprintf(&TextBuf[strlen(TextBuf)],"%02d",QueueMsg());
      strcat(TextBuf,&macroPtr[2]);
      strcpy(TextTemplate,TextBuf);
      break;

     // Decline Code
     case 'D':
      break;
    }
  }

 PutFld(FN_TEXTRESPONSE,TextTemplate);
 SendMsg(POSRESPONSE_DEST);
}

//-------------------------------------------------------------------
// Check if a transaction is a Post Authorization for a PreAuthorized
// transaction.
//-------------------------------------------------------------------
BYTE CheckAuth::doPostAuth()
{
 BYTE Rcode=0;

 // Go to the Preauth database to see if there is a preauth to close
 // out.
 SyncMsgBuf.ClearMsg();
 SyncMsgBuf.PutFld(FN_TRANTYPE,T_DOPOSTAUTH);
 SyncMsgBuf.CopyFld(FN_MERCHANTRECORD,this);
 SyncMsgBuf.CopyFld(FN_LICENSESTATE,this);
 SyncMsgBuf.CopyFld(FN_LICENSE,this);
 SyncMsgBuf.CopyFld(FN_BANKNUMBER,this);
 SyncMsgBuf.CopyFld(FN_BANKACCOUNT,this);
 SyncMsgBuf.PutFld(FN_AMOUNTROUNDED,(DWORD)CheckAmount());
 SyncMsgBuf.SendRecv(PREAUTHDB_DEST,NetTimeout,NetUIRetry);
 SyncMsgBuf.GetFld(FN_RCODE,Rcode);

 //logg(__LINE__, "ChAuth Checkauth.cpp In doPostAuth()", "",RCODE_PREAUTHAPPROVAL,(int)Rcode);

 // If there was a preauth on file, see if we're exceeding the amount
 if ( Rcode==RCODE_PREAUTHAPPROVAL &&
      CheckAmount() > (uint) merchantRec.Data.PreauthAmount )
 {
// MAB062899 Not the way Lin wants it to work
//  Rcode=RCODE_PREAUTHEXCEEDED;
 ; //logg(__LINE__, "ChAuth In doPostAuth", "",RCODE_PREAUTHEXCEEDED,0);
 }

 return Rcode;
}

//---------------------------------------------------
// Attempt to Do a Pre-Authorization, returning rcode
//---------------------------------------------------
BYTE CheckAuth::doPreAuth()
{
 BYTE Rcode=0;

 SyncMsgBuf.ClearMsg();
 SyncMsgBuf.PutFld(FN_TRANTYPE,T_DOPREAUTH);
 SyncMsgBuf.CopyFld(FN_MERCHANTRECORD,this);
 SyncMsgBuf.CopyFld(FN_LICENSESTATE,this);
 SyncMsgBuf.CopyFld(FN_LICENSE,this);
 SyncMsgBuf.CopyFld(FN_BANKNUMBER,this);
 SyncMsgBuf.CopyFld(FN_BANKACCOUNT,this);
 SyncMsgBuf.SendRecv(PREAUTHDB_DEST,NetTimeout,NetUIRetry);
 SyncMsgBuf.GetFld(FN_RCODE,Rcode);
 //logg(__LINE__, "ChAuth Checkauth.cpp In doPreAuth()", "Rcode is",(long)Rcode,0);
 return Rcode;
}

//----------------------------------------------------------
// Attempt to Do a Forced Pre-Authorization, returning rcode
//----------------------------------------------------------
BYTE CheckAuth::forcePreAuth()
{
 BYTE Rcode=0;

 SyncMsgBuf.ClearMsg();
 SyncMsgBuf.PutFld(FN_TRANTYPE,T_FORCEPREAUTH);
 SyncMsgBuf.CopyFld(FN_MERCHANTRECORD,this);
 SyncMsgBuf.CopyFld(FN_LICENSESTATE,this);
 SyncMsgBuf.CopyFld(FN_LICENSE,this);
 SyncMsgBuf.CopyFld(FN_BANKNUMBER,this);
 SyncMsgBuf.CopyFld(FN_BANKACCOUNT,this);
 SyncMsgBuf.SendRecv(PREAUTHDB_DEST,NetTimeout,NetUIRetry);
 SyncMsgBuf.GetFld(FN_RCODE,Rcode);
 return Rcode;
}

//---------------------------------------------------------------
// Test the Preauth flag. Optionally set the flag and the amount to
// the Merchant Preauth amount.
//---------------------------------------------------------------
bool CheckAuth::isPreauth(bool set)
{
 if ( set )
  {
   PutFld(FN_PREAUTHFLAG,(BYTE)'Y');
   if ( hasMerchantRec )
    {
     char AmountBuf[20];
     sprintf(AmountBuf,"%d.00",merchantRec.Data.PreauthAmount);
     DelFld(FN_AMOUNTROUNDED);
     PutFld(FN_AMOUNT,AmountBuf);
    }
   return true;
  }
 else
  return GetFld(FN_PREAUTHFLAG);
}

//------------------------------------------------------------
// Return the Number of Decline Rcodes that have been breached
//------------------------------------------------------------
int CheckAuth::numRcodesBreached()
{
 BYTE Rcodes[25];
 int  Count=sizeof(Rcodes);
 int  i, n;

 if ( ! GetFld(FN_RCODES,Rcodes,Count) )
   return 0;
 for (i=n=0; i<Count; ++i)
  if ( ! ISAPPROVALRCODE(Rcodes[i]) )
   ++n;

 return n;
}

//----------------------------------------
// Return Pointer to Activity and How many
//----------------------------------------
ACTIVITY* CheckAuth::hasActivity(int* ActivityCount)
{
 int Size;

 ACTIVITY* Ptr=(ACTIVITY*)GetFldPtr(FN_ACTIVITY,&Size);
 if ( Ptr==NULL )
  {
   if ( ActivityCount )
    *ActivityCount=0;
   return NULL;
  }
 else
  {
   if ( ActivityCount )
    *ActivityCount = Size / sizeof(ACTIVITY);
   return Ptr;
  }
}

//-------------------------------------------
// Get Approval Rcode appropriate for service
//-------------------------------------------
BYTE CheckAuth::getApprovalRcode()
{
 struct MerchantRecord* Rec=(struct MerchantRecord*)
                                     GetFldPtr(FN_MERCHANTRECORD);
 if ( Rec==NULL )
  return RCODE_GUARANTEEAPPROVAL;

 switch( Rec->MerchantService )
  {
   default:
   case CHECKSERVICE_GUARANTEE:  return RCODE_GUARANTEEAPPROVAL;
   case CHECKSERVICE_PREAUTH:
     return (BYTE) (isPreauth() ? RCODE_PREAUTHAPPROVAL : RCODE_GUARANTEEAPPROVAL);
   case CHECKSERVICE_HOLDCHECK:  return RCODE_HOLDCHECKAPPROVAL;
   case CHECKSERVICE_VERIFY:     return RCODE_VERIFYAPPROVAL;
   case CHECKSERVICE_GUARANTEEVERIFY:
      return (BYTE) (isGuarantee() ? RCODE_GUARANTEEAPPROVAL :
                                     RCODE_VERIFYAPPROVAL);
  }
}


//-------------------------------------------------------------------
// Calculate the Positive File limits using the positive file record
// and the rules record.
//-------------------------------------------------------------------
bool CheckAuth::getPosfileLimits(uint& OverridePeriod,
                                 uint& MaxOverrides,
                                 uint& MaxAmount,
                                 uint& MaxAccum)
{
 int i;
 PositiveRulesRecord* Rules;
 BYTE Age;

 // If there are Parms in the Positive File record we use them unless
 // they are elminated by SIC or ACCOUNT restrictions
 if ( PosFileRecord.Parms.OverridePeriod )
  {
   // See if eliminated by merchant SIC code restrictions
   if ( merchantRec.Data.MerchantSic[0] &&
        PosFileRecord.Parms.SicCodeRestrictions[0] )
    {
     short Sic=(short)atoi(merchantRec.Data.MerchantSic);
     for (i=0; i<3; ++i)
      if ( PosFileRecord.Parms.SicCodeRestrictions[i] == Sic )
        break;

     if ( i==3 )
      goto UseMerchantLimits;
    }

   // See if eliminated by merchant account restrictions
   if ( PosFileRecord.Parms.AccountRestrictions[0][0] )
    {
     for (i=0; i<3; ++i)
      if ( memcmp(PosFileRecord.Parms.AccountRestrictions[i],
                  merchantRec.Data.MerchantID,3) == 0 )
        break;
     if ( i==3 )
      goto UseMerchantLimits;
    }

   OverridePeriod = PosFileRecord.Parms.OverridePeriod;
   MaxOverrides = PosFileRecord.Parms.NumberOfOverridesAllowed;
   MaxAmount = PosFileRecord.Parms.MaxOverrideAmount;
   MaxAccum = PosFileRecord.Parms.MaxOverrideAccum;
   return true;
  }

 UseMerchantLimits:

 // Get the positive file rules
 if ( (Rules=(PositiveRulesRecord*)GetFldPtr(FN_POSITIVERULESRECORD))
                   == NULL )
  return false;

 // Make sure this record has been initialized
 if ( isEmpty(Rules->WaitForPositiveDays) )
  return false;

 int now=(uint)time(NULL);
 // Make sure it's been long enough since the first transaction.
 if (  now < PosFileRecord.FirstApproval ||
       ( (now - PosFileRecord.FirstApproval) <
         (Rules->WaitForPositiveDays * (60*60*24))) )
  return false;

 // Use these from the Rules record
 OverridePeriod = Rules->OverridePeriod;
 MaxOverrides = Rules->MaxOverrides;
 MaxAccum = Rules->MaxOverrideAccum;

 // Get the limit from the Age/Multiplier Table
 if ( (Age=(BYTE)ConsumerAge()) == 0 )
  i=8;
 else
  for (i=0; i<7 && Age > Rules->AgeTbl[i].MaxAge; ++i);
 MaxAmount = Rules->AgeTbl[i].Limit;

 // If the multiplier line is blank, there is no adjustment
 if ( isEmpty(Rules->MultiplyIfNumApprovals) )
   return true;

 // See if Multiplication Factor applies
 if ( PosFileRecord.NumApprovals >= Rules->MultiplyIfNumApprovals &&
      PosFileRecord.AmountApprovals >= Rules->MultiplyIfAmountApprovals &&
      (uint) (time(NULL) - PosFileRecord.LastApproval) <=
          (uint) (Rules->MultiplyIfLastWithinMonths * (30*60*60*24)) )
  {
   uint YearsOnFile = (time(NULL) - PosFileRecord.FirstApproval) /
                          (365*60*60*24);
   uint adjustValue;

   if ( YearsOnFile==0 )
    return true;

   adjustValue = Rules->AgeTbl[i].Multiplier[min((uint)5, YearsOnFile)][0];

   // The adjustment value has an assumed decimal point, ie., 10 = 1.0
   if ( isEmpty(adjustValue) ||
        adjustValue==10 ||
        adjustValue==0 )
     return true;

   // Increase by an order of magnitude for more precision in the
   // math
   MaxAmount *= 10;

   if ( adjustValue < 10 )
     MaxAmount=MaxAmount/10 * adjustValue;
   else
     if ( adjustValue < 20)
      MaxAmount += MaxAmount/10 * (adjustValue-10);
     else
      {
       MaxAmount *= adjustValue / 10;
       MaxAmount += MaxAmount/10 * (adjustValue%10);
      }

   // Go back to original order of magnitude
   MaxAmount /= 10;
  }

 return true;
}

//--------------------------------------------
// Check if Positive file override can be done
//--------------------------------------------
bool CheckAuth::isPositive()
{
 int i;
 int Len;
 uint NumPosFileOverrides;
 int AmountPosFileOverrides;
 int ActivityCount;
 ACTIVITY* ActivityRecs;
 uint MaxOverrideAmount;
 uint OverridePeriod;
 uint NumberOfOverridesAllowed;
 uint MaxOverrideAccum;

 // If we don't have a Positive record, go and get one
 if ( ! hasPositiveFileRecord )
  {
   // If this field is present then we've already been to the
   // positive file and there is no record on file.
   if ( GetFld(FN_POSITIVEFILESTATUS) )
    return false;
   // Indicate been there, done that
   PutFld(FN_POSITIVEFILESTATUS,(BYTE)1);
   SyncMsgBuf.ClearMsg();
   SyncMsgBuf.PutFld(FN_TRANTYPE,T_GETRECORD);
   SyncMsgBuf.CopyFld(FN_BANKNUMBER,this);
   SyncMsgBuf.CopyFld(FN_BANKACCOUNT,this);
   SyncMsgBuf.CopyFld(FN_LICENSESTATE,this);
   SyncMsgBuf.CopyFld(FN_LICENSE,this);
   SyncMsgBuf.CopyFld(FN_PHONE,this);
   SyncMsgBuf.SendRecv(POSITIVEFILE_DEST,NetTimeout,NetUIRetry);
   CopyFld(FN_POSITIVEFILERECORD,&SyncMsgBuf);
   Len=sizeof(PosFileRecord);
   hasPositiveFileRecord = GetFld(FN_POSITIVEFILERECORD,&PosFileRecord,Len);
   if ( ! hasPositiveFileRecord )
    return false;
   // JHE 06/98
   // Do not consider it a positive file hit if I'm only matching on
   // the Phone number unless the Phone Number is all I have.
   if ( (PosFileRecord.Flags & POSFILEFLAGS_HITKEYPHONE) &&
       !( (PosFileRecord.Flags & POSFILEFLAGS_HITKEYMICR) ||
          (PosFileRecord.Flags & POSFILEFLAGS_HITKEYLICENSE) ) )
     if ( GetFld(FN_LICENSESTATE) || GetFld(FN_BANKNUMBER) )
      {
       DelFld(FN_POSITIVEFILERECORD);
       return hasPositiveFileRecord = false;
      }
  }

 if ( ! getPosfileLimits(OverridePeriod,NumberOfOverridesAllowed,
                         MaxOverrideAmount,MaxOverrideAccum) )
  return false;

 if ( (uint) (AmountPosFileOverrides = CheckAmount()) >
      (uint) MaxOverrideAmount )
  return false;

 NumPosFileOverrides=1;

 // Count the previous Positive file approvals
 if ( (ActivityRecs=hasActivity(&ActivityCount)) != 0 )
  for (i=0; i<ActivityCount; ++i)
   if ( ActivityRecs[i].Rcode == RCODE_POSFILEAPPROVAL &&
        ActivityRecs[i].Age <= (BYTE) OverridePeriod )
    {
     ++NumPosFileOverrides;
     AmountPosFileOverrides += ActivityRecs[i].Amount;
    }

 return NumPosFileOverrides <= NumberOfOverridesAllowed &&
        (uint) AmountPosFileOverrides <= MaxOverrideAccum;
}

//------------------------------------------------------
// Update the Positive file with transaction information
//------------------------------------------------------
void CheckAuth::PosFileUpdate()
{
 SyncMsgBuf.ClearMsg();
 SyncMsgBuf.PutFld(FN_TRANTYPE,T_POSFILEUPDATE);
 SyncMsgBuf.CopyFld(FN_BANKNUMBER,this);
 SyncMsgBuf.CopyFld(FN_BANKACCOUNT,this);
 SyncMsgBuf.CopyFld(FN_LICENSESTATE,this);
 SyncMsgBuf.CopyFld(FN_LICENSE,this);
 SyncMsgBuf.CopyFld(FN_DOB,this);
 SyncMsgBuf.CopyFld(FN_AMOUNTROUNDED,this);
 SyncMsgBuf.CopyFld(FN_PHONE,this);
 SyncMsgBuf.CopyFld(FN_CONSUMERNAME,this);
 SyncMsgBuf.CopyFld(FN_BUSINESSNAME,this);
 SyncMsgBuf.SendRecv(POSITIVEFILE_DEST,NetTimeout,NetUIRetry);
}

//------------------------------------------------------------
// Shift the case of a sting of non-null terminated characters
//------------------------------------------------------------
static void shiftCase(char* String,int Len,bool Upper)
{
 register i;
 for (i=0; i<Len; ++i)
  String[i] = (char) (Upper ? toupper(String[i]) : tolower(String[i]));
}

//----------------------------------------------
// JHE061698
// Return if Maximum Overrides has been reached.
//---------------------------------------------

bool CheckAuth::maxOverrides()
{
 uint i;
 int ActivityCount;
 ACTIVITY* ActivityRecs=hasActivity(&ActivityCount);
 uint Count=0;
 unsigned int Amount=CheckAmount();

 if ( ActivityRecs==0 )
  return false;

 for (i=0; i<ActivityCount; ++i)
  if ( (ActivityRecs[i].Rcode == RCODE_R1APPROVAL ||
        ActivityRecs[i].Rcode == RCODE_R3APPROVAL ) &&
     ActivityRecs[i].Age < OVERRIDE_PERIOD )
  ++Count;

 if ( Count >= OVERRIDE_MAX_IF_UNDER_AMOUNT )
  return true;

 if ( Count >= OVERRIDE_MAX_IF_OVER_AMOUNT &&
      Amount >= OVERRIDE_AMOUNT )
  return true;

 return false;
}

// Moved to CheckAuth.CPP from FieldValidation.
#if VERSION_061000
// MAB060999
// This makes sure that the relationship between the Bank Number and the
// Bank Account is validated. For any Bank Number in the (list), there is
// a Minimum and Maximum length for the Bank Account Number.
// For now, these values will be read from a file. If this file gets too
// large, the values may be put in a Velocis Database, perhaps even added
// to the BankPhone table.
//$$**********************************************************************
//bool CheckAuth::isValidBankAccountAndBankNumber(int fn)
bool CheckAuth::isValidBankAccountAndBankNumber()
{
 char  szBankNumber[32];
 char  szBankAccount[32];
 int   iReturn;
 int   iLenBankNumber;
 int   iLenBankAccount;

 int  iLenMicr;

 logg(__LINE__, "ChAuth isValidBankAccountAndBankNumber()","", 0 , 0);

 iLenMicr=sizeof(szBankNumber);
 if ( ! GetFld(FN_BANKNUMBER,iLenMicr,szBankNumber) )
  return true;  // No BankNumber, so skip test
 logg(__LINE__, "ChAuth isValidBankAccountAndBankNumber() - Routing",szBankNumber, 0 , 0);

 if ( ! GetFld(FN_BANKACCOUNT,iLenMicr,szBankAccount) )
  return true;  // No BankAccount, so skip test
 logg(__LINE__, "ChAuth isValidBankAccountAndBankNumber() - Account",szBankAccount, 0 , 0);

 EditLen=sizeof(szBankNumber);
/*
 FN(BANKNUMBER);
 iLenBankNumber = GetFldLen(Fn);
 iReturn = GetFld(Fn,szBankNumber,EditLen);
 szBankNumber[iLenBankNumber - 1] = '\0';
 logg(__LINE__, "ChAuth ValidRouteAcct()-BankNum is",szBankNumber,FN_BANKNUMBER,iLenBankNumber);

 EditLen=sizeof(szBankAccount);
 FN_(BANKACCOUNT);
 Fn = Fn_;
 iLenBankAccount = GetFldLen(Fn);
 iReturn = GetFld(Fn,szBankAccount,EditLen);
 szBankAccount[iLenBankAccount - 1] = '\0';
 logg(__LINE__, "ChAuth ValidRouteAcct()-BankAccount is",szBankAccount,FN_BANKACCOUNT,iLenBankAccount);
*/

 iReturn = isValidTableBankNumber(szBankNumber, szBankAccount);
 if(iReturn == LENGTH_ERROR)
 {
    logg(__LINE__, "ChAuth Start Validation","Invalid",0,iReturn);
    return false;
 }

 logg(__LINE__, "ChAuth Start Validation","Valid",0,iReturn);
 return true;
}

//$$**********************************************************************
int CheckAuth::isValidTableBankNumber(char* pszBankNumber,char *pszBankAccount)
{
    char  szbuf[LEN_ACCT_BUF];
    FILE  *fin;
    int   ii;
    int   iReturn = 0;
    int   iCount;
    long  lBankNumber;
    static int siRecordCount;
    static int siDataHasBeenReadFromFile;
    static BANK_ROUTING_DATA dat[NUM_ROUTES_TO_READ_FROM_FILE];

    logg(__LINE__, "ChAuth Start Validation","",0,0);

    // get the data from file to memory once
    if(!( siDataHasBeenReadFromFile))  // file has not been read yet
    {
       logg(__LINE__, "ChAuth Start Validation","Read File",0,0);

      iReturn = 0;
      if((fin = fopen("g:\\programs\\routing.dat", "r")) == NULL)  // couldn't open file

      {
         iReturn = FILE_OPEN_ERROR;
         logg(__LINE__, "ChAuth File open fail on G Drive","",0,0);
      }
      if(iReturn)
      {
         iReturn = 0;
         logg(__LINE__, "ChAuth File open 2","",0,0);
         if((fin = fopen("d:\\programs\\routing.dat", "r")) == NULL)  // couldn't open file
         {
            iReturn = FILE_OPEN_ERROR;
            logg(__LINE__, "ChAuth File open fail on D drive","",0,0);
         }
      }
      if(iReturn)
      {
         iReturn = 0;
         logg(__LINE__, "ChAuth File open 3","",0,0);
         if((fin = fopen("routing.dat", "r")) == NULL)  // couldn't open file
         {
            iReturn = FILE_OPEN_ERROR;
            logg(__LINE__, "ChAuth File open fail in . directory","",0,0);
         }
      }

      if(iReturn)
         return iReturn;

      for(ii = 0; ii < NUM_ROUTES_TO_READ_FROM_FILE; ii++)
      {
         memset(szbuf,'\0',sizeof(szbuf));
         fgets(szbuf, LEN_ACCT_BUF, fin);
         logg(__LINE__, "ChAuth Buf is",szbuf,0,0);

         if(feof(fin))
            break;

         sscanf(szbuf,"%ld %d %d ",
           &dat[ii].lBankNumber, &dat[ii].iMinChars, &dat[ii].iMaxChars);

         //logg(__LINE__, "ChAuth Test dat","",dat[ii].lBankNumber,ii);

         siRecordCount++;
       }

       fclose(fin);
       siDataHasBeenReadFromFile = 1;
    }

    lBankNumber = atol(pszBankNumber);

    // MAB111599
    iReturn = 0;

    logg(__LINE__, "ChAuth Validation","Past Read File",lBankNumber,siRecordCount);
    // find data for Bank Route
    for(iCount = 0; iCount < siRecordCount; iCount++)
    {
       logg(__LINE__, "ChAuth Test dat","",dat[iCount].lBankNumber,iCount);
       if(dat[iCount].lBankNumber == lBankNumber)
       {
          logg(__LINE__, "ChAuth Test dat","",(long)dat[iCount].iMaxChars,dat[iCount].iMinChars);
          // check length
          if ( ((int)strlen(pszBankAccount) <= dat[iCount].iMaxChars) &&
               ((int)strlen(pszBankAccount) >= dat[iCount].iMinChars) )
             return 0;
          else
             iReturn = LENGTH_ERROR; // this is the only one that matters
       }
    }
    logg(__LINE__, "ChAuth Test dat","",0, iReturn);
    return iReturn;
}

#endif


#if 1
//----------------------------------------------------------------------
int CheckAuth::isThisACreditCardBankMicr(char *pszBankNumber,
                                         char *pszBankAccount )
{
    //char  szNumBuf[LEN_ACCT_BUF];
    char  szbuf[LEN_ACCT_BUF];
    FILE  *fin;
    int   ii;
    int   iReturn = 0;
    int   iCount;
    long  lBankNumber;
    long  lBankAccount;

    static int siRecordCount;
    static int siDataHasBeenReadFromFile;
    static alBankNumber[NUM_BANK_NUMBERS_TO_CHECK_1];
    static alBankAccount[NUM_BANK_NUMBERS_TO_CHECK_1];

    //logg(__LINE__, "ChAuth Start Validation","",0,0);

    // get the data from file to memory once
    if(!( siDataHasBeenReadFromFile))  // file has not been read yet
    {
       //logg(__LINE__, "ChAuth Start Validation","Read File",0,0);

      iReturn = 0;
      if((fin = fopen("g:\\programs\\crdtcard.dat", "r")) == NULL)  // couldn't open file

      {
         iReturn = FILE_OPEN_ERROR;
         //logg(__LINE__, "ChAuth File open fail on G Drive","",0,0);
      }
      if(iReturn)
      {
         iReturn = 0;
         //logg(__LINE__, "ChAuth File open 2","",0,0);
         if((fin = fopen("c:\\programs\\crdtcard.dat", "r")) == NULL)  // couldn't open file
         {
            iReturn = FILE_OPEN_ERROR;
            //logg(__LINE__, "ChAuth File open fail on D drive","",0,0);
         }
      }
      if(iReturn)
      {
         iReturn = 0;
         //logg(__LINE__, "ChAuth File open 3","",0,0);
         if((fin = fopen("crdtcard.dat", "r")) == NULL)  // couldn't open file
         {
            iReturn = FILE_OPEN_ERROR;
            //logg(__LINE__, "ChAuth File open fail in . directory","",0,0);
         }
      }

      if(iReturn)
         return iReturn;

      for(ii = 0; ii < NUM_BANK_NUMBERS_TO_CHECK_1; ii++)
      {
         memset(szbuf,'\0',sizeof(szbuf));
         fgets(szbuf, LEN_ACCT_BUF, fin);
         //logg(__LINE__, "ChAuth Buf is",szbuf,0,0);

         if(feof(fin))
            break;

         sscanf(szbuf,"%ld %ld",alBankNumber[ii], alBankAccount[ii]);

         siRecordCount++;
       }

       fclose(fin);
       siDataHasBeenReadFromFile = 1;
    }

    lBankNumber = atol(pszBankNumber);
    lBankAccount = atol(pszBankAccount);

    //logg(__LINE__, "ChAuth Validation","Past Read File",lBankNumber,0);
    // find data for Bank Route and Account
    for(iCount = 0; iCount < siRecordCount; iCount++)
    {
       //logg(__LINE__, "ChAuth Test in ","",lBankNumber,iCount);
       //logg(__LINE__, "ChAuth Test dat","",dat[iCount].lBankNumber,siRecordCount);
       if( (alBankNumber[iCount]  == lBankNumber) &&
           (alBankAccount[iCount] == lBankAccount)
         )
          return 1;
    }
    return 0;
}

//$$**********************************************************************
bool CheckAuth::isValidTableCreditCardMicr(int fn)
{
 char  szBankNumber[32];
 char  szBankAccount[32];
 int   iReturn;
 int   iLenBankNumber;
 int   iLenBankAccount;

 // First, get the Bank Number and the Bank Account
 EditLen=sizeof(szBankNumber);

 FN(BANKNUMBER);
 iLenBankNumber = GetFldLen(Fn);
 iReturn = GetFld(Fn,szBankNumber,EditLen);
 szBankNumber[iLenBankNumber - 1] = '\0';
 //logg(__LINE__, "ChAuth ValidRouteAcct()-BankNum is",szBankNumber,FN_BANKNUMBER,iLenBankNumber);

 EditLen=sizeof(szBankAccount);
 FN_(BANKACCOUNT);
 Fn = Fn_;
 iLenBankAccount = GetFldLen(Fn);
 iReturn = GetFld(Fn,szBankAccount,EditLen);
 szBankAccount[iLenBankAccount - 1] = '\0';
 //logg(__LINE__, "ChAuth ValidRouteAcct()-BankAccount is",szBankAccount,FN_BANKACCOUNT,iLenBankAccount);

// iReturn = isValidTableCrditCardMicr(szBankNumber, szBankAccount);
 if(iReturn == LENGTH_ERROR)
    return false;

 return true;
}

#endif



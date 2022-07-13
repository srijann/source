#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "authfn.h"
#include "recfiltr.h"
#include "rcodes.h"

#define TEN_MINUTES (10 * 60)

static int _atoi(char *String,int Len);
static BYTE _calcage(char* mmddyy);
static void AuditToActivity(AUDIT*,ACTIVITY*,TINQ*);
static void CheckToBadCheck(CHECK*,BADCHECK*,TINQ*);

//-------------------------------
// Audit Filter Class Constructor
//-------------------------------
AuditFilter::AuditFilter() :
 numRecs(0)
{
}

//-----------------------
// Insert an Audit record
//-----------------------
void AuditFilter::Insert(AUDIT* Audit,TINQ* Inq)
{
 ACTIVITY Act;
 int i;
 int oldest;

 // Convert it to an Activity record
 AuditToActivity(Audit,&Act,Inq);

 // If it's a Voice Transaction, see if it replaces a previous Electronic
 // Decline Transaction
 if ( memcmp(Act.Output.BVStatus,"  ",2) )
  {
   for (i=0; i<numRecs; ++i)
    if ( Act.Age == Recs[i].Age &&
         memcmp(Act.MerchID,Recs[i].MerchID,sizeof(Act.MerchID)) == 0 &&
         Act.Amount == Recs[i].Amount &&
         memcmp(Recs[i].Output.BVStatus,"  ",2) == 0 &&
         ! ISAPPROVALRCODE(Recs[i].Rcode) &&
         Act.Time >= Recs[i].Time &&
         Act.Time - Recs[i].Time <= TEN_MINUTES )
      {
       memcpy(&Recs[i],&Act,sizeof(ACTIVITY));
       return;
      }
  }
 // If it's an electronic Decline Transaction, see if it's superseded by a
 // previous Voice transaction
 else
 if ( ! ISAPPROVALRCODE(Act.Rcode) )
  {
   for (i=0; i<numRecs; ++i)
    if ( Act.Age == Recs[i].Age &&
         memcmp(Act.MerchID,Recs[i].MerchID,sizeof(Act.MerchID)) == 0 &&
         Act.Amount == Recs[i].Amount &&
         memcmp(Recs[i].Output.BVStatus,"  ",2) &&
         Act.Time <= Recs[i].Time &&
         Recs[i].Time - Act.Time <= TEN_MINUTES )
      {
       memcpy(&Recs[i],&Act,sizeof(ACTIVITY));
       return;
      }
  }

 // Dup Detect and find oldest
 for (oldest=i=0; i<numRecs; ++i)
  {
   if ( memcmp(&Recs[i],&Act,sizeof(ACTIVITY)) == 0 )
     return;
   if ( Recs[i].Age > Recs[oldest].Age ||
        Recs[i].Time < Recs[oldest].Time )
     oldest=i;
  } 

 // Insert at the end if there's room
 if ( numRecs < MAXACTIVITYRECORDS )
  {
   memcpy(&Recs[numRecs],&Act,sizeof(ACTIVITY));
   ++numRecs;
  }
 // If full, replace the oldest record with the new record
 else
   if ( Act.Age < Recs[oldest].Age ||
        Act.Time > Recs[oldest].Time )
     memcpy(&Recs[oldest],&Act,sizeof(ACTIVITY));
}

//------------------------------------------
// Put the Activiy records to an IPC Message
//------------------------------------------
void AuditFilter::PutMsg(IPCMessage& Msg)
{
 if ( numRecs )
  Msg.PutFld(FN_ACTIVITY,Recs,numRecs * sizeof(ACTIVITY));
}

//-------------------------------
// Check Filter Class Constructor
//-------------------------------
CheckFilter::CheckFilter() 
{
 Reset();
}

void CheckFilter::Reset()
{
 memset(MothersName,0,sizeof(MothersName));
 numRecs=0;
}

//-----------------------
// Insert a Check record
//-----------------------
void CheckFilter::Insert(CHECK* Check,TINQ* Inq)
{
 BADCHECK badCheck;
 int oldest;
 int i;

 CheckToBadCheck(Check,&badCheck,Inq);

 // Save the 1st Mother's Maiden Name we encounter
 if ( Check->AdditionalInfo[0] != ' ' &&
      MothersName[0] == 0 )
  {
   memcpy(MothersName,Check->AdditionalInfo,sizeof(Check->AdditionalInfo));
   for (i=sizeof(Check->AdditionalInfo)-1; i>=0 &&
           MothersName[i] == ' '; --i)
     MothersName[i] = 0;
  }

 // Look for a duplicate record. Remember oldest date record so we can
 // overlay if no more room
 for (oldest=i=0; i<numRecs; ++i)
  {
   if ( memcmp(&Recs[i],&badCheck,sizeof(BADCHECK)) == 0 )
    return;
   if ( memcmp(Recs[i].CheckDate,Recs[oldest].CheckDate,
                sizeof(Recs[i].CheckDate)) < 0 )
    oldest=i;
  }

 // Insert or Overlay the record
 if ( numRecs < MAXCHECKRECORDS )
  {
   memcpy(&Recs[numRecs],&badCheck,sizeof(BADCHECK));
   ++numRecs;
   return;
  }
 else
  if ( memcmp(badCheck.CheckDate,Recs[oldest].CheckDate,
                sizeof(badCheck.CheckDate)) > 0 )
     memcpy(&Recs[oldest],&badCheck,sizeof(BADCHECK));
}

//------------------------------------------
// Put the Check records to an IPC Message
//------------------------------------------
void CheckFilter::PutMsg(IPCMessage& Msg)
{
 if ( numRecs )
  {
   Msg.PutFld(FN_BADCHECKS,Recs,numRecs * sizeof(BADCHECK));
   if ( MothersName[0] != 0 )
    Msg.PutFld(FN_MOTHERSNAME,MothersName);
  }

}

//-------------------------------------------------------------
// Do Ascii to integer conversion on non null-terminated string
//-------------------------------------------------------------
static int _atoi(char *String,int Len)
{
 char Buf[20];
 memcpy(Buf,String,Len);
 Buf[Len]=0;
 return atoi(Buf);
}

static char montbl[13] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

//-------------------------------------
// Calculate Age of transaction in days
//-------------------------------------
static BYTE _calcage(char* mmddyy)
{
 time_t now=time(NULL);
 struct tm* t=localtime(&now);

 BYTE mon=(BYTE)_atoi(&mmddyy[0],2);
 BYTE day=(BYTE)_atoi(&mmddyy[2],2);
 BYTE yr=(BYTE)_atoi(&mmddyy[4],2);
 WORD DaysLastYear;
 int i;

 // calc the Julian date
 montbl[2] = (yr%4) ? 28 : 29;

 for (i=1; i<mon; ++i)
  day += montbl[i];
 if ( ++t->tm_yday >= day )
  return (BYTE) t->tm_yday - day;

 DaysLastYear = ((yr-1) % 4) ? 365 : 366;

 return (BYTE) (t->tm_yday + (DaysLastYear - day+1));
}

static bool samePhone(char* Phone1,char* Phone2)
{
 char P1[10];
 char P2[10];

 memset(P1,' ',sizeof(P1));
 memset(P2,' ',sizeof(P2));
 register i, j;
 for (i=j=0; i<12 && j < 10; ++i)
  if ( isdigit(Phone1[i]) )
   P1[j++]=Phone1[i];
 for (i=j=0; i<12 && j < 10; ++i)
  if ( isdigit(Phone2[i]) )
   P2[j++]=Phone2[i];

 return (memcmp(P1,P2,10)==0);
}


//--------------------------------------------------
// Format a KW AUDIT record to a SPS Activity Record
//--------------------------------------------------
static void AuditToActivity(AUDIT* AuditRec,ACTIVITY* ActivityRec,TINQ* Inq)
{
 char *decPt;
 char sAmount[sizeof(AuditRec->Amount)+1];

 // Version BYTE was added 1/98. Current version is 1
 ActivityRec->Version=1;

 memcpy(ActivityRec->MerchID,AuditRec->TID,sizeof(ActivityRec->MerchID));
 memcpy(ActivityRec->Date,AuditRec->Date,sizeof(ActivityRec->Date));

 // Convert TIME to seconds since midnight
 ActivityRec->Time = _atoi(&AuditRec->Time[0],2) * 3600 +
                     _atoi(&AuditRec->Time[3],2) * 60 +
                     _atoi(&AuditRec->Time[6],2);

 ActivityRec->CheckNumber = _atoi(AuditRec->CheckNumber,
                                    sizeof(AuditRec->CheckNumber));
 memcpy(ActivityRec->Sic,AuditRec->Sic,sizeof(ActivityRec->Sic));
 ActivityRec->Rcode = (BYTE) _atoi(AuditRec->OutputCode,2);
 if ( AuditRec->RetryOutputCode[0] == ' ' )
  ActivityRec->PriorRcode=0;
 else
  ActivityRec->PriorRcode = (BYTE) _atoi(AuditRec->RetryOutputCode,2);
 memcpy(&ActivityRec->Output,AuditRec->TotalOutput,
          sizeof(ActivityRec->Output));


 // Convert amount to whole dollars rounded.
 //
 // See if it has a decimal point. If it has no decimal point, assume
 // that it's whole dollars. If it has a decimal point, round it up
 // to the nearest dollar.
 memset(sAmount,0,sizeof(sAmount));
 memcpy(sAmount,AuditRec->Amount,sizeof(AuditRec->Amount));
 if ( (decPt=strchr(sAmount,'.')) == NULL )
   ActivityRec->Amount = atoi(sAmount);
 else
  {
   ActivityRec->Amount=atoi(sAmount) * 100 + atoi(&decPt[1]);
   // Round to nearest dollor
   ActivityRec->Amount += 50;
   ActivityRec->Amount /= 100;
  }

 ActivityRec->Age = _calcage(ActivityRec->Date);

 // 1/98 The Activity records now have an IDs field the same as
 // checks.
 int numIdHits=0;
 memset(ActivityRec->Ids,' ',sizeof(ActivityRec->Ids));
 if ( Inq->StateCode[0] != ' ' &&
      memcmp(Inq->StateCode,AuditRec->StateCode,2) == 0 &&
      memcmp(Inq->License,AuditRec->DriversLicense,15) == 0 )
  ActivityRec->Ids[numIdHits++]='L';

 if ( Inq->BankNumber[0] != ' ' &&
      memcmp(Inq->BankNumber,AuditRec->BankNumber,9) == 0 &&
      memcmp(Inq->BankAccount,AuditRec->BankAccount,16) == 0 )
  ActivityRec->Ids[numIdHits++]='M';

 if ( Inq->PhoneNumber[0] != ' ' &&
      samePhone(Inq->PhoneNumber,AuditRec->PhoneNumber) )
     ActivityRec->Ids[numIdHits++]='P';

 if ( Inq->SSN[0] != ' ' &&
      memcmp(Inq->SSN,AuditRec->SSN,9) == 0 )
  ActivityRec->Ids[numIdHits]='S';
}

//---------------------------------------------------
// Format a KW Check record to a SPS Bad Check record
//---------------------------------------------------
#define COPYCHK(name) memcpy(BadCheckRec->name,CheckRec->name,\
                         sizeof(BadCheckRec->name))

static void CheckToBadCheck(CHECK* CheckRec,BADCHECK* BadCheckRec,TINQ* Inq)
{
 int numIdHits=0;
 COPYCHK(MerchId);
 COPYCHK(CheckNumber);
 // We put our record into format yymmdd so that we can compare and
 // sort on it easier
 memcpy(BadCheckRec->CheckDate,&CheckRec->CheckDate[4],2);
 memcpy(&BadCheckRec->CheckDate[2],CheckRec->CheckDate,4);
 COPYCHK(TypeReturnedCheck);
 COPYCHK(Amount);
 memset(BadCheckRec->Ids,' ',sizeof(BadCheckRec->Ids));
 if ( Inq->StateCode[0] != ' ' &&
      memcmp(Inq->StateCode,CheckRec->StateCode,2) == 0 &&
      memcmp(Inq->License,CheckRec->DriversLicense,15) == 0 )
  BadCheckRec->Ids[numIdHits++]='L';
 if ( Inq->BankNumber[0] != ' ' &&
      memcmp(Inq->BankNumber,CheckRec->BankNumber,9) == 0 &&
      memcmp(Inq->BankAccount,CheckRec->Account,16) == 0 )
  BadCheckRec->Ids[numIdHits++]='M';
  
 if ( Inq->PhoneNumber[0] != ' ' &&
      ( samePhone(Inq->PhoneNumber,CheckRec->HomePhone) ||
        samePhone(Inq->PhoneNumber,CheckRec->WorkPhone)) )
    BadCheckRec->Ids[numIdHits++]='P';

 if ( Inq->SSN[0] != ' ' &&
      memcmp(Inq->SSN,CheckRec->SSN,9) == 0 )
  BadCheckRec->Ids[numIdHits]='S';
}



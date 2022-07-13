#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "kwrec.h"

static int _atoi(char *String,int Len);
static BYTE _calcage(char* mmddyy);

/*
 typedef struct {
  char Sentinel;               // A
  char InquiryNumber[9];
  char Reference[6];
  char TID[25];
  char Date[6];
  char Time[8];
  char Port[2];
  char BankNumber[10];
  char BankAccount[16];
  char CheckNumber[6];
  char StateCode[4];
  char DriversLicense[15];
  char SSN[9];
  char DOB[6];
  char Amount[10];
  char OutputCode[2];
  char TotalOutput[54];
  char TypeInquiry;
  char BillingGuarantee;
  char Misc1[25];
  char Misc2[25];
  char Misc3[25];
  char Misc4[25];
  char Misc5[25];
 } AUDIT;

// This is AUDIT formatted to SPS ACTIVITY
typedef struct {
  BYTE  Age;                 // How many days ago this transaction was
  DWORD AMOUNT;
 } ACTIVITY;

*/

void AuditToActivity(AUDIT* AuditRec,ACTIVITY* ActivityRec)
{
 char *decPt;
 char sAmount[sizeof(AuditRec->Amount)+1];

 memcpy(ActivityRec->MerchID,AuditRec->TID,sizeof(ActivityRec->MerchID));
 memcpy(&ActivityRec->Date[0],&AuditRec->Date[0],2);
 memcpy(&ActivityRec->Date[2],&AuditRec->Date[3],2);
 memcpy(&ActivityRec->Date[4],&AuditRec->Date[6],2);
 memcpy(&ActivityRec->Time[0],&AuditRec->Time[0],2);
 memcpy(&ActivityRec->Time[2],&AuditRec->Time[3],2);
 memcpy(&ActivityRec->Time[4],&AuditRec->Time[6],2);
 memcpy(ActivityRec->BankNumber,AuditRec->BankNumber,
         sizeof(ActivityRec->BankNumber));
 memcpy(ActivityRec->AccountNumber,AuditRec->BankAccount,
         sizeof(ActivityRec->AccountNumber));
 ActivityRec->CheckNumber = _atoi(AuditRec->CheckNumber,
                                    sizeof(AuditRec->CheckNumber));
 memcpy(ActivityRec->StateCode,AuditRec->StateCode,
           sizeof(ActivityRec->StateCode));
 memcpy(ActivityRec->DriversLicense,AuditRec->DriversLicense,
           sizeof(ActivityRec->DriversLicense));
 memcpy(ActivityRec->SSN,AuditRec->SSN,sizeof(ActivityRec->SSN));
 memcpy(ActivityRec->DOB,AuditRec->DOB,sizeof(ActivityRec->DOB));
 memset(ActivityRec->Sic,' ',sizeof(ActivityRec->Sic));
 ActivityRec->Rcode = (BYTE) _atoi(AuditRec->OutputCode,2);
 memcpy(ActivityRec->TotalOutput,AuditRec->TotalOutput,
          sizeof(ActivityRec->TotalOutput));


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
}

static int _atoi(char *String,int Len)
{
 char Buf[20];
 memcpy(Buf,String,Len);
 Buf[Len]=0;
 return atoi(Buf);
}

static char montbl[13] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

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

#include <stdio.h>
void main(int argc,char *argv[])
{
 printf("AGE=%d\n",_calcage(argv[1]));
}

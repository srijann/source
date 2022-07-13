
#define MAXAUDITRECS 10
#define MAXCHECKRECS 10
AUDIT Audit[MAXAUDITRECS];
ACTIVITY Activity[MAXAUDITRECS];
CHECK Check[MAXCHECKRECS];
MERCHANT Merch;

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


void AuditToActivity(AUDIT* AuditRec,ACTIVITY* ActivityRec)
{
 char *decPt;
 char sAmount[sizeof(AuditRec->Amount)+1];

 memcpy(ActivityRec->MerchID,AuditRec->TID,sizeof(ActivityRec->MerchID));
 memcpy(ActivityRec->Date,AuditRec->Date,sizeof(ActivityRec->Date));
 //memcpy(&ActivityRec->Time[0],&AuditRec->Time[0],2);
 //memcpy(&ActivityRec->Time[2],&AuditRec->Time[3],2);
 //memcpy(&ActivityRec->Time[4],&AuditRec->Time[6],2);
 //memcpy(ActivityRec->BankNumber,AuditRec->BankNumber,
 //        sizeof(ActivityRec->BankNumber));
 //memcpy(ActivityRec->AccountNumber,AuditRec->BankAccount,
 //        sizeof(ActivityRec->AccountNumber));
 ActivityRec->CheckNumber = _atoi(AuditRec->CheckNumber,
                                    sizeof(AuditRec->CheckNumber));
 //memcpy(ActivityRec->StateCode,AuditRec->StateCode,
 //          sizeof(ActivityRec->StateCode));
 //memcpy(ActivityRec->DriversLicense,AuditRec->DriversLicense,
 //          sizeof(ActivityRec->DriversLicense));
 //memcpy(ActivityRec->SSN,AuditRec->SSN,sizeof(ActivityRec->SSN));
 //memcpy(ActivityRec->DOB,AuditRec->DOB,sizeof(ActivityRec->DOB));
 memcpy(ActivityRec->Sic,AuditRec->Sic,sizeof(ActivityRec->Sic));
 ActivityRec->Rcode = (BYTE) _atoi(AuditRec->OutputCode,2);
 if ( AuditRec->RetryOutputCode[0] == ' ' )
  ActivityRec->PriorRcode=0;
 else
  ActivityRec->PriorRcode = (BYTE) _atoi(AuditRec->RetryOutputCode,2);
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

//-------------------------
// Format KW to SPS message
//-------------------------
static int FormatKWtoSPS(void)
{
 int NumAuditRecs=0;
 int NumCheckRecs=0;
 int NumMerchRecs=0;
 char *Ptr;
 int i;

 if ( (Ptr=strtok(IOBuf,"\n")) == NULL )
   return false;

 while (Ptr!=NULL)
  {
   switch(*Ptr)
    {
     case 'A': if ( strlen(Ptr) != sizeof(AUDIT) )
                 return false;
                if ( NumAuditRecs <  MAXAUDITRECS )
                 {
                  memcpy(&Audit[NumAuditRecs],Ptr,sizeof(AUDIT));
                  ++NumAuditRecs;
                 }
                break;

     case 'M':  ++NumMerchRecs;
                if ( strlen(Ptr) != sizeof(MERCHANT) )
                  return false;
                memcpy(&Merch,Ptr,sizeof(MERCHANT));
                break;

     case 'I':  break;

     case 'C':  if ( strlen(Ptr) != sizeof(CHECK) )
                  return false;
                if ( NumCheckRecs < MAXCHECKRECS )
                 {
                  memcpy(&Check[NumCheckRecs],Ptr,sizeof(CHECK));
                  ++NumCheckRecs;
                 }
                break;

     default:  return false;
    }
   Ptr=strtok(NULL,"\n");
  }

 if ( NumMerchRecs==0 )
   return false;

 Msg.PutFld(FN_KWMERCHANTRECORD,&Merch,sizeof(Merch));

 // Put the Audit records
 if ( NumAuditRecs )
  {
   for (i=0;i<NumAuditRecs;++i)
    AuditToActivity(&Audit[i],&Activity[i]);
   Msg.PutFld(FN_ACTIVITY,&Activity,NumAuditRecs * sizeof(ACTIVITY));
  } 

 // Put the Check records
 if ( NumCheckRecs )
  Msg.PutFld(FN_BADCHECKS,&Check,NumCheckRecs * sizeof(CHECK));

 return true;
}
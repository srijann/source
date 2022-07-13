//----------------------------------------------------------------------------
//  Project Kwsimv
//
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    Kwsimv Application
//  FILE:         kwsimvdlgclient.cpp
//  AUTHOR:
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of ResponseDialog (TDialog).
//
//----------------------------------------------------------------------------

#include "win4me.h"

#include <stdio.h>
#include <time.h>
#include "ResponseDialog.h"
#include "checkauthfn.h"


//{{ResponseDialog Implementation}}

#define ACTROW(n)\
 ActRows[n-1].DATE=new TEdit(this,IDC_ACT##n##DATE,7);\
 ActRows[n-1].MERCH=new TEdit(this,IDC_ACT##n##MERCH,6);\
 ActRows[n-1].AMT=new TEdit(this,IDC_ACT##n##AMT,9);\
 ActRows[n-1].CKNUM=new TEdit(this,IDC_ACT##n##CKNUM,7);\
 ActRows[n-1].RC=new TEdit(this,IDC_ACT##n##RC,3);\
 ActRows[n-1].BV=new TEdit(this,IDC_ACT##n##BV,3);\
 ActRows[n-1].BVDT=new TEdit(this,IDC_ACT##n##BVDT,7);\
 ActRows[n-1].BVBAL=new TEdit(this,IDC_ACT##n##BVBAL,9)
#define CHECKSROW(n)\
 CheckRows[n-1].DATE=new TEdit(this,IDC_CK##n##DATE,7);\
 CheckRows[n-1].MERCH=new TEdit(this,IDC_CK##n##MERCH,6);\
 CheckRows[n-1].AMT=new TEdit(this,IDC_CK##n##AMT,7);\
 CheckRows[n-1].CKNUM=new TEdit(this,IDC_CK##n##CKNUM,5);\
 CheckRows[n-1].TYPE=new TEdit(this,IDC_CK##n##TYPE,3)

//--------------------------------------------------------
// ResponseDialog
// ~~~~~~~~~~
// Construction/Destruction handling.
//
ResponseDialog::ResponseDialog(TWindow* parent, TResId resId, TModule* module)
:
  TDialog(parent, resId, module)
{
 ACTROW(1);
 ACTROW(2);
 ACTROW(3);
 ACTROW(4);
 ACTROW(5);
 ACTROW(6);
 ACTROW(7);
 ACTROW(8);
 ACTROW(9);
 ACTROW(10);
 CHECKSROW(1);
 CHECKSROW(2);
 CHECKSROW(3);
 CHECKSROW(4);
 CHECKSROW(5);

 KWRCODE=new TEdit(this,IDC_KWRCODE,3);
 KWTEXT=new TEdit(this,IDC_KWTEXT,53);
}


ResponseDialog::~ResponseDialog()
{
  Destroy();

  // INSERT>> Your destructor code here.
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
void ResponseDialog::ActivityToMsg(IPCMessage& Msg)
{
 int i;
 char Buf[50];

 for (i=0; i<MAXACTIVITY; ++i)
  {
   ActRows[i].DATE->Transfer(Buf,tdGetData);
   if ( ! Buf[0] )
    break;
   memcpy(Act[i].Date,Buf,sizeof(Act[i].Date));
   Act[i].Age = _calcage(Act[i].Date);
   ActRows[i].MERCH->Transfer(Buf,tdGetData);
   memcpy(Act[i].MerchID,Buf,sizeof(Act[i].MerchID));
   ActRows[i].AMT->Transfer(Buf,tdGetData);
   Act[i].Amount=atoi(Buf);
   ActRows[i].CKNUM->Transfer(Buf,tdGetData);
   Act[i].CheckNumber=atoi(Buf);
   ActRows[i].RC->Transfer(Buf,tdGetData);
   Act[i].Rcode=(BYTE)atoi(Buf);

   memset(&Act[i].Output,' ',sizeof(VOICEOUTPUT));
   ActRows[i].BV->Transfer(Buf,tdGetData);
   if ( Buf[0] )
     memcpy(Act[i].Output.BVStatus,Buf,sizeof(Act[i].Output.BVStatus));
   ActRows[i].BVDT->Transfer(Buf,tdGetData);
   if ( Buf[0] )
     memcpy(Act[i].Output.BVAccountDate,Buf,sizeof(Act[i].Output.BVAccountDate));
   ActRows[i].BVBAL->Transfer(Buf,tdGetData);
   if ( Buf[0] )
     memcpy(Act[i].Output.BVAverageBalance,Buf,sizeof(Act[i].Output.BVAverageBalance));
  }

 if ( i )
  Msg.PutFld(FN_ACTIVITY,&Act,sizeof(ACTIVITY)*i);
}

void ResponseDialog::ChecksToMsg(IPCMessage& Msg)
{
 int i;
 char Buf[50];

 for (i=0; i<MAXCHECKS; ++i)
  {
   CheckRows[i].DATE->Transfer(Buf,tdGetData);
   if ( ! Buf[0] )
    break;
   memcpy(Check[i].DateReceived,Buf,sizeof(Check[i].DateReceived));
   CheckRows[i].MERCH->Transfer(Buf,tdGetData);
   memcpy(Check[i].MerchId,Buf,sizeof(Check[i].MerchId));
   CheckRows[i].AMT->Transfer(Buf,tdGetData);
   memcpy(Check[i].Amount,Buf,sizeof(Check[i].Amount));
   CheckRows[i].CKNUM->Transfer(Buf,tdGetData);
   memcpy(Check[i].CheckNumber,Buf,sizeof(Check[i].CheckNumber));
   CheckRows[i].TYPE->Transfer(Buf,tdGetData);
   memcpy(Check[i].TypeReturnedCheck,Buf,sizeof(Check[i].TypeReturnedCheck));
  }
 if ( i )
  Msg.PutFld(FN_BADCHECKS,&Check,sizeof(CHECK)*i);
}

void ResponseDialog::ResponseToMsg(IPCMessage& Msg)
{
 char Buf[100];

 KWRCODE->Transfer(Buf,tdGetData);
 KWTEXT->Transfer(&Buf[2],tdGetData);
 Msg.PutFld(FN_TEXTRESPONSE,Buf);
}

void ResponseDialog::ProcessMessage(IPCMessage& Msg)
{
 ActivityToMsg(Msg);
 ChecksToMsg(Msg);
 ResponseToMsg(Msg);
}



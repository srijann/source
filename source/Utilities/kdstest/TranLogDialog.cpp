//----------------------------------------------------------------------------
//  Project TranLog
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    TranLog Application
//  FILE:         TranLogDialog.cpp
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of TranLogDialog (TDialog).
//
//----------------------------------------------------------------------------

#include "win4me.h"

#include "TranLogDialog.h"
#include "checkauthfn.h"
#include "trantype.h"
#include "kwrec.h"


//{{TranLogDialog Implementation}}


#define DEFDCCTL(name,len,fn)\
  name[0]=DCMan->Add(IDC_##name,len+1,fn,-1);\
 for (i=1; i<NUMROWS; ++i)\
  name[i] = new TEdit(this,IDC_##name+i,len+1)
#define DEFCTL(name,len)\
  for (i=0; i<NUMROWS; ++i)\
   name[i] = new TEdit(this,IDC_##name+i,len+1)


//--------------------------------------------------------
// TranLogDialog
// ~~~~~~~~~~
// Construction/Destruction handling.
//
TranLogDialog::TranLogDialog(TWindow* parent, TResId resId, TModule* module)
:
  TDialog(parent, resId, module)
{
 int i;

 DCMan = new DialogControlManager(this,20);
 DEFDCCTL(APP,4,FN_APPROVALNUM);
 DEFDCCTL(RC,2,FN_RCODE);
 DEFDCCTL(MERCH,5,FN_MERCHID);
 DEFDCCTL(AMOUNT,10,FN_AMOUNT);
 DEFDCCTL(RC,2,FN_RCODE);
 DEFDCCTL(PHONE,10,FN_PHONE);
 DEFDCCTL(SSN,9,FN_SSN);
 DEFDCCTL(ACCT,16,FN_BANKACCOUNT);
 DEFDCCTL(ABA,9,FN_BANKNUMBER);
 DEFDCCTL(LICENSE,15,FN_LICENSE);
 DEFDCCTL(STATE,2,FN_LICENSESTATE);
 DEFDCCTL(CHECKNUM,6,FN_CHECKNUMBER);
 DEFCTL(BVSTATUS,2);
 DEFCTL(BVDATE,6);
 DEFCTL(BAL,5);
 DEFCTL(OPERID,4);
}


TranLogDialog::~TranLogDialog()
{
  Destroy();
  delete DCMan;
}


void TranLogDialog::ProcessInqMessage(IPCMessage& Msg)
{
 scroll();
 DCMan->CopyAllFromMsg(&Msg);
}

static void transfer(TEdit* Ctl,char* Text,int Len)
{
 char Buf[50];

 memcpy(Buf,Text,Len);
 Buf[Len]=0;
 Ctl->Transfer(Buf,tdSetData);
}

void TranLogDialog::ProcessReplyMessage(IPCMessage& Msg)
{
 VOICEOUTPUT Vout;
 int Len=sizeof(Vout);

 DCMan->CopyFromMsg(FN_RCODE,&Msg);
 DCMan->CopyFromMsg(FN_APPROVALNUM,&Msg);
 if ( Msg.GetFld(FN_TEXTRESPONSE,&Vout,Len) )
  {
   transfer(BVSTATUS[0],Vout.BVStatus,sizeof(Vout.BVStatus));
   transfer(BVDATE[0],Vout.BVAccountDate,sizeof(Vout.BVAccountDate));
   transfer(BAL[0],Vout.BVAverageBalance,sizeof(Vout.BVAverageBalance));
   transfer(OPERID[0],Vout.OperatorId,sizeof(Vout.OperatorId));
  }
}

void TranLogDialog::copy(TEdit* dest,TEdit* source)
{
 char Buf[100];

 source->Transfer(Buf,tdGetData);
 dest->Transfer(Buf,tdSetData);
}

#define SCROLL(fld) for(i=NUMROWS-1; i>=1; --i) copy(fld[i],fld[i-1])

void TranLogDialog::scroll()
{
 int i;
 SCROLL(MERCH);
 SCROLL(BVSTATUS);
 SCROLL(BVDATE);
 SCROLL(BAL);
 SCROLL(APP);
 SCROLL(RC);
 SCROLL(PHONE);
 SCROLL(SSN);
 SCROLL(ACCT);
 SCROLL(ABA);
 SCROLL(LICENSE);
 SCROLL(STATE);
 SCROLL(CHECKNUM);
 SCROLL(AMOUNT);
 SCROLL(OPERID);
}





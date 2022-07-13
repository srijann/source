//----------------------------------------------------------------------------
//  Project Kwtest
//
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    Kwtest Application
//  FILE:         kwtstdlc.cpp
//  AUTHOR:
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of TKwtestDlgClient (TDialog).
//
//----------------------------------------------------------------------------

#include "win4me.h"
#include "kwtstapp.h"
#include "kwtstdlc.h"
#include "CheckAuthFn.h"
#include "Trantype.h"
#include "EditUns.h"
#include "DestName.h"

//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(TKwtestDlgClient, TDialog)
//{{TKwtestDlgClientRSP_TBL_BEGIN}}
  EV_BN_CLICKED(IDC_SENDBUTTON, BNClicked),
  EV_BN_CLICKED(IDC_ACTCLEAR, BNClickedActClear),
//{{TKwtestDlgClientRSP_TBL_END}}
END_RESPONSE_TABLE;


#define ED(name) name=new TEdit(this,IDC_##name,name##_LEN+1)
#define BT(name) name=new TButton(this,IDC_##name)
#define ACT(n)\
 ACTEDIT[n-1].MERCHID = new TEdit(this,IDC_ACT##n##MERCHID,6);\
 ACTEDIT[n-1].AGE = new TEditUns(this,IDC_ACT##n##AGE,3);\
 ACTEDIT[n-1].CHECKNUM = new TEditUns(this,IDC_ACT##n##CHECKNUM,7);\
 ACTEDIT[n-1].AMOUNT = new TEditUns(this,IDC_ACT##n##AMOUNT,8);\
 ACTEDIT[n-1].SIC = new TEdit(this,IDC_ACT##n##SIC,5);\
 ACTEDIT[n-1].RCODE = new TEditUns(this,IDC_ACT##n##RCODE,3)


//{{TKwtestDlgClient Implementation}}


//--------------------------------------------------------
// TKwtestDlgClient
// ~~~~~~~~~~
// Construction/Destruction handling.
//
#define AMOUNT_LEN 9
#define STATE_LEN 2
#define CHECKNUM_LEN 6
#define PHONE_LEN 12
#define MICR_LEN 25
#define MERCHNUM_LEN 5
#define RESPONSE_LEN 16
#define DL_LEN 25
#define DOB_LEN 6
#define RCODE_LEN 2
#define ZIPCODE_LEN 10
#define SSN_LEN 9
#define ABA_LEN 9
#define ACCOUNT_LEN 16

TKwtestDlgClient::TKwtestDlgClient(TWindow* parent, TResId resId, TModule* module)
:
  TDialog(parent, resId, module)
{
   ED(AMOUNT);
   ED(STATE);
   ED(CHECKNUM);
   ED(PHONE);
   ED(ABA);
   ED(ACCOUNT);
   ED(MERCHNUM);
   ED(RESPONSE);
   ED(DL);
   ED(DOB);
   ED(RCODE);
   BT(SENDBUTTON);
   ED(ZIPCODE);
   ED(SSN);

   ACT(1);
   ACT(2);
   ACT(3);
   ACT(4);
   ACT(5);
   ACT(6);
   ACT(7);
   ACT(8);
   ACT(9);

   CHECKSBOX=new TCheckBox(this,IDC_CHECKSBOX);
   BIG = new TCheckBox(this,IDC_BIG);
}


TKwtestDlgClient::~TKwtestDlgClient()
{
  Destroy();
}

void TKwtestDlgClient::BNClicked()
{
 int Count;
 int i;
 char Sic[5];

 Msg.ClearMsg();
 Msg.PutFld(FN_TRANTYPE,T_POSAUTH);
 Msg.PutFld(FN_SOURCETYPE,(BYTE)'P');
 WinToMsg(AMOUNT,FN_AMOUNT);
 WinToMsg(STATE,FN_LICENSESTATE);
 WinToMsg(CHECKNUM,FN_CHECKNUMBER);
 WinToMsg(PHONE,FN_PHONE);
 WinToMsg(ABA,FN_BANKNUMBER);
 WinToMsg(ACCOUNT,FN_BANKACCOUNT);
 WinToMsg(MERCHNUM,FN_MERCHID);
 WinToMsg(DL,FN_LICENSE );
 WinToMsg(DOB,FN_DOB);
 WinToMsg(SSN,FN_SSN);
 WinToMsg(ZIPCODE,FN_ZIPCODE);

 if ( CHECKSBOX->GetCheck() == BF_CHECKED )
   if ( BIG->GetCheck() == BF_CHECKED )
     Msg.PutFld(FN_BADCHECKS,&BadCheck,sizeof(BADCHECK)*20);
   else
     Msg.PutFld(FN_BADCHECKS,&BadCheck,sizeof(BADCHECK));

 ActToMsg();
 RESPONSE->Transfer("",tdSetData);
 RCODE->Transfer("",tdSetData);
 SetCapture();
 SetCursor(0,IDC_WAIT);

 if ( ! Msg.SendRecv(POSAUTH_DEST,5000,true) )
  {
   SetCursor(0,IDC_ARROW);
   ReleaseCapture();
   return;
  }

 MsgToWin(RESPONSE,FN_TEXTRESPONSE);
 MsgToWin(RCODE,FN_RCODE);
 SetCursor(0,IDC_ARROW);
 ReleaseCapture();
}

void TKwtestDlgClient::SetupWindow()
{
 TDialog::SetupWindow();
}


#define EN(fld) fld->SetReadOnly(Enable ? false : true)
void TKwtestDlgClient::EnableEdit(bool Enable)
{
 EN(AMOUNT);
 EN(STATE);
 EN(CHECKNUM);
 EN(PHONE);
 EN(ABA);
 EN(ACCOUNT);
 EN(MERCHNUM);
 EN(DL);
 EN(DOB);
}

#define CL(fld) fld->Transfer("",tdSetData)
void TKwtestDlgClient::ClearEdit(void)
{
 CL(AMOUNT);
 CL(STATE);
 CL(CHECKNUM);
 CL(PHONE);
 CL(ABA);
 CL(ACCOUNT);
 CL(MERCHNUM);
 CL(DL);
 CL(DOB);

}

void TKwtestDlgClient::MsgToWin(TEdit* Fld,FLDNUM Fn)
{
 Msg.GetFld(Fn,sizeof(TransBuf),TransBuf);
 Fld->Transfer(TransBuf,tdSetData);
}

void TKwtestDlgClient::WinToMsg(TEdit* Fld,FLDNUM Fn)
{
 Fld->Transfer(TransBuf,tdGetData);
 if ( TransBuf[0] )
   Msg.PutFld(Fn,TransBuf);
}

void TKwtestDlgClient::ActToMsg()
{
 int i;
 memset(&Act,0,sizeof(Act));

 for (i=0; i<MAXACT; ++i)
  {
   ACTEDIT[i].MERCHID->Transfer(TransBuf,tdGetData);
   if ( ! TransBuf[0] )
     break;
   memcpy(Act[i].MerchID,TransBuf,sizeof(Act[i].MerchID));
   ACTEDIT[i].AGE->get(Act[i].Age);
   ACTEDIT[i].AMOUNT->get(Act[i].Amount);
   ACTEDIT[i].CHECKNUM->get(Act[i].CheckNumber);
   ACTEDIT[i].SIC->Transfer(Act[i].Sic,tdGetData);
   ACTEDIT[i].RCODE->get(Act[i].Rcode);
  }

 if (i==0 )
  return;

 if ( BIG->GetCheck() == BF_CHECKED )
  {
   for (i=9; i<20; ++i)
    memcpy(&Act[i],&Act[0],sizeof(ACTIVITY));
   i=20;
  }

 Msg.PutFld(FN_ACTIVITY,&Act,sizeof(ACTIVITY)*i);
}

void TKwtestDlgClient::BNClickedActClear()
{
 int i;

 for (i=0; i<MAXACT; ++i)
  {
   ACTEDIT[i].AGE->Transfer("",tdSetData);
   ACTEDIT[i].MERCHID->Transfer("",tdSetData);
   ACTEDIT[i].AMOUNT->Transfer("",tdSetData);
   ACTEDIT[i].CHECKNUM->Transfer("",tdSetData);
   ACTEDIT[i].SIC->Transfer("",tdSetData);
   ACTEDIT[i].RCODE->Transfer("",tdSetData);
  }
}


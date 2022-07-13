//----------------------------------------------------------------------------
//  Project checkauth
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    checkauth.apx Application
//  FILE:         fetchreferral.cpp
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of TFetchReferral (TDialog).
//
//----------------------------------------------------------------------------
#include "win4me.h"
#include "chkauth.h"
#include "fetchreferral.h"
#include "CheckauthFn.h"
#include "CheckAuth.h"
#include "DestName.h"
#include "TranType.h"
#include "AppNotify.h"

//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(TFetchReferral, TDialog)
//{{TFetchReferralRSP_TBL_BEGIN}}
  EV_BN_CLICKED(IDOK, BNClicked),
  EV_BN_CLICKED(IDCANCEL, BNCancelClicked),
//{{TFetchReferralRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{TFetchReferral Implementation}}
#define REFERRALNUM_LEN 2
#define WAIT_TIME 5000

TFetchReferral::TFetchReferral(TWindow* parent, CheckAuth& MsgPt)
:
 TDialog(parent, IDD_FETCHREFERRAL, 0),
 MSG(MsgPt)
{
  // INSERT>> Your constructor code here.
 REFERRALNUM = new TEdit(this,IDC_REFERRALNUM,REFERRALNUM_LEN+1);
 OKAYBUTTON = new TButton(this,IDOK);
}


TFetchReferral::~TFetchReferral()
{
 Destroy();
}

void TFetchReferral::BNCancelClicked()
{
 AppNotifyMsg(Parent,NOTIFY_DIALOGCOMPLETE,0);
 CmOk();
}

void TFetchReferral::BNClicked()
{
 char ReferralNum[REFERRALNUM_LEN+1];
 HWND captureWin;

 REFERRALNUM->Transfer(ReferralNum,tdGetData);

 if ( strlen(ReferralNum) )
  {
   MSG.PutFld(FN_REFERRALNUM,ReferralNum);
   MSG.PutFld(FN_TRANTYPE,T_GETREFERRAL);

   captureWin=GetApplication()->GetMainWindow()->SetCapture();
   GetApplication()->GetMainWindow()->SetCursor(0,IDC_WAIT);
   MSG.SendRecv(REFERRALQUEUE_DEST,WAIT_TIME,true);
   GetApplication()->GetMainWindow()->SetCursor(0,IDC_ARROW);
   ::SetCapture(captureWin);
   MSG.initClassMembers();
  }

 AppNotifyMsg(Parent,NOTIFY_DIALOGCOMPLETE,0);
 CmOk();
}



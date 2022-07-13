//----------------------------------------------------------------------------
//  Project VoiceAuth
//
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    VoiceAuth.apx Application
//  FILE:         verifyreferraldialog.cpp
//  AUTHOR:
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of VerifyReferralDialog (TDialog).
//
//----------------------------------------------------------------------------
#include "win4me.h"
#include "verifyreferraldialog.h"
#include "CheckAuth.rh"
#include "Chkauth.h"
#include "AppNotify.h"

//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(VerifyReferralDialog, TDialog)
//{{VerifyReferralDialogRSP_TBL_BEGIN}}
  EV_BN_CLICKED(IDOK, BNOkClicked),
  EV_BN_CLICKED(IDCANCEL, BNCancelClicked),
//{{VerifyReferralDialogRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{VerifyReferralDialog Implementation}}


VerifyReferralDialog::VerifyReferralDialog(TWindow* parent, CheckAuth& Msg)
:
  TDialog(parent, IDD_VERIFYREFERRAL),
  MSG(Msg)
{
 MerchId=new TEdit(this,IDC_VFYMERCHID);
 MerchName=new TEdit(this,IDC_VFYMERCHNAME);
 Amount=new TEdit(this,IDC_VFYAMOUNT);
}


VerifyReferralDialog::~VerifyReferralDialog()
{
  Destroy(IDCANCEL);
}


void VerifyReferralDialog::SetupWindow()
{
 char amount[20];

 TDialog::SetupWindow();

 if ( MSG.hasMerchantRec )
  {
   MerchId->Transfer(MSG.merchantRec.Data.MerchantID,tdSetData);
   MerchName->Transfer(MSG.merchantRec.Data.MerchantName,tdSetData);
  }

 if ( MSG.GetFld(FN_HOLDCHECKAMOUNT4,sizeof(amount),amount) ||
      MSG.GetFld(FN_HOLDCHECKAMOUNT3,sizeof(amount),amount) ||
      MSG.GetFld(FN_HOLDCHECKAMOUNT2,sizeof(amount),amount) ||
      MSG.GetFld(FN_AMOUNT,sizeof(amount),amount) )
  Amount->Transfer(amount,tdSetData);
}

void VerifyReferralDialog::BNOkClicked()
{
 AppNotifyMsg(Parent,NOTIFY_DIALOGCOMPLETE,0);
 CmOk();
}


void VerifyReferralDialog::BNCancelClicked()
{
 MSG.ClearMsg();
 AppNotifyMsg(Parent,NOTIFY_DIALOGCOMPLETE,0);
 CmOk();
}


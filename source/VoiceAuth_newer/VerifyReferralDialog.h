//----------------------------------------------------------------------------
//  Project VoiceAuth
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    VoiceAuth.apx Application
//  FILE:         verifyreferraldialog.h
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for VerifyReferralDialog (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(verifyreferraldialog_h)              // Sentry, use file only if it's not already included.
#define verifyreferraldialog_h

#include <owl/dialog.h>
#include <owl/edit.h>

#include <owl/commctrl.h>

#include "Dialogs.rh"
#include "CheckAuth.h"


//{{TDialog = VerifyReferralDialog}}
class VerifyReferralDialog : public TDialog {
  TEdit* MerchId;
  TEdit* MerchName;
  TEdit* Amount;
  CheckAuth& MSG;
  public:
    VerifyReferralDialog(TWindow* parent, CheckAuth&);
    virtual ~VerifyReferralDialog();

//{{VerifyReferralDialogVIRTUAL_BEGIN}}
  public:
    virtual void SetupWindow();
//{{VerifyReferralDialogVIRTUAL_END}}

//{{VerifyReferralDialogRSP_TBL_BEGIN}}
  protected:
    void BNOkClicked();
    void BNCancelClicked();
//{{VerifyReferralDialogRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(VerifyReferralDialog);
};    //{{VerifyReferralDialog}}


#endif  // verifyreferraldialog_h sentry.


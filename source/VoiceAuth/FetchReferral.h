//----------------------------------------------------------------------------
//  Project checkauth
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    checkauth.apx Application
//  FILE:         fetchreferral.h
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for TFetchReferral (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(fetchreferral_h)              // Sentry, use file only if it's not already included.
#define fetchreferral_h
#include <owl/dialog.h>
#include <owl/button.h>
#include <owl/edit.h>
#include "Dialogs.rh"
#include "checkauth.h"

//{{TDialog = TFetchReferral}}
class TFetchReferral : public TDialog {
  TEdit* REFERRALNUM;
  TButton* OKAYBUTTON;
  CheckAuth& MSG;
  LRESULT ProcessReply(WPARAM MsgType, LPARAM Id);
  bool InProgress;
  public:
    TFetchReferral(TWindow* parent, CheckAuth&);
    virtual ~TFetchReferral();

//{{TFetchReferralVIRTUAL_BEGIN}}
  public:
//{{TFetchReferralVIRTUAL_END}}

//{{TFetchReferralRSP_TBL_BEGIN}}
  protected:
  void BNClicked();
  void BNCancelClicked();
//{{TFetchReferralRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(TFetchReferral);
};    //{{TFetchReferral}}


#endif  // fetchreferral_h sentry.


//----------------------------------------------------------------------------
//  Project VoiceAuth
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    VoiceAuth.apx Application
//  FILE:         approvaldialog.h
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for ApprovalDialog (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(approvaldialog_h)              // Sentry, use file only if it's not already included.
#define approvaldialog_h

#include <owl/dialog.h>
#include <owl/edit.h>

#include <owl/commctrl.h>
#include "Dialogs.rh"
#include "checkauth.h"


//{{TDialog = ApprovalDialog}}
class ApprovalDialog : public TDialog {
  TEdit* APPNUM;
  CheckAuth& authMsg;
  public:
    ApprovalDialog(TWindow* parent, CheckAuth& Msg);
    virtual ~ApprovalDialog();

//{{ApprovalDialogVIRTUAL_BEGIN}}
  public:
    virtual void SetupWindow();
    virtual TResult EvCommand(uint id, THandle hWndCtl, uint notifyCode);
//{{ApprovalDialogVIRTUAL_END}}

//{{ApprovalDialogRSP_TBL_BEGIN}}
  protected:
    void BNClicked();
//{{ApprovalDialogRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(ApprovalDialog);
};    //{{ApprovalDialog}}


#endif  // approvaldialog_h sentry.


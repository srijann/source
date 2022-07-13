//----------------------------------------------------------------------------
//  Project VoiceAuth
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    VoiceAuth.apx Application
//  FILE:         holdcheckapprovaldialog.h
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for HoldCheckApprovalDialog (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(holdcheckapprovaldialog_h)              // Sentry, use file only if it's not already included.
#define holdcheckapprovaldialog_h

#include <owl/dialog.h>
#include <owl/edit.h>

#include <owl/commctrl.h>

#include "Dialogs.rh"
#include "CheckAuth.h"
#include "DCManage.h"

//{{TDialog = HoldCheckApprovalDialog}}
class HoldCheckApprovalDialog : public TDialog {
  TEdit* APPNUM1;
  TEdit* APPNUM2;
  TEdit* APPNUM3;
  TEdit* APPNUM4;
  CheckAuth& authMsg;
  DialogControlManager* DCMan;
  char   sourceTypes[5];

  public:
    HoldCheckApprovalDialog(TWindow* parent, CheckAuth& Msg);
    virtual ~HoldCheckApprovalDialog();
    virtual void SetupWindow();
    virtual TResult EvCommand(uint id, THandle hWndCtl, uint notifyCode);
  protected:
    void BNClicked();
DECLARE_RESPONSE_TABLE(HoldCheckApprovalDialog);
};    //{{HoldCheckApprovalDialog}}


#endif  // holdcheckapprovaldialog_h sentry.


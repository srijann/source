//----------------------------------------------------------------------------
//  Project VoiceAuth
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    VoiceAuth.apx Application
//  FILE:         employerdialog.h
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for EmployerDialog (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(employerdialog_h)              // Sentry, use file only if it's not already included.
#define employerdialog_h

#include <owl/dialog.h>


#include <owl/commctrl.h>
#include "EmployerDialog.rh"            // Definition of all resources.
#include "CheckAuth.h"
#include "MsgWin.h"
#include "DCManage.h"

//{{TDialog = EmployerDialog}}
class EmployerDialog : public TDialog {
  CheckAuth &authMsg;
  bool   enableSpouseGroup();
  void   enableEmployerGroup();
  bool   spouseEnabled;
  bool   employerEnabled;        
  bool   displayMode;

  public:
    bool editData();
    DialogControlManager* DCMan;
    EmployerDialog(TWindow* parent,CheckAuth& Msg,TMsgWin* MsgWin);
    virtual ~EmployerDialog();

//{{EmployerDialogVIRTUAL_BEGIN}}
  public:
    virtual TResult EvCommand(uint id, THandle hWndCtl, uint notifyCode);
    virtual void SetupWindow();
//{{EmployerDialogVIRTUAL_END}}

//{{EmployerDialogRSP_TBL_BEGIN}}
  protected:
    void CBNSelchange();
//{{EmployerDialogRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(EmployerDialog);
};    //{{EmployerDialog}}


#endif  // employerdialog_h sentry.


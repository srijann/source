//----------------------------------------------------------------------------
//  Project VoiceAuth
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    VoiceAuth.apx Application
//  FILE:         declinedialog.h
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for DeclineDialog (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(declinedialog_h)              // Sentry, use file only if it's not already included.
#define declinedialog_h

#include <owl/dialog.h>


#include <owl/commctrl.h>
#include "Dialogs.rh"


//{{TDialog = DeclineDialog}}
class DeclineDialog : public TDialog {
  public:
    DeclineDialog(TWindow* parent);
    virtual ~DeclineDialog();

//{{DeclineDialogVIRTUAL_BEGIN}}
  public:
    virtual TResult EvCommand(uint id, THandle hWndCtl, uint notifyCode);
//{{DeclineDialogVIRTUAL_END}}

//{{DeclineDialogRSP_TBL_BEGIN}}
  protected:
    void BNClicked();
//{{DeclineDialogRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(DeclineDialog);
};    //{{DeclineDialog}}


#endif  // declinedialog_h sentry.


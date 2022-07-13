//----------------------------------------------------------------------------
//  Project SPSFileEdit
//  Secure Payment Systems
//  Copyright © 1997. All Rights Reserved.
//
//  SUBSYSTEM:    SPSFileEdit.apx Application
//  FILE:         maindialog.h
//  AUTHOR:       Jack Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for MainDialog (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(maindialog_h)              // Sentry, use file only if it's not already included.
#define maindialog_h

#include <owl/dialog.h>

#include "posfilemanapp.rh"            // Definition of all resources.


//{{TDialog = MainDialog}}
class MainDialog : public TDialog {
  public:
    MainDialog(TWindow* parent, TResId resId = IDD_MAIN, TModule* module = 0);
    virtual ~MainDialog();

//{{MainDialogVIRTUAL_BEGIN}}
  public:
    virtual TResult EvCommand(uint id, THandle hWndCtl, uint notifyCode);
//{{MainDialogVIRTUAL_END}}
};    //{{MainDialog}}


#endif  // maindialog_h sentry.


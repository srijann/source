//----------------------------------------------------------------------------
//  Project spsmanager
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    spsmanager.apx Application
//  FILE:         FormDialog.h
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for FormDialog (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(FormDialog_h)              // Sentry, use file only if it's not already included.
#define FormDialog_h

#include <owl/dialog.h>
#include <classlib/file.h>

#include "spsmanagerapp.rh"            // Definition of all resources.
#include "rectype.h"

//{{TDialog = FormDialog}}
class FormDialog : public TDialog {
  short RecType;
  public:
    FormDialog(TWindow* parent,TResId resId,short Type=0);
    virtual ~FormDialog();
    short GetType() { return RecType; };
    virtual bool IsModified() { return ModifiedFlag; };
    virtual void ResetModified() { ModifiedFlag=false; };
    virtual void SetModified() { ModifiedFlag=true; };
    virtual bool Save() { return true; };
    virtual bool SaveTemp(TFile* File) { return true; };
    virtual void RePaint(int RecType) {return;};
    virtual void SetTop();
    virtual bool Validate() {return true;};

  protected:
  bool  ModifiedFlag;
  bool  FailFlag;

//{{FormDialogVIRTUAL_BEGIN}}
  public:
    virtual TResult EvCommand(uint id, THandle hWndCtl, uint notifyCode);
    virtual bool CanClose();
    virtual TResult DefWindowProc(uint msg, TParam1 p1, TParam2 p2);
    virtual bool Create();
//{{FormDialogVIRTUAL_END}}

//{{FormDialogRSP_TBL_BEGIN}}
  protected:
//{{FormDialogRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(FormDialog);
};    //{{FormDialog}}


#endif  // FormDialog_h sentry.


//----------------------------------------------------------------------------
//  Project spsmanager
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    spsmanager.apx Application
//  FILE:         responsemsgdialog.h
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for ResponseMsgDialog (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(responsemsgdialog_h)              // Sentry, use file only if it's not already included.
#define responsemsgdialog_h

#include <owl/dialog.h>
#include <owl/edit.h>

#include "PosResponse.rh"
#include "PosResponse.h"
#include "FormDialog.h"

#define NUMFIELDS 80

//{{TDialog = ResponseMsgDialog}}
class ResponseMsgDialog : public FormDialog {

  PosResponse* origRec;
  PosResponse  newRec;

  bool SetupComplete;

  TEdit* Flds[NUMFIELDS];

  public:
    ResponseMsgDialog(TWindow* parent, PosResponse* OrigRec );
    virtual ~ResponseMsgDialog();
    virtual bool IsModified();
    virtual bool Save();
    virtual bool SaveTemp(TFile* File);
    virtual void ResetModified();

//{{ResponseMsgDialogVIRTUAL_BEGIN}}
  public:
    virtual void SetupWindow();
    virtual TResult EvCommand(uint id, THandle hWndCtl, uint notifyCode);
//{{ResponseMsgDialogVIRTUAL_END}}
};    //{{ResponseMsgDialog}}


#endif  // responsemsgdialog_h sentry.


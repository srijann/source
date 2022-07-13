//----------------------------------------------------------------------------
//  Project spsmanager
//
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    spsmanager.apx Application
//  FILE:         UnvFundsdialog.h
//  AUTHOR:
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for UnvFundsDialog (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(UnvFundsdialog_h)              // Sentry, use file only if it's not already included.
#define UnvFundsdialog_h

#include <owl/dialog.h>
#include <owl/tabctrl.h>
#include <owl/checkbox.h>

#include "UnvFundsDialog.rh"
#include "UnvFundsRules.h"
#include "FormDialog.h"
#include "edituns.h"

//{{TDialog = UnvFundsDialog}}
class UnvFundsDialog : public FormDialog {
  void UpdateWin();
  void ChangeHandler(uint Id);
  bool SetupComplete;

  UnvFunds* origRec;
  UnvFunds  newRec;

  public:
    UnvFundsDialog(TWindow* parent, UnvFunds* OrigRec );
    virtual ~UnvFundsDialog();
    virtual bool IsModified();
    virtual bool Save();
    virtual bool SaveTemp(TFile* File);
    virtual void ResetModified();
    virtual bool Validate();
//{{UnvFundsDialogVIRTUAL_BEGIN}}
  public:
    virtual void SetupWindow();
    TResult EvCommand(uint id, THandle hWndCtl, uint notifyCode);
//{{UnvFundsDialogVIRTUAL_END}}

//{{UnvFundsDialogRSP_TBL_BEGIN}}
  protected:
//{{UnvFundsDialogRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(UnvFundsDialog);
};    //{{UnvFundsDialog}}


#endif  // UnvFundsdialog_h sentry.


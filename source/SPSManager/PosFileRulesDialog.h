//----------------------------------------------------------------------------
//  Project spsmanager
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    spsmanager.apx Application
//  FILE:         posfilerulesdialog.h
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for PosFileRulesDialog (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(posfilerulesdialog_h)              // Sentry, use file only if it's not already included.
#define posfilerulesdialog_h

#include <owl/dialog.h>
#include <owl/commctrl.h>

#include "PosFileDialog.rh"
#include "PositiveRules.h"
#include "FormDialog.h"
#include "EditUns.h"

#define NUMROWS 9
#define NUMCOLS 6

//{{TDialog = PosFileRulesDialog}}
class PosFileRulesDialog : public FormDialog {
  TEditUns* POSMAXOVERRIDES;
  TEditUns* POSPERIOD;
  TEditUns* OVERRIDEACCUM;
  TEditUns* WAITBEFOREPOSITIVE;
  TEditUns* MULTIPLYNUMAPPROVED;
  TEditUns* MULTIPLYDOLLARSAPPROVED;
  TEditUns* MULTIPLYMONTHS;

  struct
   {
    TEditUns* AMT;
    TEdit* MULTI[NUMCOLS];
   } Row[NUMROWS];

  PositiveRules* origRules;
  PositiveRules  newRules;
  void multiplierIn(TEdit* Ctl,unsigned char& Val);
  void multiplierOut(TEdit* Ctl,unsigned char Val);
  bool SetupComplete;
  void ChangeHandler(uint Id);

  public:
    PosFileRulesDialog(TWindow* parent, PositiveRules* OrigRules);
    virtual ~PosFileRulesDialog();
    virtual bool IsModified();
    virtual bool Save();
    virtual bool SaveTemp(TFile* File);
    virtual void ResetModified();
    virtual bool Validate();

//{{PosFileRulesDialogVIRTUAL_BEGIN}}
  public:
    virtual void SetupWindow();
    virtual TResult EvCommand(uint id, THandle hWndCtl, uint notifyCode);
//{{PosFileRulesDialogVIRTUAL_END}}

//{{PosFileRulesDialogRSP_TBL_BEGIN}}
  protected:
    void TCNSelchange(TNotify& tcn);
//{{PosFileRulesDialogRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(PosFileRulesDialog);
};    //{{PosFileRulesDialog}}


#endif  // posfilerulesdialog_h sentry.


//----------------------------------------------------------------------------
//  Project spsmanager
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    spsmanager.apx Application
//  FILE:         exitdialog.h
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for ExitDialog (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(exitdialog_h)              // Sentry, use file only if it's not already included.
#define exitdialog_h

#include <owl/dialog.h>
#include <owl/radiobut.h>

#include <owl/commctrl.h>

#include "SpsManagerApp.rh"            // Definition of all resources.


//{{TDialog = ExitDialog}}
class ExitDialog : public TDialog {
  TRadioButton* SAVENOW;
  TRadioButton* SAVEWORK;
  TRadioButton* DISCARD;
  TRadioButton* REVIEW;
  int* Selection;

  public:
    ExitDialog(TWindow* parent, int* Sel,
               TResId resId = IDD_EXITDIALOG, TModule* module = 0);
    virtual ~ExitDialog();

//{{ExitDialogVIRTUAL_BEGIN}}
  public:
    virtual void SetupWindow();
//{{ExitDialogVIRTUAL_END}}

//{{ExitDialogRSP_TBL_BEGIN}}
  protected:
    void BNCancelClicked();
    void BNSaveNowClicked();
    void BNSaveWorkClicked();
    void BNReviewClicked();
    void BNDiscardClicked();
//{{ExitDialogRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(ExitDialog);
};    //{{ExitDialog}}


#endif  // exitdialog_h sentry.


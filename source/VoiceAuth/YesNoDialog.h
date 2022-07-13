//----------------------------------------------------------------------------
//  Project VoiceAuth
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    VoiceAuth.apx Application
//  FILE:         yesnodialog.h
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for YesNoDialog (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(yesnodialog_h)              // Sentry, use file only if it's not already included.
#define yesnodialog_h

#include <owl/dialog.h>


#include <owl/commctrl.h>
#include <owl/static.h>
#include "Dialogs.rh"            // Definition of all resources.


//{{TDialog = YesNoDialog}}
class YesNoDialog : public TDialog {
  int& yesNo;
  TStatic* TEXT;
  char* text;
  public:
    YesNoDialog(TWindow* parent, int& YesNo,char* Text=0);
    virtual ~YesNoDialog();

//{{YesNoDialogVIRTUAL_BEGIN}}
    virtual void SetupWindow();
//{{YesNoDialogVIRTUAL_END}}

//{{YesNoDialogRSP_TBL_BEGIN}}
  protected:
    void BNCancelClicked();
    void BNOkClicked();
//{{YesNoDialogRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(YesNoDialog);
};    //{{YesNoDialog}}


#endif  // yesnodialog_h sentry.


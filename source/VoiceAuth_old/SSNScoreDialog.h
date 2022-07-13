//----------------------------------------------------------------------------
//  Project VoiceAuth
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    VoiceAuth.apx Application
//  FILE:         ssnscoredialog.h
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for SSNScoreDialog (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(ssnscoredialog_h)              // Sentry, use file only if it's not already included.
#define ssnscoredialog_h

#include <owl/dialog.h>

#include "Dialogs.rh"            // Definition of all resources.
#include "EditVar.h"

//{{TDialog = SSNScoreDialog}}
class SSNScoreDialog : public TDialog {
  int& score;
  TEdit* SSN;
  char* theSSN;
  public:
    SSNScoreDialog(TWindow* parent, char* ssn,int& Score);
    virtual ~SSNScoreDialog();

//{{SSNScoreDialogVIRTUAL_BEGIN}}
  public:
    virtual void SetupWindow();
//{{SSNScoreDialogVIRTUAL_END}}
};    //{{SSNScoreDialog}}


#endif  // ssnscoredialog_h sentry.


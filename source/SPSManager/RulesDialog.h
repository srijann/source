//----------------------------------------------------------------------------
//  Project spsmanager
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    spsmanager.apx Application
//  FILE:         rulesdialog.h
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for RulesDialog (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(rulesdialog_h)              // Sentry, use file only if it's not already included.
#define rulesdialog_h

#include <owl/dialog.h>

#include "AuthRulesDialog.rh"          // Definition of all resources.


//{{TDialog = RulesDialog}}
class RulesDialog : public TDialog {
  public:
    RulesDialog(TWindow* parent, TResId resId = IDD_RULES, TModule* module = 0);
    virtual ~RulesDialog();
};    //{{RulesDialog}}


#endif  // rulesdialog_h sentry.


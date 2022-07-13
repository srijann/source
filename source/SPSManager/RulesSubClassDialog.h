//----------------------------------------------------------------------------
//  Project spsmanager
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    spsmanager.apx Application
//  FILE:         rulessubclassdialog.h
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for RulesSubClassDialog (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(rulessubclassdialog_h)              // Sentry, use file only if it's not already included.
#define rulessubclassdialog_h

#include <owl/dialog.h>

#include "spsmanagerapp.rh"            // Definition of all resources.


//{{TDialog = RulesSubClassDialog}}
class RulesSubClassDialog : public TDialog {
  public:
    RulesSubClassDialog(TWindow* parent, TResId resId = IDD_BASICAUTH, TModule* module = 0);
    virtual ~RulesSubClassDialog();
};    //{{RulesSubClassDialog}}


#endif  // rulessubclassdialog_h sentry.


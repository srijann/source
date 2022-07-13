//----------------------------------------------------------------------------
//  Project spsmanager
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    spsmanager.apx Application
//  FILE:         authrulesdilalog.h
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for AuthRulesDilalog (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(authrulesdilalog_h)              // Sentry, use file only if it's not already included.
#define authrulesdilalog_h

#include <owl/dialog.h>

#include "spsmanagerapp.rh"            // Definition of all resources.


//{{TDialog = AuthRulesDilalog}}
class AuthRulesDilalog : public TDialog {
  public:
    AuthRulesDilalog(TWindow* parent, TResId resId = IDD_BASICAUTH, TModule* module = 0);
    virtual ~AuthRulesDilalog();
};    //{{AuthRulesDilalog}}


#endif  // authrulesdilalog_h sentry.


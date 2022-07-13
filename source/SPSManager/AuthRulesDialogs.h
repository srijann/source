//----------------------------------------------------------------------------
//  Project spsmanager
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    spsmanager.apx Application
//  FILE:         authrulesdialogs.h
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for AuthRulesDialogs (TWindow).
//
//----------------------------------------------------------------------------
#if !defined(authrulesdialogs_h)              // Sentry, use file only if it's not already included.
#define authrulesdialogs_h

#include <owl/window.h>
#include <owl/dialog.h>

class DialogManager {
  public:
  TDialog* RulesWin;
  TDialog* DateTimeWin;
  TDialog* ProductWin;
  TDialog* CurrentWin;
 };

//{{TWindow = AuthRulesDialogs}}
class AuthRulesDialogs : public TWindow {
  DialogManager* Manager;
  public:
    AuthRulesDialogs(TWindow* parent, const char far* title = 0, TModule* module = 0);
    virtual ~AuthRulesDialogs();
};    //{{AuthRulesDialogs}}


#endif  // authrulesdialogs_h sentry.


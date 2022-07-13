//----------------------------------------------------------------------------
//  Project spsmanager
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    spsmanager.apx Application
//  FILE:         ruleedit.h
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for TRuleEdit (TEdit).
//
//----------------------------------------------------------------------------
#if !defined(ruleedit_h)              // Sentry, use file only if it's not already included.
#define ruleedit_h

#include <owl/edit.h>

#include "AuthRules.h"

//{{TEdit = TRuleEdit}}
class TRuleEdit : public TEdit {
  AuthRules* Rules;
  int RuleNum;
  public:
    TRuleEdit(TWindow* parent, int id, AuthRules* rules,
              int ruleNum, uint textLimit = 0, TModule* module = 0);
    virtual ~TRuleEdit();

//{{TRuleEditVIRTUAL_BEGIN}}
  public:
    virtual void SetupWindow();
//{{TRuleEditVIRTUAL_END}}
};    //{{TRuleEdit}}


#endif  // ruleedit_h sentry.


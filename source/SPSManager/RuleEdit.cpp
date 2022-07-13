//----------------------------------------------------------------------------
//  Project spsmanager
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    spsmanager.apx Application
//  FILE:         ruleedit.cpp
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of TRuleEdit (TEdit).
//
//----------------------------------------------------------------------------
#include "Win4me.h"
#include "ruleedit.h"


//{{TRuleEdit Implementation}}


TRuleEdit::TRuleEdit(TWindow* parent, int id, AuthRules* rules,
                     int ruleNum, uint textLimit, TModule* module)
:
 TEdit(parent, id, textLimit, module),
 Rules(rules),
 RuleNum(ruleNum)
{
}


TRuleEdit::~TRuleEdit()
{
  Destroy(IDCANCEL);
}


void TRuleEdit::SetupWindow()
{
 char Buf[10];
 TEdit::SetupWindow();
}


//----------------------------------------------------------------------------
//  Project spsmanager
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    spsmanager.apx Application
//  FILE:         authrulesdilalog.cpp
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of AuthRulesDilalog (TDialog).
//
//----------------------------------------------------------------------------
#include <owl/pch.h>

#include "authrulesdilalog.h"


//{{AuthRulesDilalog Implementation}}


AuthRulesDilalog::AuthRulesDilalog(TWindow* parent, TResId resId, TModule* module)
:
    TDialog(parent, resId, module)
{
  // INSERT>> Your constructor code here.

}


AuthRulesDilalog::~AuthRulesDilalog()
{
  Destroy(IDCANCEL);

  // INSERT>> Your destructor code here.

}


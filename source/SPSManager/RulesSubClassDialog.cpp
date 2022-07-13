//----------------------------------------------------------------------------
//  Project spsmanager
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    spsmanager.apx Application
//  FILE:         rulessubclassdialog.cpp
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of RulesSubClassDialog (TDialog).
//
//----------------------------------------------------------------------------
#include <owl/pch.h>

#include "rulessubclassdialog.h"


//{{RulesSubClassDialog Implementation}}


RulesSubClassDialog::RulesSubClassDialog(TWindow* parent, TResId resId, TModule* module)
:
    TDialog(parent, resId, module)
{
  // INSERT>> Your constructor code here.

}


RulesSubClassDialog::~RulesSubClassDialog()
{
  Destroy(IDCANCEL);

  // INSERT>> Your destructor code here.

}


//----------------------------------------------------------------------------
//  Project spsmanager
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    spsmanager.apx Application
//  FILE:         rulesdialog.cpp
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of RulesDialog (TDialog).
//
//----------------------------------------------------------------------------
#include "Win4me.h"

#include "rulesdialog.h"


//{{RulesDialog Implementation}}


RulesDialog::RulesDialog(TWindow* parent, TResId resId, TModule* module)
:
    TDialog(parent, resId, module)
{
  // INSERT>> Your constructor code here.

}


RulesDialog::~RulesDialog()
{
  Destroy(IDCANCEL);

  // INSERT>> Your destructor code here.

}


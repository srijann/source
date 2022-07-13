//----------------------------------------------------------------------------
//  Project spsmanager
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    spsmanager.apx Application
//  FILE:         accountdialog.cpp
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of AccountDialog (TDialog).
//
//----------------------------------------------------------------------------
#include "Win4me.h"
#include "accountdialog.h"


//{{AccountDialog Implementation}}


AccountDialog::AccountDialog(TWindow* parent, TResId resId, TModule* module)
:
    TDialog(parent, resId, module)
{
}


AccountDialog::~AccountDialog()
{
  Destroy(IDCANCEL);

  // INSERT>> Your destructor code here.

}


void AccountDialog::SetupWindow()
{
 TDialog::SetupWindow();
 SetCaption("New Account");
}


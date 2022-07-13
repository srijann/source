//----------------------------------------------------------------------------
//  Project SPSFileEdit
//  Secure Payment Systems
//  Copyright © 1997. All Rights Reserved.
//
//  SUBSYSTEM:    SPSFileEdit.apx Application
//  FILE:         maindialog.cpp
//  AUTHOR:       Jack Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of MainDialog (TDialog).
//
//----------------------------------------------------------------------------
#include "win4me.h"

#include "maindialog.h"


//{{MainDialog Implementation}}


MainDialog::MainDialog(TWindow* parent, TResId resId, TModule* module)
:
    TDialog(parent, resId, module)
{
  // INSERT>> Your constructor code here.

}


MainDialog::~MainDialog()
{
  Destroy(IDCANCEL);

  // INSERT>> Your destructor code here.

}


TResult MainDialog::EvCommand(uint id, THandle hWndCtl, uint notifyCode)
{
  TResult result;

  switch ( notifyCode )
   {
    case 0:
       if ( id==IDCANCEL || id==IDOK )
         return 0;
      break;
   }

  result = TDialog::EvCommand(id, hWndCtl, notifyCode);
  return result;
}


//----------------------------------------------------------------------------
//  Project spsmanager
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    spsmanager.apx Application
//  FILE:         editpopup.cpp
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of TEditPopup (TEdit).
//
//----------------------------------------------------------------------------
#include <owl/pch.h>

#include "editpopup.h"


//{{TEditPopup Implementation}}


TEditPopup::TEditPopup(TWindow* parent, int id, TModule* module)
:
    TEdit(parent, id, text, x, y, w, h, textLimit, multiline, module)
{
  // Override the default window style for TEdit.
  //
      Attr.Style |= WS_BORDER | WS_CAPTION | WS_CHILD | WS_VISIBLE;
    Attr.Style &= ~(WS_MINIMIZEBOX);

  // INSERT>> Your constructor code here.

}


TEditPopup::~TEditPopup()
{
  Destroy(IDCANCEL);

  // INSERT>> Your destructor code here.

}


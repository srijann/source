//----------------------------------------------------------------------------
//  Project spsmanager
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    spsmanager.apx Application
//  FILE:         dbfield.cpp
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of TDbField (TEdit).
//
//----------------------------------------------------------------------------
#include <owl/pch.h>

#include "dbfield.h"


//{{TDbField Implementation}}


TDbField::TDbField(TWindow* parent, int id, const char far* text, int x, int y, int w, int h, uint textLimit, bool multiline, TModule* module)
:
    TEdit(parent, id, text, x, y, w, h, textLimit, multiline, module)
{
  // INSERT>> Your constructor code here.

}


TDbField::~TDbField()
{
  Destroy(IDCANCEL);

  // INSERT>> Your destructor code here.

}


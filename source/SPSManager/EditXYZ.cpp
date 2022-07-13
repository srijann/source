//----------------------------------------------------------------------------
//  Project SpsManager
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    SpsManager.apx Application
//  FILE:         editxyz.cpp
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of TEditXYZ (TEdit).
//
//----------------------------------------------------------------------------
#include <owl/pch.h>

#include "editxyz.h"


//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(TEditXYZ, TEdit)
//{{TEditXYZRSP_TBL_BEGIN}}
  EV_WM_CHAR,
//{{TEditXYZRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{TEditXYZ Implementation}}


TEditXYZ::TEditXYZ(TWindow* parent, int id, const char far* text, int x, int y, int w, int h, uint textLimit, bool multiline, TModule* module)
:
    TEdit(parent, id, text, x, y, w, h, textLimit, multiline, module)
{
  // INSERT>> Your constructor code here.

}


TEditXYZ::~TEditXYZ()
{
  Destroy(IDCANCEL);

  // INSERT>> Your destructor code here.

}


void TEditXYZ::EvChar(uint key, uint repeatCount, uint flags)
{
    TEdit::EvChar(key, repeatCount, flags);

  // INSERT>> Your code here.

}


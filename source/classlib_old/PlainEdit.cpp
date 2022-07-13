//----------------------------------------------------------------------------
//  Project Classlib
//
//  Copyright © 1996. All Rights Reserved.
//
//  FILE:         editvar.cpp
//  AUTHOR:       Jack Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of PlainEdit::PlainEdit (TEdit).
//
//  This is simply a TEdit that doesn't have a pop up menu (the right
//  mouse click is ignored. There was probably a better way of doing this
//  but I'll be damned if I could figure it out.
//----------------------------------------------------------------------------
#include "win4me.h"

#include <stdio.h>
#include "PlainEdit.h"

//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(PlainEdit, TEdit)
//{{PlainEditRSP_TBL_BEGIN}}
EV_WM_RBUTTONDOWN,
//{{PlainEditRSP_TBL_END}}
END_RESPONSE_TABLE;

//{{PlainEdit::PlainEdit Implementation}}


PlainEdit::PlainEdit(TWindow* parent, int Id, const char far* text, int x, int y,
            int w, int h, uint textLimit,
            bool multiline, TModule* module):
 TEdit(parent,Id,text,x,y,w,h,textLimit,multiline,module)
{
}

PlainEdit::PlainEdit(TWindow* parent, int resourceId, uint textLimit,
            TModule* module) :
 TEdit(parent,resourceId,textLimit,module)
{
}

void PlainEdit::EvRButtonDown(uint,TPoint&)
{
}


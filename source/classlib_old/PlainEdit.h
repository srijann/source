//----------------------------------------------------------------------------
//  Project SpsManager
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    SpsManager.apx Application
//  FILE:         PLAINEDIT.h
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for TPLAINEDIT (TEdit).
//
//----------------------------------------------------------------------------
#ifndef __PLAINEDIT_H
#define __PLAINEDIT_H

#include <owl/edit.h>


//{{TEdit = TPLAINEDIT}}
class PlainEdit : public TEdit {
  public:
  PlainEdit(TWindow* parent, int Id, const char far* text, int x, int y,
            int w, int h, uint textLimit = 0,
            bool multiline = false, TModule* module = 0);
  PlainEdit(TWindow* parent, int resourceId, uint textLimit = 0,
            TModule* module = 0);
//{{TPLAINEDITVIRTUAL_BEGIN}}
  public:
  void EvRButtonDown(uint,TPoint&);
//{{TPLAINEDITVIRTUAL_END}}
DECLARE_RESPONSE_TABLE(PlainEdit);
};    //{{TPLAINEDIT}}


#endif  // PLAINEDIT_h sentry.


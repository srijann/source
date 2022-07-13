//----------------------------------------------------------------------------
//  Project spsmanager
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    spsmanager.apx Application
//  FILE:         dbfield.h
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for TDbField (TEdit).
//
//----------------------------------------------------------------------------
#if !defined(dbfield_h)              // Sentry, use file only if it's not already included.
#define dbfield_h

#include <owl/edit.h>

#include "spsmanagerapp.rh"            // Definition of all resources.


//{{TEdit = TDbField}}
class TDbField : public TEdit {
  public:
    TDbField(TWindow* parent, int id, const char far* text, int x, int y, int w, int h, uint textLimit = 0, bool multiline = false, TModule* module = 0);
    virtual ~TDbField();
};    //{{TDbField}}


#endif  // dbfield_h sentry.


//----------------------------------------------------------------------------
//  Project spsmanager
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    spsmanager.apx Application
//  FILE:         editpopup.h
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for TEditPopup (TEdit).
//
//----------------------------------------------------------------------------
#if !defined(editpopup_h)              // Sentry, use file only if it's not already included.
#define editpopup_h

#include <owl/edit.h>

#include "spsmanagerapp.rh"            // Definition of all resources.


//{{TEdit = TEditPopup}}
class TEditPopup : public TEdit {
  public:
    TEditPopup(TWindow* parent, int id, TModule* module = 0);
    virtual ~TEditPopup();
};    //{{TEditPopup}}


#endif  // editpopup_h sentry.


//----------------------------------------------------------------------------
//  Project SpsManager
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    SpsManager.apx Application
//  FILE:         editxyz.h
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for TEditXYZ (TEdit).
//
//----------------------------------------------------------------------------
#if !defined(editxyz_h)              // Sentry, use file only if it's not already included.
#define editxyz_h

#include <owl/edit.h>

#include "spsmanagerapp.rh"            // Definition of all resources.


//{{TEdit = TEditXYZ}}
class TEditXYZ : public TEdit {
  public:
    TEditXYZ(TWindow* parent, int id, const char far* text, int x, int y, int w, int h, uint textLimit = 0, bool multiline = false, TModule* module = 0);
    virtual ~TEditXYZ();

//{{TEditXYZRSP_TBL_BEGIN}}
  protected:
    void EvChar(uint key, uint repeatCount, uint flags);
//{{TEditXYZRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(TEditXYZ);
};    //{{TEditXYZ}}


#endif  // editxyz_h sentry.


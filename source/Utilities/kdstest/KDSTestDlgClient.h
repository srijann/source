//----------------------------------------------------------------------------
//  Project KDSTest
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    KDSTest Application
//  FILE:         kdstestdlgclient.h
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for TKDSTestDlgClient (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(kdstestdlgclient_h)              // Sentry, use file only if it's not already included.
#define kdstestdlgclient_h

#include "kdstestapp.rh"                  // Definition of all resources.


//{{TDialog = TKDSTestDlgClient}}
class TKDSTestDlgClient : public TDialog {
  public:
    TKDSTestDlgClient(TWindow* parent, TResId resId = IDD_CLIENT, TModule* module = 0);
    virtual ~TKDSTestDlgClient();

};    //{{TKDSTestDlgClient}}


#endif  // kdstestdlgclient_h sentry.

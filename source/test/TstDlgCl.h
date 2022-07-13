//----------------------------------------------------------------------------
//  Project Test
//  
//  Copyright © 1997. All Rights Reserved.
//
//  SUBSYSTEM:    Test Application
//  FILE:         tstdlgcl.h
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for TTestDlgClient (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(tstdlgcl_h)              // Sentry, use file only if it's not already included.
#define tstdlgcl_h

#include "testapp.rh"                  // Definition of all resources.


#include <owl/commctrl.h>

//{{TDialog = TTestDlgClient}}
class TTestDlgClient : public TDialog {
  public:
    TTestDlgClient(TWindow* parent, TResId resId = IDD_CLIENT, TModule* module = 0);
    virtual ~TTestDlgClient();


//{{TTestDlgClientRSP_TBL_BEGIN}}
  protected:
    void BNClicked();
//{{TTestDlgClientRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(TTestDlgClient);
};    //{{TTestDlgClient}}


#endif  // tstdlgcl_h sentry.

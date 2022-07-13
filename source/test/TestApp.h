//----------------------------------------------------------------------------
//  Project Test
//  
//  Copyright © 1997. All Rights Reserved.
//
//  SUBSYSTEM:    Test Application
//  FILE:         testapp.h
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for TTestApp (TApplication).
//
//----------------------------------------------------------------------------
#if !defined(testapp_h)              // Sentry, use file only if it's not already included.
#define testapp_h

#include <owl/opensave.h>


#include "testapp.rh"            // Definition of all resources.


//
// FrameWindow must be derived to override Paint for Preview and Print.
//
//{{TDecoratedFrame = TSDIDecFrame}}
class TSDIDecFrame : public TDecoratedFrame {
  public:
    TSDIDecFrame(TWindow* parent, const char far* title, TWindow* clientWnd, bool trackMenuSelection = false, TModule* module = 0);
    ~TSDIDecFrame();
};    //{{TSDIDecFrame}}


//{{TApplication = TTestApp}}
class TTestApp : public TApplication {
  private:


  public:
    TTestApp();
    virtual ~TTestApp();

//{{TTestAppVIRTUAL_BEGIN}}
  public:
    virtual void InitMainWindow();
//{{TTestAppVIRTUAL_END}}

//{{TTestAppRSP_TBL_BEGIN}}
  protected:
    void CmHelpAbout();
//{{TTestAppRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(TTestApp);
};    //{{TTestApp}}


#endif  // testapp_h sentry.

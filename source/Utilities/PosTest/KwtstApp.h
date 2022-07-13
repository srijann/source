//----------------------------------------------------------------------------
//  Project Kwtest
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    Kwtest Application
//  FILE:         kwtstapp.h
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for TKwtestApp (TApplication).
//
//----------------------------------------------------------------------------
#if !defined(kwtstapp_h)              // Sentry, use file only if it's not already included.
#define kwtstapp_h

#include <owl/opensave.h>


#include "kwtstapp.rh"            // Definition of all resources.


//
// FrameWindow must be derived to override Paint for Preview and Print.
//
//{{TDecoratedFrame = TSDIDecFrame}}
class TSDIDecFrame : public TDecoratedFrame {
  public:
    TSDIDecFrame(TWindow* parent, const char far* title, TWindow* clientWnd, bool trackMenuSelection = false, TModule* module = 0);
    ~TSDIDecFrame();
};    //{{TSDIDecFrame}}


//{{TApplication = TKwtestApp}}
class TKwtestApp : public TApplication {
  private:


  public:
    TKwtestApp();
    virtual ~TKwtestApp();

//{{TKwtestAppVIRTUAL_BEGIN}}
  public:
    virtual void InitMainWindow();
//{{TKwtestAppVIRTUAL_END}}

//{{TKwtestAppRSP_TBL_BEGIN}}
  protected:
    void CmHelpAbout();
//{{TKwtestAppRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(TKwtestApp);
};    //{{TKwtestApp}}


#endif  // kwtstapp_h sentry.

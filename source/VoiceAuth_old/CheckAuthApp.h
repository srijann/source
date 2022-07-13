//----------------------------------------------------------------------------
//  Project CheckAuth
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    CheckAuth Application
//  FILE:         checkauthapp.h
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for TCheckAuthApp (TApplication).
//
//----------------------------------------------------------------------------
#if !defined(checkauthapp_h)              // Sentry, use file only if it's not already included.
#define checkauthapp_h

#include <owl/opensave.h>


#include "checkauthapp.rh"            // Definition of all resources.


//
// FrameWindow must be derived to override Paint for Preview and Print.
//
//{{TDecoratedFrame = TSDIDecFrame}}
class TSDIDecFrame : public TDecoratedFrame {
  public:
    TSDIDecFrame(TWindow* parent, const char far* title, TWindow* clientWnd, bool trackMenuSelection = false, TModule* module = 0);
    ~TSDIDecFrame();
};    //{{TSDIDecFrame}}


//{{TApplication = TCheckAuthApp}}
class TCheckAuthApp : public TApplication {
  private:


  public:
    TCheckAuthApp();
    virtual ~TCheckAuthApp();

//{{TCheckAuthAppVIRTUAL_BEGIN}}
  public:
    virtual void InitMainWindow();
//{{TCheckAuthAppVIRTUAL_END}}

//{{TCheckAuthAppRSP_TBL_BEGIN}}
  protected:
    void CmHelpAbout();
  void ENSetfocus();
//{{TCheckAuthAppRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(TCheckAuthApp);
};    //{{TCheckAuthApp}}


#endif  // checkauthapp_h sentry.

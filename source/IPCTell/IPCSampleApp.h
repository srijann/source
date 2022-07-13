//----------------------------------------------------------------------------
//  Project IPCSample
//  JHE Software, Inc.
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    IPCSample Application
//  FILE:         IPCSampleApp.h
//  AUTHOR:       Jack H. Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for IPCSampleApp (TApplication).
//
//----------------------------------------------------------------------------
#if !defined(IPCSampleApp_h)              // Sentry, use file only if it's not already included.
#define IPCSampleApp_h

#include <owl/opensave.h>


#include "IPCSample.rh"            // Definition of all resources.


//
// FrameWindow must be derived to override Paint for Preview and Print.
//
//{{TDecoratedFrame = TSDIDecFrame}}
class TSDIDecFrame : public TDecoratedFrame {
  public:
    TSDIDecFrame(TWindow* parent, const char far* title, TWindow* clientWnd, bool trackMenuSelection = false, TModule* module = 0);
    ~TSDIDecFrame();

//{{TSDIDecFrameRSP_TBL_BEGIN}}
  protected:
//{{TSDIDecFrameRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(TSDIDecFrame);
};    //{{TSDIDecFrame}}


//{{TApplication = IPCSampleApp}}
class IPCSampleApp : public TApplication {
  private:


  public:
    IPCSampleApp();
    virtual ~IPCSampleApp();

//{{IPCSampleAppVIRTUAL_BEGIN}}
  public:
    virtual void InitMainWindow();
//{{IPCSampleAppVIRTUAL_END}}

//{{IPCSampleAppRSP_TBL_BEGIN}}
  protected:
    void CmHelpAbout();
//{{IPCSampleAppRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(IPCSampleApp);
};    //{{IPCSampleApp}}


#endif  // IPCSampleApp_h sentry.

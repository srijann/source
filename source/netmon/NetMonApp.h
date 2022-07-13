//----------------------------------------------------------------------------
//  Project NetMon
//  Secure Payment Systems
//  Copyright © 1997. All Rights Reserved.
//
//  SUBSYSTEM:    NetMon Application
//  FILE:         netmonapp.h
//  AUTHOR:       Jack H. Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for TNetMonApp (TApplication).
//
//----------------------------------------------------------------------------
#if !defined(netmonapp_h)              // Sentry, use file only if it's not already included.
#define netmonapp_h

#include <owl/opensave.h>


#include "netmonapp.rh"            // Definition of all resources.


//
// FrameWindow must be derived to override Paint for Preview and Print.
//
//{{TDecoratedFrame = TSDIDecFrame}}
class TSDIDecFrame : public TDecoratedFrame {
  public:
    TSDIDecFrame(TWindow* parent, const char far* title, TWindow* clientWnd, bool trackMenuSelection = false, TModule* module = 0);
    ~TSDIDecFrame();
};    //{{TSDIDecFrame}}


//{{TApplication = TNetMonApp}}
class TNetMonApp : public TApplication {
  private:


  public:
    TNetMonApp();
    virtual ~TNetMonApp();

//{{TNetMonAppVIRTUAL_BEGIN}}
  public:
    virtual void InitMainWindow();
//{{TNetMonAppVIRTUAL_END}}

//{{TNetMonAppRSP_TBL_BEGIN}}
  protected:
    void CmHelpAbout();
//{{TNetMonAppRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(TNetMonApp);
};    //{{TNetMonApp}}


#endif  // netmonapp_h sentry.

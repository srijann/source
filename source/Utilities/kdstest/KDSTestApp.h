//----------------------------------------------------------------------------
//  Project KDSTest
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    KDSTest Application
//  FILE:         kdstestapp.h
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for TKDSTestApp (TApplication).
//
//----------------------------------------------------------------------------
#if !defined(kdstestapp_h)              // Sentry, use file only if it's not already included.
#define kdstestapp_h

#include <owl/opensave.h>


#include "kdstestapp.rh"            // Definition of all resources.
#include "TranLogDialog.h"
#include "ResponseDialog.h"

//
// FrameWindow must be derived to override Paint for Preview and Print.
//
//{{TDecoratedFrame = TSDIDecFrame}}
class TSDIDecFrame : public TDecoratedFrame {
  TranLogDialog* logDialog;
  ResponseDialog* responseDialog;
  IPCMessage Msg;
  public:
    TSDIDecFrame(TWindow* parent, const char far* title, TWindow* clientWnd, bool trackMenuSelection = false, TModule* module = 0);
    ~TSDIDecFrame();

//{{TSDIDecFrameVIRTUAL_BEGIN}}
  public:
    virtual void SetupWindow();
//{{TSDIDecFrameVIRTUAL_END}}

//{{TSDIDecFrameRSP_TBL_BEGIN}}
  protected:
    void CeViewlog(TCommandEnabler &tce);
    LRESULT ProcessMessage(WPARAM MsgType, LPARAM Id);
    void CeViewresponse(TCommandEnabler &tce);
    void CmViewlog();
    void CmViewresponse();
//{{TSDIDecFrameRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(TSDIDecFrame);
};    //{{TSDIDecFrame}}


//{{TApplication = TKDSTestApp}}
class TKDSTestApp : public TApplication {
  private:


  public:
    TKDSTestApp();
    virtual ~TKDSTestApp();

//{{TKDSTestAppVIRTUAL_BEGIN}}
  public:
    virtual void InitMainWindow();
//{{TKDSTestAppVIRTUAL_END}}

//{{TKDSTestAppRSP_TBL_BEGIN}}
  protected:
    void CmHelpAbout();
//{{TKDSTestAppRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(TKDSTestApp);
};    //{{TKDSTestApp}}


#endif  // kdstestapp_h sentry.

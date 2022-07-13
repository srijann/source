//----------------------------------------------------------------------------
//  Project PosFileMan
//  Secure Payment Systems
//  Copyright © 1997. All Rights Reserved.
//
//  SUBSYSTEM:    PosFileMan Application
//  FILE:         posfilemanapp.h
//  AUTHOR:       Jack Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for TPosFileManApp (TApplication).
//
//----------------------------------------------------------------------------
#if !defined(posfilemanapp_h)              // Sentry, use file only if it's not already included.
#define posfilemanapp_h

#include <owl/controlb.h>
#include <owl/docking.h>
#include <owl/opensave.h>

#include "PosfileManApp.rh"


//
// FrameWindow must be derived to override Paint for Preview and Print.
//
//{{TDecoratedFrame = TSDIDecFrame}}
class TSDIDecFrame : public TDecoratedFrame {
  public:
    TSDIDecFrame(TWindow* parent, const char far* title, TWindow* clientWnd, bool trackMenuSelection = false, TModule* module = 0);
    ~TSDIDecFrame();

//{{TPosFileManAppVIRTUAL_BEGIN}}
  public:
    virtual void SetupWindow();
//{{TPosFileManAppVIRTUAL_END}}
};    //{{TSDIDecFrame}}


//{{TApplication = TPosFileManApp}}
class TPosFileManApp : public TApplication {
  private:

    void SetupSpeedBar(TDecoratedFrame* frame);
    BankPhoneDialog* bankPhoneDialog;
    TPosFileManDlgClient* posFileDialog;
    FileEditDialog* currentDialog;


  public:
    TPosFileManApp();
    virtual ~TPosFileManApp();

    THarbor*        ApxHarbor;

//{{TPosFileManAppVIRTUAL_BEGIN}}
  public:
    virtual void InitMainWindow();
//{{TPosFileManAppVIRTUAL_END}}

//{{TPosFileManAppRSP_TBL_BEGIN}}
  protected:
    void CmHelpAbout();
    void CeViewbankphone(TCommandEnabler &tce);
    void CmViewbankphone();
    void CmViewpositive();
    void CeViewpositive(TCommandEnabler &tce);
    void CeGetrecord(TCommandEnabler &tce);
    void CeEditclearall(TCommandEnabler &tce);
    void CeFileadd(TCommandEnabler &tce);
    void CeFilechange(TCommandEnabler &tce);
    void CeFiledelete(TCommandEnabler &tce);
    void CmEditclearall();
    void CmFileadd();
    void CmFilechange();
    void CmFiledelete();
    void CmGetrecord();
//{{TPosFileManAppRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(TPosFileManApp);
};    //{{TPosFileManApp}}


#endif  // posfilemanapp_h sentry.

//----------------------------------------------------------------------------
//  Project LogSearch
//  Secure Payment Systems
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    LogSearch Application
//  FILE:         logsearchapp.h
//  AUTHOR:       Jack Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for TLogSearchApp (TApplication).
//
//----------------------------------------------------------------------------
#if !defined(logsearchapp_h)              // Sentry, use file only if it's not already included.
#define logsearchapp_h

#include <owl/controlb.h>
#include <owl/docking.h>
#include <owl/opensave.h>


#include "logsearchapp.rh"            // Definition of all resources.
#include "SearchInfo.h"
#include "PickListDialog.h"
#include "MainDisplayDialog.h"

//
// FrameWindow must be derived to override Paint for Preview and Print.
//
//{{TDecoratedFrame = TSDIDecFrame}}
class TSDIDecFrame : public TDecoratedFrame {
  public:
    TSDIDecFrame(TWindow* parent, const char far* title, TWindow* clientWnd, bool trackMenuSelection = false, TModule* module = 0);
    ~TSDIDecFrame();

//{{TLogSearchAppVIRTUAL_BEGIN}}
  public:
    virtual void SetupWindow();
//{{TLogSearchAppVIRTUAL_END}}
};    //{{TSDIDecFrame}}


//{{TApplication = TLogSearchApp}}
class TLogSearchApp : public TApplication {
  SEARCHINFO searchInfo;
  PickListDialog* PickList;
  MainDisplayDialog* DisplayDialog;
  uint MsgCount;
  uint CurrentMsgIndex;

  private:

    void SetupSpeedBar(TDecoratedFrame* frame);

  public:
    TLogSearchApp();
    virtual ~TLogSearchApp();

    THarbor*        ApxHarbor;

//{{TLogSearchAppVIRTUAL_BEGIN}}
  public:
    virtual void InitMainWindow();
//{{TLogSearchAppVIRTUAL_END}}

//{{TLogSearchAppRSP_TBL_BEGIN}}
  protected:
    void CmHelpAbout();
    void CeEditfind(TCommandEnabler &tce);
    void CeEditfindnext(TCommandEnabler &tce);
    void CeEditfindprev(TCommandEnabler &tce);
    void CeFileprint(TCommandEnabler &tce);
    void CmEditfind();
    void CePicklist(TCommandEnabler &tce);
    void CmPicklist();
    LRESULT ProcessAppNotify(WPARAM MsgType, LPARAM Id);
    void CmEditfindnext();
    void CmEditfindprev();
    void CmPrintoptions();
//{{TLogSearchAppRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(TLogSearchApp);
};    //{{TLogSearchApp}}


#endif  // logsearchapp_h sentry.

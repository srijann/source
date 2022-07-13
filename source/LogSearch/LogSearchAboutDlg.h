//----------------------------------------------------------------------------
//  Project LogSearch
//  Secure Payment Systems
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    LogSearch Application
//  FILE:         logsearchaboutdlg.h
//  AUTHOR:       Jack Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for TLogSearchAboutDlg (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(logsearchaboutdlg_h)              // Sentry, use file only if it's not already included.
#define logsearchaboutdlg_h

#include <owl/static.h>


#include <owl/commctrl.h>
#include "logsearchapp.rh"                  // Definition of all resources.


//{{TDialog = TLogSearchAboutDlg}}
class TLogSearchAboutDlg : public TDialog {
  public:
    TLogSearchAboutDlg(TWindow* parent, TResId resId = IDD_ABOUT, TModule* module = 0);
    virtual ~TLogSearchAboutDlg();

//{{TLogSearchAboutDlgVIRTUAL_BEGIN}}
  public:
    void SetupWindow();
//{{TLogSearchAboutDlgVIRTUAL_END}}

//{{TLogSearchAboutDlgRSP_TBL_BEGIN}}
  protected:
    void CeEditfindnext(TCommandEnabler &tce);
    void CeEditfindprev(TCommandEnabler &tce);
    void BNKillfocus();
//{{TLogSearchAboutDlgRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(TLogSearchAboutDlg);
};    //{{TLogSearchAboutDlg}}


// Reading the VERSIONINFO resource.
//
class TProjectRCVersion {
  public:
    TProjectRCVersion(TModule* module);
    virtual ~TProjectRCVersion();

    bool GetProductName(LPSTR& prodName);
    bool GetProductVersion(LPSTR& prodVersion);
    bool GetCopyright(LPSTR& copyright);
    bool GetDebug(LPSTR& debug);

  protected:
    uint8 far*  TransBlock;
    void far*   FVData;

  private:
    // Don't allow this object to be copied.
    //
    TProjectRCVersion(const TProjectRCVersion&);
    TProjectRCVersion& operator = (const TProjectRCVersion&);
};


#endif  // logsearchaboutdlg_h sentry.

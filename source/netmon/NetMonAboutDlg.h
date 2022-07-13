//----------------------------------------------------------------------------
//  Project NetMon
//  Secure Payment Systems
//  Copyright © 1997. All Rights Reserved.
//
//  SUBSYSTEM:    NetMon Application
//  FILE:         netmonaboutdlg.h
//  AUTHOR:       Jack H. Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for TNetMonAboutDlg (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(netmonaboutdlg_h)              // Sentry, use file only if it's not already included.
#define netmonaboutdlg_h

#include <owl/static.h>

#include "netmonapp.rh"                  // Definition of all resources.


//{{TDialog = TNetMonAboutDlg}}
class TNetMonAboutDlg : public TDialog {
  public:
    TNetMonAboutDlg(TWindow* parent, TResId resId = IDD_ABOUT, TModule* module = 0);
    virtual ~TNetMonAboutDlg();

//{{TNetMonAboutDlgVIRTUAL_BEGIN}}
  public:
    void SetupWindow();
//{{TNetMonAboutDlgVIRTUAL_END}}
};    //{{TNetMonAboutDlg}}


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


#endif  // netmonaboutdlg_h sentry.

//----------------------------------------------------------------------------
//  Project PosFileMan
//  Secure Payment Systems
//  Copyright © 1997. All Rights Reserved.
//
//  SUBSYSTEM:    PosFileMan Application
//  FILE:         posfilemanaboutdlg.h
//  AUTHOR:       Jack Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for TPosFileManAboutDlg (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(posfilemanaboutdlg_h)              // Sentry, use file only if it's not already included.
#define posfilemanaboutdlg_h

#include <owl/static.h>

#include "posfilemanapp.rh"                  // Definition of all resources.


//{{TDialog = TPosFileManAboutDlg}}
class TPosFileManAboutDlg : public TDialog {
  public:
    TPosFileManAboutDlg(TWindow* parent, TResId resId = IDD_ABOUT, TModule* module = 0);
    virtual ~TPosFileManAboutDlg();

//{{TPosFileManAboutDlgVIRTUAL_BEGIN}}
  public:
    void SetupWindow();
//{{TPosFileManAboutDlgVIRTUAL_END}}

//{{TPosFileManAboutDlgRSP_TBL_BEGIN}}
  protected:
//{{TPosFileManAboutDlgRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(TPosFileManAboutDlg);
};    //{{TPosFileManAboutDlg}}


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


#endif  // posfilemanaboutdlg_h sentry.

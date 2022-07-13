//----------------------------------------------------------------------------
//  Project SpsManager
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    SpsManager Application
//  FILE:         spsmanageraboutdlg.h
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for TSpsManagerAboutDlg (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(spsmanageraboutdlg_h)              // Sentry, use file only if it's not already included.
#define spsmanageraboutdlg_h

#include <owl/static.h>

#include "spsmanagerapp.rh"                  // Definition of all resources.


//{{TDialog = TSpsManagerAboutDlg}}
class TSpsManagerAboutDlg : public TDialog {
  public:
    TSpsManagerAboutDlg(TWindow* parent, TResId resId = IDD_ABOUT, TModule* module = 0);
    virtual ~TSpsManagerAboutDlg();

//{{TSpsManagerAboutDlgVIRTUAL_BEGIN}}
  public:
    void SetupWindow();
//{{TSpsManagerAboutDlgVIRTUAL_END}}
};    //{{TSpsManagerAboutDlg}}


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


#endif  // spsmanageraboutdlg_h sentry.

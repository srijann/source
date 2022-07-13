//----------------------------------------------------------------------------
//  Project KDSTest
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    KDSTest Application
//  FILE:         kdstestaboutdlg.h
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for TKDSTestAboutDlg (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(kdstestaboutdlg_h)              // Sentry, use file only if it's not already included.
#define kdstestaboutdlg_h

#include <owl/static.h>

#include "kdstestapp.rh"                  // Definition of all resources.


//{{TDialog = TKDSTestAboutDlg}}
class TKDSTestAboutDlg : public TDialog {
  public:
    TKDSTestAboutDlg(TWindow* parent, TResId resId = IDD_ABOUT, TModule* module = 0);
    virtual ~TKDSTestAboutDlg();

//{{TKDSTestAboutDlgVIRTUAL_BEGIN}}
  public:
    void SetupWindow();
//{{TKDSTestAboutDlgVIRTUAL_END}}
};    //{{TKDSTestAboutDlg}}


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


#endif  // kdstestaboutdlg_h sentry.

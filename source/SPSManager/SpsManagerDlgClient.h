//----------------------------------------------------------------------------
//  Project SpsManager
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    SpsManager Application
//  FILE:         spsmanagerdlgclient.h
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for TSpsManagerDlgClient (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(spsmanagerdlgclient_h)              // Sentry, use file only if it's not already included.
#define spsmanagerdlgclient_h

#include "spsmanagerapp.rh"                  // Definition of all resources.


//{{TDialog = TSpsManagerDlgClient}}
class TSpsManagerDlgClient : public TDialog {
  public:
    TSpsManagerDlgClient(TWindow* parent, TResId resId = IDD_CLIENT, TModule* module = 0);
    virtual ~TSpsManagerDlgClient();


//{{TSpsManagerDlgClientVIRTUAL_BEGIN}}
  public:
    virtual TResult EvCommand(uint id, THandle hWndCtl, uint notifyCode);
    virtual void SetupWindow();
//{{TSpsManagerDlgClientVIRTUAL_END}}

//{{TSpsManagerDlgClientRSP_TBL_BEGIN}}
  protected:
//{{TSpsManagerDlgClientRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(TSpsManagerDlgClient);
};    //{{TSpsManagerDlgClient}}


#endif  // spsmanagerdlgclient_h sentry.

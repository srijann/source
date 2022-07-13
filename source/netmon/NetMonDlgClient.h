//----------------------------------------------------------------------------
//  Project NetMon
//  Secure Payment Systems
//  Copyright © 1997. All Rights Reserved.
//
//  SUBSYSTEM:    NetMon Application
//  FILE:         netmondlgclient.h
//  AUTHOR:       Jack H. Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for TNetMonDlgClient (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(netmondlgclient_h)              // Sentry, use file only if it's not already included.
#define netmondlgclient_h

#include "netmonapp.rh"                  // Definition of all resources.


//{{TDialog = TNetMonDlgClient}}
class TNetMonDlgClient : public TDialog {
  public:
    TNetMonDlgClient(TWindow* parent, TResId resId = IDD_CLIENT, TModule* module = 0);
    virtual ~TNetMonDlgClient();


//{{TNetMonDlgClientVIRTUAL_BEGIN}}
  public:
    virtual TResult EvCommand(uint id, THandle hWndCtl, uint notifyCode);
//{{TNetMonDlgClientVIRTUAL_END}}
};    //{{TNetMonDlgClient}}


#endif  // netmondlgclient_h sentry.

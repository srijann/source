//----------------------------------------------------------------------------
//  Project LogSearch
//  Secure Payment Systems
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    LogSearch Application
//  FILE:         logsearchdlgclient.h
//  AUTHOR:       Jack Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for TLogSearchDlgClient (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(logsearchdlgclient_h)              // Sentry, use file only if it's not already included.
#define logsearchdlgclient_h

#include "logsearchapp.rh"                  // Definition of all resources.


//{{TDialog = TLogSearchDlgClient}}
class TLogSearchDlgClient : public TDialog {
  public:
    TLogSearchDlgClient(TWindow* parent, TResId resId = IDD_LOGSEARCHMAIN, TModule* module = 0);
    virtual ~TLogSearchDlgClient();


//{{TLogSearchDlgClientVIRTUAL_BEGIN}}
  public:
    virtual void SetupWindow();
//{{TLogSearchDlgClientVIRTUAL_END}}
};    //{{TLogSearchDlgClient}}


#endif  // logsearchdlgclient_h sentry.

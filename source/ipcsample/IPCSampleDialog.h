//----------------------------------------------------------------------------
//  Project IPCSample
//  JHE Software, Inc.
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    IPCSample Application
//  FILE:         IPCSample.h
//  AUTHOR:       Jack H. Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for TIPCSampleDlgClient (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(IPCSample_h)              // Sentry, use file only if it's not already included.
#define IPCSample_h

#include <owl/edit.h>
#include <owl/button.h>
#include <owl/radiobut.h>
#include "ipcmsg.h"
#include "IPCSample.rh"                 // Definition of all resources.



//{{TDialog = TIPCSampleDlgClient}}
class TIPCSampleDlgClient : public TDialog, IPCMessage {
  TEdit* MsgText;
  TEdit* MsgReply;
  TEdit* ProcessName;
  TEdit* AltProcessName;
  TEdit* DebugText;
  TButton* SendButton;
  TRadioButton* SyncButton;
  TRadioButton* ASyncButton;
  void Waiting(bool);

  public:
    TIPCSampleDlgClient(TWindow* parent, TResId resId = IDD_CLIENT, TModule* module = 0);
    virtual ~TIPCSampleDlgClient();
    LRESULT ProcessMessage(WPARAM w, LPARAM lParam);

//{{TIPCSampleDlgClientVIRTUAL_BEGIN}}
  public:
    virtual void SetupWindow();
//{{TIPCSampleDlgClientVIRTUAL_END}}

//{{TIPCSampleDlgClientRSP_TBL_BEGIN}}
  protected:
  void BNClicked();
//{{TIPCSampleDlgClientRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(TIPCSampleDlgClient);
};    //{{TIPCSampleDlgClient}}


#endif  // IPCSample_h sentry.

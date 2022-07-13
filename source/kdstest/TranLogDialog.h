//----------------------------------------------------------------------------
//  Project TranLog
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    TranLog Application
//  FILE:         trnlgdlc.h
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for TranLogDialog (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(trnlgdlc_h)              // Sentry, use file only if it's not already included.
#define trnlgdlc_h

#include "TranLog.rh"

#include <owl/commctrl.h>
#include "Dcmanage.h"

#define NUMROWS 16

//{{TDialog = TranLogDialog}}
class TranLogDialog : public TDialog {
  TEdit* BVSTATUS[NUMROWS];
  TEdit* BVDATE[NUMROWS];
  TEdit* BAL[NUMROWS];
  TEdit* APP[NUMROWS];
  TEdit* RC[NUMROWS];
  TEdit* PHONE[NUMROWS];
  TEdit* SSN[NUMROWS];
  TEdit* ACCT[NUMROWS];
  TEdit* ABA[NUMROWS];
  TEdit* LICENSE[NUMROWS];
  TEdit* STATE[NUMROWS];
  TEdit* CHECKNUM[NUMROWS];
  TEdit* AMOUNT[NUMROWS];
  TEdit* MERCH[NUMROWS];

  DialogControlManager* DCMan;
  void copy(TEdit* dest,TEdit* source);
  void scroll();

  public:
    TranLogDialog(TWindow* parent, TResId resId = IDD_TRANLOG, TModule* module = 0);
    virtual ~TranLogDialog();
    void ProcessInqMessage(IPCMessage& Msg);
    void ProcessReplyMessage(IPCMessage& Msg);
};    //{{TranLogDialog}}


#endif  // trnlgdlc_h sentry.

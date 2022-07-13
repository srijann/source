//----------------------------------------------------------------------------
//  Project LogSearch
//  Secure Payment Systems
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    LogSearch.apx Application
//  FILE:         holdcheckdialog.h
//  AUTHOR:       Jack Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for HoldCheckDialog (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(holdcheckdialog_h)              // Sentry, use file only if it's not already included.
#define holdcheckdialog_h

#include <owl/dialog.h>
#include <owl/static.h>

#include "HoldCheckDialog.rh"
#include "IPCTransfer.h"


//{{TDialog = HoldCheckDialog}}
class HoldCheckDialog : public TDialog {
  TStatic* HCVEHICLEVALUE;
  TStatic* HCAMOUNT1;
  TStatic* HCCHECKNUM1;
  TStatic* HCAPPNUM4;
  TStatic* HCAPPNUM3;
  TStatic* HCAPPNUM2;
  TStatic* HCAPPNUM1;
  TStatic* HCAMOUNT2;
  TStatic* HCCHECKNUM2;
  TStatic* HCAMOUNT3;
  TStatic* HCCHECKNUM3;
  TStatic* HCAMOUNT4;
  TStatic* HCCHECKNUM4;
  TStatic* HCSUM;
  IPCTransfer Transfer;
  uint getAmount(IPCMessage* Msg,FLDNUM Fn);
  public:
    HoldCheckDialog(TWindow* parent);
    virtual ~HoldCheckDialog();
    void Display(IPCMessage*);
};    //{{HoldCheckDialog}}


#endif  // holdcheckdialog_h sentry.


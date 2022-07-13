//----------------------------------------------------------------------------
//  Project LogSearch
//  Secure Payment Systems
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    LogSearch.apx Application
//  FILE:         rulesdialog.h
//  AUTHOR:       Jack Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for Rules2Dialog (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(rules2dialog_h)              // Sentry, use file only if it's not already included.
#define rules2dialog_h

#include <owl/dialog.h>
#include <owl/static.h>

#include "AuthRulesDialog.rh"

//{{TDialog = Rules2Dialog}}
class Rules2Dialog : public TDialog {
  TStatic* R7DailyNum;
  TStatic* R7DailyAmt;
  TStatic* R7Per1Days;
  TStatic* R7Per2Days;
  TStatic* R7Per3Days;
  TStatic* R7Per1Num;
  TStatic* R7Per2Num;
  TStatic* R7Per3Num;
  TStatic* R7Per1Amt;
  TStatic* R7Per2Amt;
  TStatic* R7Per3Amt;
  TStatic* R8Amt;
  TStatic* R8Accum;

  void clearAll();
  void numOut(TStatic* Ctl,int Val);
  void convert(IPCMessage*);
  struct AuthRulesRecord Rules;

  public:
    Rules2Dialog(TWindow* parent);
    virtual ~Rules2Dialog();
    void Display(IPCMessage*);
};    //{{Rules2Dialog}}


#endif  // rulesdialog_h sentry.


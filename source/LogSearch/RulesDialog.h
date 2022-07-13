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
//  Class definition for RulesDialog (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(rulesdialog_h)              // Sentry, use file only if it's not already included.
#define rulesdialog_h

#include <owl/dialog.h>
#include <owl/static.h>


#include "AuthRulesDialog.rh"

//{{TDialog = RulesDialog}}
class RulesDialog : public TDialog {
  TStatic* CountableAmt;
  TStatic* CountableAccum;
  TStatic* R1OvrdAmt2;
  TStatic* R1OvrdAmt1;
  TStatic* R1OvrdAge2;
  TStatic* R1OvrdAge1;
  TStatic* R1ReferAmt2;
  TStatic* R1ReferAge2;
  TStatic* R1ReferAmt1;
  TStatic* R1ReferAge1;
  TStatic* R2Sic1Trans;
  TStatic* R2Sic1Days;
  TStatic* R2Sic1Amt;
  TStatic* R2Sic2Trans;
  TStatic* R2Sic2Amt;
  TStatic* R2Acct1Amt;
  TStatic* R2Acct2Amt;
  TStatic* R2Loc1Amt;
  TStatic* R2Loc2Amt;
  TStatic* R2Sic2Days;
  TStatic* R2Loc2Days;
  TStatic* R2Loc2Trans;
  TStatic* R2Loc1Days;
  TStatic* R2Loc1Trans;
  TStatic* R2Acct2Days;
  TStatic* R2Acct2Trans;
  TStatic* R2Acct1Trans;
  TStatic* R2Acct1Days;

  TStatic* R3AgeReferAge1;
  TStatic* R3AgeReferAge2;
  TStatic* R3AgeReferAge3;
  TStatic* R3AgeReferAge4;
  TStatic* R3AgeReferNum1From;
  TStatic* R3AgeReferNum1To;
  TStatic* R3AgeReferNum2From;
  TStatic* R3AgeReferNum2To;
  TStatic* R3AgeReferNum3From;
  TStatic* R3AgeReferNum3To;
  TStatic* R3AgeReferNum4From;
  TStatic* R3AgeReferNum4To;
  TStatic* R3AgeReferAmt1From;
  TStatic* R3AgeReferAmt1To;
  TStatic* R3AgeReferAmt2From;
  TStatic* R3AgeReferAmt2To;
  TStatic* R3AgeReferAmt3From;
  TStatic* R3AgeReferAmt3To;
  TStatic* R3AgeReferAmt4From;
  TStatic* R3AgeReferAmt4To;
  TStatic* R3ReferNum1From;
  TStatic* R3ReferNum1To;
  TStatic* R3ReferAmt1From;
  TStatic* R3ReferAmt1To;
  TStatic* R3ReferNum2From;
  TStatic* R3ReferNum2To;
  TStatic* R3ReferAmt2From;
  TStatic* R3ReferAmt2To;
  TStatic* R3ReferNum3From;
  TStatic* R3ReferNum3To;
  TStatic* R3ReferAmt3From;
  TStatic* R3ReferAmt3To;
  TStatic* R3ReferNum4From;
  TStatic* R3ReferNum4To;
  TStatic* R3ReferAmt4From;
  TStatic* R3ReferAmt4To;
  TStatic* R3AgeOvrdAge1;
  TStatic* R3AgeOvrdNum1From;
  TStatic* R3AgeOvrdNum1To;
  TStatic* R3AgeOvrdAmt1From;
  TStatic* R3AgeOvrdAmt1To;
  TStatic* R3AgeOvrdAge2;
  TStatic* R3AgeOvrdNum2From;
  TStatic* R3AgeOvrdNum2To;
  TStatic* R3AgeOvrdAmt2From;
  TStatic* R3AgeOvrdAmt2To;
  TStatic* R3AgeOvrdAge3;
  TStatic* R3AgeOvrdNum3From;
  TStatic* R3AgeOvrdNum3To;
  TStatic* R3AgeOvrdAmt3From;
  TStatic* R3AgeOvrdAmt3To;
  TStatic* R3AgeOvrdAge4;
  TStatic* R3AgeOvrdNum4From;
  TStatic* R3AgeOvrdNum4To;
  TStatic* R3AgeOvrdAmt4From;
  TStatic* R3AgeOvrdAmt4To;
  TStatic* R3OvrdNum1From;
  TStatic* R3OvrdNum1To;
  TStatic* R3OvrdAmt1From;
  TStatic* R3OvrdAmt1To;
  TStatic* R3OvrdNum2From;
  TStatic* R3OvrdNum2To;
  TStatic* R3OvrdAmt2From;
  TStatic* R3OvrdAmt2To;
  TStatic* R3OvrdNum3From;
  TStatic* R3OvrdNum3To;
  TStatic* R3OvrdAmt3From;
  TStatic* R3OvrdAmt3To;
  TStatic* R3OvrdNum4From;
  TStatic* R3OvrdNum4To;
  TStatic* R3OvrdAmt4From;
  TStatic* R3OvrdAmt4To;

  TStatic* R4BackDays;
  TStatic* R4BackGap;
  TStatic* R4FwdDays3;
  TStatic* R4FwdDays2;
  TStatic* R4FwdDays1;
  TStatic* R4FwdGap3;
  TStatic* R4FwdGap2;
  TStatic* R4FwdGap1;
  TStatic* R5Amt;
  TStatic* R6Amt;
  TStatic* R6Accum;

  void clearAll();
  void numOut(TStatic* Ctl,int Val);
  struct AuthRulesRecord Rules;
  void convert(IPCMessage*);
  public:
    RulesDialog(TWindow* parent);
    virtual ~RulesDialog();
    void Display(IPCMessage*);
};    //{{RulesDialog}}


#endif  // rulesdialog_h sentry.


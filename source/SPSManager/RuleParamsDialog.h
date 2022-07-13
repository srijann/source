//----------------------------------------------------------------------------
//  Project spsmanager
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    spsmanager.apx Application
//  FILE:         ruleparamsdialog.h
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for RuleParamsDialog (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(ruleparamsdialog_h)              // Sentry, use file only if it's not already included.
#define ruleparamsdialog_h

#include <owl/dialog.h>
#include "LevelSelect.h"
#include "AuthRulesDialog.rh"
#include "AuthRules.h"
#include "FormDialog.h"
#include "EditUns.h"

//{{TDialog = RuleParamsDialog}}
class RuleParamsDialog : public FormDialog {
  AuthRules*  workingRules;
  AuthRules*  globalRules;
  AuthRules*  classRules;
  AuthRules*  subClassRules;

  unsigned char workingLevel;

  bool SetupComplete;
  void ChangeHandler(uint Id);
  AuthRules* RuleSetSelect(BYTE* Enabler,bool& rdonly,bool&hilite);
  void GroupOut(int Id);

  TLevelSelect* CountableEnable;
  TEditUns* CountableAmt;
  TEditUns* CountableAccum;
  TLevelSelect* R1Enable;
  TLevelSelect* R1DateTime;
  TEditUns* R1OvrdAmt2;
  TEditUns* R1OvrdAmt1;
  TEditUns* R1OvrdAge2;
  TEditUns* R1OvrdAge1;
  TLevelSelect* R1Product;
  TEditUns* R1ReferAmt2;
  TEditUns* R1ReferAge2;
  TEditUns* R1ReferAmt1;
  TEditUns* R1ReferAge1;
  TLevelSelect* R2Product;
  TLevelSelect* R2DateTime;
  TLevelSelect* R2Enable;
  TEditUns* R2Sic1Trans;
  TEditUns* R2Sic1Days;
  TEditUns* R2Sic1Amt;
  TEditUns* R2Sic2Trans;
  TEditUns* R2Sic2Amt;
  TEditUns* R2Acct1Amt;
  TEditUns* R2Acct2Amt;
  TEditUns* R2Loc1Amt;
  TEditUns* R2Loc2Amt;
  TEditUns* R2Sic2Days;
  TEditUns* R2Loc2Days;
  TEditUns* R2Loc2Trans;
  TEditUns* R2Loc1Days;
  TEditUns* R2Loc1Trans;
  TEditUns* R2Acct2Days;
  TEditUns* R2Acct2Trans;
  TEditUns* R2Acct1Trans;
  TEditUns* R2Acct1Days;
  TLevelSelect* R3Product;
  TLevelSelect* R3DateTime;
  TLevelSelect* R3Enable;

  TEditUns* R3AgeReferAge1;
  TEditUns* R3AgeReferAge2;
  TEditUns* R3AgeReferAge3;
  TEditUns* R3AgeReferAge4;
  TEditUns* R3AgeReferNum1From;
  TEditUns* R3AgeReferNum1To;
  TEditUns* R3AgeReferNum2From;
  TEditUns* R3AgeReferNum2To;
  TEditUns* R3AgeReferNum3From;
  TEditUns* R3AgeReferNum3To;
  TEditUns* R3AgeReferNum4From;
  TEditUns* R3AgeReferNum4To;
  TEditUns* R3AgeReferAmt1From;
  TEditUns* R3AgeReferAmt1To;
  TEditUns* R3AgeReferAmt2From;
  TEditUns* R3AgeReferAmt2To;
  TEditUns* R3AgeReferAmt3From;
  TEditUns* R3AgeReferAmt3To;
  TEditUns* R3AgeReferAmt4From;
  TEditUns* R3AgeReferAmt4To;
  TEditUns* R3ReferNum1From;
  TEditUns* R3ReferNum1To;
  TEditUns* R3ReferAmt1From;
  TEditUns* R3ReferAmt1To;
  TEditUns* R3ReferNum2From;
  TEditUns* R3ReferNum2To;
  TEditUns* R3ReferAmt2From;
  TEditUns* R3ReferAmt2To;
  TEditUns* R3ReferNum3From;
  TEditUns* R3ReferNum3To;
  TEditUns* R3ReferAmt3From;
  TEditUns* R3ReferAmt3To;
  TEditUns* R3ReferNum4From;
  TEditUns* R3ReferNum4To;
  TEditUns* R3ReferAmt4From;
  TEditUns* R3ReferAmt4To;
  TEditUns* R3AgeOvrdAge1;
  TEditUns* R3AgeOvrdNum1From;
  TEditUns* R3AgeOvrdNum1To;
  TEditUns* R3AgeOvrdAmt1From;
  TEditUns* R3AgeOvrdAmt1To;
  TEditUns* R3AgeOvrdAge2;
  TEditUns* R3AgeOvrdNum2From;
  TEditUns* R3AgeOvrdNum2To;
  TEditUns* R3AgeOvrdAmt2From;
  TEditUns* R3AgeOvrdAmt2To;
  TEditUns* R3AgeOvrdAge3;
  TEditUns* R3AgeOvrdNum3From;
  TEditUns* R3AgeOvrdNum3To;
  TEditUns* R3AgeOvrdAmt3From;
  TEditUns* R3AgeOvrdAmt3To;
  TEditUns* R3AgeOvrdAge4;
  TEditUns* R3AgeOvrdNum4From;
  TEditUns* R3AgeOvrdNum4To;
  TEditUns* R3AgeOvrdAmt4From;
  TEditUns* R3AgeOvrdAmt4To;
  TEditUns* R3OvrdNum1From;
  TEditUns* R3OvrdNum1To;
  TEditUns* R3OvrdAmt1From;
  TEditUns* R3OvrdAmt1To;
  TEditUns* R3OvrdNum2From;
  TEditUns* R3OvrdNum2To;
  TEditUns* R3OvrdAmt2From;
  TEditUns* R3OvrdAmt2To;
  TEditUns* R3OvrdNum3From;
  TEditUns* R3OvrdNum3To;
  TEditUns* R3OvrdAmt3From;
  TEditUns* R3OvrdAmt3To;
  TEditUns* R3OvrdNum4From;
  TEditUns* R3OvrdNum4To;
  TEditUns* R3OvrdAmt4From;
  TEditUns* R3OvrdAmt4To;
  TEditUns* R4BackDays;
  TEditUns* R4BackGap;
  TEditUns* R4FwdDays3;
  TEditUns* R4FwdDays2;
  TEditUns* R4FwdDays1;
  TEditUns* R4FwdGap3;
  TEditUns* R4FwdGap2;
  TEditUns* R4FwdGap1;
  TLevelSelect* R4Product;
  TLevelSelect* R4DateTime;
  TLevelSelect* R4Enable;
  TLevelSelect* R5Enable;
  TLevelSelect* R5DateTime;
  TLevelSelect* R5Product;
  TEditUns* R5Amt;
  TLevelSelect* R6Enable;
  TLevelSelect* R6DateTime;
  TLevelSelect* R6Product;
  TEditUns* R6Amt;
  TEditUns* R6Accum;

  bool Validate();

  public:
    RuleParamsDialog(TWindow*  parent,AuthRules*  GlobalRules,
                  AuthRules*  ClassRules=0,AuthRules*  SubClassRules=0);
    virtual ~RuleParamsDialog();

//{{RuleParamsDialogVIRTUAL_BEGIN}}
  public:
    virtual void SetupWindow();
    virtual TResult EvCommand(uint id, THandle hWndCtl, uint notifyCode);
//{{RuleParamsDialogVIRTUAL_END}}

//{{RuleParamsDialogRSP_TBL_BEGIN}}
  protected:
//{{RuleParamsDialogRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(RuleParamsDialog);
};    //{{RuleParamsDialog}}


#endif  // ruleparamsdialog_h sentry.




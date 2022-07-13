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
#if !defined(ruleparams2dialog_h)              // Sentry, use file only if it's not already included.
#define ruleparams2dialog_h

#include <owl/dialog.h>
#include "LevelSelect.h"
#include "AuthRulesDialog.rh"
#include "AuthRules.h"
#include "FormDialog.h"
#include "EditUns.h"

//{{TDialog = RuleParams2Dialog}}
class RuleParams2Dialog : public FormDialog {
  AuthRules*  workingRules;
  AuthRules*  globalRules;
  AuthRules*  classRules;
  AuthRules*  subClassRules;

  unsigned char workingLevel;

  bool SetupComplete;
  void ChangeHandler(uint Id);
  AuthRules* RuleSetSelect(BYTE* Enabler,bool& rdonly,bool&hilite);
  void GroupOut(int Id);

  TLevelSelect* R7Enable;
  TLevelSelect* R7DateTime;
  TLevelSelect* R7Product;
  TEditUns* R7DailyNum;
  TEditUns* R7DailyAmt;
  TEditUns* R7Per1Days;
  TEditUns* R7Per2Days;
  TEditUns* R7Per3Days;
  TEditUns* R7Per1Num;
  TEditUns* R7Per2Num;
  TEditUns* R7Per3Num;
  TEditUns* R7Per1Amt;
  TEditUns* R7Per2Amt;
  TEditUns* R7Per3Amt;
  TLevelSelect* R8Enable;
  TLevelSelect* R8DateTime;
  TLevelSelect* R8Product;
  TEditUns* R8Amt;
  TEditUns* R8Accum;

  bool Validate();

  public:
    RuleParams2Dialog(TWindow*  parent,AuthRules*  GlobalRules,
                  AuthRules*  ClassRules=0,AuthRules*  SubClassRules=0);
    virtual ~RuleParams2Dialog();

//{{RuleParams2DialogVIRTUAL_BEGIN}}
  public:
    virtual void SetupWindow();
    virtual TResult EvCommand(uint id, THandle hWndCtl, uint notifyCode);
//{{RuleParams2DialogVIRTUAL_END}}

//{{RuleParams2DialogRSP_TBL_BEGIN}}
  protected:
//{{RuleParams2DialogRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(RuleParams2Dialog);
};    //{{RuleParams2Dialog}}


#endif  // ruleparams2dialog_h sentry.




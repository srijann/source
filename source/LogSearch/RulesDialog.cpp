//----------------------------------------------------------------------------
//  Project LogSearch
//  Secure Payment Systems
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    LogSearch.apx Application
//  FILE:         rulesdialog.cpp
//  AUTHOR:       Jack Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of RulesDialog (TDialog).
//
//----------------------------------------------------------------------------
#include "win4me.h"

#include <stdio.h>
#include "SpsManDB.h"
#include "OldSPSManDB.h"
#include "rulesdialog.h"
#include "CheckAuthFN.h"
#include "unsnum.h"


//{{RulesDialog Implementation}}

#define STATIC(name) name = new TStatic(this,IDC_##name)

RulesDialog::RulesDialog(TWindow* parent)
:
    TDialog(parent, IDD_RULES)
{
  STATIC(CountableAmt);
  STATIC(CountableAccum);
  STATIC(R1OvrdAmt2);
  STATIC(R1OvrdAmt1);
  STATIC(R1OvrdAge2);
  STATIC(R1OvrdAge1);
  STATIC(R1ReferAmt2);
  STATIC(R1ReferAge2);
  STATIC(R1ReferAmt1);
  STATIC(R1ReferAge1);
  STATIC(R2Sic1Trans);
  STATIC(R2Sic1Days);
  STATIC(R2Sic1Amt);
  STATIC(R2Sic2Trans);
  STATIC(R2Sic2Amt);
  STATIC(R2Acct1Amt);
  STATIC(R2Acct2Amt);
  STATIC(R2Loc1Amt);
  STATIC(R2Loc2Amt);
  STATIC(R2Sic2Days);
  STATIC(R2Loc2Days);
  STATIC(R2Loc2Trans);
  STATIC(R2Loc1Days);
  STATIC(R2Loc1Trans);
  STATIC(R2Acct2Days);
  STATIC(R2Acct2Trans);
  STATIC(R2Acct1Trans);
  STATIC(R2Acct1Days);

  STATIC(R3AgeReferAge1);
  STATIC(R3AgeReferAge2);
  STATIC(R3AgeReferAge3);
  STATIC(R3AgeReferAge4);
  STATIC(R3AgeReferNum1From);
  STATIC(R3AgeReferNum1To);
  STATIC(R3AgeReferNum2From);
  STATIC(R3AgeReferNum2To);
  STATIC(R3AgeReferNum3From);
  STATIC(R3AgeReferNum3To);
  STATIC(R3AgeReferNum4From);
  STATIC(R3AgeReferNum4To);
  STATIC(R3AgeReferAmt1From);
  STATIC(R3AgeReferAmt1To);
  STATIC(R3AgeReferAmt2From);
  STATIC(R3AgeReferAmt2To);
  STATIC(R3AgeReferAmt3From);
  STATIC(R3AgeReferAmt3To);
  STATIC(R3AgeReferAmt4From);
  STATIC(R3AgeReferAmt4To);
  STATIC(R3ReferNum1From);
  STATIC(R3ReferNum1To);
  STATIC(R3ReferAmt1From);
  STATIC(R3ReferAmt1To);
  STATIC(R3ReferNum2From);
  STATIC(R3ReferNum2To);
  STATIC(R3ReferAmt2From);
  STATIC(R3ReferAmt2To);
  STATIC(R3ReferNum3From);
  STATIC(R3ReferNum3To);
  STATIC(R3ReferAmt3From);
  STATIC(R3ReferAmt3To);
  STATIC(R3ReferNum4From);
  STATIC(R3ReferNum4To);
  STATIC(R3ReferAmt4From);
  STATIC(R3ReferAmt4To);
  STATIC(R3AgeOvrdAge1);
  STATIC(R3AgeOvrdNum1From);
  STATIC(R3AgeOvrdNum1To);
  STATIC(R3AgeOvrdAmt1From);
  STATIC(R3AgeOvrdAmt1To);
  STATIC(R3AgeOvrdAge2);
  STATIC(R3AgeOvrdNum2From);
  STATIC(R3AgeOvrdNum2To);
  STATIC(R3AgeOvrdAmt2From);
  STATIC(R3AgeOvrdAmt2To);
  STATIC(R3AgeOvrdAge3);
  STATIC(R3AgeOvrdNum3From);
  STATIC(R3AgeOvrdNum3To);
  STATIC(R3AgeOvrdAmt3From);
  STATIC(R3AgeOvrdAmt3To);
  STATIC(R3AgeOvrdAge4);
  STATIC(R3AgeOvrdNum4From);
  STATIC(R3AgeOvrdNum4To);
  STATIC(R3AgeOvrdAmt4From);
  STATIC(R3AgeOvrdAmt4To);
  STATIC(R3OvrdNum1From);
  STATIC(R3OvrdNum1To);
  STATIC(R3OvrdAmt1From);
  STATIC(R3OvrdAmt1To);
  STATIC(R3OvrdNum2From);
  STATIC(R3OvrdNum2To);
  STATIC(R3OvrdAmt2From);
  STATIC(R3OvrdAmt2To);
  STATIC(R3OvrdNum3From);
  STATIC(R3OvrdNum3To);
  STATIC(R3OvrdAmt3From);
  STATIC(R3OvrdAmt3To);
  STATIC(R3OvrdNum4From);
  STATIC(R3OvrdNum4To);
  STATIC(R3OvrdAmt4From);
  STATIC(R3OvrdAmt4To);

  STATIC(R4BackDays);
  STATIC(R4BackGap);
  STATIC(R4FwdDays3);
  STATIC(R4FwdDays2);
  STATIC(R4FwdDays1);
  STATIC(R4FwdGap3);
  STATIC(R4FwdGap2);
  STATIC(R4FwdGap1);
  STATIC(R5Amt);
  STATIC(R6Amt);
  STATIC(R6Accum);
}


RulesDialog::~RulesDialog()
{
  Destroy(IDCANCEL);
}

#define TRANSFER(rule,name) if (isEmpty(Rules.rule.rule##name) ||\
                                Rules.rule.rule##Enable==' ' )\
                              rule##name->Clear();\
                            else\
                              numOut(rule##name,Rules.rule.rule##name)

void RulesDialog::Display(IPCMessage* Msg)
{
 int Len;

 clearAll();

 if ( ! Msg->GetFld(FN_AUTHRULES,&Rules,Len) )
   return;

 if ( Len == sizeof(struct OldRulesRecord) )
  convert(Msg);
 else
 if ( Len != sizeof(Rules) )
  return;

 TRANSFER(Countable,Amt);
 TRANSFER(Countable,Accum);
 TRANSFER(R1,OvrdAmt2);
 TRANSFER(R1,OvrdAmt1);
 TRANSFER(R1,OvrdAge2);
 TRANSFER(R1,OvrdAge1);
 TRANSFER(R1,ReferAmt2);
 TRANSFER(R1,ReferAge2);
 TRANSFER(R1,ReferAmt1);
 TRANSFER(R1,ReferAge1);
 TRANSFER(R2,Sic1Trans);
 TRANSFER(R2,Sic1Days);
 TRANSFER(R2,Sic1Amt);
 TRANSFER(R2,Sic2Trans);
 TRANSFER(R2,Sic2Amt);
 TRANSFER(R2,Acct1Amt);
 TRANSFER(R2,Acct2Amt);
 TRANSFER(R2,Loc1Amt);
 TRANSFER(R2,Loc2Amt);
 TRANSFER(R2,Sic2Days);
 TRANSFER(R2,Loc2Days);
 TRANSFER(R2,Loc2Trans);
 TRANSFER(R2,Loc1Days);
 TRANSFER(R2,Loc1Trans);
 TRANSFER(R2,Acct2Days);
 TRANSFER(R2,Acct2Trans);
 TRANSFER(R2,Acct1Trans);
 TRANSFER(R2,Acct1Days);
 TRANSFER(R3,AgeReferAge1);
 TRANSFER(R3,AgeReferAge2);
 TRANSFER(R3,AgeReferAge3);
 TRANSFER(R3,AgeReferAge4);
 TRANSFER(R3,AgeReferNum1From);
 TRANSFER(R3,AgeReferNum1To);
 TRANSFER(R3,AgeReferNum2From);
 TRANSFER(R3,AgeReferNum2To);
 TRANSFER(R3,AgeReferNum3From);
 TRANSFER(R3,AgeReferNum3To);
 TRANSFER(R3,AgeReferNum4From);
 TRANSFER(R3,AgeReferNum4To);
 TRANSFER(R3,AgeReferAmt1From);
 TRANSFER(R3,AgeReferAmt1To);
 TRANSFER(R3,AgeReferAmt2From);
 TRANSFER(R3,AgeReferAmt2To);
 TRANSFER(R3,AgeReferAmt3From);
 TRANSFER(R3,AgeReferAmt3To);
 TRANSFER(R3,AgeReferAmt4From);
 TRANSFER(R3,AgeReferAmt4To);
 TRANSFER(R3,ReferNum1From);
 TRANSFER(R3,ReferNum1To);
 TRANSFER(R3,ReferAmt1From);
 TRANSFER(R3,ReferAmt1To);
 TRANSFER(R3,ReferNum2From);
 TRANSFER(R3,ReferNum2To);
 TRANSFER(R3,ReferAmt2From);
 TRANSFER(R3,ReferAmt2To);
 TRANSFER(R3,ReferNum3From);
 TRANSFER(R3,ReferNum3To);
 TRANSFER(R3,ReferAmt3From);
 TRANSFER(R3,ReferAmt3To);
 TRANSFER(R3,ReferNum4From);
 TRANSFER(R3,ReferNum4To);
 TRANSFER(R3,ReferAmt4From);
 TRANSFER(R3,ReferAmt4To);
 TRANSFER(R3,AgeOvrdAge1);
 TRANSFER(R3,AgeOvrdNum1From);
 TRANSFER(R3,AgeOvrdNum1To);
 TRANSFER(R3,AgeOvrdAmt1From);
 TRANSFER(R3,AgeOvrdAmt1To);
 TRANSFER(R3,AgeOvrdAge2);
 TRANSFER(R3,AgeOvrdNum2From);
 TRANSFER(R3,AgeOvrdNum2To);
 TRANSFER(R3,AgeOvrdAmt2From);
 TRANSFER(R3,AgeOvrdAmt2To);
 TRANSFER(R3,AgeOvrdAge3);
 TRANSFER(R3,AgeOvrdNum3From);
 TRANSFER(R3,AgeOvrdNum3To);
 TRANSFER(R3,AgeOvrdAmt3From);
 TRANSFER(R3,AgeOvrdAmt3To);
 TRANSFER(R3,AgeOvrdAge4);
 TRANSFER(R3,AgeOvrdNum4From);
 TRANSFER(R3,AgeOvrdNum4To);
 TRANSFER(R3,AgeOvrdAmt4From);
 TRANSFER(R3,AgeOvrdAmt4To);
 TRANSFER(R3,OvrdNum1From);
 TRANSFER(R3,OvrdNum1To);
 TRANSFER(R3,OvrdAmt1From);
 TRANSFER(R3,OvrdAmt1To);
 TRANSFER(R3,OvrdNum2From);
 TRANSFER(R3,OvrdNum2To);
 TRANSFER(R3,OvrdAmt2From);
 TRANSFER(R3,OvrdAmt2To);
 TRANSFER(R3,OvrdNum3From);
 TRANSFER(R3,OvrdNum3To);
 TRANSFER(R3,OvrdAmt3From);
 TRANSFER(R3,OvrdAmt3To);
 TRANSFER(R3,OvrdNum4From);
 TRANSFER(R3,OvrdNum4To);
 TRANSFER(R3,OvrdAmt4From);
 TRANSFER(R3,OvrdAmt4To);
 TRANSFER(R4,BackDays);
 TRANSFER(R4,BackGap);
 TRANSFER(R4,FwdDays3);
 TRANSFER(R4,FwdDays2);
 TRANSFER(R4,FwdDays1);
 TRANSFER(R4,FwdGap3);
 TRANSFER(R4,FwdGap2);
 TRANSFER(R4,FwdGap1);
 TRANSFER(R5,Amt);
 TRANSFER(R6,Amt);
 TRANSFER(R6,Accum);
}

#define CLEAR(name) name->Clear()
void RulesDialog::clearAll()
{
  CLEAR(CountableAmt);
  CLEAR(CountableAccum);
  CLEAR(R1OvrdAmt2);
  CLEAR(R1OvrdAmt1);
  CLEAR(R1OvrdAge2);
  CLEAR(R1OvrdAge1);
  CLEAR(R1ReferAmt2);
  CLEAR(R1ReferAge2);
  CLEAR(R1ReferAmt1);
  CLEAR(R1ReferAge1);
  CLEAR(R2Sic1Trans);
  CLEAR(R2Sic1Days);
  CLEAR(R2Sic1Amt);
  CLEAR(R2Sic2Trans);
  CLEAR(R2Sic2Amt);
  CLEAR(R2Acct1Amt);
  CLEAR(R2Acct2Amt);
  CLEAR(R2Loc1Amt);
  CLEAR(R2Loc2Amt);
  CLEAR(R2Sic2Days);
  CLEAR(R2Loc2Days);
  CLEAR(R2Loc2Trans);
  CLEAR(R2Loc1Days);
  CLEAR(R2Loc1Trans);
  CLEAR(R2Acct2Days);
  CLEAR(R2Acct2Trans);
  CLEAR(R2Acct1Trans);
  CLEAR(R2Acct1Days);
  CLEAR(R3AgeReferAge1);
  CLEAR(R3AgeReferAge2);
  CLEAR(R3AgeReferAge3);
  CLEAR(R3AgeReferAge4);
  CLEAR(R3AgeReferNum1From);
  CLEAR(R3AgeReferNum1To);
  CLEAR(R3AgeReferNum2From);
  CLEAR(R3AgeReferNum2To);
  CLEAR(R3AgeReferNum3From);
  CLEAR(R3AgeReferNum3To);
  CLEAR(R3AgeReferNum4From);
  CLEAR(R3AgeReferNum4To);
  CLEAR(R3AgeReferAmt1From);
  CLEAR(R3AgeReferAmt1To);
  CLEAR(R3AgeReferAmt2From);
  CLEAR(R3AgeReferAmt2To);
  CLEAR(R3AgeReferAmt3From);
  CLEAR(R3AgeReferAmt3To);
  CLEAR(R3AgeReferAmt4From);
  CLEAR(R3AgeReferAmt4To);
  CLEAR(R3ReferNum1From);
  CLEAR(R3ReferNum1To);
  CLEAR(R3ReferAmt1From);
  CLEAR(R3ReferAmt1To);
  CLEAR(R3ReferNum2From);
  CLEAR(R3ReferNum2To);
  CLEAR(R3ReferAmt2From);
  CLEAR(R3ReferAmt2To);
  CLEAR(R3ReferNum3From);
  CLEAR(R3ReferNum3To);
  CLEAR(R3ReferAmt3From);
  CLEAR(R3ReferAmt3To);
  CLEAR(R3ReferNum4From);
  CLEAR(R3ReferNum4To);
  CLEAR(R3ReferAmt4From);
  CLEAR(R3ReferAmt4To);
  CLEAR(R3AgeOvrdAge1);
  CLEAR(R3AgeOvrdNum1From);
  CLEAR(R3AgeOvrdNum1To);
  CLEAR(R3AgeOvrdAmt1From);
  CLEAR(R3AgeOvrdAmt1To);
  CLEAR(R3AgeOvrdAge2);
  CLEAR(R3AgeOvrdNum2From);
  CLEAR(R3AgeOvrdNum2To);
  CLEAR(R3AgeOvrdAmt2From);
  CLEAR(R3AgeOvrdAmt2To);
  CLEAR(R3AgeOvrdAge3);
  CLEAR(R3AgeOvrdNum3From);
  CLEAR(R3AgeOvrdNum3To);
  CLEAR(R3AgeOvrdAmt3From);
  CLEAR(R3AgeOvrdAmt3To);
  CLEAR(R3AgeOvrdAge4);
  CLEAR(R3AgeOvrdNum4From);
  CLEAR(R3AgeOvrdNum4To);
  CLEAR(R3AgeOvrdAmt4From);
  CLEAR(R3AgeOvrdAmt4To);
  CLEAR(R3OvrdNum1From);
  CLEAR(R3OvrdNum1To);
  CLEAR(R3OvrdAmt1From);
  CLEAR(R3OvrdAmt1To);
  CLEAR(R3OvrdNum2From);
  CLEAR(R3OvrdNum2To);
  CLEAR(R3OvrdAmt2From);
  CLEAR(R3OvrdAmt2To);
  CLEAR(R3OvrdNum3From);
  CLEAR(R3OvrdNum3To);
  CLEAR(R3OvrdAmt3From);
  CLEAR(R3OvrdAmt3To);
  CLEAR(R3OvrdNum4From);
  CLEAR(R3OvrdNum4To);
  CLEAR(R3OvrdAmt4From);
  CLEAR(R3OvrdAmt4To);
  CLEAR(R4BackDays);
  CLEAR(R4BackGap);
  CLEAR(R4FwdDays3);
  CLEAR(R4FwdDays2);
  CLEAR(R4FwdDays1);
  CLEAR(R4FwdGap3);
  CLEAR(R4FwdGap2);
  CLEAR(R4FwdGap1);
  CLEAR(R5Amt);
  CLEAR(R6Amt);
  CLEAR(R6Accum);
}

void RulesDialog::numOut(TStatic* Ctl,int Val)
{
 char Buf[10];

 sprintf(Buf,"%d",Val);
 Ctl->Transfer(Buf,tdSetData);
}

#define CPY(Var) memcpy(&Rules.Var,&oldRules->Var,sizeof(Rules.Var))
#define R3SET(Var) Rules.R3.R3##Var=oldRules->R3.R3##Var
#define R3TO(Var) Rules.R3.R3##Var##To=oldRules->R3.R3##Var
void RulesDialog::convert(IPCMessage* Msg)
{
 struct OldRulesRecord* oldRules=(struct OldRulesRecord*)Msg->GetFldPtr(FN_AUTHRULES);
 memset(&Rules,0,sizeof(Rules));
 strcpy(Rules.AuthRulesKey.ClassName,oldRules->AuthRulesClassName);
 strcpy(Rules.AuthRulesKey.SubClassName,oldRules->AuthRulesSubClassName);
 Rules.AuthRulesTStamp = oldRules->AuthRulesTStamp;
 CPY(Countable);
 CPY(R1);
 CPY(R2);
 CPY(R4);
 CPY(R5);
 CPY(R6);
 memset(&Rules.R3,0xff,sizeof(Rules.R3));
 R3SET(Enable);
 R3SET(DateTime);
 R3SET(Product);
 R3SET(AgeReferAge1);
 R3SET(AgeReferAge2);
 R3SET(AgeOvrdAge1);
 R3SET(AgeOvrdAge2);
 R3TO(AgeReferNum1);
 R3TO(AgeReferNum2);
 R3TO(AgeReferAmt1);
 R3TO(AgeReferAmt2);
 R3TO(ReferNum1);
 R3TO(ReferAmt1);
 R3TO(ReferNum2);
 R3TO(ReferAmt2);
 R3TO(AgeOvrdNum1);
 R3TO(AgeOvrdAmt1);
 R3TO(AgeOvrdNum2);
 R3TO(AgeOvrdAmt2);
 R3TO(OvrdNum1);
 R3TO(OvrdAmt1);
 R3TO(OvrdNum2);
 R3TO(OvrdAmt2);
}


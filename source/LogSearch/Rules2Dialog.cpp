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
//  Source file for implementation of Rules2Dialog (TDialog).
//
//----------------------------------------------------------------------------
#include "win4me.h"

#include <stdio.h>
#include "SpsManDB.h"
#include "OldSpsManDB.h"
#include "Rules2Dialog.h"
#include "CheckAuthFN.h"
#include "unsnum.h"


//{{Rules2Dialog Implementation}}

#define STATIC(name) name = new TStatic(this,IDC_##name)

Rules2Dialog::Rules2Dialog(TWindow* parent)
:
    TDialog(parent, IDD_RULES2)
{
  STATIC(R7DailyNum);
  STATIC(R7DailyAmt);
  STATIC(R7Per1Days);
  STATIC(R7Per2Days);
  STATIC(R7Per3Days);
  STATIC(R7Per1Num);
  STATIC(R7Per2Num);
  STATIC(R7Per3Num);
  STATIC(R7Per1Amt);
  STATIC(R7Per2Amt);
  STATIC(R7Per3Amt);
  STATIC(R8Amt);
  STATIC(R8Accum);
}


Rules2Dialog::~Rules2Dialog()
{
  Destroy(IDCANCEL);
}

#define TRANSFER(rule,name) if (isEmpty(Rules.rule.rule##name) ||\
                                Rules.rule.rule##Enable==' ' )\
                              rule##name->Clear();\
                            else\
                              numOut(rule##name,Rules.rule.rule##name)

void Rules2Dialog::Display(IPCMessage* Msg)
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

 TRANSFER(R7,DailyNum);
 TRANSFER(R7,DailyAmt);
 TRANSFER(R7,Per1Days);
 TRANSFER(R7,Per2Days);
 TRANSFER(R7,Per3Days);
 TRANSFER(R7,Per1Num);
 TRANSFER(R7,Per2Num);
 TRANSFER(R7,Per3Num);
 TRANSFER(R7,Per1Amt);
 TRANSFER(R7,Per2Amt);
 TRANSFER(R7,Per3Amt);
 TRANSFER(R8,Amt);
 TRANSFER(R8,Accum);
}

#define CLEAR(name) name->Clear()
void Rules2Dialog::clearAll()
{
  CLEAR(R7DailyNum);
  CLEAR(R7DailyAmt);
  CLEAR(R7Per1Days);
  CLEAR(R7Per2Days);
  CLEAR(R7Per3Days);
  CLEAR(R7Per1Num);
  CLEAR(R7Per2Num);
  CLEAR(R7Per3Num);
  CLEAR(R7Per1Amt);
  CLEAR(R7Per2Amt);
  CLEAR(R7Per3Amt);
  CLEAR(R8Amt);
  CLEAR(R8Accum);
}

void Rules2Dialog::numOut(TStatic* Ctl,int Val)
{
 char Buf[10];

 sprintf(Buf,"%d",Val);
 Ctl->Transfer(Buf,tdSetData);
}

#define CPY(Var) memcpy(&Rules.Var,&oldRules->Var,sizeof(Rules.Var))
#define R3SET(Var) Rules.R3.R3##Var=oldRules->R3.R3##Var
#define R3TO(Var) Rules.R3.R3##Var##To=oldRules->R3.R3##Var
void Rules2Dialog::convert(IPCMessage* Msg)
{
 struct OldRulesRecord* oldRules=(struct OldRulesRecord*)Msg->GetFldPtr(FN_AUTHRULES);
 memset(&Rules,0,sizeof(Rules));
 strcpy(Rules.AuthRulesKey.ClassName,oldRules->AuthRulesClassName);
 strcpy(Rules.AuthRulesKey.SubClassName,oldRules->AuthRulesSubClassName);
 Rules.AuthRulesTStamp = oldRules->AuthRulesTStamp;
 CPY(R7);
 CPY(R8);
}


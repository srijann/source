//----------------------------------------------------------------------------
//  Project spsmanager
//
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    spsmanager.apx Application
//  FILE:         ruleparamsdialog.cpp
//  AUTHOR:
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of RuleParamsDialog (TDialog).
//
//----------------------------------------------------------------------------
#include "win4me.h"
#pragma hdrstop
#include <owl/validate.h>
#include "ruleparamsdialog.h"

#define AMTLEN 5+1
#define CHECKNUMLEN 6+1
#define NUMTRANSLEN 2+1
#define NUMDAYSLEN 2+1
#define AGELEN 2+1
#define CHECKGAPLEN 3+1

//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(RuleParamsDialog, TDialog)
//{{RuleParamsDialogRSP_TBL_BEGIN}}
//{{RuleParamsDialogRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{RuleParamsDialog Implementation}}

// These macros simplify the creation of the Controls
#define LSCTL(name) name=new TLevelSelect(this,IDC_##name,Levels)
#define AMTCTL(name) name=new TEditUns(this,IDC_##name,AMTLEN);\
  name->SetValidator(new TRangeValidator(0,USHRT_MAX-1))
#define DAYSCTL(name) name=new TEditUns(this,IDC_##name,NUMDAYSLEN)
#define TRANSCTL(name) name=new TEditUns(this,IDC_##name,NUMTRANSLEN)
#define AGECTL(name) name=new TEditUns(this,IDC_##name,AGELEN)
#define CHKNUMCTL(name) name=new TEditUns(this,IDC_##name,CHECKNUMLEN)
#define CHKGAPCTL(name) name=new TEditUns(this,IDC_##name,CHECKGAPLEN)
//------------------
// Class Constructor
//------------------
RuleParamsDialog::RuleParamsDialog(TWindow* parent,
      AuthRules* GlobalRules,   // Always required
      AuthRules* ClassRules,    // If 0, changing Global rules
      AuthRules* SubClassRules) // If 0, changing Class Rules
:
 FormDialog(parent, IDD_RULES),
 subClassRules(SubClassRules),
 globalRules(GlobalRules),
 classRules(ClassRules),
 SetupComplete(false)
{
 char* Levels;

 // Determine what level of rules we're working with
 if ( subClassRules )
  {
   // working with SubClass
   workingRules=subClassRules;
   workingLevel='S';
   Levels=" SCG";
  }
 else
  if ( classRules )
   {
    // working with Class
    workingRules=classRules;
    workingLevel='C';
    Levels=" CG";
   }
  else
   {
    // working with Global
    workingRules=globalRules;
    workingLevel='G';
    Levels=" G";
   }

 // Set up the Controls, using macros above

 LSCTL(CountableEnable);
 AMTCTL(CountableAmt);
 AMTCTL(CountableAccum);
 LSCTL(R1Enable);
 LSCTL(R1DateTime);
 AMTCTL(R1OvrdAmt2);
 AMTCTL(R1OvrdAmt1);
 AGECTL(R1OvrdAge2);
 AGECTL(R1OvrdAge1);
 LSCTL(R1Product);
 AMTCTL(R1ReferAmt2);
 AGECTL(R1ReferAge2);
 AMTCTL(R1ReferAmt1);
 AGECTL(R1ReferAge1);
 LSCTL(R2Product);
 LSCTL(R2DateTime);
 LSCTL(R2Enable);
 TRANSCTL(R2Sic1Trans);
 DAYSCTL(R2Sic1Days);
 AMTCTL(R2Sic1Amt);
 TRANSCTL(R2Sic2Trans);
 AMTCTL(R2Sic2Amt);
 AMTCTL(R2Acct1Amt);
 AMTCTL(R2Acct2Amt);
 AMTCTL(R2Loc1Amt);
 AMTCTL(R2Loc2Amt);
 DAYSCTL(R2Sic2Days);
 DAYSCTL(R2Loc2Days);
 TRANSCTL(R2Loc2Trans);
 DAYSCTL(R2Loc1Days);
 TRANSCTL(R2Loc1Trans);
 DAYSCTL(R2Acct2Days);
 TRANSCTL(R2Acct2Trans);
 TRANSCTL(R2Acct1Trans);
 DAYSCTL(R2Acct1Days);

 LSCTL(R3Product);
 LSCTL(R3DateTime);
 LSCTL(R3Enable);

 AGECTL(R3AgeReferAge1);
 AGECTL(R3AgeReferAge2);
 AGECTL(R3AgeReferAge3);
 AGECTL(R3AgeReferAge4);
 AGECTL(R3AgeOvrdAge1);
 AGECTL(R3AgeOvrdAge2);
 AGECTL(R3AgeOvrdAge3);
 AGECTL(R3AgeOvrdAge4);
 AMTCTL(R3AgeOvrdAmt1From);
 AMTCTL(R3AgeOvrdAmt2From);
 AMTCTL(R3AgeOvrdAmt3From);
 AMTCTL(R3AgeOvrdAmt4From);
 AMTCTL(R3AgeReferAmt1From);
 AMTCTL(R3AgeReferAmt2From);
 AMTCTL(R3AgeReferAmt3From);
 AMTCTL(R3AgeReferAmt4From);
 CHKNUMCTL(R3AgeOvrdNum1From);
 CHKNUMCTL(R3AgeOvrdNum2From);
 CHKNUMCTL(R3AgeOvrdNum3From);
 CHKNUMCTL(R3AgeOvrdNum4From);
 CHKNUMCTL(R3AgeReferNum1From);
 CHKNUMCTL(R3AgeReferNum2From);
 CHKNUMCTL(R3AgeReferNum3From);
 CHKNUMCTL(R3AgeReferNum4From);
 AMTCTL(R3ReferAmt1From);
 AMTCTL(R3ReferAmt2From);
 AMTCTL(R3ReferAmt3From);
 AMTCTL(R3ReferAmt4From);
 AMTCTL(R3OvrdAmt1From);
 AMTCTL(R3OvrdAmt2From);
 AMTCTL(R3OvrdAmt3From);
 AMTCTL(R3OvrdAmt4From);
 CHKNUMCTL(R3OvrdNum1From);
 CHKNUMCTL(R3OvrdNum2From);
 CHKNUMCTL(R3OvrdNum3From);
 CHKNUMCTL(R3OvrdNum4From);
 CHKNUMCTL(R3ReferNum1From);
 CHKNUMCTL(R3ReferNum2From);
 CHKNUMCTL(R3ReferNum3From);
 CHKNUMCTL(R3ReferNum4From);

 AMTCTL(R3AgeOvrdAmt1To);
 AMTCTL(R3AgeOvrdAmt2To);
 AMTCTL(R3AgeOvrdAmt3To);
 AMTCTL(R3AgeOvrdAmt4To);

 AMTCTL(R3AgeReferAmt1To);
 AMTCTL(R3AgeReferAmt2To);
 AMTCTL(R3AgeReferAmt3To);
 AMTCTL(R3AgeReferAmt4To);

 CHKNUMCTL(R3AgeOvrdNum1To);
 CHKNUMCTL(R3AgeOvrdNum2To);
 CHKNUMCTL(R3AgeOvrdNum3To);
 CHKNUMCTL(R3AgeOvrdNum4To);
 CHKNUMCTL(R3AgeReferNum1To);
 CHKNUMCTL(R3AgeReferNum2To);
 CHKNUMCTL(R3AgeReferNum3To);
 CHKNUMCTL(R3AgeReferNum4To);

 AMTCTL(R3ReferAmt1To);
 AMTCTL(R3ReferAmt2To);
 AMTCTL(R3ReferAmt3To);
 AMTCTL(R3ReferAmt4To);

 AMTCTL(R3OvrdAmt1To);
 AMTCTL(R3OvrdAmt2To);
 AMTCTL(R3OvrdAmt3To);
 AMTCTL(R3OvrdAmt4To);

 CHKNUMCTL(R3OvrdNum1To);
 CHKNUMCTL(R3OvrdNum2To);
 CHKNUMCTL(R3OvrdNum3To);
 CHKNUMCTL(R3OvrdNum4To);

 CHKNUMCTL(R3ReferNum1To);
 CHKNUMCTL(R3ReferNum2To);
 CHKNUMCTL(R3ReferNum3To);
 CHKNUMCTL(R3ReferNum4To);

 DAYSCTL(R4BackDays);
 CHKGAPCTL(R4BackGap);
 DAYSCTL(R4FwdDays3);
 DAYSCTL(R4FwdDays2);
 DAYSCTL(R4FwdDays1);
 CHKGAPCTL(R4FwdGap3);
 CHKGAPCTL(R4FwdGap2);
 CHKGAPCTL(R4FwdGap1);
 LSCTL(R4Product);
 LSCTL(R4DateTime);
 LSCTL(R4Enable);
 LSCTL(R5Enable);
 LSCTL(R5DateTime);
 LSCTL(R5Product);
 AMTCTL(R5Amt);
 LSCTL(R6Enable);
 LSCTL(R6DateTime);
 LSCTL(R6Product);
 AMTCTL(R6Amt);
 AMTCTL(R6Accum);
}

//-----------------
// Class Destructor
//-----------------
RuleParamsDialog::~RuleParamsDialog()
{
  Destroy(IDCANCEL);
 // We don't think we allocated anything
}

//-----------------------------------------------------------------
// Determine which level of rules to display, based on the Enabler
// in the workingRules. Also determine if data entry is allowed for
// rules and if rules should be displayed dim.
//-----------------------------------------------------------------
AuthRules* RuleParamsDialog::RuleSetSelect(BYTE* Enabler,bool& rdonly,bool& dim)
{
 BYTE  globalEnabler;
 BYTE  classEnabler;
 char* cglobalRules;
 char* cclassRules;
 int   Offset;

 // If at working level, use working rules and allow entry
 if ( *Enabler==workingLevel )
  {
   rdonly=false;
   dim=false;
   return workingRules;
  }

 // If disabled at working level, show no parameters and allow no entry
 if ( *Enabler == ' ' )
  {
   rdonly=true;
   dim=true;
   return 0;
  }

 // the workingRules level is pointing to another level....

 // No entry will be allowed.
 rdonly = true;
 dim = false;

 // Get the global rules Enabler by using the same byte offset
 // as the workingRules.
 cglobalRules=(char*)&globalRules->Data;
 Offset=Enabler-(BYTE*)&workingRules->Data;
 globalEnabler=cglobalRules[Offset];

 // if the working Rules is pointing to the global rules, use the
 // enabler of the global rules
 if ( *Enabler=='G' )
  {
   // Not enabled, don't display parms
   if ( globalEnabler == ' ' )
    {
     dim=true;
     return 0;
    }
   else
   // Global enabled, display parms
     return globalRules;
  }

 // Once we get here we know that the working rules are pointing up
 // to the class level. We have to determine if the Class level is,
 // itself pointing up to the global level before we decide what
 // level to use.

 // Pull the enabler from the Class rules using the same byte offset
 // (If we get here we should have Class rules but just in case somebody
 //  screwed up we check for a NULL pointer).

 if (classRules==NULL)
  return 0;
 cclassRules=(char*)&classRules->Data;
 classEnabler=cclassRules[Offset];

 // If class Level enabled, display class parms
 if ( classEnabler == 'C' )
  return classRules;

 // If class Level is disabled, display no parms
 if ( classEnabler == ' ' )
  return 0;

 // Class level must point to global level. If Global level is
 // disabled, display no parms.
 if ( globalEnabler ==' ' )
  return 0;

 // If global level is enabled, show the parms dim to indicate
 // that we are two levels away from the working Rules.
 if ( globalEnabler == 'G' )
  {
   dim=true;
   return globalRules;
  }

 // Should be impossible to get here but what the heck
 return 0;
}

// Macros for simplifying the GroupOut function

// Note: we get the dim effect by disabling the window. Rdonly displays
// it in background color but not dimmed.

// Output a field, depending on level
#define OUTPUT(n,name)\
 if (Rules) n##name->Set(Rules->Data.n.n##name); else n##name->Transfer("",tdSetData);\
   n##name->EnableWindow(!dim);\
   n##name->SetReadOnly(rdonly);

// Select which rules and how to display
#define SETRULES(n,name)\
 Rules=RuleSetSelect(&workingRules->Data.n.n##name,rdonly,dim)


//----------------------------------------------------------------------
// Output a group of fields based on the control ID of the group enabler
//----------------------------------------------------------------------
void RuleParamsDialog::GroupOut(int RuleEnabler)
{
 AuthRules* Rules;
 bool dim;
 bool rdonly;

 // Output the group

 switch( RuleEnabler )
  {
   case IDC_CountableEnable:
     SETRULES(Countable,Enable);
     OUTPUT(Countable,Amt);
     OUTPUT(Countable,Accum);
     break;
   case IDC_R1Enable:
     SETRULES(R1,Enable);
     OUTPUT(R1,OvrdAmt2);
     OUTPUT(R1,OvrdAmt1);
     OUTPUT(R1,OvrdAge2);
     OUTPUT(R1,OvrdAge1);
     OUTPUT(R1,ReferAmt2);
     OUTPUT(R1,ReferAge2);
     OUTPUT(R1,ReferAmt1);
     OUTPUT(R1,ReferAge1);
     break;
   case IDC_R2Enable:
     SETRULES(R2,Enable);
     OUTPUT(R2,Sic1Trans);
     OUTPUT(R2,Sic1Days);
     OUTPUT(R2,Sic1Amt);
     OUTPUT(R2,Sic2Trans);
     OUTPUT(R2,Sic2Amt);
     OUTPUT(R2,Acct1Amt);
     OUTPUT(R2,Acct2Amt);
     OUTPUT(R2,Loc1Amt);
     OUTPUT(R2,Loc2Amt);
     OUTPUT(R2,Sic2Days);
     OUTPUT(R2,Loc2Days);
     OUTPUT(R2,Loc2Trans);
     OUTPUT(R2,Loc1Days);
     OUTPUT(R2,Loc1Trans);
     OUTPUT(R2,Acct2Days);
     OUTPUT(R2,Acct2Trans);
     OUTPUT(R2,Acct1Trans);
     OUTPUT(R2,Acct1Days);
     break;
   case IDC_R3Enable:
     SETRULES(R3,Enable);
     OUTPUT(R3,AgeOvrdAge4);
     OUTPUT(R3,AgeOvrdAge3);
     OUTPUT(R3,AgeOvrdAge2);
     OUTPUT(R3,AgeOvrdAge1);
     OUTPUT(R3,AgeReferAge4);
     OUTPUT(R3,AgeReferAge3);
     OUTPUT(R3,AgeReferAge2);
     OUTPUT(R3,AgeReferAge1);
     OUTPUT(R3,OvrdAmt1From);
     OUTPUT(R3,ReferAmt1From);
     OUTPUT(R3,ReferAmt2From);
     OUTPUT(R3,ReferAmt3From);
     OUTPUT(R3,ReferAmt4From);
     OUTPUT(R3,AgeOvrdAmt2From);
     OUTPUT(R3,AgeOvrdAmt3From);
     OUTPUT(R3,AgeOvrdAmt4From);
     OUTPUT(R3,AgeOvrdAmt1From);
     OUTPUT(R3,AgeReferAmt2From);
     OUTPUT(R3,AgeReferAmt3From);
     OUTPUT(R3,AgeReferAmt4From);
     OUTPUT(R3,AgeReferAmt1From);
     OUTPUT(R3,AgeOvrdNum2From);
     OUTPUT(R3,AgeOvrdNum3From);
     OUTPUT(R3,AgeOvrdNum4From);
     OUTPUT(R3,AgeOvrdNum1From);
     OUTPUT(R3,AgeReferNum2From);
     OUTPUT(R3,AgeReferNum3From);
     OUTPUT(R3,AgeReferNum4From);
     OUTPUT(R3,AgeReferNum1From);
     OUTPUT(R3,OvrdAmt2From);
     OUTPUT(R3,OvrdAmt3From);
     OUTPUT(R3,OvrdAmt4From);
     OUTPUT(R3,OvrdNum2From);
     OUTPUT(R3,OvrdNum3From);
     OUTPUT(R3,OvrdNum4From);
     OUTPUT(R3,OvrdNum1From);
     OUTPUT(R3,ReferNum1From);
     OUTPUT(R3,ReferNum2From);
     OUTPUT(R3,ReferNum3From);
     OUTPUT(R3,ReferNum4From);
     OUTPUT(R3,OvrdAmt1To);
     OUTPUT(R3,ReferAmt1To);
     OUTPUT(R3,ReferAmt2To);
     OUTPUT(R3,ReferAmt3To);
     OUTPUT(R3,ReferAmt4To);
     OUTPUT(R3,AgeOvrdAmt2To);
     OUTPUT(R3,AgeOvrdAmt3To);
     OUTPUT(R3,AgeOvrdAmt4To);
     OUTPUT(R3,AgeOvrdAmt1To);
     OUTPUT(R3,AgeReferAmt2To);
     OUTPUT(R3,AgeReferAmt3To);
     OUTPUT(R3,AgeReferAmt4To);
     OUTPUT(R3,AgeReferAmt1To);
     OUTPUT(R3,AgeOvrdNum2To);
     OUTPUT(R3,AgeOvrdNum3To);
     OUTPUT(R3,AgeOvrdNum4To);
     OUTPUT(R3,AgeOvrdNum1To);
     OUTPUT(R3,AgeReferNum2To);
     OUTPUT(R3,AgeReferNum3To);
     OUTPUT(R3,AgeReferNum4To);
     OUTPUT(R3,AgeReferNum1To);
     OUTPUT(R3,OvrdAmt1To);
     OUTPUT(R3,OvrdAmt2To);
     OUTPUT(R3,OvrdAmt3To);
     OUTPUT(R3,OvrdAmt4To);
     OUTPUT(R3,OvrdNum1To);
     OUTPUT(R3,OvrdNum2To);
     OUTPUT(R3,OvrdNum3To);
     OUTPUT(R3,OvrdNum4To);
     OUTPUT(R3,ReferNum1To);
     OUTPUT(R3,ReferNum2To);
     OUTPUT(R3,ReferNum3To);
     OUTPUT(R3,ReferNum4To);
     break;
   case IDC_R4Enable:
     SETRULES(R4,Enable);
     OUTPUT(R4,BackDays);
     OUTPUT(R4,BackGap);
     OUTPUT(R4,FwdDays3);
     OUTPUT(R4,FwdDays2);
     OUTPUT(R4,FwdDays1);
     OUTPUT(R4,FwdGap3);
     OUTPUT(R4,FwdGap2);
     OUTPUT(R4,FwdGap1);
     break;
  case IDC_R5Enable:
     SETRULES(R5,Enable);
     OUTPUT(R5,Amt);
     break;
  case IDC_R6Enable:
     SETRULES(R6,Enable);
     OUTPUT(R6,Amt);
     OUTPUT(R6,Accum);
     break;
  }
}

//--------------------------------------------------
// Override virtual function for the initial display
//--------------------------------------------------

// A macro for simplifying the following function
#define OUTPUTLEVEL(n,name) n##name->SetLevel(workingRules->Data.n.n##name)
void RuleParamsDialog::SetupWindow()
{
 TDialog::SetupWindow();

 OUTPUTLEVEL(Countable,Enable);
 GroupOut(IDC_CountableEnable);
 OUTPUTLEVEL(R1,Enable);
 OUTPUTLEVEL(R1,DateTime);
 OUTPUTLEVEL(R1,Product);
 GroupOut(IDC_R1Enable);
 OUTPUTLEVEL(R2,Product);
 OUTPUTLEVEL(R2,DateTime);
 OUTPUTLEVEL(R2,Enable);
 GroupOut(IDC_R2Enable);
 OUTPUTLEVEL(R3,Product);
 OUTPUTLEVEL(R3,DateTime);
 OUTPUTLEVEL(R3,Enable);
 GroupOut(IDC_R3Enable);
 OUTPUTLEVEL(R4,Product);
 OUTPUTLEVEL(R4,DateTime);
 OUTPUTLEVEL(R4,Enable);
 GroupOut(IDC_R4Enable);
 OUTPUTLEVEL(R5,Enable);
 OUTPUTLEVEL(R5,DateTime);
 OUTPUTLEVEL(R5,Product);
 GroupOut(IDC_R5Enable);
 OUTPUTLEVEL(R6,Enable);
 OUTPUTLEVEL(R6,DateTime);
 OUTPUTLEVEL(R6,Product);
 GroupOut(IDC_R6Enable);
 SetupComplete=true;
}

//-----------------------------------------------------
// Handle the EV_CHANGE command to handle level changes
//-----------------------------------------------------
TResult RuleParamsDialog::EvCommand(uint id, THandle hWndCtl, uint notifyCode)
{
  TResult result;

  result = FormDialog::EvCommand(id, hWndCtl, notifyCode);

  switch ( notifyCode )
   {
    case EN_CHANGE:  if ( SetupComplete )
                       ChangeHandler(id);
                     break;
   }

  return result;
}

// Macros for simplifying the ChangeHandler function

#define TRANSFER(n,name) case IDC_##n##name:\
 if (workingRules->Data.n.n##Enable==workingLevel)\
   n##name->Get(workingRules->Data.n.n##name);\
   break

#define TRANSFERLEVEL(n,name) case IDC_##n##name:\
   n##name->GetLevel(&workingRules->Data.n.n##name); break


// Transfer Level change from window to record and output the
// corresponding group of fields

#define CHANGELEVEL(n,focusctl) case IDC_##n##Enable:\
 n##Enable->GetLevel(&workingRules->Data.n.n##Enable);\
 GroupOut(IDC_##n##Enable);\
 if (workingRules->Data.n.n##Enable==workingLevel) {\
   n##focusctl->SetFocus(); n##focusctl->SetSelection(0,5);} break

void RuleParamsDialog::ChangeHandler(uint Id)
{
 switch( Id )
  {
  CHANGELEVEL(Countable,Amt);
  TRANSFER(Countable,Amt);
  TRANSFER(Countable,Accum);
  CHANGELEVEL(R1,ReferAge1);
  TRANSFERLEVEL(R1,DateTime);
  TRANSFERLEVEL(R1,Product);
  TRANSFER(R1,OvrdAmt2);
  TRANSFER(R1,OvrdAmt1);
  TRANSFER(R1,OvrdAge2);
  TRANSFER(R1,OvrdAge1);
  TRANSFER(R1,ReferAmt2);
  TRANSFER(R1,ReferAge2);
  TRANSFER(R1,ReferAmt1);
  TRANSFER(R1,ReferAge1);

  TRANSFERLEVEL(R2,Product);
  TRANSFERLEVEL(R2,DateTime);
  CHANGELEVEL(R2,Sic1Trans);
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

  TRANSFERLEVEL(R3,Product);
  TRANSFERLEVEL(R3,DateTime);
  CHANGELEVEL(R3,AgeReferAge1);
  TRANSFER(R3,AgeReferAge1);
  TRANSFER(R3,AgeReferAge2);
  TRANSFER(R3,AgeReferAge3);
  TRANSFER(R3,AgeReferAge4);
  TRANSFER(R3,AgeOvrdAge1);
  TRANSFER(R3,AgeOvrdAge2);
  TRANSFER(R3,AgeOvrdAge3);
  TRANSFER(R3,AgeOvrdAge4);
  TRANSFER(R3,ReferAmt1From);
  TRANSFER(R3,ReferAmt2From);
  TRANSFER(R3,ReferAmt3From);
  TRANSFER(R3,ReferAmt4From);
  TRANSFER(R3,AgeOvrdAmt2From);
  TRANSFER(R3,AgeOvrdAmt3From);
  TRANSFER(R3,AgeOvrdAmt4From);
  TRANSFER(R3,AgeOvrdAmt1From);
  TRANSFER(R3,AgeReferAmt2From);
  TRANSFER(R3,AgeReferAmt3From);
  TRANSFER(R3,AgeReferAmt4From);
  TRANSFER(R3,AgeReferAmt1From);
  TRANSFER(R3,AgeOvrdNum2From);
  TRANSFER(R3,AgeOvrdNum3From);
  TRANSFER(R3,AgeOvrdNum4From);
  TRANSFER(R3,AgeOvrdNum1From);
  TRANSFER(R3,AgeReferNum2From);
  TRANSFER(R3,AgeReferNum3From);
  TRANSFER(R3,AgeReferNum4From);
  TRANSFER(R3,AgeReferNum1From);
  TRANSFER(R3,OvrdAmt1From);
  TRANSFER(R3,OvrdAmt2From);
  TRANSFER(R3,OvrdAmt3From);
  TRANSFER(R3,OvrdAmt4From);
  TRANSFER(R3,OvrdNum1From);
  TRANSFER(R3,OvrdNum2From);
  TRANSFER(R3,OvrdNum3From);
  TRANSFER(R3,OvrdNum4From);
  TRANSFER(R3,ReferNum1From);
  TRANSFER(R3,ReferNum2From);
  TRANSFER(R3,ReferNum3From);
  TRANSFER(R3,ReferNum4From);
  TRANSFER(R3,OvrdAmt1To);
  TRANSFER(R3,ReferAmt1To);
  TRANSFER(R3,ReferAmt2To);
  TRANSFER(R3,ReferAmt3To);
  TRANSFER(R3,ReferAmt4To);
  TRANSFER(R3,AgeOvrdAmt2To);
  TRANSFER(R3,AgeOvrdAmt3To);
  TRANSFER(R3,AgeOvrdAmt4To);
  TRANSFER(R3,AgeOvrdAmt1To);
  TRANSFER(R3,AgeReferAmt2To);
  TRANSFER(R3,AgeReferAmt3To);
  TRANSFER(R3,AgeReferAmt4To);
  TRANSFER(R3,AgeReferAmt1To);
  TRANSFER(R3,AgeOvrdNum2To);
  TRANSFER(R3,AgeOvrdNum3To);
  TRANSFER(R3,AgeOvrdNum4To);
  TRANSFER(R3,AgeOvrdNum1To);
  TRANSFER(R3,AgeReferNum2To);
  TRANSFER(R3,AgeReferNum3To);
  TRANSFER(R3,AgeReferNum4To);
  TRANSFER(R3,AgeReferNum1To);
  TRANSFER(R3,OvrdAmt2To);
  TRANSFER(R3,OvrdAmt3To);
  TRANSFER(R3,OvrdAmt4To);
  TRANSFER(R3,OvrdNum2To);
  TRANSFER(R3,OvrdNum3To);
  TRANSFER(R3,OvrdNum4To);
  TRANSFER(R3,OvrdNum1To);
  TRANSFER(R3,ReferNum1To);
  TRANSFER(R3,ReferNum2To);
  TRANSFER(R3,ReferNum3To);
  TRANSFER(R3,ReferNum4To);

  TRANSFER(R4,BackDays);
  TRANSFER(R4,BackGap);
  TRANSFER(R4,FwdDays3);
  TRANSFER(R4,FwdDays2);
  TRANSFER(R4,FwdDays1);
  TRANSFER(R4,FwdGap3);
  TRANSFER(R4,FwdGap2);
  TRANSFER(R4,FwdGap1);
  TRANSFERLEVEL(R4,Product);
  TRANSFERLEVEL(R4,DateTime);
  CHANGELEVEL(R4,FwdGap1);

  CHANGELEVEL(R5,Amt);
  TRANSFERLEVEL(R5,DateTime);
  TRANSFERLEVEL(R5,Product);
  TRANSFER(R5,Amt);
  
  CHANGELEVEL(R6,Amt);
  TRANSFERLEVEL(R6,DateTime);
  TRANSFERLEVEL(R6,Product);
  TRANSFER(R6,Amt);
  TRANSFER(R6,Accum);
  }
}

//------------------------------
// Perform all Field validations
//------------------------------
#define VALID(name) if ( ! name->IsValid() )\
 { name->SetFocus(); name->ValidatorError(); return false; }
bool RuleParamsDialog::Validate()
{
 VALID(CountableAmt);
 VALID(CountableAccum);
 // Make sure all the amounts are in range
 VALID(R1OvrdAmt2);
 VALID(R1OvrdAmt1);
 VALID(R1ReferAmt2);
 VALID(R1ReferAmt1);
 VALID(R2Sic1Amt);
 VALID(R2Sic2Amt);
 VALID(R2Acct1Amt);
 VALID(R2Acct2Amt);
 VALID(R2Loc1Amt);
 VALID(R2Loc2Amt);

 VALID(R3OvrdAmt1From);
 VALID(R3ReferAmt1From);
 VALID(R3ReferAmt2From);
 VALID(R3AgeOvrdAmt2From);
 VALID(R3AgeOvrdAmt1From);
 VALID(R3AgeReferAmt2From);
 VALID(R3AgeReferAmt1From);
 VALID(R3OvrdAmt2From);

 VALID(R3OvrdAmt1To);
 VALID(R3ReferAmt1To);
 VALID(R3ReferAmt2To);
 VALID(R3AgeOvrdAmt2To);
 VALID(R3AgeOvrdAmt1To);
 VALID(R3AgeReferAmt2To);
 VALID(R3AgeReferAmt1To);
 VALID(R3OvrdAmt2To);

 VALID(R3OvrdAmt3To);
 VALID(R3ReferAmt3To);
 VALID(R3ReferAmt4To);
 VALID(R3AgeOvrdAmt4To);
 VALID(R3AgeOvrdAmt3To);
 VALID(R3AgeReferAmt4To);
 VALID(R3AgeReferAmt3To);
 VALID(R3OvrdAmt4To);

 VALID(R3OvrdAmt3From);
 VALID(R3ReferAmt3From);
 VALID(R3ReferAmt4From);
 VALID(R3AgeOvrdAmt4From);
 VALID(R3AgeOvrdAmt3From);
 VALID(R3AgeReferAmt4From);
 VALID(R3AgeReferAmt3From);
 VALID(R3OvrdAmt4From);

 VALID(R5Amt);
 VALID(R6Amt);
 VALID(R6Accum);
 return true;
}

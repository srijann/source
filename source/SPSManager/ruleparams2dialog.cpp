//----------------------------------------------------------------------------
//  Project spsmanager
//
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    spsmanager.apx Application
//  FILE:         RuleParams2Dialog.cpp
//  AUTHOR:
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of RuleParams2Dialog (TDialog).
//
//----------------------------------------------------------------------------
#include "win4me.h"
#pragma hdrstop
#include <owl/validate.h>
#include "ruleparams2dialog.h"

#define AMTLEN 5+1
#define NUMTRANSLEN 2+1
#define NUMDAYSLEN 2+1

//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(RuleParams2Dialog, TDialog)
//{{RuleParams2DialogRSP_TBL_BEGIN}}
//{{RuleParams2DialogRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{RuleParams2Dialog Implementation}}

// These macros simplify the creation of the Controls

// Create a Level Selector control
#define LSCTL(name) name=new TLevelSelect(this,IDC_##name,Levels)

// Create an Amount Control
#define AMTCTL(name) name=new TEditUns(this,IDC_##name,AMTLEN);\
  name->SetValidator(new TRangeValidator(0,USHRT_MAX-1))

// Create a Num Days Control
#define DAYSCTL(name) name=new TEditUns(this,IDC_##name,NUMDAYSLEN)

// Create a Num Transactions Control
#define TRANSCTL(name) name=new TEditUns(this,IDC_##name,NUMTRANSLEN)

//------------------
// Class Constructor
//------------------
RuleParams2Dialog::RuleParams2Dialog(TWindow* parent,
      AuthRules* GlobalRules,   // Always required
      AuthRules* ClassRules,    // If 0, changing Global rules
      AuthRules* SubClassRules) // If 0, changing Class Rules
:
 FormDialog(parent, IDD_RULES2),
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

 LSCTL(R7Enable);
 LSCTL(R7DateTime);
 LSCTL(R7Product);
 TRANSCTL(R7DailyNum);
 AMTCTL(R7DailyAmt);
 DAYSCTL(R7Per1Days);
 DAYSCTL(R7Per2Days);
 DAYSCTL(R7Per3Days);
 TRANSCTL(R7Per1Num);
 TRANSCTL(R7Per2Num);
 TRANSCTL(R7Per3Num);
 AMTCTL(R7Per1Amt);
 AMTCTL(R7Per2Amt);
 AMTCTL(R7Per3Amt);
 LSCTL(R8Enable);
 LSCTL(R8DateTime);
 LSCTL(R8Product);
 AMTCTL(R8Amt);
 AMTCTL(R8Accum);
}

//-----------------
// Class Destructor
//-----------------
RuleParams2Dialog::~RuleParams2Dialog()
{
  Destroy(IDCANCEL);
 // We don't think we allocated anything
}

//-----------------------------------------------------------------
// Determine which level of rules to display, based on the Enabler
// in the workingRules. Also determine if data entry is allowed for
// rules and if rules should be displayed dim.
//-----------------------------------------------------------------
AuthRules* RuleParams2Dialog::RuleSetSelect(BYTE* Enabler,bool& rdonly,bool& dim)
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

 if ( classRules==NULL )
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
void RuleParams2Dialog::GroupOut(int RuleEnabler)
{
 AuthRules* Rules;
 bool dim;
 bool rdonly;

 // Output the group

 switch( RuleEnabler )
  {
  case IDC_R7Enable:
     SETRULES(R7,Enable);
     OUTPUT(R7,DailyNum);
     OUTPUT(R7,DailyAmt);
     OUTPUT(R7,Per1Days);
     OUTPUT(R7,Per2Days);
     OUTPUT(R7,Per3Days);
     OUTPUT(R7,Per1Num);
     OUTPUT(R7,Per2Num);
     OUTPUT(R7,Per3Num);
     OUTPUT(R7,Per1Amt);
     OUTPUT(R7,Per2Amt);
     OUTPUT(R7,Per3Amt);
     break;
   case IDC_R8Enable:
     SETRULES(R8,Enable);
     OUTPUT(R8,Amt);
     OUTPUT(R8,Accum);
     break;
  }
}

//--------------------------------------------------
// Override virtual function for the initial display
//--------------------------------------------------

// A macro for simplifying the following function
#define OUTPUTLEVEL(n,name) n##name->SetLevel(workingRules->Data.n.n##name)
void RuleParams2Dialog::SetupWindow()
{
 TDialog::SetupWindow();

 OUTPUTLEVEL(R7,Enable);
 OUTPUTLEVEL(R7,DateTime);
 OUTPUTLEVEL(R7,Product);
 GroupOut(IDC_R7Enable);
 OUTPUTLEVEL(R8,Enable);
 OUTPUTLEVEL(R8,DateTime);
 OUTPUTLEVEL(R8,Product);
 GroupOut(IDC_R8Enable);
 SetupComplete=true;
}

//-----------------------------------------------------
// Handle the EV_CHANGE command to handle level changes
//-----------------------------------------------------
TResult RuleParams2Dialog::EvCommand(uint id, THandle hWndCtl, uint notifyCode)
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

void RuleParams2Dialog::ChangeHandler(uint Id)
{
 switch( Id )
  {
  CHANGELEVEL(R7,DailyNum);
  TRANSFERLEVEL(R7,DateTime);
  TRANSFERLEVEL(R7,Product);
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
  CHANGELEVEL(R8,Amt);
  TRANSFERLEVEL(R8,DateTime);
  TRANSFERLEVEL(R8,Product);
  TRANSFER(R8,Amt);
  TRANSFER(R8,Accum);
  }
}

//------------------------------
// Perform all Field validations
//------------------------------
#define VALID(name) if ( ! name->IsValid() )\
 { name->SetFocus(); name->ValidatorError(); return false; }
bool RuleParams2Dialog::Validate()
{
 VALID(R7DailyAmt);
 VALID(R7Per1Amt);
 VALID(R7Per2Amt);
 VALID(R7Per3Amt);
 VALID(R8Amt);
 VALID(R8Accum);

 return true;
}

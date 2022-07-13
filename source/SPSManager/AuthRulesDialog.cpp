//----------------------------------------------------------------------------
//  Project spsmanager
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    spsmanager.apx Application
//  FILE:         AuthRulesdialog.cpp
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of AuthRulesDialog (TDialog).
//
//----------------------------------------------------------------------------
#include "win4me.h"
#pragma hdrstop
#include "AuthRulesdialog.h"
#include "TranType.h"
#include "CheckAuthFn.h"
#include "DBManMsg.h"
#include "SaveFile.h"

//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(AuthRulesDialog, TDialog)
//{{AuthRulesDialogRSP_TBL_BEGIN}}
  EV_TCN_SELCHANGE(IDC_AUTHRULESTAB, TCNSelchange),
  EV_TCN_SELCHANGING(IDC_AUTHRULESTAB, TCNSelchanging),
//{{AuthRulesDialogRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{AuthRulesDialog Implementation}}

//------------------
// Class Constructor
//------------------
AuthRulesDialog::AuthRulesDialog(TWindow* parent,
                                 AuthRules* GlobalRules,
                                 AuthRules* ClassRules,
                                 AuthRules* SubClassRules,
                                 bool IsNew)
:
 FormDialog(parent,IDD_AUTHRULES,RecTypeAuthRules),
 globalRules(GlobalRules),
 classRules(ClassRules),
 subClassRules(SubClassRules)
{
 char Title[sizeof(workingRules->Data.AuthRulesKey.ClassName)+
            sizeof(workingRules->Data.AuthRulesKey.SubClassName)+12];

 Tab = new TTabControl(this,IDC_AUTHRULESTAB);

 // Figure which rules are the working rules
  // Determine what level of rules we're working with
 if ( subClassRules )
   workingRules=subClassRules;
 else
  if ( classRules )
   workingRules=classRules;
  else
    workingRules=globalRules;

 // Save the original rules so we can tell if the rules have
 // been modified
 origRules=new AuthRules(&workingRules->Data);
 strcpy(Title,(const char*)workingRules->Data.AuthRulesKey.ClassName);
 if ( workingRules->Data.AuthRulesKey.SubClassName[0] )
  {
   strcat(Title,".");
   strcat(Title,(const char*)workingRules->Data.AuthRulesKey.SubClassName);
  }
 strcat(Title," AuthRules");
 SetCaption(Title);

 if ( IsNew )
  SetModified();
}


//-----------
// Destructor
//-----------
AuthRulesDialog::~AuthRulesDialog()
{
 Destroy(IDCANCEL);
 delete origRules;
 if ( subClassRules )
  delete subClassRules;
 if ( classRules )
  delete classRules;
 if ( globalRules )
  delete globalRules;
}

//---------------------------------------------
// Handle the Tab Selection
//---------------------------------------------
void AuthRulesDialog::TCNSelchange(TNotify&)
{

 CurrentWin->Show(SW_HIDE);
 switch( Tab->GetSel() )
  {
   case 0: CurrentWin = RulesWin; break;
   case 1: CurrentWin = Rules2Win; break;
   case 2: CurrentWin = DateTimeWin; break;
   case 3: CurrentWin = ProductWin; break;
  }

 CurrentWin->Show(SW_SHOW);
 CurrentWin->SetFocus();
}

//---------------------------------------------------
// Don't allow Tab switch with invalid data in window
//---------------------------------------------------
bool AuthRulesDialog::TCNSelchanging(TNotify&)
{
 if ( ! CurrentWin->Validate() )
   return true;
 else
   return false;  
}

//--------------------------------------------------------------------
// Override of SetuWindow to set up Tab Control and create sub-windows
//--------------------------------------------------------------------
void AuthRulesDialog::SetupWindow()
{
 TDialog::SetupWindow();
 Tab->Add("Rules 1-6");
 Tab->Add("Rules 7-8");
 Tab->Add("Date/Time Adjust");
 Tab->Add("Product Adjust");
 RulesWin = new RuleParamsDialog(this,globalRules,
                                 classRules,subClassRules);
 RulesWin->Create();
 Rules2Win = new RuleParams2Dialog(this,globalRules,
                                   classRules,subClassRules);
 Rules2Win->Create();
 ProductWin = new ProductAdjustDialog(this,workingRules);
 ProductWin->Create();
 DateTimeWin = new DateTimeDialog(this,workingRules);
 DateTimeWin->Create();
 CurrentWin = RulesWin;
 CurrentWin->Show(SW_SHOW);
 CurrentWin->SetFocus();
}

//----------------------------------------------------------------
// Return the Modified status by comparing the original rules with
// the working rules.
//----------------------------------------------------------------
bool AuthRulesDialog::IsModified()
{
 if ( FormDialog::IsModified() )
  return true;
 else
  return (memcmp(&workingRules->Data,&origRules->Data,
                sizeof(workingRules->Data))==0) ? false : true;
}

//-----------------------------
// Handle the Save menu command
//-----------------------------
bool AuthRulesDialog::Save()
{
 DBManMessage Msg;

 if ( ! Msg.DBPut(this,T_PUTAUTHRULES,FN_AUTHRULES,workingRules) )
  return false;

 ResetModified();
 return true;
}

//----------------------------------
// Handle the Save Temp menu command
//----------------------------------
bool AuthRulesDialog::SaveTemp(TFile* File)
{
 SAVERECORD_SENTINEL Sentinel;

 Sentinel.Type = GetType();
 Sentinel.Version = SaveRecordVersion[Sentinel.Type];
 Sentinel.Len = sizeof(globalRules->Data);
 if ( classRules )
  Sentinel.Len += sizeof(classRules->Data);
 if ( subClassRules )
  Sentinel.Len += sizeof(subClassRules->Data);

 if ( File->Write(&Sentinel,sizeof(Sentinel)) != sizeof(Sentinel) ||
      File->Write(&globalRules->Data,sizeof(globalRules->Data))
            != sizeof(globalRules->Data) )
  return false;

 if ( classRules &&
      File->Write(&classRules->Data,sizeof(classRules->Data)) !=
        sizeof(classRules->Data) )
  return false;       

 if ( subClassRules &&
      File->Write(&subClassRules->Data,sizeof(subClassRules->Data)) !=
        sizeof(subClassRules->Data) )
  return false;

 ResetModified();
 return true;
}

void AuthRulesDialog::ResetModified()
{
 memcpy(&origRules->Data,&workingRules->Data,sizeof(origRules->Data));
 FormDialog::ResetModified();
}

bool AuthRulesDialog::Validate()
{
 return CurrentWin->Validate();
}



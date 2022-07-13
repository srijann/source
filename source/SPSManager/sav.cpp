//----------------------------------------------------------------------------
//  Project spsmanager
//
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    spsmanager.apx Application
//  FILE:         UnvFundsRulesdialog.cpp
//  AUTHOR:
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of UnvFundsRulesDialog (TDialog).
//
//----------------------------------------------------------------------------
#include "win4me.h"
#pragma hdrstop
#include <owl/validate.h>
#include "UnvFundsDialog.h"
#include "TranType.h"
#include "CheckAuthFn.h"
#include "DBManMsg.h"
#include "SaveFile.h"

//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(UnvFundsRulesDialog, TDialog)
//{{UnvFundsRulesDialogRSP_TBL_BEGIN}}
//{{UnvFundsRulesDialogRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{UnvFundsRulesDialog Implementation}}

//------------------
// Class Constructor
//------------------
UnvFundsRulesDialog::UnvFundsRulesDialog(TWindow* parent,UnvFundsRules* OrigRec)
:
 FormDialog(parent,IDD_SECRULES,RecTypeUnvFundsRules),
 origRec(OrigRec),
 SetupComplete(false)
{
 char Title[100];

 strcpy(Title,(char*)origRec->Data.UnvFundsRulesClassName);
 strcat(Title," Unverified Funds Rules");
 SetCaption(Title);
 memcpy(&newRec.Data,&origRec->Data,sizeof(newRec.Data));

}



//-----------------
// Class Destructor
//-----------------
UnvFundsRulesDialog::~UnvFundsRulesDialog()
{
 Destroy(IDCANCEL);
 delete origRec;
}

//-----------------------------------------------------
// Handle the EV_CHANGE command to handle level changes
//-----------------------------------------------------
TResult UnvFundsRulesDialog::EvCommand(uint id, THandle hWndCtl, uint notifyCode)
{
  TResult result;

  result = FormDialog::EvCommand(id, hWndCtl, notifyCode);

  switch ( notifyCode )
   {
    case BN_CLICKED:
    case EN_CHANGE:  if ( SetupComplete )
                       ChangeHandler(id);
                     break;
   }

  return result;
}

//----------------------------------------------------------
// Update the entire window for the current transaction type
//----------------------------------------------------------

void UnvFundsRulesDialog::UpdateWin()
{
}

//-----------------------------------------------------
// Override virtual function to provide initial display
//-----------------------------------------------------
void UnvFundsRulesDialog::SetupWindow()
{
 TDialog::SetupWindow();
 UpdateWin();
 SetupComplete=true;
}

//----------------------
// Query modified status
//----------------------
bool UnvFundsRulesDialog::IsModified()
{
 if ( FormDialog::IsModified() )
  return true;
 else
  return (memcmp(&origRec->Data,&newRec.Data,
                  sizeof(origRec->Data)) ==0) ? false : true;
}

//-------------------------
// Save Changes to DataBase
//-------------------------
bool UnvFundsRulesDialog::Save()
{
 DBManMessage Msg;

 if ( ! Msg.DBPut(this,T_PUTSECRULES,FN_SECRULESRECORD,&newRec) )
  return false;

 ResetModified();
 return true;
}

//--------------------------
// Save changes to temp file
//--------------------------
bool UnvFundsRulesDialog::SaveTemp(TFile* File)
{
 SAVERECORD_SENTINEL Sentinel;

 Sentinel.Type = GetType();
 Sentinel.Version = SaveRecordVersion[Sentinel.Type];
 Sentinel.Len = sizeof(newRec.Data);

 if ( File->Write(&Sentinel,sizeof(Sentinel)) != sizeof(Sentinel) ||
      File->Write(&newRec.Data,sizeof(newRec.Data))
            != sizeof(newRec.Data) )
  return false;

 ResetModified();
 return true;

}

//------------------------
// Reset the modified flag
//------------------------
void UnvFundsRulesDialog::ResetModified()
{
 memcpy(&origRec->Data,&newRec.Data,sizeof(origRec->Data));
 FormDialog::ResetModified();
}


void UnvFundsRulesDialog::ChangeHandler(uint Id)
{
}


//------------------------------
// Perform all Field validations
//------------------------------
bool UnvFundsRulesDialog::Validate()
{
}



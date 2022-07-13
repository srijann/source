//----------------------------------------------------------------------------
//  Project spsmanager
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    spsmanager.apx Application
//  FILE:         posfilerulesdialog.cpp
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of PosFileRulesDialog (TDialog).
//
//----------------------------------------------------------------------------
#include "win4me.h"
#pragma hdrstop

#include "posfilerulesdialog.h"
#include "DecimalValidator.h"
#include "DBManMsg.h"
#include "TranType.h"
#include "SaveFile.h"

//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(PosFileRulesDialog, TDialog)
//{{PosFileRulesDialogRSP_TBL_BEGIN}}
//{{PosFileRulesDialogRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{PosFileRulesDialog Implementation}}

// Macro to define a Row of controls
#define DEFROW(n) Row[n-1].AMT = new TEditUns(this,IDC_POSLIMITAMT##n,5);\
                   for (i=1; i<=NUMCOLS; ++i)\
                    {\
                     Row[n-1].MULTI[i-1] = new TEdit(this,IDC_POSLIMITAMT##n +i,4);\
                     Row[n-1].MULTI[i-1]->SetValidator( new TDecimalValidator(1,1) );\
                    }

PosFileRulesDialog::PosFileRulesDialog(TWindow* parent, PositiveRules* OrigRules)
:
  FormDialog(parent, IDD_POSFILELIMITS,RecTypePosfileRules),
  origRules(OrigRules),
  SetupComplete(false)
{
 int i;
 char Title[100];

 strcpy(Title,(char*)origRules->Data.PositiveRulesClassName);
 strcat(Title," Positive File Rules");
 SetCaption(Title);
 memcpy(&newRules.Data,&origRules->Data,sizeof(newRules.Data));

 POSMAXOVERRIDES=new TEditUns(this,IDC_POSMAXOVERRIDES,3);
 POSPERIOD=new TEditUns(this,IDC_POSPERIOD,3);
 WAITBEFOREPOSITIVE = new TEditUns(this,IDC_WAITBEFOREPOSITIVE,3);
 MULTIPLYNUMAPPROVED = new TEditUns(this,IDC_MULTIPLYNUMAPPROVED,3);
 MULTIPLYDOLLARSAPPROVED = new TEditUns(this,IDC_MULTIPLYDOLLARSAPPROVED,7);
 MULTIPLYMONTHS = new TEditUns(this,IDC_MULTIPLYMONTHS,3);
 OVERRIDEACCUM = new TEditUns(this,IDC_OVERRIDEACCUM,7);

 DEFROW(1);
 DEFROW(2);
 DEFROW(3);
 DEFROW(4);
 DEFROW(5);
 DEFROW(6);
 DEFROW(7);
 DEFROW(8);
 DEFROW(9);
}


PosFileRulesDialog::~PosFileRulesDialog()
{
 Destroy(IDCANCEL);
 delete origRules;
}

void PosFileRulesDialog::SetupWindow()
{
 int r, c;

 TDialog::SetupWindow();
 POSMAXOVERRIDES->Set(origRules->Data.MaxOverrides);
 POSPERIOD->Set(origRules->Data.OverridePeriod);
 WAITBEFOREPOSITIVE->Set(origRules->Data.WaitForPositiveDays);
 MULTIPLYNUMAPPROVED->Set(origRules->Data.MultiplyIfNumApprovals);
 MULTIPLYDOLLARSAPPROVED->Set(origRules->Data.MultiplyIfAmountApprovals);
 MULTIPLYMONTHS->Set(origRules->Data.MultiplyIfLastWithinMonths);
 OVERRIDEACCUM->Set(origRules->Data.MaxOverrideAccum);

 for (r=0; r<NUMROWS; ++r )
  {
   Row[r].AMT->Set(origRules->Data.AgeTbl[r].Limit);
   for (c=0; c<NUMCOLS; ++c)
    multiplierOut(Row[r].MULTI[c],origRules->Data.AgeTbl[r].Multiplier[c][0]);
  }

 SetupComplete=true;
}

//----------------------------------------------
// Transfer an Edit field to a multiplier Value
//----------------------------------------------
void PosFileRulesDialog::multiplierIn(TEdit* Ctl,unsigned char& Val)
{
 char Buf[10];
 char *dp;

 Ctl->Transfer(Buf,tdGetData);
 if ( Buf[0] == 0 )
  Empty(Val);
 else
  {
   Val = (unsigned char) (atoi(Buf) * 10);
   if ( (dp=strchr(Buf,'.')) != NULL )
     Val += (unsigned char) atoi(&dp[1]);
   // If it's 0 or 1.0 set it to empty 
   if ( ! Val || Val == 10 )
    Empty(Val);
  }
}

//---------------------------------------------
// Transfer a multiplier value to an edit field
//---------------------------------------------
void PosFileRulesDialog::multiplierOut(TEdit* Ctl,unsigned char Val)
{
 char Buf[10];

 if ( isEmpty(Val) )
  Buf[0] = 0;
 else
  sprintf(Buf,"%d.%0d",Val/10,Val%10);
 Ctl->Transfer(Buf,tdSetData);
}

//----------------------
// Query modified status
//----------------------
bool PosFileRulesDialog::IsModified()
{
 if ( FormDialog::IsModified() )
  return true;
 else
  return (memcmp(&origRules->Data,&newRules.Data,
                  sizeof(origRules->Data)) ==0) ? false : true;
}

//-------------------------
// Save Changes to DataBase
//-------------------------
bool PosFileRulesDialog::Save()
{
 DBManMessage Msg;

 if ( ! Validate() )
  return false;

 if ( ! Msg.DBPut(this,T_PUTPOSFILERULES,FN_POSITIVERULESRECORD,&newRules) )
  return false;

 ResetModified();
 return true;
}

//--------------------------
// Save changes to temp file
//--------------------------
bool PosFileRulesDialog::SaveTemp(TFile* File)
{
 SAVERECORD_SENTINEL Sentinel;

 Sentinel.Type = GetType();
 Sentinel.Version = SaveRecordVersion[Sentinel.Type];
 Sentinel.Len = sizeof(newRules.Data);

 if ( File->Write(&Sentinel,sizeof(Sentinel)) != sizeof(Sentinel) ||
      File->Write(&newRules.Data,sizeof(newRules.Data))
            != sizeof(newRules.Data) )
  return false;

 ResetModified();
 return true;
}

//------------------------
// Reset the modified flag
//------------------------
void PosFileRulesDialog::ResetModified()
{
 memcpy(&origRules->Data,&newRules.Data,sizeof(origRules->Data));
 FormDialog::ResetModified();
}

TResult PosFileRulesDialog::EvCommand(uint id, THandle hWndCtl, uint notifyCode)
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

//---------------------------------------------------
// Handle changes, transferring controls to variables
//---------------------------------------------------
#define GET(name,parm) case IDC_##name:\
                         name->Get(newRules.Data.parm); break
void PosFileRulesDialog::ChangeHandler(uint Id)
{
 int row;
 int col;

 switch (Id)
  {
   GET(POSMAXOVERRIDES,MaxOverrides);
   GET(POSPERIOD,OverridePeriod);
   GET(WAITBEFOREPOSITIVE,WaitForPositiveDays);
   GET(MULTIPLYNUMAPPROVED,MultiplyIfNumApprovals);
   GET(MULTIPLYDOLLARSAPPROVED,MultiplyIfAmountApprovals);
   GET(MULTIPLYMONTHS,MultiplyIfLastWithinMonths);
   GET(OVERRIDEACCUM,MaxOverrideAccum);
   default:
    row=Id/1000;
    col=Id%1000;
    if ( row < 1 || row > NUMROWS ||
         col > NUMCOLS )
     return;
    --row; 
    if ( col==0 )
      Row[row].AMT->Get(newRules.Data.AgeTbl[row].Limit);
    else
      multiplierIn(Row[row].MULTI[col-1],
                     newRules.Data.AgeTbl[row].Multiplier[col-1][0]);
  }

}

//--------------------
// Do field validation
//--------------------
#define REQUIRE(name,fld) if ( isEmpty(newRules.Data.fld) )\
                           {\
                            MessageBox("FIELD REQUIRED");\
                            name->SetFocus();\
                            return false;\
                           }

bool PosFileRulesDialog::Validate()
{
 int i;
 
 // All these are required
 REQUIRE(POSMAXOVERRIDES,MaxOverrides);
 REQUIRE(POSPERIOD,OverridePeriod);
 REQUIRE(WAITBEFOREPOSITIVE,WaitForPositiveDays);
 REQUIRE(OVERRIDEACCUM,MaxOverrideAccum);

 // If any of the multiplier fields are present then they are all
 // required.
 if ( ! isEmpty(newRules.Data.MultiplyIfAmountApprovals) ||
      ! isEmpty(newRules.Data.MultiplyIfNumApprovals) ||
      ! isEmpty(newRules.Data.MultiplyIfLastWithinMonths) )
  {
   REQUIRE(MULTIPLYNUMAPPROVED,MultiplyIfNumApprovals);
   REQUIRE(MULTIPLYDOLLARSAPPROVED,MultiplyIfAmountApprovals);
   REQUIRE(MULTIPLYMONTHS,MultiplyIfLastWithinMonths);
  }

 // All of the Age/Multiplier Limits are required
 for (i=0; i<NUMROWS; ++i)
  if ( isEmpty(newRules.Data.AgeTbl[i].Limit) )
   {
    MessageBox("FIELD REQUIRED");
    Row[i].AMT->SetFocus();
    return false;
   }

 return true;
}


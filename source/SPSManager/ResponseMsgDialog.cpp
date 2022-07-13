//----------------------------------------------------------------------------
//  Project spsmanager
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    spsmanager.apx Application
//  FILE:         responsemsgdialog.cpp
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of ResponseMsgDialog (TDialog).
//
//----------------------------------------------------------------------------
#include "win4me.h"
#include "responsemsgdialog.h"
#include "TranType.h"
#include "CheckAuthFn.h"
#include "DBManMsg.h"
#include "SaveFile.h"

//{{ResponseMsgDialog Implementation}}

// Macro for creating Control Interfaces for the Fields
#define FLD(n) Flds[n-20]=new TEdit(this,IDC_RC##n,33);

ResponseMsgDialog::ResponseMsgDialog(TWindow* parent,PosResponse* OrigRec)
:
 FormDialog(parent,IDD_POSRESPONSE,RecTypePosResponseMsg),
 origRec(OrigRec),
 SetupComplete(false)
{
 char Title[100];

 strcpy(Title,(char*)origRec->Data.ResponseClassName);
 strcat(Title," Response Messages");
 SetCaption(Title);
 memcpy(&newRec.Data,&origRec->Data,sizeof(newRec.Data));
 memset(Flds,0,sizeof(Flds));
 
 FLD(21);
 FLD(22);
 FLD(23);
 FLD(24);
 FLD(25);
 FLD(26);
 FLD(27);
 FLD(28);
 FLD(30);
 FLD(31);
 FLD(40);
 FLD(50);
 FLD(51);
 FLD(52);
 FLD(53);
 FLD(54);
 FLD(55);
 FLD(56);
 FLD(57);
 FLD(58);
 FLD(59);
 FLD(90);
 FLD(91);
 FLD(92);
 FLD(93);
 FLD(94);
 FLD(95);
 FLD(96);
 FLD(97);
 FLD(98);
 FLD(99);
 FLD(80);
 FLD(81);
 FLD(82);
 FLD(83);
 FLD(84);
 FLD(85);
 FLD(87);
 FLD(73);
 FLD(74);
}


ResponseMsgDialog::~ResponseMsgDialog()
{
  Destroy(IDCANCEL);
  delete origRec;
}

//----------------------
// Query modified status
//----------------------
bool ResponseMsgDialog::IsModified()
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
bool ResponseMsgDialog::Save()
{
 DBManMessage Msg;

 if ( ! Msg.DBPut(this,T_PUTPOSRESPONSEREC,FN_POSRESPONSERECORD,&newRec) )
  return false;

 ResetModified();
 return true;
}

//--------------------------
// Save changes to temp file
//--------------------------
bool ResponseMsgDialog::SaveTemp(TFile* File)
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
void ResponseMsgDialog::ResetModified()
{
 memcpy(&origRec->Data,&newRec.Data,sizeof(origRec->Data));
 FormDialog::ResetModified();
}

void ResponseMsgDialog::SetupWindow()
{
 int i;

 TDialog::SetupWindow();
 for (i=0; i<NUMFIELDS; ++i)
  if ( Flds[i] )
   Flds[i]->Transfer(newRec.Data.ResponseText[i],tdSetData);
 SetupComplete=true;
}

//------------------------------------------------------
// Handle changes to fields, transferring the new values
//------------------------------------------------------
TResult ResponseMsgDialog::EvCommand(uint id, THandle hWndCtl, uint notifyCode)
{
  TResult result;
  int i;

  result = FormDialog::EvCommand(id, hWndCtl, notifyCode);

  switch ( notifyCode )
   {
    case EN_CHANGE:  if ( SetupComplete &&
                          (i=id-120) < 100 &&
                          Flds[i] )
                       Flds[i]->Transfer(newRec.Data.ResponseText[i],tdGetData);
                     break;
   }

  return result;

}

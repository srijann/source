//----------------------------------------------------------------------------
//  Project SPSFileEdit
//  Secure Payment Systems
//  Copyright © 1997. All Rights Reserved.
//
//  SUBSYSTEM:    SPSFileEdit.apx Application
//  FILE:         bankphonedialog.cpp
//  AUTHOR:       Jack Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of BankPhoneDialog (TDialog).
//
//----------------------------------------------------------------------------
#include "win4me.h"

#include "bankphonedialog.h"
#include <string.h>


#define MAXEDITBUF 50
//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(BankPhoneDialog, TDialog)
//{{BankPhoneDialogRSP_TBL_BEGIN}}
  EV_COMMAND(CM_EDITCLEARALL, CmEditclearall),
  EV_COMMAND_ENABLE(CM_EDITCLEARALL, CeEditclearall),
  EV_COMMAND(CM_FILEADD, CmFileadd),
  EV_COMMAND_ENABLE(CM_FILEADD, CeFileadd),
  EV_COMMAND(CM_FILECHANGE, CmFilechange),
  EV_COMMAND_ENABLE(CM_FILECHANGE, CeFilechange),
  EV_COMMAND(CM_FILEDELETE, CmFiledelete),
  EV_COMMAND_ENABLE(CM_FILEDELETE, CeFiledelete),
  EV_COMMAND(CM_GETRECORD, CmGetrecord),
  EV_COMMAND_ENABLE(CM_GETRECORD, CeGetrecord),
//{{BankPhoneDialogRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{BankPhoneDialog Implementation}}


BankPhoneDialog::BankPhoneDialog(TWindow* parent, TResId resId)
:
 FileEditDialog(parent, resId),
 haveCurrentRec(false),
 newRecIsCurrentRec(false),
 hasBankNumber(false)
{
 BANKNUMBER = new TEdit(this,IDC_BANKNUMBER,sizeof(newRec.BankNumber));
 BANKNAME = new TEdit(this,IDC_BANKNAME,sizeof(newRec.BankName));
 PHONENUMBER = new TEdit(this,IDC_PHONENUMBER,sizeof(newRec.PhoneNumber));
 COMMENT = new TEdit(this,IDC_COMMENT,sizeof(newRec.Comment));
 memset(&newRec,0,sizeof(newRec));
 memset(&currentRec,0,sizeof(currentRec));
}


BankPhoneDialog::~BankPhoneDialog()
{
  Destroy(IDCANCEL);
}

//---------------------
// Clear all the fields
//---------------------
void BankPhoneDialog::CmEditclearall()
{
 memset(&newRec,0,sizeof(newRec));
 memset(&currentRec,0,sizeof(currentRec));
 haveCurrentRec=newRecIsCurrentRec=hasBankNumber=false;
 Display();
 BANKNUMBER->SetFocus();
}


void BankPhoneDialog::CeEditclearall(TCommandEnabler &tce)
{
 tce.Enable( hasData(BANKNUMBER) ||
             hasData(BANKNAME) ||
             hasData(PHONENUMBER) ||
             hasData(COMMENT) );
}


void BankPhoneDialog::CmFileadd()
{
 HWND captureWin;
 DBRESULT Result;
 bool ret;
 int  Len;

 if ( ! editData() )
  return;

 Msg.ClearMsg();
 Msg.PutFld(FN_TRANTYPE,T_ADDRECORD);
 Msg.PutFld(FN_BANKPHONERECORD,&newRec,sizeof(newRec));
 captureWin=GetApplication()->GetMainWindow()->SetCapture();
 GetApplication()->GetMainWindow()->SetCursor(0,IDC_WAIT);
 ret = Msg.SendRecv(BANKPHONE_DEST,(WORD)7500,true);
 GetApplication()->GetMainWindow()->SetCursor(0,IDC_ARROW);
 ::SetCapture(captureWin);

 if ( ! ret )
  return;

 // Presence of DBRESULT structure indicates a problem
 Len=sizeof(DBRESULT);
 if ( Msg.GetFld(FN_DBRESULT,&Result,Len) )
  {
   MessageBox(Result.Text,"Add Failed",MB_OK);
   return;
  }

 Len=sizeof(newRec);
 memset(&newRec,0,sizeof(newRec));
 Msg.GetFld(FN_BANKPHONERECORD,&newRec,Len);
 memcpy(&currentRec,&newRec,sizeof(currentRec));
 haveCurrentRec=newRecIsCurrentRec=true;
 Display();
 BANKNUMBER->SetFocus();
}


void BankPhoneDialog::CeFileadd(TCommandEnabler &tce)
{
 tce.Enable( hasBankNumber &&
             (haveCurrentRec==false || newRecIsCurrentRec==false) );
}


void BankPhoneDialog::CmFilechange()
{
 HWND captureWin;
 DBRESULT Result;
 bool ret;
 int  Len;

 if ( ! editData() )
  return;

 Msg.ClearMsg();
 Msg.PutFld(FN_TRANTYPE,T_CHANGERECORD);

 Msg.PutFld(FN_BANKPHONERECORD,&newRec,sizeof(newRec));
 captureWin=GetApplication()->GetMainWindow()->SetCapture();
 GetApplication()->GetMainWindow()->SetCursor(0,IDC_WAIT);
 ret = Msg.SendRecv(BANKPHONE_DEST,(WORD)7500,true);
 GetApplication()->GetMainWindow()->SetCursor(0,IDC_ARROW);
 ::SetCapture(captureWin);

 if ( ! ret )
  return;

 // Presence of DBRESULT structure indicates a problem
 Len=sizeof(DBRESULT);
 if ( Msg.GetFld(FN_DBRESULT,&Result,Len) )
  {
   MessageBox(Result.Text,"Change Failed",MB_OK);
   return;
  }

 memcpy(&currentRec,&newRec,sizeof(currentRec));
 haveCurrentRec=newRecIsCurrentRec=true;
}


void BankPhoneDialog::CeFilechange(TCommandEnabler &tce)
{
 tce.Enable( hasBankNumber &&
             haveCurrentRec &&
             newRecIsCurrentRec &&
             memcmp(&newRec,&currentRec,sizeof(newRec)) );
}


void BankPhoneDialog::CmFiledelete()
{
 HWND captureWin;
 DBRESULT Result;
 bool ret;
 int  Len;

 Msg.ClearMsg();
 Msg.PutFld(FN_TRANTYPE,T_DELETERECORD);
 Msg.PutFld(FN_BANKNUMBER,currentRec.BankNumber);

 captureWin=GetApplication()->GetMainWindow()->SetCapture();
 GetApplication()->GetMainWindow()->SetCursor(0,IDC_WAIT);
 ret = Msg.SendRecv(BANKPHONE_DEST,(WORD)7500,true);
 GetApplication()->GetMainWindow()->SetCursor(0,IDC_ARROW);
 ::SetCapture(captureWin);

 if ( ! ret )
  return;

 // Presence of DBRESULT structure indicates a problem
 Len=sizeof(DBRESULT);
 if ( Msg.GetFld(FN_DBRESULT,&Result,Len) )
  {
   MessageBox(Result.Text,"Delete Failed",MB_OK);
   return;
  }

 haveCurrentRec=newRecIsCurrentRec=false;
 BANKNUMBER->SetFocus();
}


void BankPhoneDialog::CeFiledelete(TCommandEnabler &tce)
{
 tce.Enable( hasBankNumber && haveCurrentRec && newRecIsCurrentRec );
}


void BankPhoneDialog::CmGetrecord()
{
 HWND captureWin;
 DBRESULT Result;
 bool ret;
 int  Len;

 Msg.ClearMsg();
 Msg.PutFld(FN_TRANTYPE,T_GETRECORD);
 Msg.PutFld(FN_BANKNUMBER,newRec.BankNumber);

 captureWin=GetApplication()->GetMainWindow()->SetCapture();
 GetApplication()->GetMainWindow()->SetCursor(0,IDC_WAIT);
 ret = Msg.SendRecv(BANKPHONE_DEST,(WORD)7500,true);
 GetApplication()->GetMainWindow()->SetCursor(0,IDC_ARROW);
 ::SetCapture(captureWin);

 if ( ! ret )
  return;

 // Presence of DBRESULT structure indicates a problem
 Len=sizeof(DBRESULT);
 if ( Msg.GetFld(FN_DBRESULT,&Result,Len) )
  {
   MessageBox(Result.Text,"Get Failed",MB_OK);
   return;
  }

 Len=sizeof(newRec);
 memset(&newRec,0,sizeof(newRec));
 Msg.GetFld(FN_BANKPHONERECORD,&newRec,Len);
 memcpy(&currentRec,&newRec,sizeof(currentRec));

 haveCurrentRec=newRecIsCurrentRec=true;
 Display();
 BANKNUMBER->SetFocus();
}


void BankPhoneDialog::CeGetrecord(TCommandEnabler &tce)
{
 tce.Enable( hasBankNumber );
}


TResult BankPhoneDialog::EvCommand(uint id, THandle hWndCtl, uint notifyCode)
{
  TResult result;

  switch ( notifyCode )
   {
    case 0:
       if ( id==IDCANCEL || id==IDOK )
         return 0;
      break;

    case EN_CHANGE:  ChangeHandler(id);
                     break;
   }

  result = TDialog::EvCommand(id, hWndCtl, notifyCode);
  return result;
}

void BankPhoneDialog::ChangeHandler(uint id)
{
 switch(id)
  {
   case IDC_BANKNUMBER:  BANKNUMBER->Transfer(newRec.BankNumber,tdGetData);
                         break;
   case IDC_BANKNAME:    BANKNAME->Transfer(newRec.BankName,tdGetData);
                         break;
   case IDC_PHONENUMBER: PHONENUMBER->Transfer(newRec.PhoneNumber,tdGetData);
                         break;
   case IDC_COMMENT:     COMMENT->Transfer(newRec.Comment,tdGetData);
                         break;
  }

 hasBankNumber = strlen(newRec.BankNumber) >= 8;

 // If we have a current record, check to see if it's the same
 // as the new record. Consider it the same record if at least one
 // of the keys matches
 if ( haveCurrentRec )
   newRecIsCurrentRec =
     (strcmp(newRec.BankNumber,currentRec.BankNumber) == 0 );
}

//---------------------------
// Test if a control has data
//---------------------------
int BankPhoneDialog::hasData(TEdit* Ctl)
{
 char Buf[MAXEDITBUF];

 Ctl->Transfer(Buf,tdGetData);
 return strlen(Buf);
}

//---------------------------
// Display the Current Record
//---------------------------
void BankPhoneDialog::Display()
{
 BANKNUMBER->Transfer(currentRec.BankNumber,tdSetData);
 BANKNAME->Transfer(currentRec.BankName,tdSetData);
 PHONENUMBER->Transfer(currentRec.PhoneNumber,tdSetData);
 COMMENT->Transfer(currentRec.Comment,tdSetData);
}

//----------------------------
// Edit data before committing
//----------------------------
bool BankPhoneDialog::editData()
{
 return true;
}




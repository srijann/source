//----------------------------------------------------------------------------
//  Project PosFileMan
//  Secure Payment Systems
//  Copyright © 1997. All Rights Reserved.
//
//  SUBSYSTEM:    PosFileMan Application
//  FILE:         posfilemandlgclient.cpp
//  AUTHOR:       Jack Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of TPosFileManDlgClient (TDialog).
//
//----------------------------------------------------------------------------

#include "win4me.h"

#include <stdio.h>
#include <time.h>
#include "posfilemandlgclient.h"

//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(TPosFileManDlgClient, TDialog)
//{{TPosFileManDlgClientRSP_TBL_BEGIN}}
  EV_COMMAND_ENABLE(CM_GETRECORD, CeGetrecord),
  EV_COMMAND_ENABLE(CM_FILEDELETE, CeFiledelete),
  EV_COMMAND_ENABLE(CM_FILECHANGE, CeFilechange),
  EV_COMMAND_ENABLE(CM_FILEADD, CeFileadd),
  EV_COMMAND(CM_EDITCLEARALL, CmEditclearall),
  EV_COMMAND_ENABLE(CM_EDITCLEARALL, CeEditclearall),
  EV_COMMAND(CM_GETRECORD, CmGetrecord),
  EV_COMMAND(CM_FILEADD, CmFileadd),
  EV_COMMAND(CM_FILECHANGE, CmFilechange),
  EV_COMMAND(CM_FILEDELETE, CmFiledelete),
//{{TPosFileManDlgClientRSP_TBL_END}}
END_RESPONSE_TABLE;

#define MAXEDITBUF 26

//{{TPosFileManDlgClient Implementation}}

#define EDIT(name,len) name=new TEdit(this,IDC_##name,len+1)

//--------------------------------------------------------
// TPosFileManDlgClient
// ~~~~~~~~~~
// Construction/Destruction handling.
//
TPosFileManDlgClient::TPosFileManDlgClient(TWindow* parent, TResId resId, TModule*)
:
  FileEditDialog(parent, resId),
  haveCurrentRec(false),
  hasIds(false),
  newRecIsCurrentRec(false)
{
 EDIT(BANKNUMBER,9);
 EDIT(BANKACCOUNT,16);
 EDIT(LICENSESTATE,2);
 EDIT(LICENSE,15);
 EDIT(CONSUMERNAME,25);
 EDIT(BUSINESSNAME,25);
 EDIT(OVERRIDESALLOWED,2);
 EDIT(OVERRIDEDAYS,3);
 EDIT(MAXOVERRIDEAMOUNT,5);
 EDIT(MAXOVERRIDEACCUM,5);
 EDIT(ACCOUNT1,3);
 EDIT(ACCOUNT2,3);
 EDIT(ACCOUNT3,3);
 EDIT(SIC1,4);
 EDIT(SIC2,4);
 EDIT(SIC3,4);
 EDIT(DATEADDED,0);
 EDIT(DATELASTAPPROVAL,0);
 EDIT(APPROVEDTRANSACTIONS,0);
 EDIT(APPROVEDDOLLARS,0);
 EDIT(DOB,6);
 EDIT(PHONE,10);
 memset(&newRec,0,sizeof(newRec));
}

//-----------------
// Class Destructor
//-----------------
TPosFileManDlgClient::~TPosFileManDlgClient()
{
  Destroy();
}


//-----------------
// Clear all fields
//-----------------
void TPosFileManDlgClient::CmEditclearall()
{
 clearAll();
 memset(&newRec,0,sizeof(newRec));
 memset(&currentRec,0,sizeof(currentRec));
 haveCurrentRec=newRecIsCurrentRec=false;
 BANKNUMBER->SetFocus();
}

//------------------
// Clear All enabler
//------------------
void TPosFileManDlgClient::CeEditclearall(TCommandEnabler &tce)
{
 tce.Enable(
  hasData(BANKNUMBER) ||
  hasData(BANKACCOUNT) ||
  hasData(LICENSESTATE) ||
  hasData(LICENSE) ||
  hasData(CONSUMERNAME) ||
  hasData(BUSINESSNAME) ||
  hasData(OVERRIDESALLOWED) ||
  hasData(OVERRIDEDAYS) ||
  hasData(MAXOVERRIDEAMOUNT) ||
  hasData(MAXOVERRIDEACCUM) ||
  hasData(ACCOUNT1) ||
  hasData(ACCOUNT2) ||
  hasData(ACCOUNT3) ||
  hasData(SIC1) ||
  hasData(SIC2) ||
  hasData(SIC3) ||
  hasData(DATEADDED) ||
  hasData(DATELASTAPPROVAL) ||
  hasData(APPROVEDTRANSACTIONS) ||
  hasData(APPROVEDDOLLARS) ||
  hasData(DOB) ||
  hasData(PHONE) );

}

//----------------
// Command Handler
//----------------
TResult TPosFileManDlgClient::EvCommand(uint id, THandle hWndCtl, uint notifyCode)
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

//------------------------
// Handle change to fields
//------------------------
#define TEXTCHANGE(name,fldname) case IDC_##name:\
                             name->Transfer(newRec.fldname,tdGetData);\
                             break
#define NUMCHANGE(name,fldname) case IDC_##name:\
                             EditToNum(name,newRec.fldname);\
                             break                              
void TPosFileManDlgClient::ChangeHandler(uint id)
{
 switch(id)
  {
   TEXTCHANGE(BANKNUMBER,BankNumber);
   TEXTCHANGE(BANKACCOUNT,BankAccount);
   TEXTCHANGE(LICENSESTATE,LicenseState);
   TEXTCHANGE(LICENSE,License);
   TEXTCHANGE(CONSUMERNAME,ConsumerInfo.ConsumerName);
   TEXTCHANGE(BUSINESSNAME,ConsumerInfo.BusinessName);
   TEXTCHANGE(PHONE,PhoneKey);
   TEXTCHANGE(ACCOUNT1,Parms.AccountRestrictions[0]);
   TEXTCHANGE(ACCOUNT2,Parms.AccountRestrictions[1]);
   TEXTCHANGE(ACCOUNT3,Parms.AccountRestrictions[2]);
   NUMCHANGE(OVERRIDESALLOWED,Parms.NumberOfOverridesAllowed);
   NUMCHANGE(OVERRIDEDAYS,Parms.OverridePeriod);
   NUMCHANGE(MAXOVERRIDEAMOUNT,Parms.MaxOverrideAmount);
   NUMCHANGE(MAXOVERRIDEACCUM,Parms.MaxOverrideAccum);
   NUMCHANGE(SIC1,Parms.SicCodeRestrictions[0]);
   NUMCHANGE(SIC2,Parms.SicCodeRestrictions[1]);
   NUMCHANGE(SIC3,Parms.SicCodeRestrictions[2]);
   NUMCHANGE(DOB,DateOfBirth);
  }

 // Set the hasIds flag to indicate if we have valid ids on the screen 
 hasIds = ( (strlen(newRec.BankNumber) == 9 &&
             strlen(newRec.BankAccount)) ||
            strlen(newRec.PhoneKey) == 10  || 
            (strlen(newRec.LicenseState)== 2 &&
             strlen(newRec.License)) );

 // If we have a current record, check to see if it's the same
 // as the new record. Consider it the same record if at least one
 // of the keys matches
 if ( haveCurrentRec )
   newRecIsCurrentRec = (newRec.BankNumber[0] &&
                         memcmp(newRec.BankNumber,
                                currentRec.BankNumber,
                                sizeof(struct MicrKey)) == 0) ||

                        (newRec.PhoneKey[0] &&
                         strcmp(newRec.PhoneKey,currentRec.PhoneKey) == 0 ) ||

                        (newRec.LicenseState[0] &&
                         memcmp(newRec.LicenseState,
                                currentRec.LicenseState,
                                sizeof(struct LicenseKey)) ==0 );
}

//---------------------------
// Transfer control to binary
//---------------------------
void TPosFileManDlgClient::EditToNum(TEdit* Ctl,unsigned short& Num)
{
 char Buf[10];
 Ctl->Transfer(Buf,tdGetData);
 if ( Buf[0] )
  Num = (unsigned short) atoi(Buf);
 else
  Num = 0;
}

//---------------------------
// Transfer control to binary
//---------------------------
void TPosFileManDlgClient::EditToNum(TEdit* Ctl,unsigned long& Num)
{
 char Buf[10];
 Ctl->Transfer(Buf,tdGetData);
 if ( Buf[0] )
  Num = (unsigned long) atoi(Buf);
 else
  Num = 0;
}

//---------------------------
// Transfer control to binary
//---------------------------
void TPosFileManDlgClient::NumToEdit(TEdit* Ctl,unsigned long Num,int Len)
{
 char Buf[20];

 if ( Len )
  sprintf(Buf,"%0*d",Len,Num);
 else
  sprintf(Buf,"%d",Num);
 Ctl->Transfer(Buf,tdSetData);
}

//---------------------------
// Test if a control has data
//---------------------------
int TPosFileManDlgClient::hasData(TEdit* Ctl)
{
 char Buf[MAXEDITBUF];

 Ctl->Transfer(Buf,tdGetData);
 return strlen(Buf);
}

//---------------------------
// Display the Current Record
//---------------------------
#define TEXTOUT(name,fldname) name->Transfer(currentRec.fldname,tdSetData)
#define NUMOUT(name,fldname) NumToEdit(name,currentRec.fldname)
#define FNUMOUT(name,fldname,len) NumToEdit(name,currentRec.fldname,len)
void TPosFileManDlgClient::Display()
{
 clearAll();
 TEXTOUT(BANKNUMBER,BankNumber);
 TEXTOUT(BANKACCOUNT,BankAccount);
 TEXTOUT(LICENSESTATE,LicenseState);
 TEXTOUT(LICENSE,License);
 if ( currentRec.DateOfBirth )
  FNUMOUT(DOB,DateOfBirth,6);
 DateToEdit(DATEADDED,currentRec.FirstApproval);

 if ( currentRec.LastApproval )
   DateToEdit(DATELASTAPPROVAL,currentRec.LastApproval);

 NUMOUT(APPROVEDTRANSACTIONS,NumApprovals);
 NUMOUT(APPROVEDDOLLARS,AmountApprovals);

 TEXTOUT(CONSUMERNAME,ConsumerInfo.ConsumerName);
 TEXTOUT(BUSINESSNAME,ConsumerInfo.BusinessName);
 TEXTOUT(PHONE,PhoneKey);

 if ( currentRec.Parms.OverridePeriod )
  {
   TEXTOUT(ACCOUNT1,Parms.AccountRestrictions[0]);
   TEXTOUT(ACCOUNT2,Parms.AccountRestrictions[1]);
   TEXTOUT(ACCOUNT3,Parms.AccountRestrictions[2]);
   NUMOUT(OVERRIDESALLOWED,Parms.NumberOfOverridesAllowed);
   NUMOUT(OVERRIDEDAYS,Parms.OverridePeriod);
   NUMOUT(MAXOVERRIDEAMOUNT,Parms.MaxOverrideAmount);
   NUMOUT(MAXOVERRIDEACCUM,Parms.MaxOverrideAccum);
   if ( currentRec.Parms.SicCodeRestrictions[0] )
     NUMOUT(SIC1,Parms.SicCodeRestrictions[0]);
   if ( currentRec.Parms.SicCodeRestrictions[1] )
     NUMOUT(SIC2,Parms.SicCodeRestrictions[1]);
   if ( currentRec.Parms.SicCodeRestrictions[2] )
     NUMOUT(SIC3,Parms.SicCodeRestrictions[2]);
  }

}

//-----------------
// Clear All fields
//-----------------
void TPosFileManDlgClient::clearAll()
{
 BANKNUMBER->Transfer("",tdSetData);
 BANKACCOUNT->Transfer("",tdSetData);
 LICENSESTATE->Transfer("",tdSetData);
 LICENSE->Transfer("",tdSetData);
 CONSUMERNAME->Transfer("",tdSetData);
 BUSINESSNAME->Transfer("",tdSetData);
 OVERRIDESALLOWED->Transfer("",tdSetData);
 OVERRIDEDAYS->Transfer("",tdSetData);
 MAXOVERRIDEAMOUNT->Transfer("",tdSetData);
 MAXOVERRIDEACCUM->Transfer("",tdSetData);
 ACCOUNT1->Transfer("",tdSetData);
 ACCOUNT2->Transfer("",tdSetData);
 ACCOUNT3->Transfer("",tdSetData);
 SIC1->Transfer("",tdSetData);
 SIC2->Transfer("",tdSetData);
 SIC3->Transfer("",tdSetData);
 DATEADDED->Transfer("",tdSetData);
 DATELASTAPPROVAL->Transfer("",tdSetData);
 APPROVEDTRANSACTIONS->Transfer("",tdSetData);
 APPROVEDDOLLARS->Transfer("",tdSetData);
 DOB->Transfer("",tdSetData);
 PHONE->Transfer("",tdSetData);
}

//---------------
// Add the record
//---------------
void TPosFileManDlgClient::CmFileadd()
{
 HWND captureWin;
 DBRESULT Result;
 bool ret;
 int  Len;

 if ( ! editData() )
  return;

 Msg.ClearMsg();
 Msg.PutFld(FN_TRANTYPE,T_ADDRECORD);
 Msg.PutFld(FN_POSITIVEFILERECORD,&newRec,sizeof(newRec));
 captureWin=GetApplication()->GetMainWindow()->SetCapture();
 GetApplication()->GetMainWindow()->SetCursor(0,IDC_WAIT);
 ret = Msg.SendRecv(POSITIVEFILE_DEST,(WORD)7500,true);
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
 Msg.GetFld(FN_POSITIVEFILERECORD,&newRec,Len);
 memcpy(&currentRec,&newRec,sizeof(currentRec));
 haveCurrentRec=newRecIsCurrentRec=true;
 Display();

 // Set focus to first key with data
 if ( currentRec.BankNumber[0] )
  {
   BANKNUMBER->SetFocus();
   BANKNUMBER->SetSelection(0,-1);
  }
 else
 if ( currentRec.LicenseState[0] )
  {
   LICENSESTATE->SetFocus();
   LICENSESTATE->SetSelection(0,-1);
  }
 else
  {
   PHONE->SetFocus();
   PHONE->SetSelection(0,-1);
  }
}

//-------------------
// Get record handler
//-------------------
void TPosFileManDlgClient::CmGetrecord()
{
 HWND captureWin;
 DBRESULT Result;
 bool ret;
 int  Len;

 Msg.ClearMsg();
 Msg.PutFld(FN_TRANTYPE,T_GETRECORD);
 if ( newRec.BankNumber[0] )
  {
   Msg.PutFld(FN_BANKNUMBER,newRec.BankNumber);
   Msg.PutFld(FN_BANKACCOUNT,newRec.BankAccount);
  }
 if( newRec.LicenseState[0] )
  {
   Msg.PutFld(FN_LICENSESTATE,newRec.LicenseState);
   Msg.PutFld(FN_LICENSE,newRec.License);
  }
 if ( newRec.PhoneKey[0] )
  Msg.PutFld(FN_PHONE,newRec.PhoneKey);

 captureWin=GetApplication()->GetMainWindow()->SetCapture();
 GetApplication()->GetMainWindow()->SetCursor(0,IDC_WAIT);
 ret = Msg.SendRecv(POSITIVEFILE_DEST,(WORD)7500,true);
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
 Msg.GetFld(FN_POSITIVEFILERECORD,&newRec,Len);
 memcpy(&currentRec,&newRec,sizeof(currentRec));
 haveCurrentRec=newRecIsCurrentRec=true;
 Display();
 BANKNUMBER->SetFocus();
}

//--------------------------------------
// Send date to control in form mm/dd/yy
//--------------------------------------
void TPosFileManDlgClient::DateToEdit(TEdit* Ctl,unsigned long Ts)
{
 struct tm* t=localtime( (time_t*)&Ts);
 char Buf[20];

 sprintf(Buf,"%02d/%02d/%02d",t->tm_mon+1,t->tm_mday,t->tm_year%100);
 Ctl->Transfer(Buf,tdSetData);
}

//-------------------------
// Change (Update) a record
//-------------------------
void TPosFileManDlgClient::CmFilechange()
{
 HWND captureWin;
 DBRESULT Result;
 bool ret;
 int  Len;

 if ( ! editData() )
  return;

 Msg.ClearMsg();
 Msg.PutFld(FN_TRANTYPE,T_CHANGERECORD);

 // Set the flag to indicate which key matches the current record.
 // The handler needs to know this so it can update the right one.
 // If none of the keys match then we should not be here because
 // change isn't allowed
 newRec.Flags &= POSFILEFLAGS_HITKEYMICR;
 newRec.Flags &= POSFILEFLAGS_HITKEYLICENSE;
 newRec.Flags &= POSFILEFLAGS_HITKEYPHONE;

 if ( newRec.BankNumber[0] &&
      strcmp(newRec.BankNumber,currentRec.BankNumber) == 0 &&
      strcmp(newRec.BankAccount,currentRec.BankAccount) == 0 )
  newRec.Flags |= POSFILEFLAGS_HITKEYMICR;
 else
 if ( newRec.LicenseState[0] &&
      strcmp(newRec.LicenseState,currentRec.LicenseState) == 0 &&
      strcmp(newRec.License,currentRec.License) == 0 )
  newRec.Flags |= POSFILEFLAGS_HITKEYLICENSE;
 else
  newRec.Flags |= POSFILEFLAGS_HITKEYPHONE;

 Msg.PutFld(FN_POSITIVEFILERECORD,&newRec,sizeof(newRec));
 captureWin=GetApplication()->GetMainWindow()->SetCapture();
 GetApplication()->GetMainWindow()->SetCursor(0,IDC_WAIT);
 ret = Msg.SendRecv(POSITIVEFILE_DEST,(WORD)7500,true);
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

//----------------
// Delete a record
//----------------
void TPosFileManDlgClient::CmFiledelete()
{
 HWND captureWin;
 DBRESULT Result;
 bool ret;
 int  Len;

 Msg.ClearMsg();
 Msg.PutFld(FN_TRANTYPE,T_DELETERECORD);
 if ( currentRec.BankNumber[0] )
  {
   Msg.PutFld(FN_BANKNUMBER,currentRec.BankNumber);
   Msg.PutFld(FN_BANKACCOUNT,currentRec.BankAccount);
  }
 if( currentRec.LicenseState[0] )
  {
   Msg.PutFld(FN_LICENSESTATE,currentRec.LicenseState);
   Msg.PutFld(FN_LICENSE,currentRec.License);
  }
 if ( currentRec.PhoneKey[0] )
  Msg.PutFld(FN_PHONE,currentRec.PhoneKey);

 captureWin=GetApplication()->GetMainWindow()->SetCapture();
 GetApplication()->GetMainWindow()->SetCursor(0,IDC_WAIT);
 ret = Msg.SendRecv(POSITIVEFILE_DEST,(WORD)7500,true);
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

//---------------------------------------------------
// Edit the data to see if Change or Add is allowable
//---------------------------------------------------
#define REQUIRE(name) if ( ! hasData(name) )\
                       {\
                        MessageBox("Parameter Value Required");\
                        name->SetFocus();\
                        return false;\
                       }

bool TPosFileManDlgClient::editData()
{
 // If Date of Birth is present, it must be valid
 if ( newRec.DateOfBirth &&
      ! isValidDate(newRec.DateOfBirth) )
  {
   MessageBox("Invalid MMDDYY Date");
   DOB->SetFocus();
   DOB->SetSelection(0,-1);
   return false;
  }

 // If any parms are present, all the numeric parms must all be present
 if ( hasData(OVERRIDESALLOWED) ||
      hasData(OVERRIDEDAYS) ||
      hasData(MAXOVERRIDEAMOUNT) ||
      hasData(MAXOVERRIDEACCUM) ||
      hasData(ACCOUNT1) ||
      hasData(ACCOUNT2) ||
      hasData(ACCOUNT3) ||
      hasData(SIC1) ||
      hasData(SIC2) ||
      hasData(SIC3) )
  {
   REQUIRE(OVERRIDESALLOWED);
   REQUIRE(OVERRIDEDAYS);
   REQUIRE(MAXOVERRIDEAMOUNT);
   REQUIRE(MAXOVERRIDEACCUM);
  }

 return true;
}

//--------------
// Validate Date
//--------------
bool TPosFileManDlgClient::isValidDate(unsigned long lDate)
{
 char Date[20];

 sprintf(Date,"%06d",lDate);

 if ( memcmp(Date,"01",2) < 0 ||
      memcmp(Date,"12",2) > 0 )
  return false;

 if ( memcmp(&Date[2],"01",2) < 0 ||
      memcmp(&Date[2],"31",2) > 0 )
  return false;

 return true;
}

//-----------------
// Get a New record
//-----------------
void TPosFileManDlgClient::CeGetrecord(TCommandEnabler &tce)
{
 tce.Enable( hasIds );
}

//----------------------
// Delete record enabler
//----------------------
void TPosFileManDlgClient::CeFiledelete(TCommandEnabler &tce)
{
 tce.Enable( hasIds && haveCurrentRec && newRecIsCurrentRec );
}

//----------------------
// Change record enabler
//----------------------
void TPosFileManDlgClient::CeFilechange(TCommandEnabler &tce)
{
 tce.Enable( hasIds &&
             haveCurrentRec &&
             newRecIsCurrentRec &&
             memcmp(&newRec,&currentRec,sizeof(newRec)) );
}

//-------------------
// Add record enabler
//-------------------
void TPosFileManDlgClient::CeFileadd(TCommandEnabler &tce)
{
 tce.Enable( hasIds &&
             (haveCurrentRec==false || newRecIsCurrentRec==false) );
}


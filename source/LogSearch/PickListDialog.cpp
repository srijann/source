//----------------------------------------------------------------------------
//  Project LogSearch
//  Secure Payment Systems
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    LogSearch.apx Application
//  FILE:         picklistdialog.cpp
//  AUTHOR:       Jack Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of PickListDialog (TDialog).
//
//----------------------------------------------------------------------------
#include "win4me.h"

#include <stdio.h>
#include <time.h>
#include "picklistdialog.h"
#include "AppNotify.h"
#include "CheckAuthFn.h"
#include "DateStuff.h"
#include "TranType.h"
#include "DestName.h"
#include "Dbdefs.h"
#include "Rcodes.h"

//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(PickListDialog, TDialog)
//{{PickListDialogRSP_TBL_BEGIN}}
  EV_LBN_DBLCLK(IDC_LISTBOX, LBNDblclk),
  EV_BN_CLICKED(IDC_QUITBTN, BNQuitClicked),
  EV_BN_CLICKED(IDC_CANCELBTN, BNCancelClicked),
  EV_BN_CLICKED(IDC_SELECTBTN, BNSelectClicked),
  EV_REGISTERED(IPCNotifyMessage, ProcessMessage ),
//{{PickListDialogRSP_TBL_END}}
END_RESPONSE_TABLE;

#define TIMEOUT 6000

//{{PickListDialog Implementation}}


PickListDialog::PickListDialog(TWindow* parent)
:
 TDialog(parent, IDD_PICKLIST)
{
  // INSERT>> Your constructor code here.
 LISTBOX = new TListBox(this,IDC_LISTBOX);
 QUITBTN = new TButton(this,IDC_QUITBTN);
 SELECTBTN = new TButton(this,IDC_SELECTBTN);
 CANCELBTN = new TButton(this,IDC_CANCELBTN);
}


PickListDialog::~PickListDialog()
{
  Destroy(IDCANCEL);
}


void PickListDialog::SetupWindow()
{
 TDialog::SetupWindow();
}

void PickListDialog::LBNDblclk()
{
 AppNotifyMsg(Parent,IDOK,LISTBOX->GetSelIndex());
}

//------------------------------------------
// Handle Click of Quit/Continue Button
//------------------------------------------
void PickListDialog::BNQuitClicked()
{
 if ( continueBTN )
  {
   requestNextMsg();
   if ( *msgCount==maxMsg )
    {
     *msgCount=0;
     LISTBOX->ClearList();
    }
   continueBTN=false;
   QUITBTN->SetCaption("Quit");
   LISTBOX->SetCursor(0,IDC_WAIT);
  }
 else
  endOfSearch();
}


//-------------------------
// This Dialog Doesn't Quit
//-------------------------
TResult PickListDialog::EvCommand(uint id, THandle hWndCtl, uint notifyCode)
{
 TResult result;
  switch( notifyCode )
   {
    case 0:
       if ( id==IDCANCEL )
         return 0;

       if ( id==IDOK )
        {
         LBNDblclk();
         return 0;
        }
      break;
    default:
      break;
   }

  result = TDialog::EvCommand(id, hWndCtl, notifyCode);
  return result;
}


void PickListDialog::BNCancelClicked()
{
 AppNotifyMsg(Parent,IDCANCEL,0);
}

void PickListDialog::BNSelectClicked()
{
 LBNDblclk();
}

//------------------------------------------------------------------
// Insert the Starting Search Key into the Request Message based on
// Search Info Structure
//-----------------------------------------------------------------
void PickListDialog::setupSearchKeys()
{
 switch (searchInfo->FldNum1)
  {
   case FN_MERCHID:
      Msg.PutFld(FN_MERCHID,searchInfo->KeyFld1);
      break;
   case FN_LICENSESTATE:
      Msg.PutFld(FN_LICENSESTATE,searchInfo->KeyFld1);
      Msg.PutFld(FN_LICENSE,searchInfo->KeyFld2);
      break;
   case FN_BANKNUMBER:
      Msg.PutFld(FN_BANKNUMBER,searchInfo->KeyFld1);
      Msg.PutFld(FN_BANKACCOUNT,searchInfo->KeyFld2);
      break;
   case FN_PHONE:
      Msg.PutFld(FN_PHONE,searchInfo->KeyFld1);
      break;
   case FN_SSN:
      Msg.PutFld(FN_SSN,searchInfo->KeyFld1);
      break;
   case FN_OPERATORID:
      Msg.PutFld(FN_OPERATORID,searchInfo->KeyFld1);
      break;
   case FN_APPROVALNUM:
      Msg.PutFld(FN_APPROVALNUM,searchInfo->KeyFld1);
      break;
  }

}

//------------------------------------------------------------------
// Insert the Starting Search Date into the Request Message based on
// Search Info structure
//------------------------------------------------------------------
void PickListDialog::setupSearchDate()
{
 if ( searchInfo->AllDates )
  return;

 Msg.PutFld(FN_DATE,searchInfo->ToDate);
}

//------------------------------------
// Build List according to Search Info
//------------------------------------
void PickListDialog::Build(SEARCHINFO* SearchInfo,IPCMessage* MsgStack,
                           uint* MsgCount,uint MaxMsg)
{
 maxMsg=MaxMsg;
 msgStack=MsgStack;
 msgCount=MsgCount;
 searchInfo=SearchInfo;
 *msgCount=0;
 // Clear the Box and Initiate the Search
 LISTBOX->ClearList();

 // Set up Initial Search Message according to callers' SearchInfo
 Msg.ClearMsg();
 Msg.PutFld(FN_TRANTYPE,T_DBSEARCH);
 setupSearchKeys();
 setupSearchDate();
 LISTBOX->SetCursor(0,IDC_WAIT);
 LISTBOX->SetFocus();
 quitSearch=false;
 continueBTN=false;
 eof=false;
 retryCount=0;
 if ( ! Msg.SendMsg(LOGGER_DEST,GetHandle(),TIMEOUT) )
   MessageBox("ERROR SENDING MSG");
 QUITBTN->SetCaption("Quit");
 QUITBTN->Show(SW_NORMAL);
 SELECTBTN->EnableWindow(false);
 CANCELBTN->EnableWindow(false);
}

// Add Item to List
void PickListDialog::addToList(IPCMessage* Msg)
{
 char fmtBuf[200];
 time_t timeStamp;
 struct tm* t;
 char MerchId[6];
 char LicenseState[3];
 char License[16];
 char BankAccount[17];
 char BankNumber[10];
 char Amount[9];
 BYTE Rcode;

 memset(fmtBuf,0,sizeof(fmtBuf));
 Msg->GetFld(FN_TIMESTAMP,(DWORD)timeStamp);
 t=localtime(&timeStamp);
 Msg->GetFld(FN_MERCHID,sizeof(MerchId),MerchId);
 Msg->GetFld(FN_LICENSESTATE,sizeof(LicenseState),LicenseState);
 Msg->GetFld(FN_LICENSE,sizeof(License),License);
 Msg->GetFld(FN_BANKACCOUNT,sizeof(BankAccount),BankAccount);
 Msg->GetFld(FN_BANKNUMBER,sizeof(BankNumber),BankNumber);
 Msg->GetFld(FN_AMOUNT,sizeof(Amount),Amount);
 Msg->GetFld(FN_RCODE,Rcode);
 char sRcode[4];
 if ( rcInfo.IsApprovalRcode(Rcode) )
  strcpy(sRcode,"APP");
 else
  if ( Rcode == RCODE_CALLCANCELLED )
   strcpy(sRcode,"CAN");
  else
   strcpy(sRcode,"DEC");
 sprintf(fmtBuf,"%02d%02d%02d %02d%02d %s %-5s %8s %-9s %-16s %-2s %-15s",
          t->tm_mon+1,t->tm_mday,t->tm_year%100,
          t->tm_hour,t->tm_min,sRcode,MerchId,Amount,BankNumber,BankAccount,
          LicenseState,License);
 LISTBOX->AddString(fmtBuf);
}

//----------------------------------------
// Handle Incoming Response from SPSLogger
//----------------------------------------
LRESULT PickListDialog::ProcessMessage(WPARAM MsgType, LPARAM Id)
{
 DBRESULT Result;
 int      Len;

 // Is it a TIMEOUT?
 if ( MsgType==IPCMessageTimedOut )
  {
   if ( quitSearch )
    return 1;
   // When a Message TIMES out make sure you reset the SYNCID of your
   // local IPCMessage either by calling ResetMsgSyncId or ClearMsg.
   // If you don't do this, a late response won't be discarded if/when
   // it arrives because the SYNCID's will match.
   Msg.ResetMsgSyncId();

   // Do 2 Automatic retries and then ask the user for a retry/cancel
   if ( ++retryCount < 3 ||
        MessageBox("NO RESPONSE FROM SPSLOGGER",0,MB_RETRYCANCEL) == IDRETRY )
     Msg.SendMsg(LOGGER_DEST,GetHandle(),TIMEOUT);
   else
     endOfSearch();

   return 1;
  }

 if ( ! Msg.RecvReply(Id) ||
      quitSearch )
  return 1;

 // Presence of a DBRESULT structure indicates either an end of file
 // or I/O error condition
 Len=sizeof(DBRESULT);
 if ( Msg.GetFld(FN_DBRESULT,&Result,Len) )
  {
   if ( Result.ResponseCode != DB_NOTFOUND )
    MessageBox(Result.Text);
   eof=true;
   endOfSearch();
   return 1;
  }

 // If Message doesn't match search criteria then we're at the end
 // of the search.
 if ( ! isMatch() )
  {
   eof=true;
   endOfSearch();
   return 1;
  }

 // Add the Message to the Message Stack and Continue

 msgStack[*msgCount].CopyMsgIn(&Msg);
 addToList(&Msg);
 (*msgCount)++;

 // If Max, end of search
 if ( *msgCount == maxMsg )
  {
   endOfSearch();
   return 1;
  }

 // Send request for next record
 requestNextMsg();
 return 1;
}

//-------------------------------
// Handle end of search condition
//-------------------------------
void PickListDialog::endOfSearch()
{
 quitSearch=true;
 if ( *msgCount==0 )
  {
   QUITBTN->Show(SW_HIDE);
   CANCELBTN->EnableWindow(true);
   LISTBOX->AddString("NO RECORDS FOUND");
   LISTBOX->SetCursor(0,IDC_ARROW);
   return;
  }

 if ( eof )
  QUITBTN->Show(SW_HIDE);
 else
  {
   continueBTN=true;
   QUITBTN->SetCaption("Continue");
  }

 CANCELBTN->EnableWindow(true);
 SELECTBTN->EnableWindow(true);
 LISTBOX->SetCursor(0,IDC_ARROW);
 LISTBOX->SetSelIndex(0);
 LISTBOX->SetFocus(); 
}

//-----------------------------------
// Send a request for the next record
//-----------------------------------
void PickListDialog::requestNextMsg()
{
 int i=*msgCount -1;

 quitSearch=false;
 Msg.ClearMsg();
 Msg.CopyFld(FN_DBKEY,&msgStack[i]);
 Msg.PutFld(FN_TRANTYPE,T_DBNEXT);
 retryCount=0;
 Msg.SendMsg(LOGGER_DEST,GetHandle(),TIMEOUT);
}

//-----------------------------------------
// Check if Message Matches search criteria
//-----------------------------------------
bool PickListDialog::isDateMatch()
{
 time_t tsThen;
 struct tm* then;
 char thenDate[7];

 if ( searchInfo->AllDates )
  return true;

 // Get Timestamp
 if ( ! Msg.GetFld(FN_TIMESTAMP,(DWORD)tsThen) )
  return true;
 then=localtime(&tsThen);
 sprintf(thenDate,"%04d%02d%02d",then->tm_year+1900,then->tm_mon+1,
            then->tm_mday);

 return strcmp(thenDate,searchInfo->FromDate) >= 0 &&
        strcmp(thenDate,searchInfo->ToDate) <= 0;
}

//-----------------------------------------
// Check if Message Matches search criteria
//-----------------------------------------
bool PickListDialog::isKeyMatch()
{
 char fldBuf[sizeof(searchInfo->KeyFld1)];
 DWORD  D1, D2;

 if ( searchInfo->noKeyFlds )
  return true;

 switch (searchInfo->FldNum1)
  {
   case FN_MERCHID:
      if ( ! Msg.GetFld(FN_MERCHID,sizeof(fldBuf),fldBuf) )
        return false;
      return memcmp(fldBuf,searchInfo->KeyFld1,
                                  strlen(searchInfo->KeyFld1)) == 0;
   case FN_LICENSESTATE:
      if ( ! Msg.GetFld(FN_LICENSESTATE,sizeof(fldBuf),fldBuf) )
        return false;
      if ( strcmp(fldBuf,searchInfo->KeyFld1) )
        return false;
      if ( ! Msg.GetFld(FN_LICENSE,sizeof(fldBuf),fldBuf) )
        return searchInfo->KeyFld2[0] == 0;
      return memcmp(fldBuf,searchInfo->KeyFld2,
                                     strlen(searchInfo->KeyFld2))==0;
   case FN_BANKNUMBER:
      if ( ! Msg.GetFld(FN_BANKNUMBER,sizeof(fldBuf),fldBuf) )
        return false;
      if ( memcmp(fldBuf,searchInfo->KeyFld1,strlen(searchInfo->KeyFld1)))
        return false;
      if ( ! Msg.GetFld(FN_BANKACCOUNT,sizeof(fldBuf),fldBuf) )
        return searchInfo->KeyFld2[0] == 0;
      return memcmp(fldBuf,searchInfo->KeyFld2,
                                     strlen(searchInfo->KeyFld2))==0;

   case FN_PHONE:
      if ( ! Msg.GetFld(FN_PHONE,sizeof(fldBuf),fldBuf) )
        return false;
      return memcmp(fldBuf,searchInfo->KeyFld1,
                                  strlen(searchInfo->KeyFld1)) == 0;
   case FN_SSN:
      if ( ! Msg.GetFld(FN_SSN,sizeof(fldBuf),fldBuf) )
        return false;
      return memcmp(fldBuf,searchInfo->KeyFld1,
                                  strlen(searchInfo->KeyFld1)) == 0;
   case FN_OPERATORID:
      D1=atoi(searchInfo->KeyFld1);
      if ( ! Msg.GetFld(FN_OPERATORID,D2) )
       return false;
      return D1 == D2;
   case FN_APPROVALNUM:
      D1=atoi(searchInfo->KeyFld1);
      if ( ! Msg.GetFld(FN_APPROVALNUM,D2) )
       return false;
      return D1 == D2;
  }

 return true;
}


//-----------------------------------------
// Check if Message Matches search criteria
//-----------------------------------------
bool PickListDialog::isMatch()
{
 if ( ! isDateMatch() ||
      ! isKeyMatch() )
  return false;

 return true;
}

void PickListDialog::SetSelection(int Index)
{
 LISTBOX->SetSelIndex(Index);
}



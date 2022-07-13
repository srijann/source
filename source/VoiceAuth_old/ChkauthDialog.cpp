//----------------------------------------------------------------------------
//  Project CheckAuth
//
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    CheckAuth Application
//  FILE:         checkauthdlgclient.cpp
//  AUTHOR:
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of TCheckAuthDlgClient (TDialog).
//
//----------------------------------------------------------------------------

#include "win4me.h"
#include "chkauth.h"
#include <stdio.h>

#include "checkauthapp.h"
#include "checkservice.h"
#include "chkauthdialog.h"
#include "fetchreferral.h"
#include "VerifyReferralDialog.h"
#include "splib.h"
#include "MsgWin.rh"
#include "MsgWin.h"
#include "Msgid.h"
#include "DestName.h"
#include "TranType.h"
#include "ApprovalDialog.h"
#include "HoldCheckApprovaldialog.h"
#include "YesNoDialog.h"
#include "DeclineDialog.h"
#include "rcodes.h"
#include "kwrec.h"
#include "BankVerify.h"
#include "OperatorId.h"

//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(TCheckAuthDlgClient, TDialog)
//{{TCheckAuthDlgClientRSP_TBL_BEGIN}}
  EV_EN_KILLFOCUS(IDC_AMOUNT, AdjustAmount),
  EV_CBN_SELCHANGE(IDC_CHECKTYPE, enableBusinessName),
  EV_BN_CLICKED(IDC_STARTBUTTON, doNext),
  EV_BN_CLICKED(IDC_CANCELBUTTON, cancelTransaction),
  EV_BN_CLICKED(IDC_VIEWBTN, ViewClicked),
  EV_WM_TIMER,
  EV_REGISTERED("VOICEAUTH", ProcessChkauthNotify ),
//{{TCheckAuthDlgClientRSP_TBL_END}}
END_RESPONSE_TABLE;

//{{TCheckAuthDlgClient Implementation}}

void Disable(TWindow* Win,void*);

void logg(int iLine, char *buf1, char *buf2, int i, long l);
void logg(int iLine, char *buf1, char *buf2, int i, long l)
{
   FILE  *fout;
   return;
   fout = fopen("err.log", "a");
   fprintf(fout,"@%d %s - %s. .%ld. .%d.\n", iLine, buf1, buf2, i, l);
   fclose(fout);
}


//--------------------------------------------------------
// TCheckAuthDlgClient
// ~~~~~~~~~~
// Construction/Destruction handling.
//
TCheckAuthDlgClient::TCheckAuthDlgClient(TWindow* parent, TResId resId, TModule* module)
:
  TDialog(parent, resId, module),
  CurrentDialog(0),
  holdCheckDialog(0),
  decisionDialog(0),
  employerDialog(0),
  replyPending(false),
  lockFocus(false)
{
 // Get Filenames from the REGISTRY
 if ( ! IPCEnumRegistry(HKEY_LOCAL_MACHINE,"SOFTWARE\\VoiceAuth",true,
                        "MessageFile",sizeof(MessageFileName),
                        MessageFileName) )
  {
   MessageBox("ERROR READING REGISTRY","INITIALIZATION ERROR",
              MB_ICONERROR|MB_OK);
   throw;
  }

 // Create the Message Window
 Instruct = new TMsgWin(this,IDD_MSGWIN,MessageFileName);

 // Create a Check Auth Message.. Set it up for 10 second timeouts
 // and automatic retry/cancel box
 AuthMsg=new CheckAuth();
 AuthMsg->setNetTimeout(10000);
 AuthMsg->setNetUIRetry();

 // Create another IPCMessage for various other uses
 AuthReqMsg=new IPCMessage();

 // Call the function to set up the controls (it's in chkauthcontrols.cpp)
 SetupControls();
}

//-----------------
// Class Destructor
//-----------------
TCheckAuthDlgClient::~TCheckAuthDlgClient()
{
 Destroy();
 // Delete our stuff
 delete AuthMsg;
 delete AuthReqMsg;
 delete DCMan;
 delete ProductList;
}

//-------------------------------
// Cancel the current transaction
//-------------------------------
void TCheckAuthDlgClient::cancelTransaction()
{
 AuthMsg->PutFld(FN_RCODE,(BYTE)RCODE_CALLCANCELLED);
 endTransaction();
}

//----------------------------
// End the current transaction
//----------------------------
void TCheckAuthDlgClient::endTransaction()
{
 // If Kenwood was waiting for an answer, send a cancel
 if ( replyPending )
  sendCancel();

 // If I have a transaction to Log, log it
 if ( logPending )
  logTransaction();

 // If I had a dialog open, delete it
 if ( CurrentDialog )
   delete CurrentDialog;

 // If I had a hold check dialog, delete it
 if ( holdCheckDialog )
   delete holdCheckDialog;

 // If I had an Employment info dialog, delete it.
 if ( employerDialog )
   delete employerDialog;
 CurrentDialog=0;
 holdCheckDialog=0;
 decisionDialog=0;
 employerDialog=0;

 // Stop the Progress Bar
 KillTimer(1);
 TimerBar->SetValue(0);

 // Clear all the Readonly flags for the next transaction
 DCMan->ClearAllReadOnly();

 // Set up to wait for next transaction
 WaitForStart();
}

//----------------------------
// Handle click of VIEW button
//----------------------------
void TCheckAuthDlgClient::ViewClicked()
{
 TDialog* Views[4];
 int n=0;
 int i;

 // Build Table of available Views
 Views[n++] = this;
 if ( holdCheckDialog )
   Views[n++]=holdCheckDialog;
 if ( employerDialog )
   Views[n++]=employerDialog;
 if ( decisionDialog )
   Views[n++]=decisionDialog;

 // Find the Current View
 if ( CurrentView == 0 )
  CurrentView = this;
 for (i=0; i<n && CurrentView != Views[i]; ++i);

 if ( i != 0 )
   CurrentView->Show(SW_HIDE);
 if ( ++i >= n )
   i=0;
 CurrentView=Views[i];
 if ( CurrentView == decisionDialog )
  CurrentView->Show(SW_SHOWNORMAL);
 else
  if ( i > 0  )
   CurrentView->Show(SW_SHOWNA);
}

//-----------------------------------
// Go into the WAITING for START authStep
//------------------------------------
void TCheckAuthDlgClient::WaitForStart()
{
 // Disable all the controls and then re-enable the buttons
 ForEach(Disable);
 StartButton->EnableWindow(true);
 ViewButton->EnableWindow(false);
 Instruct->EnableWindow(true);
 Instruct->Say(MID_START);
 authStep=STEP_STARTING;
 DCMan->ClearAll();
 Parent->SetCaption("Check Authorization");
 StartButton->SetFocus();
 CurrentView=this;
 lockFocus=false;
}

//-------------------------
// Timer Messages come here
//-------------------------
void TCheckAuthDlgClient::EvTimer(uint timerId)
{
 // Does the progress bar
 TDialog::EvTimer(timerId);
 StepTimer();
}

//-----------------------------------------
// Disable a Window .. For use with foreach
//-----------------------------------------
void Disable(TWindow* Win,void*)
{
 Win->EnableWindow(false);
}

//------------------------------------------------------
// Override SetupWindow to create the Message Window and
// Put up the Initial Message
//------------------------------------------------------
void TCheckAuthDlgClient::SetupWindow()
{
 TDialog::SetupWindow();
 SetupLists();
 Instruct->Create();
 WaitForStart();
}


//----------------------------------------------------------
// Override EvCommand to prevent ESCape from the main dialog
//----------------------------------------------------------
TResult TCheckAuthDlgClient::EvCommand(uint id, THandle hWndCtl, uint notifyCode)
{
  TResult result;

  switch( notifyCode )
   {
    case 0:
      // Don't Allow ESCAPE
      if ( id==IDCANCEL )
       {
        // If instruction window has a buddy (special handling), we
        // want to toggle it's position when escape is hit
        Instruct->Move();
        return 0;
       } 

      // For reasons unknown, the first press of the enter key is not
      // interpreted as a press of the Start Button. Instead we get a
      // Menu Item 1 selection and the Dialog terminates.
      // Here, we override this action by changing the id to
      // the Start button.

      if ( id==IDOK )
       id=IDC_STARTBUTTON;
      break;

    case EN_SETFOCUS:
    case CBN_SETFOCUS:
      // Don't allow any change of focus if another dialog
      // is in progress.
      if ( ! lockFocus )
        DCMan->SetFocus((int)id);
      else
        return 0;
      break;

    case EN_KILLFOCUS:
      break;

    default:
      break;
   }

  result = TDialog::EvCommand(id, hWndCtl, notifyCode);

  // INSERT>> Your code here.

  return result;
}

//-----------------------------
// StepTimer (the progress bar)
//-----------------------------
void TCheckAuthDlgClient::StepTimer(void)
{
 int Value=TimerBar->GetValue() + TIMERBAR_STEP;
 int i;

 if ( Value >= TIMERBAR_SECONDS-1)
  {
   TimerBar->SetValue(TIMERBAR_SECONDS-1);
   StopTime=time(NULL);
   KillTimer(1);
   for (i=0; i<5; ++i)
     MessageBeep(0xffffffff);
  }
 else
  TimerBar->SetValue( Value );
}

//-----------------------------------------------------------------
// Handle the Notify Message
//
//  Other Dialogs send us notification messages when they complete.
//-----------------------------------------------------------------
LRESULT TCheckAuthDlgClient::ProcessChkauthNotify(WPARAM MsgType,LPARAM Id)
{
 switch(MsgType)
  {
   case NOTIFY_DIALOGCOMPLETE:
     if ( CurrentDialog )
      {
       delete CurrentDialog;
       CurrentDialog=0;
      }
     lockFocus=false;

     // The decision Dialog will notify us of an approval or
     // a decline
     switch( Id )
      {
       case NOTIFY_APPROVED:
          ViewButton->EnableWindow(false);
          replyPending=false;
          if ( ! AuthMsg->isHoldCheckMerchant() ||
               ! sendHoldCheckApprovals() )
           {
            authStep=STEP_APPROVED;
            Instruct->Say(MID_APPROVAL);
            lockFocus=true;
            CurrentDialog=new ApprovalDialog(this,(CheckAuth&)*AuthMsg);
            CurrentDialog->Create();
           }
          return 0;

       case NOTIFY_DECLINED:
          ViewButton->EnableWindow(false);
          replyPending=false;
          if ( ! AuthMsg->isHoldCheckMerchant() ||
               ! sendHoldCheckDeclines() )
           {
            authStep=STEP_DECLINED;
            Instruct->Say(MID_DECLINED);
            lockFocus=true;
            CurrentDialog=new DeclineDialog(this);
            CurrentDialog->Create();
           }
          return 0;

       default: break;
      }
     break;
  }

 doNext();
 return 0;
}

//---------------------------------------------------------------
// We come here when the start button is clicked or when the user
// Presses Enter. It does state transition stuff when necessary.
//---------------------------------------------------------------
void TCheckAuthDlgClient::doNext()
{
 TDialog* ReferralDialog;

 // Ignore Enter key in main window if focus is locked

 if ( lockFocus )
   return;

 // What we do depends on the state we were in
 switch(authStep)
  {
   // Was waiting for a transaction
   case STEP_STARTING:
     // Disable start button and set timer
     StartButton->EnableWindow(false);
     SetTimer(1,TIMERBAR_STEP * 1000,NULL);
     StartTime=time(NULL);
     // Execute the Referral Queue Fetch.
     AuthMsg->ClearMsg();
     AuthMsg->initClassMembers();

     // It does no good to save the TimeStamp in the
     // message at this point.
     AuthMsg->PutFld(FN_TIMESTAMP,(DWORD)time(NULL));

     Instruct->ShowWindow(SW_SHOW);
     Instruct->Say(MID_REFERRALNUM);
     lockFocus=true;
     CurrentDialog = ReferralDialog =
         new TFetchReferral(this,(CheckAuth&)*AuthMsg);
     ReferralDialog->Create();
     CancelButton->EnableWindow(true);
     authStep=STEP_FETCHINGREFERRAL;
     break;

   // Referral has been requested.
   case STEP_FETCHINGREFERRAL:
     // Use presence of merchant Record to indicate if we
     // got a message from the queue. If we got one we
     // have to verify if it's correct
     AuthMsg->initClassMembers();
     if ( AuthMsg->hasMerchantRec )
      {
       authStep=STEP_VERIFYINGREFERRAL;
       Instruct->Say(MID_VERIFYREFERRAL);
       lockFocus=true;
       CurrentDialog = ReferralDialog =
           new VerifyReferralDialog(this,*AuthMsg);
       ReferralDialog->Create();
      }
     else
     // No Referral, get the Merchant Id
      {
       authStep=STEP_FETCHINGMERCHANTDATA;
       DCMan->SetFocus(IDC_MERCHNUM);
      }
     break;

   // The referral has been verified Yes/No.
   case STEP_VERIFYINGREFERRAL:
     // If we have a referral, go set up the window with
     // all the fields
     AuthMsg->initClassMembers();
     if ( AuthMsg->hasMerchantRec )
       doReferral();
     else
     // No referral, we have to get Merchant Id
      {
       authStep=STEP_FETCHINGMERCHANTDATA;
       DCMan->SetFocus(IDC_MERCHNUM);
      }
     break;

   // Was fetching Merchant Information
   case STEP_FETCHINGMERCHANTDATA:
     getMerchantData();
     break;

   // Was querying for Preauth Yes/No
   case STEP_PREAUTHQUERY:
   //logg(__LINE__, "In chkauthdlg.cpp doNext()", "",0,0);
     if ( YesNoAnswer )
      {
       //logg(__LINE__, "In chkauthdlg.cpp doNext()", "YesNoAnswer",0,0);
       AuthMsg->isPreauth(true);
       DCMan->CopyFromMsg(IDC_AMOUNT,AuthMsg);
      }
     enableFields(AuthMsg->Voice);
     collectAuthData(AuthMsg->Voice);
     authStep=STEP_COLLECTINGAUTHDATA;
     break;

   // Was Collecting Amount for Guarantee/Verify
   case STEP_COLLECTINGAMOUNT:
     AdjustAmount();
     verifyMerchantData();
     break;

   // Was Collection Hold Check Data
   case STEP_COLLECTINGHOLDCHECKDATA:
     ViewButton->EnableWindow(true);
     authStep=STEP_COLLECTINGAUTHDATA;
     DCMan->CopyAllFromMsg(AuthMsg);
     enableFields(AuthMsg->Voice);
     // Go collect rest of Auth Data
     collectAuthData(AuthMsg->Voice);
     break;

   // Was Collecting Initial Auth Data
   case STEP_COLLECTINGAUTHDATA:
     AdjustAmount();
     // Go collect rest of Auth Data
     if ( collectAuthData(AuthMsg->Voice) )
      doAuthorization();
     break;

   // Was Collecting Employment Data
   case STEP_COLLECTINGEMPLOYERDATA:
     ViewButton->EnableWindow(true);
     // Do the Authorization
     doAuthorization();
     break;

   // Was Collection Bank Verify Data
   case STEP_COLLECTINGBVDATA:
     // If we have all the Auth Data, do the Decision
     if ( collectAuthData(AuthMsg->Security) )
       doDecision();
     break;

   // Was Deciding to Approve or Decline
   case STEP_APPROVED:
   case STEP_DECLINED:
   case STEP_DECIDING:
     endTransaction();
     break;


   default:
     MYTRACE("UNRECOGNIZED MODE IN doNext");
     break;
  }
}

//------------------------------------------------------------------
// Verify Merchant Data
//   Used for Guarantee/Verify. Make sure we have the right control
//   classes before proceeding.
//-----------------------------------------------------------------
void TCheckAuthDlgClient::verifyMerchantData()
{
 int  ret;
 HWND captureWin;

 DCMan->CopyAllToMsg(AuthMsg);
 // Have to have a valid amount to determine if guarantee or
 // verify
 if ( ! AuthMsg->GetFld(FN_AMOUNT) )
  {
   DCMan->SetFocus(FN_AMOUNT);
   return;
  }
 if ( ! AuthMsg->isValidAmount() )
  {
   DCMan->SetFocus(FN_AMOUNT,MID_INVALIDAMOUNT);
   return;
  }

 // The ManagerDB program returns Guarantee controls by default when
 // the amount is not present. If we determine now that we're verify,
 // we have to go get the verify controls
 if ( AuthMsg->isGuarantee() )
  {
   setMerchantCaption();
   authStep=STEP_COLLECTINGAUTHDATA;
   enableFields(AuthMsg->Voice);
   collectAuthData(AuthMsg->Voice);
   return;
  }

 // Go get verify Controls
 AuthMsg->ClearMsg();
 AuthMsg->initClassMembers();
 DCMan->CopyAllToMsg(AuthMsg);
 AuthMsg->PutFld(FN_TRANTYPE,T_VOICEAUTH);
 AuthMsg->PutFld(FN_SOURCETYPE,(BYTE)'V');
 Instruct->Say(MID_GETTINGMERCHANT);
 captureWin=GetApplication()->GetMainWindow()->SetCapture();
 GetApplication()->GetMainWindow()->SetCursor(0,IDC_WAIT);
 lockFocus=true;
 ret = AuthMsg->SendRecv(MANAGERDB_DEST,(WORD)10000,true);
 lockFocus=false;
 GetApplication()->GetMainWindow()->SetCursor(0,IDC_ARROW);
 ::SetCapture(captureWin);

 if ( ! ret )
  {
   DCMan->SetFocus(IDC_MERCHNUM);
   return;
  }

 AuthMsg->initClassMembers();
 if ( ! AuthMsg->hasMerchantRec )
  {
   DCMan->SetFocus(IDC_MERCHNUM,MID_NOMERCHANT);
   return;
  }

 setMerchantCaption();

 if ( ! AuthMsg->hasDataReq )
  {
   DCMan->SetFocus(IDC_MERCHNUM,MID_NODATAREQ);
   return;
  }

 if ( ! AuthMsg->hasAuthRules )
  {
   DCMan->SetFocus(IDC_MERCHNUM,MID_NOAUTHRULES);
   return;
  }

 enableFields(AuthMsg->Voice);
 authStep=STEP_COLLECTINGAUTHDATA;
 if ( collectAuthData(AuthMsg->Voice) )
   doAuthorization();
}

//--------------------
// Fetch Merchant Data
//
// Go to the Database and get all the Merchant Data
//--------------------
void TCheckAuthDlgClient::getMerchantData()
{
 int  ret;
 HWND captureWin;

 AuthMsg->ClearMsg();
 AuthMsg->initClassMembers();
 DCMan->CopyAllToMsg(AuthMsg);
 if ( ! AuthMsg->GetFld(FN_MERCHID) )
  {
   DCMan->SetFocus(IDC_MERCHNUM);
   return;
  }
 if ( ! AuthMsg->isValidMerchantID() )
  {
   DCMan->SetFocus(IDC_MERCHNUM,MID_INVALIDMERCHID);
   return;
  }
 AuthMsg->PutFld(FN_TRANTYPE,T_VOICEAUTH);
 AuthMsg->PutFld(FN_SOURCETYPE,(BYTE)'V');
 Instruct->Say(MID_GETTINGMERCHANT);
 captureWin=GetApplication()->GetMainWindow()->SetCapture();
 GetApplication()->GetMainWindow()->SetCursor(0,IDC_WAIT);
 lockFocus=true;
 ret = AuthMsg->SendRecv(MANAGERDB_DEST,(WORD)10000,true);
 lockFocus=false;
 GetApplication()->GetMainWindow()->SetCursor(0,IDC_ARROW);
 ::SetCapture(captureWin);

 if ( ! ret )
  {
   DCMan->SetFocus(IDC_MERCHNUM);
   return;
  }

 AuthMsg->initClassMembers();
 if ( ! AuthMsg->hasMerchantRec )
  {
   DCMan->SetFocus(IDC_MERCHNUM,MID_NOMERCHANT);
   return;
  }

 setMerchantCaption();

 // Init the Product Box
 if ( ProductList->isValidGroupNumber(
                          AuthMsg->merchantRec.Data.ProductGroup) )
   ProductList->InitComboBoxClass( DCMan->ComboBoxPtr(IDC_PRODUCTCLASS),
         AuthMsg->merchantRec.Data.ProductGroup);
 else
   DCMan->ComboBoxPtr(IDC_PRODUCTCLASS)->ClearList();

 if ( ! AuthMsg->hasDataReq )
  {
   DCMan->SetFocus(IDC_MERCHNUM,MID_NODATAREQ);
   return;
  }

 if ( ! AuthMsg->hasAuthRules )
  {
   DCMan->SetFocus(IDC_MERCHNUM,MID_NOAUTHRULES);
   return;
  }

 // Set Merchant Number to ReadOnly .. Can't change it any more
 DCMan->EditPtr(IDC_MERCHNUM)->SetReadOnly(true);

 // If it's a Hold Check Merchant, we create the Hold Check
 // Dialog 
 if ( AuthMsg->isHoldCheckMerchant() )
  {
   authStep=STEP_COLLECTINGHOLDCHECKDATA;
   lockFocus=true;
   holdCheckDialog=new HoldCheckDialog(this,*AuthMsg,Instruct);
   holdCheckDialog->Create();
   return;
  }

 // If it's a Preauth Merchant, we have to invoke the Preauth
 // Query Dialog
 if ( AuthMsg->isPreauthMerchant() )
  {
   //logg(__LINE__, "In chkauthdlg.cpp FetMerchantData()", "isPreAuthMerchant()",0,0);
   authStep=STEP_PREAUTHQUERY;
   Instruct->Say(MID_ISPREAUTHQUERY);
   lockFocus=true;
   CurrentDialog=new YesNoDialog(this,YesNoAnswer,"PREAUTH?");
   CurrentDialog->Create();
   return;
  }

 // If it's Guarantee/Verify we have to get the Amount to figure
 // if it's guarantee or verify
 if ( AuthMsg->merchantRec.Data.MerchantService==
              CHECKSERVICE_GUARANTEEVERIFY )
  {
   authStep=STEP_COLLECTINGAMOUNT;
   DCMan->SetFocus(IDC_AMOUNT);
   return;
  }

 enableFields(AuthMsg->Voice);
 DCMan->SetFocus(IDC_AMOUNT);
 authStep=STEP_COLLECTINGAUTHDATA;
 if ( collectAuthData(AuthMsg->Voice) )
   doAuthorization();
}

//--------------------------------------------------
// We have a Referral. Figure out what we need to do
//--------------------------------------------------
void TCheckAuthDlgClient::doReferral()
{
 setMerchantCaption();
 DCMan->CopyAllFromMsg(AuthMsg);
 DCMan->Enable(FN_MERCHID);
 DCMan->EditPtr(IDC_MERCHNUM)->SetReadOnly(true);
 enableFields(AuthMsg->Voice);
 logPending=true;

 // If it's a Hold Check Merchant, we go first to the hold
 // check screen
 if ( AuthMsg->isHoldCheckMerchant() )
  {
   authStep=STEP_COLLECTINGHOLDCHECKDATA;
   lockFocus=true;
   holdCheckDialog=new HoldCheckDialog(this,*AuthMsg,Instruct);
   holdCheckDialog->Create();
   return;
  }

 // Not hold check, we start collecting Auth data
 authStep=STEP_COLLECTINGAUTHDATA;
 if ( collectAuthData(AuthMsg->Voice) )
  doAuthorization();
}

//--------------------------------------------------------
// Enable fields according to type using Data Requirements
//--------------------------------------------------------

// A literal macro to help.
#define ENABLE(reqBit,fldNum)\
 if ( AuthMsg->dataReq.isRequired(Type,AuthMsg->dataReq.reqBit,(WORD)Amount) ||\
      AuthMsg->GetFld(fldNum) )\
  DCMan->Enable(fldNum);\
 else\
  DCMan->Disable(fldNum)

#define ENABLE2(reqBit1,reqBit2,fldNum)\
 if ( AuthMsg->dataReq.isRequired(Type,AuthMsg->dataReq.reqBit1,(WORD)Amount) ||\
      AuthMsg->dataReq.isRequired(Type,AuthMsg->dataReq.reqBit2,(WORD)Amount) ||\
      AuthMsg->GetFld(fldNum) )\
  DCMan->Enable(fldNum);\
 else\
  DCMan->Disable(fldNum)

#define PERSONALCHECKENABLE(reqBit,fldNum)\
 if ( AuthMsg->GetFld(fldNum) ||\
      ( CheckType != 'B' && \
      AuthMsg->dataReq.isRequired(Type,AuthMsg->dataReq.reqBit,(WORD)Amount)) )\
  DCMan->Enable(fldNum);\
 else\
  DCMan->Disable(fldNum)

#define PERSONALCHECKENABLE2(reqBit1,reqBit2,fldNum)\
 if ( AuthMsg->GetFld(fldNum) ||\
      ( CheckType != 'B' && \
      (AuthMsg->dataReq.isRequired(Type,AuthMsg->dataReq.reqBit1,(WORD)Amount)\
      ||\
      AuthMsg->dataReq.isRequired(Type,AuthMsg->dataReq.reqBit2,(WORD)Amount)) ) )\
  DCMan->Enable(fldNum);\
 else\
  DCMan->Disable(fldNum)

#define BUSINESSCHECKENABLE(reqBit,fldNum)\
 if ( AuthMsg->GetFld(fldNum) ||\
      (CheckType == 'B' && \
       AuthMsg->dataReq.isRequired(Type,AuthMsg->dataReq.reqBit,(WORD)Amount)) )\
  DCMan->Enable(fldNum);\
 else\
  DCMan->Disable(fldNum)

void TCheckAuthDlgClient::enableFields(int Type)
{
 authTypeEnabled=Type;
 DWORD Amount=AuthMsg->CheckAmount();
 bool holdCheck;
 BYTE CheckType=' ';

 AuthMsg->GetFld(FN_CHECKTYPE,CheckType);

 if (Amount > USHRT_MAX )
  Amount=USHRT_MAX;
 else
 // If no amount present, use a low dollar for testing. We'll
 // re-enable later after the operator enters the dollar
 if ( Amount==0 )
  Amount=1;

 DCMan->Enable(FN_AMOUNT);

 // Hold Check Amount comes from the 1st Check on the Hold Check Screen
 // Don't let the operator change it
 if ( (holdCheck=AuthMsg->isHoldCheckMerchant()) == true ||
      AuthMsg->isPreauth() )
  {
   DCMan->EditPtr(IDC_AMOUNT)->SetReadOnly(true);
   if ( holdCheck )
    DCMan->EditPtr(IDC_CHECKNUMBER)->SetReadOnly(true);
  }

 ENABLE(CHECKTYPE,FN_CHECKTYPE);
 PERSONALCHECKENABLE2(LICENSE,LICENSEORMICR,FN_LICENSESTATE);
 PERSONALCHECKENABLE2(LICENSE,LICENSEORMICR,FN_LICENSE);
 PERSONALCHECKENABLE(DOB,FN_DOB);
 ENABLE(CHECKNUMBER,FN_CHECKNUMBER);
 ENABLE2(MICR,LICENSEORMICR,FN_BANKACCOUNT);
 ENABLE2(MICR,LICENSEORMICR,FN_BANKNUMBER);
 PERSONALCHECKENABLE(NAME,FN_CONSUMERNAME);
 ENABLE(ADDRESS,FN_ADDRESS);
 ENABLE(RESIDENCETYPE,FN_APARTMENT);
 ENABLE(CITY,FN_CITY);
 ENABLE(CONSUMERSTATE,FN_CONSUMERSTATE);
 ENABLE(ZIPCODE,FN_ZIPCODE);
 ENABLE(PHONE,FN_PHONE);
 ENABLE(SSN,FN_SSN);

 // Enable the Product Class only if the box has items in it
 if ( ( AuthMsg->dataReq.isRequired(Type,AuthMsg->dataReq.PRODUCTCLASS,
                     (WORD)Amount) ||
        AuthMsg->GetFld(FN_PRODUCTCLASS) ) )
  {
   if ( DCMan->ComboBoxPtr(FN_PRODUCTCLASS)->GetCount() > 0 )
     DCMan->Enable(FN_PRODUCTCLASS);
   else
    {
     AuthMsg->OverrideFieldValidation(FN_PRODUCTCLASS);
     DCMan->Disable(FN_PRODUCTCLASS);
    }
  }
 else
   DCMan->Disable(FN_PRODUCTCLASS);

 ENABLE(TRANSPORTMETHOD,FN_TRANSPORTMETHOD);
 BUSINESSCHECKENABLE(BUSINESSNAME,FN_BUSINESSNAME);

 ENABLE(PRIVATELABEL,FN_PRIVATELABEL);
 ENABLE(TITLE,FN_TITLE);
}

//------------------------------------------------------------
// Copy all fields from the window to the message and validate
//-------------------------------------------------------------
bool TCheckAuthDlgClient::collectAuthData(int Type)
{
 FLDNUM Fn;
 bool   isMissing;
 BYTE   Rcode;

 DCMan->CopyAllToMsg(AuthMsg);

 // We delete these fields because they are calculated and stored
 // from other fields. If the other fields change, these will be
 // incorrect. By deleting them, we cause them to be recalculated
 // when needed.
 AuthMsg->DelFld(FN_AMOUNTROUNDED);
 AuthMsg->DelFld(FN_CONSUMERAGE);

 // This will tell us if we are missing any fields or if any
 // fields are invalid.
 if ( ! AuthMsg->fieldValidation(Type,Fn,isMissing,Rcode) )
  {
   DCMan->SetFocus(Fn, isMissing ? -1 : getInvalidMsgID(Fn,Rcode));
   return false;
  }

 // I have everything needed and it's all valid.
 // See if I need Employment or Spouse/CoSigner data
 if ( ( AuthMsg->needEmployerData(Type) ||
        AuthMsg->needSpouseCoSignerData(Type) ) &&
      ! employerDialog )
  {
   ViewButton->EnableWindow(false);
   lockFocus=true;
   employerDialog=new EmployerDialog(this,*AuthMsg,Instruct);
   employerDialog->Create();
   authStep=STEP_COLLECTINGEMPLOYERDATA;
   return false;
  }

 return true;
}

//---------------------------------------------------------
// Get the Message ID related to an invalid Field condition
//---------------------------------------------------------
int TCheckAuthDlgClient::getInvalidMsgID(FLDNUM Fn,BYTE)
{
 switch(Fn)
  {
   case FN_MERCHID: return MID_INVALIDMERCHID;
   case FN_AMOUNT: return MID_INVALIDAMOUNT;
   case FN_CHECKTYPE:
     if ( AuthMsg->isHoldCheckMerchant() )
       return MID_INVALIDHOLDCHECKTYPE;
     else
       return MID_INVALIDCHECKTYPE;
   case FN_LICENSESTATE: return MID_INVALIDLICENSESTATE;
   case FN_LICENSE: return MID_INVALIDLICENSE;
   case FN_DOB: return MID_INVALIDDOB;
   case FN_CHECKNUMBER: return MID_INVALIDCHECKNUMBER;
   case FN_CONSUMERNAME: return MID_INVALIDCONSUMERNAME;
   case FN_ADDRESS: return MID_INVALIDADDRESS;
   case FN_APARTMENT: return MID_INVALIDAPARTMENT;
   case FN_CITY: return MID_INVALIDCITY;
   case FN_CONSUMERSTATE: return MID_INVALIDCONSUMERSTATE;
   case FN_ZIPCODE: return MID_INVALIDZIPCODE;
   case FN_PHONE: return MID_INVALIDPHONE;
   case FN_SSN: return MID_INVALIDSSN;
   case FN_PRODUCTCLASS: return MID_INVALIDPRODUCTCLASS;
   case FN_TRANSPORTMETHOD: return MID_INVALIDTRANSPORTMETHOD;
   case FN_BUSINESSNAME: return MID_INVALIDBUSINESSNAME;
   case FN_PRIVATELABEL: return MID_INVALIDPRIVATELABEL;
   case FN_TITLE: return MID_INVALIDTITLE;
   case FN_BANKNUMBER: return MID_INVALIDBANKNUMBER;
   case FN_BANKACCOUNT: return MID_INVALIDBANKACCOUNT;
   default: return MID_INVALIDUNKNOWN;
  }
}

//---------------------------------------------------------------------
// Set the Window Caption to indicate the merchant name and transaction
// Type.
//---------------------------------------------------------------------
void TCheckAuthDlgClient::setMerchantCaption()
{
 char Buf[100];
 strcpy(Buf,AuthMsg->MerchantName());
 strcat(Buf," (");
 switch( AuthMsg->merchantRec.Data.MerchantService )
  {
   case CHECKSERVICE_GUARANTEE:  strcat(Buf,"Guarantee)"); break;
   case CHECKSERVICE_PREAUTH:    strcat(Buf,"Preauth)"); break;
   case CHECKSERVICE_HOLDCHECK:  strcat(Buf,"Hold Check)"); break;
   case CHECKSERVICE_VERIFY:     strcat(Buf,"Verify)"); break;
   case CHECKSERVICE_GUARANTEEVERIFY:
      // If I have the Amount then I know if it's Guarantee or Verify
      if ( ! AuthMsg->GetFld(FN_AMOUNT) )
        strcat(Buf,"Guarantee+Verify)");
      else
      if ( AuthMsg->isGuarantee() )
       strcat(Buf,"Guarantee+)");
      else
       strcat(Buf,"Verify+)");
      break;
   default:
        strcat(Buf,"???)"); break;
  }

 Parent->SetCaption(Buf);
}

//-----------------------------
// Execute Decision-making Step
//-----------------------------
void TCheckAuthDlgClient::doDecision()
{
 // Make sure we have all the BV data first
 if ( ! collectAuthData(AuthMsg->Security) )
  {
   authStep=STEP_COLLECTINGBVDATA;
   return;
  }
 authStep=STEP_DECIDING;
 ViewButton->EnableWindow(true);
 lockFocus=true;
 DCMan->SetAllReadOnly();
 CurrentDialog = decisionDialog =
    new DecisionDialog(this,(CheckAuth&)*AuthMsg,
                     (IPCMessage&)*AuthReqMsg,Instruct,
                     AuthMsg->merchantRec.getMessageNumber());
 decisionDialog->Create();
 CurrentView=decisionDialog;
}

//-----------------------
// Send Approval Response
//-----------------------
void TCheckAuthDlgClient::sendApproval()
{
 VOICEOUTPUT vOut;
 HWND captureWin;

 ViewButton->EnableWindow(false);
 AuthMsg->GenerateApprovalNumber();
 AuthReqMsg->ClearMsg();
 AuthReqMsg->CopyFld(FN_TERMINFO,AuthMsg);
 AuthReqMsg->CopyFld(FN_RCODE,AuthMsg);
 AuthReqMsg->CopyFld(FN_APPROVALNUM,AuthMsg);
 memset(&vOut,' ',sizeof(vOut));
 COperatorId OperatorId;
 OperatorId.Get(vOut.OperatorId,sizeof(vOut.OperatorId));
 memcpy(vOut.BVStatus,BVSTATUS_NOTVERIFIED,sizeof(vOut.BVStatus));
 AuthReqMsg->PutFld(FN_TEXTRESPONSE,&vOut,sizeof(vOut));
 AuthReqMsg->PutFld(FN_TRANTYPE,T_AUTHREPLY);

 Instruct->Say(MID_SENDINGAUTHREPLY);
 captureWin=GetApplication()->GetMainWindow()->SetCapture();
 GetApplication()->GetMainWindow()->SetCursor(0,IDC_WAIT);
 lockFocus=true;
 AuthReqMsg->SendRecv(VOICEAUTH_DEST,(WORD)10000,true);
 lockFocus=false;
 GetApplication()->GetMainWindow()->SetCursor(0,IDC_ARROW);
 ::SetCapture(captureWin);
 replyPending=false;

 // If Hold Check Merchant and more than one amount, we
 // have to send the other approvals.
 if ( AuthMsg->isHoldCheckMerchant() &&
      sendHoldCheckApprovals() )
  return;

 authStep=STEP_APPROVED;
 Instruct->Say(MID_APPROVAL);
 lockFocus=true;
 CurrentDialog = new ApprovalDialog(this,(CheckAuth&)*AuthMsg);
 CurrentDialog->Create();
}

//-------------------------------
// Send a CALL Cancelled response
//-------------------------------
void TCheckAuthDlgClient::sendCancel()
{
 VOICEOUTPUT vOut;
 HWND captureWin;

 AuthReqMsg->ClearMsg();
 AuthReqMsg->CopyFld(FN_TERMINFO,AuthMsg);
 AuthReqMsg->CopyFld(FN_RCODE,AuthMsg);
 memset(&vOut,' ',sizeof(vOut));
 COperatorId OperatorId;
 OperatorId.Get(vOut.OperatorId,sizeof(vOut.OperatorId));
 memcpy(vOut.BVStatus,BVSTATUS_NOTVERIFIED,sizeof(vOut.BVStatus));
 AuthReqMsg->PutFld(FN_TEXTRESPONSE,&vOut,sizeof(vOut));
 AuthReqMsg->PutFld(FN_TRANTYPE,T_AUTHREPLY);

 Instruct->Say(MID_SENDINGAUTHREPLY);
 captureWin=GetApplication()->GetMainWindow()->SetCapture();
 GetApplication()->GetMainWindow()->SetCursor(0,IDC_WAIT);
 lockFocus=true;
 AuthReqMsg->SendRecv(VOICEAUTH_DEST,(WORD)10000,true);
 lockFocus=false;
 GetApplication()->GetMainWindow()->SetCursor(0,IDC_ARROW);
 ::SetCapture(captureWin);
 replyPending=false;
}

//-------------------------------------------------------------
// Add Trailing Zeroes to the Amount field if it has no decimal
// point
//-------------------------------------------------------------
void TCheckAuthDlgClient::AdjustAmount()
{
 char Buf[20];
 TEdit* Eptr;

 (Eptr=DCMan->EditPtr(IDC_AMOUNT))->Transfer(Buf,tdGetData);
 if ( Buf[0] != 0 &&
      strchr(Buf,'.') == NULL )
  {
   strcat(Buf,".00");
   Eptr->Transfer(Buf,tdSetData);
  }

 // Cause Amount ROUNDED to be re-calculated
 AuthMsg->DelFld(FN_AMOUNTROUNDED);
 DCMan->CopyToMsg(IDC_AMOUNT,AuthMsg);
 // Change in Amount may change Field Enabling
 enableFields(authTypeEnabled);
}

//---------------------------------------------------------
// Enable/Disable the Business name field if the Check Type
// selection changes
//----------------------------------------------------
void TCheckAuthDlgClient::enableBusinessName()
{
 DCMan->CopyToMsg(IDC_CHECKTYPE,AuthMsg);
 enableFields(authTypeEnabled);
}

//--------------------------------------------------------------
// Send the Authorization Request and do the Authorization rules
//--------------------------------------------------------------
void TCheckAuthDlgClient::doAuthorization()
{
 bool ret;
 HWND captureWin;
 BYTE kenwoodRcode;
 BYTE declineRcode;
 BYTE approveRcode;

 AuthReqMsg->ClearMsg();
 AdjustAmount();
 DCMan->CopyAllToMsg(AuthReqMsg);
 AuthReqMsg->PutFld(FN_TRANTYPE,T_VOICEREQ);
 AuthReqMsg->PutFld(FN_SOURCETYPE,(BYTE)'V');
 Instruct->Say(MID_SENDINGAUTHREQ);
 captureWin=GetApplication()->GetMainWindow()->SetCapture();
 GetApplication()->GetMainWindow()->SetCursor(0,IDC_WAIT);
 lockFocus=true;
 ret = AuthReqMsg->SendRecv(VOICEAUTH_DEST,(WORD)10000,true);
 lockFocus=false;
 GetApplication()->GetMainWindow()->SetCursor(0,IDC_ARROW);
 ::SetCapture(captureWin);

 if ( ! ret )
  {
   Instruct->Say(MID_NORESPONSETOAUTHREQ);
   return;
  }

 // Check for an Invalid Merchant response here
 char KenwoodTextReply[100];
 AuthReqMsg->GetFld(FN_TEXTRESPONSE,sizeof(KenwoodTextReply),KenwoodTextReply);
 if ( memcmp(KenwoodTextReply,"TID",3) == 0 )
  {
   authStep=STEP_FETCHINGMERCHANTDATA;
   DCMan->SetFocus(IDC_MERCHNUM);
   Instruct->Say(MID_INVALIDKENWOODMERCHANT);
   DCMan->EditPtr(IDC_MERCHNUM)->SetReadOnly(false);
   DCMan->EditPtr(IDC_MERCHNUM)->SetSelection(0,5);
   return;
  }

 // Set all the Fields collected to READ Only
 DCMan->SetAllPresentToReadOnly(AuthMsg);

 // We now have an outstanding Kenwood transaction for which the
 // Kenwood server is waiting for a reply. Set the flag to make sure
 // a cancel message is sent if the user clicks cancel.
 //
 // We also have a transaction to LOG
 replyPending=logPending=true;

 // Copy ACTIVITY and CHECKS from REQ to AuthMsg
 AuthMsg->CopyFld(FN_ACTIVITY,AuthReqMsg);
 AuthMsg->CopyFld(FN_BADCHECKS,AuthReqMsg);
 AuthMsg->CopyFld(FN_TERMINFO,AuthReqMsg);
 AuthMsg->CopyFld(FN_MOTHERSNAME,AuthReqMsg);
 AuthMsg->initClassMembers();

 // Apply Product Adjustments to Rules
 AuthMsg->authRules.ApplyProductAdjustments(AuthMsg);

 // Do Rules to get the Rcodes
 AuthMsg->doRules();

 //logg(__LINE__, "In chkauthdlg.cpp", "",0,0);
 // If it's a Post-Auth, We may have to approve it
 if ( AuthMsg->isPreauthMerchant() &&
      ! AuthMsg->isPreauth() &&
      doPostAuth() )
  {
  //logg(__LINE__, "In chkauthdlg.cpp", " doPostAuth() return",0,0);
  return;
  }

 kenwoodRcode=(BYTE)atoi(KenwoodTextReply);
 if ( ISKENWOODRCODE(kenwoodRcode) )
  {
   AuthMsg->PutFld(FN_RCODE,kenwoodRcode);
   enableFields(AuthMsg->Security);
   doDecision();
   return;
  }

 // Do we have Bad checks from Kenwood?
 if ( AuthMsg->GetFld(FN_BADCHECKS) )
  {
   enableFields(AuthMsg->Security);
   AuthMsg->PutFld(FN_RCODE,(BYTE)RCODE_BADCHECKS);
   doDecision();
   return;
  }

 // Do we have decline rcodes without approval override rcodes?
 AuthMsg->getRcodes(declineRcode,approveRcode);
 if ( declineRcode && ! approveRcode )
   // Check positive file
   if ( AuthMsg->isPositive() )
    approveRcode = RCODE_POSFILEAPPROVAL;
   else
    {
     enableFields(AuthMsg->Security);
     AuthMsg->PutFld(FN_RCODE,declineRcode);
     doDecision();
     return;
    }

 // Approve this check
 if ( AuthMsg->isPreauth() )
  {
   doPreAuth();
   //logg(__LINE__, "In chkauthdlg.cpp", " doPreAuth()",0,0);
   return;
  }

 if ( ! declineRcode & ! approveRcode )
  approveRcode = AuthMsg->getApprovalRcode();

 AuthMsg->PutFld(FN_RCODE,approveRcode);
 sendApproval();
}

//----------------------------------------------------
// Attempt a Post Authorization for a Preauth Merchant
//----------------------------------------------------
bool TCheckAuthDlgClient::doPostAuth()
{
 HWND captureWin;
 BYTE Rcode;
  //logg(__LINE__, "In chkauthdlg.cpp", "in doPostAuth()",0,0);

 Instruct->Say(MID_PREAUTHFETCH);
 captureWin=GetApplication()->GetMainWindow()->SetCapture();
 GetApplication()->GetMainWindow()->SetCursor(0,IDC_WAIT);
 lockFocus=true;
 Rcode = AuthMsg->doPostAuth();
 lockFocus=false;
 GetApplication()->GetMainWindow()->SetCursor(0,IDC_ARROW);
 ::SetCapture(captureWin);
 //logg(__LINE__, "In chkauthdlg.cpp doPostAuth()", "rcode",Rcode,0);

 switch( Rcode )
  {
   case RCODE_PREAUTHAPPROVAL:
    AuthMsg->PutFld(FN_RCODE,Rcode);
    sendApproval();
    //logg(__LINE__, "In chkauthdlg.cpp doPostAuth()", "PREAUTAPPROVAL is",RCODE_PREAUTHAPPROVAL,0);
    return true;

   case RCODE_PREAUTHEXCEEDED:
    AuthMsg->PutFld(FN_RCODE,Rcode);
    enableFields(AuthMsg->Security);
    doDecision();
    //logg(__LINE__, "In chkauthdlg.cpp doPostAuth()", "doDecision() PREAUTHEXCEEDED is",RCODE_PREAUTHEXCEEDED,0);
    return true;

    ////logg(__LINE__, "In chkauthdlg.cpp", "return false",0,0);
   // none of the above Process normally
   default:  return false;
  }

}

//--------------------------------------------
// Do a Pre-authorization for Preauth Merchant
//--------------------------------------------
bool TCheckAuthDlgClient::doPreAuth()
{
 HWND captureWin;
 BYTE Rcode;

 Instruct->Say(MID_PREAUTHFETCH);
 captureWin=GetApplication()->GetMainWindow()->SetCapture();
 GetApplication()->GetMainWindow()->SetCursor(0,IDC_WAIT);
 lockFocus=true;
 Rcode = AuthMsg->doPreAuth();
 lockFocus=false;
 GetApplication()->GetMainWindow()->SetCursor(0,IDC_ARROW);
 ::SetCapture(captureWin);
 //logg(__LINE__, "In chkauthdlg.cpp", "doPreAuth()",0,0);

 AuthMsg->PutFld(FN_RCODE,Rcode);

 switch( Rcode )
  {
   case RCODE_PREAUTHAPPROVAL:
    sendApproval();
   //logg(__LINE__, "In chkauthdlg.cpp", "doPreAuth() Approval",RCODE_PREAUTHAPPROVAL,0);
    return true;

   default:
   case RCODE_MULTIPLEPREAUTHS:
    enableFields(AuthMsg->Security);
    doDecision();
    //logg(__LINE__, "In chkauthdlg.cpp", "doPreAuth() Approval MuliplePreAuths",RCODE_MULTIPLEPREAUTHS,0);
    return true;
  }
}

//-----------------------------------------------------
// Send the current transaction (AuthMsg) to the logger
//-----------------------------------------------------
bool TCheckAuthDlgClient::logTransaction()
{
 bool ret;
 HWND captureWin;
 DCMan->CopyAllToMsg(AuthMsg);
 // JHE .. Fixing Log Timestamp. Use the StartTime we saved when we
 // started instead of the one we put in the message at the beginning.
 // It will not be there or will not be correct.
 COperatorId OperatorId;
 AuthMsg->PutFld(FN_OPERATORID,OperatorId.Get());
 AuthMsg->PutFld(FN_TIMESTAMP,(DWORD)StartTime);
 AuthMsg->PutFld(FN_TIMESTAMP2,(DWORD)time(NULL));
 if ( holdCheckDialog )
  holdCheckDialog->DCMan->CopyAllToMsg(AuthMsg);
 AuthMsg->PutFld(FN_TRANTYPE,T_VOICEAUTH);
 Instruct->Say(MID_LOGGINGTRANSACTION);
 captureWin=GetApplication()->GetMainWindow()->SetCapture();
 GetApplication()->GetMainWindow()->SetCursor(0,IDC_WAIT);
 lockFocus=true;
 // If sending approval, also send Positive file update
 if ( AuthMsg->GetFld(FN_APPROVALNUM) )
  AuthMsg->PosFileUpdate();
 ret = AuthMsg->SendRecv(LOGGER_DEST,(WORD)10000,true);
 lockFocus=false;
 GetApplication()->GetMainWindow()->SetCursor(0,IDC_ARROW);
 ::SetCapture(captureWin);
 logPending=false;
 return ret;
}

//--------------------------
// Send Hold Check Approvals
//--------------------------
bool TCheckAuthDlgClient::sendHoldCheckApprovals()
{
 VOICEOUTPUT vOut;
 HWND captureWin;
 FLDNUM AmountFN;
 FLDNUM ApprovalFN;
 FLDNUM ChecknumFN;
 DWORD  checkNum;
 DWORD  holdCheckcheckNum;
 WORD   approvalNum;
 int    i;
 char   ErrMsgBuf[100];

 memset(&vOut,' ',sizeof(vOut));
 COperatorId OperatorId;
 OperatorId.Get(vOut.OperatorId,sizeof(vOut.OperatorId));
 memcpy(vOut.BVStatus,BVSTATUS_NOTVERIFIED,sizeof(vOut.BVStatus));
 Instruct->Say(MID_SENDINGHOLDCHECKTRANS);
 captureWin=GetApplication()->GetMainWindow()->SetCapture();
 GetApplication()->GetMainWindow()->SetCursor(0,IDC_WAIT);
 lockFocus=true;
 AuthMsg->GetFld(FN_CHECKNUMBER,checkNum);
 AuthMsg->GetFld(FN_APPROVALNUM,approvalNum);

 for (i=1; i<=4; ++i)
  {
   switch(i)
    {
     case 1:  AmountFN=FN_HOLDCHECKAMOUNT1;
              ApprovalFN=FN_HOLDCHECKAPPROVALNUM1;
              ChecknumFN=FN_HOLDCHECKCHECKNUM1;
              break;
     case 2:  AmountFN=FN_HOLDCHECKAMOUNT2;
              ApprovalFN=FN_HOLDCHECKAPPROVALNUM2;
              ChecknumFN=FN_HOLDCHECKCHECKNUM2;
              break;
     case 3:  AmountFN=FN_HOLDCHECKAMOUNT3;
              ApprovalFN=FN_HOLDCHECKAPPROVALNUM3;
              ChecknumFN=FN_HOLDCHECKCHECKNUM3;
              break;
     case 4:  AmountFN=FN_HOLDCHECKAMOUNT4;
              ApprovalFN=FN_HOLDCHECKAPPROVALNUM4;
              ChecknumFN=FN_HOLDCHECKCHECKNUM4;
              break;
    }

   // If we already have an approval number, then we don't send this
   // one (it may have been done in POS already).
   if ( AuthMsg->GetFld(ApprovalFN) ||
        ! AuthMsg->GetFld(AmountFN) )
    continue;

   // If it has the same check number as the primary transaction, then
   // it is the primary transaction. Insert the approval number only
   AuthMsg->GetFld(ChecknumFN,holdCheckcheckNum);
   if ( checkNum == holdCheckcheckNum )
    {
     AuthMsg->PutFld(ApprovalFN,approvalNum);
     continue;
    }

   // Insert Approval Number into Auth Message
   AuthMsg->CopyFld(FN_AMOUNT,AmountFN);
   AuthMsg->GenerateApprovalNumber();
   AuthMsg->CopyFld(ApprovalFN,FN_APPROVALNUM);

   // Set up Inquiry to Kenwood Server

   // Delay for 2 seconds unless it's the first one. Trying to solve a
   // Kenwood lock-up problem. R. Cone told me to do this.
   if ( i != 1 )
     Sleep(2000);

   AuthReqMsg->ClearMsg();
   AuthReqMsg->PutFld(FN_TRANTYPE,T_VOICEREQ);
   AuthReqMsg->PutFld(FN_SOURCETYPE,(BYTE)'V');
   DCMan->CopyAllToMsg(AuthReqMsg);
   AuthReqMsg->CopyFld(FN_AMOUNT,AmountFN,AuthMsg);
   AuthReqMsg->CopyFld(FN_CHECKNUMBER,ChecknumFN,AuthMsg);
   if ( ! AuthReqMsg->SendRecv(VOICEAUTH_DEST,(WORD)10000,true) )
    {
     sprintf(ErrMsgBuf,"FAILED ON CHECK# %d",i);
     MessageBox(ErrMsgBuf,"INQUIRY FAILED",MB_OK|MB_ICONERROR);
     break;
    }

   // Set up Reply to Kenwood Server
   AuthReqMsg->PutFld(FN_TEXTRESPONSE,&vOut,sizeof(vOut));
   AuthReqMsg->PutFld(FN_TRANTYPE,T_AUTHREPLY);
   AuthReqMsg->PutFld(FN_RCODE,(BYTE)RCODE_HOLDCHECKAPPROVAL);
   AuthReqMsg->CopyFld(FN_APPROVALNUM,ApprovalFN,AuthMsg);
   if ( ! AuthReqMsg->SendRecv(VOICEAUTH_DEST,(WORD)10000,true) )
    {
     sprintf(ErrMsgBuf,"FAILED ON CHECK# %d",i);
     MessageBox(ErrMsgBuf,"REPLY FAILED",MB_OK|MB_ICONERROR);
     break;
    }
  }

 GetApplication()->GetMainWindow()->SetCursor(0,IDC_ARROW);
 ::SetCapture(captureWin);

 authStep=STEP_APPROVED;
 Instruct->Say(MID_APPROVALS);
 CurrentDialog = new HoldCheckApprovalDialog(this,(CheckAuth&)*AuthMsg);
 CurrentDialog->Create();

 return true;
}

//-------------------------
// Send Hold Check Declines
//-------------------------
bool TCheckAuthDlgClient::sendHoldCheckDeclines()
{
 VOICEOUTPUT vOut;
 HWND captureWin;
 FLDNUM AmountFN;
 FLDNUM ApprovalFN;
 FLDNUM ChecknumFN;
 DWORD  checkNum;
 DWORD  holdCheckcheckNum;
 int    i;
 int    Len;
 char   ErrMsgBuf[100];

 // Make this a generic Hold Check message instead of an Approval
 Instruct->Say(MID_SENDINGHOLDCHECKTRANS);
 captureWin=GetApplication()->GetMainWindow()->SetCapture();
 GetApplication()->GetMainWindow()->SetCursor(0,IDC_WAIT);
 lockFocus=true;
 Len=sizeof(vOut);
 AuthMsg->GetFld(FN_TEXTRESPONSE,&vOut,Len);
 AuthMsg->GetFld(FN_CHECKNUMBER,checkNum);

 for (i=1; i<=4; ++i)
  {
   switch(i)
    {
     case 1:  AmountFN=FN_HOLDCHECKAMOUNT1;
              ApprovalFN=FN_HOLDCHECKAPPROVALNUM1;
              ChecknumFN=FN_HOLDCHECKCHECKNUM1;
              break;
     case 2:  AmountFN=FN_HOLDCHECKAMOUNT2;
              ApprovalFN=FN_HOLDCHECKAPPROVALNUM2;
              ChecknumFN=FN_HOLDCHECKCHECKNUM2;
              break;
     case 3:  AmountFN=FN_HOLDCHECKAMOUNT3;
              ApprovalFN=FN_HOLDCHECKAPPROVALNUM3;
              ChecknumFN=FN_HOLDCHECKCHECKNUM3;
              break;
     case 4:  AmountFN=FN_HOLDCHECKAMOUNT4;
              ApprovalFN=FN_HOLDCHECKAPPROVALNUM4;
              ChecknumFN=FN_HOLDCHECKCHECKNUM4;
              break;
    }

   // If we already have an approval number, then we don't send this
   // one (it may have been done in POS already).
   if ( AuthMsg->GetFld(ApprovalFN) ||
        ! AuthMsg->GetFld(AmountFN) )
    continue;

   // If it has the same check number as the primary transaction, then
   // it is the primary transaction. Don't send it.
   AuthMsg->GetFld(ChecknumFN,holdCheckcheckNum);
   if ( checkNum == holdCheckcheckNum )
     continue;

   // Set up Inquiry to Kenwood Server
   AuthMsg->CopyFld(FN_AMOUNT,AmountFN);
   AuthReqMsg->ClearMsg();
   AuthReqMsg->PutFld(FN_TRANTYPE,T_VOICEREQ);
   AuthReqMsg->PutFld(FN_SOURCETYPE,(BYTE)'V');
   DCMan->CopyAllToMsg(AuthReqMsg);
   AuthReqMsg->CopyFld(FN_AMOUNT,AmountFN,AuthMsg);
   AuthReqMsg->CopyFld(FN_CHECKNUMBER,ChecknumFN,AuthMsg);
   if ( ! AuthReqMsg->SendRecv(VOICEAUTH_DEST,(WORD)10000,true) )
    {
     sprintf(ErrMsgBuf,"FAILED ON CHECK# %d",i);
     MessageBox(ErrMsgBuf,"INQUIRY FAILURE",MB_OK|MB_ICONERROR);
     break;
    }

   // Set up Reply to Kenwood Server
   AuthReqMsg->PutFld(FN_TEXTRESPONSE,&vOut,sizeof(vOut));
   AuthReqMsg->PutFld(FN_TRANTYPE,T_AUTHREPLY);
   AuthReqMsg->PutFld(FN_RCODE,(BYTE)RCODE_HOLDCHECKAPPROVAL);
   AuthReqMsg->CopyFld(FN_RCODE,AuthMsg);
   if ( ! AuthReqMsg->SendRecv(VOICEAUTH_DEST,(WORD)10000,true) )
    {
     sprintf(ErrMsgBuf,"FAILED ON CHECK# %d",i);
     MessageBox(ErrMsgBuf,"REPLY FAILURE",MB_OK|MB_ICONERROR);
     break;
    }
  }

 GetApplication()->GetMainWindow()->SetCursor(0,IDC_ARROW);
 ::SetCapture(captureWin);

 authStep=STEP_DECLINED;
 Instruct->Say(MID_DECLINED);
 CurrentDialog=new DeclineDialog(this);
 CurrentDialog->Create();

 return true;
}


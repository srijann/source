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
#include <owl/inputdia.h>
#include <owl/color.h>

#include "checkauthapp.h"
#include "chkauthdlg.h"
#include "fetchreferral.h"
#include "splib.h"
#include "Msgid.h"
#include "MsgWin.h"

#define TIMERBAR_WIDTH  540
#define TIMERBAR_HEIGHT  12
#define TIMERBAR_LEFT 6
#define TIMERBAR_TOP  6
#define TIMERBAR_STEP 4
#define TIMERBAR_SECONDS 240

#define CHECKNUMBER_LEN 6
#define FIRSTNAME_LEN 1
#define LASTNAME_LEN 15
#define MICR_LEN 25
#define DOB_LEN 6
#define LICENSE_LEN 26
#define STATE_LEN 2
#define AMOUNT_LEN 8
#define MERCHNUM_LEN 12
#define CITY_LEN 20
#define RESIDENCE_LEN 10
#define ZIP2_LEN 4
#define ZIP1_LEN 5
#define ADRSTATE_LEN 20
#define ADDRESS_LEN 25
#define PRODUCTCLASS_LEN 15
#define BUSINESSNAME_LEN 20
#define PRIVATELABEL_LEN 3
#define PHONE_LEN 7
#define AREACODE_LEN 3
#define SSN_LEN 9
#define CHECKTYPE_LEN 10
#define REFERALNUM_LEN 6
#define TRANSPORT_LEN 1

//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(TCheckAuthDlgClient, TDialog)
//{{TCheckAuthDlgClientRSP_TBL_BEGIN}}
  EV_BN_CLICKED(IDC_STARTBUTTON, NextAction),
  EV_BN_CLICKED(IDC_CANCELBUTTON, CancelClicked),
  EV_WM_TIMER,
  EV_REGISTERED(IPCNotifyMessage, ProcessNetReply ),
  EV_REGISTERED(CHKAUTHNOTIFYMSG, ProcessChkauthNotify ),
//{{TCheckAuthDlgClientRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{TCheckAuthDlgClient Implementation}}

void Disable(TWindow* Win,void*);

//--------------------------------------------------------
// TCheckAuthDlgClient
// ~~~~~~~~~~
// Construction/Destruction handling.
//
TCheckAuthDlgClient::TCheckAuthDlgClient(TWindow* parent, TResId resId, TModule* module)
:
  TDialog(parent, resId, module)
{
 TimerBar=new TGauge(this,IDC_TIMERBAR,
    TIMERBAR_LEFT,TIMERBAR_TOP,TIMERBAR_WIDTH,TIMERBAR_HEIGHT);
 TimerBar->SetRange(0,TIMERBAR_SECONDS-1);
 CancelButton=new TButton(this,IDC_CANCELBUTTON);
 StartButton=new TButton(this,IDC_STARTBUTTON);

 TRANSPORT = new TEdit(this,IDC_TRANSPORT,IDC_TRANSPORT+1);
 CHECKNUMBER = new TEdit(this,IDC_CHECKNUMBER,CHECKNUMBER_LEN+1);
 CHECKTYPE = new TEdit(this,IDC_CHECKTYPE,CHECKTYPE_LEN+1);
 FIRSTNAME = new TEdit(this,IDC_FIRSTNAME,FIRSTNAME_LEN+1);
 LASTNAME = new TEdit(this,IDC_LASTNAME,LASTNAME_LEN+1);
 MICR = new TEdit(this,IDC_MICR,MICR_LEN+1);
 DOB = new TEdit(this,IDC_DOB,DOB_LEN+1);
 LICENSE = new TEdit(this,IDC_LICENSE,LICENSE_LEN+1);
 STATE = new TEdit(this,IDC_STATE,STATE_LEN+1);
 STATE->SetValidator(new TFilterValidator("A-Z"));
 AMOUNT = new TEdit(this,IDC_AMOUNT,AMOUNT_LEN+1);
 AMOUNT->SetValidator(new TDecimalValidator(5,2));
 MERCHNUM = new TEdit(this,IDC_MERCHNUM,MERCHNUM_LEN+1);
 CITY = new TEdit(this,IDC_CITY,CITY_LEN+1);
 RESIDENCE = new TEdit(this,IDC_RESIDENCE,RESIDENCE_LEN+1);
 ZIP2 = new TEdit(this,IDC_ZIP2,ZIP2_LEN+1);
 ZIP1 = new TEdit(this,IDC_ZIP1,ZIP1_LEN+1);
 ADRSTATE = new TEdit(this,IDC_ADRSTATE,ADRSTATE_LEN+1);
 ADDRESS = new TEdit(this,IDC_ADDRESS,ADDRESS_LEN+1);
 PRODUCTCLASS = new TEdit(this,IDC_PRODUCTCLASS,PRODUCTCLASS_LEN+1);
 BUSINESSNAME = new TEdit(this,IDC_BUSINESSNAME,BUSINESSNAME_LEN+1);
 PRIVATELABEL = new TEdit(this,IDC_PRIVATELABEL,PRIVATELABEL_LEN+1);
 PHONE = new TEdit(this,IDC_PHONE,PHONE_LEN+1);
 AREACODE = new TEdit(this,IDC_AREACODE,AREACODE_LEN+1);
 SSN = new TEdit(this,IDC_SSN,SSN_LEN+1);

 ST_TRANSPORT = new TStatic(this,IDCS_TRANSPORT);
 ST_ADRSTATE = new TStatic(this,IDCS_ADRSTATE);
 ST_ADDRESS = new TStatic(this,IDCS_ADDRESS);
 ST_CHECKTYPE = new TStatic(this,IDCS_CHECKTYPE);
 ST_CHECKNUMBER = new TStatic(this,IDCS_CHECKNUMBER);
 ST_CITY = new TStatic(this,IDCS_CITY);
 ST_RESIDENCE = new TStatic(this,IDCS_RESIDENCE);
 ST_ZIP = new TStatic(this,IDCS_ZIP);
 ST_PRODUCTCLASS = new TStatic(this,IDCS_PRODUCTCLASS);
 ST_BUSINESSNAME = new TStatic(this,IDCS_BUSINESSNAME);
 ST_PRIVATELABEL = new TStatic(this,IDCS_PRIVATELABEL);
 ST_PHONE = new TStatic(this,IDCS_PHONE);
 ST_SSN = new TStatic(this,IDCS_SSN);
 ST_NAME = new TStatic(this,IDCS_NAME);
 ST_MICR = new TStatic(this,IDCS_MICR);
 ST_DOB = new TStatic(this,IDCS_DOB);
 ST_LICENSE = new TStatic(this,IDCS_LICENSE);
 ST_STATE = new TStatic(this,IDCS_STATE);
 ST_MERCHNUM = new TStatic(this,IDCS_MERCHNUM);
 ST_AMOUNT = new TStatic(this,IDCS_AMOUNT);

 Instruct = new TMsgWin(this,"Chkauth.Msg",true);
}


TCheckAuthDlgClient::~TCheckAuthDlgClient()
{
  Destroy();

  // INSERT>> Your destructor code here.
}


void TCheckAuthDlgClient::CancelClicked()
{
 KillTimer(1);
 TimerBar->SetValue(0);
 ForEach(Disable);
 StartButton->EnableWindow(true);
 StartButton->SetFocus();
 Instruct->Say(0);
}


void TCheckAuthDlgClient::EvTimer(uint timerId)
{
 TDialog::EvTimer(timerId);
 StepTimer();
}

LRESULT TCheckAuthDlgClient::ProcessNetReply(WPARAM MsgType, LPARAM Id)
{
 return 1;
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
 Instruct->Create();
 Instruct->Say(MID_START);
 mode=MODE_START;
}


//----------------------------------------------------------
// Override EvCommand to prevent ESCape from the main dialog
//----------------------------------------------------------
TResult TCheckAuthDlgClient::EvCommand(uint id, THandle hWndCtl, uint notifyCode)
{
  TResult result;

  // Do NOT Allow Escape
  if ( notifyCode==0 )
   {
    // Don't Allow ESCAPE
    if ( id==IDCANCEL )
      return 0;

    // For reasons unknown, the first press of the enter key is not
    // interpreted as a press of the Start Button. Instead we get a
    // Menu Item 1 selection and the Dialog terminates.
    // Here, we override this action by changing the id to
    // the Start button.

    if ( id==IDOK )
     id=IDC_STARTBUTTON;
   }

  result = TDialog::EvCommand(id, hWndCtl, notifyCode);

  // INSERT>> Your code here.

  return result;
}

void TCheckAuthDlgClient::StepTimer(void)
{
 int Value=TimerBar->GetValue() + TIMERBAR_STEP;

 if ( Value >= TIMERBAR_SECONDS-1)
  {
   TimerBar->SetValue(TIMERBAR_SECONDS-1);
   StopTime=time(NULL);
   KillTimer(1);
   MessageBoxf(HWindow,"TIMEOUT",MB_OK,"ELAPSED TIME %d",StopTime-StartTime);
  }
 else
  TimerBar->SetValue( Value );
}

//--------------------------------------------
// No Referral on File, Fetch a Station record
//--------------------------------------------
void TCheckAuthDlgClient::FetchStation()
{
 ST_MERCHNUM->EnableWindow(true);
 MERCHNUM->EnableWindow(true);
 Instruct->Say(MID_GETMERCHANT);
 MERCHNUM->SetFocus();

}

//--------------------------
// Handle the Notify Message
//--------------------------
#pragma argsused
LRESULT TCheckAuthDlgClient::ProcessChkauthNotify(WPARAM MsgType, LPARAM Id)
{
 switch(MsgType)
  {
   case NOTIFY_REFERRALFETCHCOMPLETE:
     CurrentDialog=0;
     FetchStation();
     break;
  }
 return 0;
}

//-----------------------------------------
// Clear All the Edit fields in the Message
//-----------------------------------------
void TCheckAuthDlgClient::ClearAllFields()
{
 TRANSPORT->Clear();
 ADRSTATE->Clear();
 ADDRESS->Clear();
 CHECKTYPE->Clear();
 CHECKNUMBER->Clear();
 CITY->Clear();
 RESIDENCE->Clear();
 ZIP2->Clear();
 ZIP1->Clear();
 PRODUCTCLASS->Clear();
 BUSINESSNAME->Clear();
 PRIVATELABEL->Clear();
 PHONE->Clear();
 AREACODE->Clear();
 SSN->Clear();
 FIRSTNAME->Clear();
 LASTNAME->Clear();
 MICR->Clear();
 DOB->Clear();
 LICENSE->Clear();
 STATE->Clear();
 AMOUNT->Clear();
 MERCHNUM->Clear();
}

//----------------------
// We come here when the start button is clicked or when the user
// Presses Enter.
//----------------------
void TCheckAuthDlgClient::NextAction()
{
 TFetchReferral* ReferralDialog;

 switch(mode)
  {
   case MODE_START:
     StartButton->EnableWindow(false);
     SetTimer(1,TIMERBAR_STEP * 1000,NULL);
     StartTime=time(NULL);
     // Execute the Referral Fetch.
     ClearMsg();
     Instruct->ShowWindow(SW_SHOW);
     Instruct->Say(MID_GETREFERRALNUM);
     CurrentDialog = ReferralDialog = new TFetchReferral(this,IPCMsgPtr());
     ReferralDialog->Create();
     CancelButton->EnableWindow(true);
     break;
   case MODE_GETREFERRAL:  // Shouldn't get here
     MYTRACE("UNEXPECTED MODE IN NextAction - GETREFERRAL");
     break;
   case MODE_GETSTATION:
     MYTRACE("NextAction - GETSTATION");
     break;
   default:
     MYTRACE("UNRECOGNIZED MODE IN NextAction");
     break;
  }

}


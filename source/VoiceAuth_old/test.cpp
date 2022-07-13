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
#include "checkservice.h"
#include "chkauthdialog.h"
#include "fetchreferral.h"
#include "VerifyReferralDialog.h"
#include "splib.h"
#include "DecimalValidator.h"
#include "MsgWin.rh"
#include "MsgWin.h"
#include "Msgid.h"
#include "DestName.h"
#include "TranType.h"
#include "DecisionDialog.h"
#include "ApprovalDialog.h"
#include "DeclineDialog.h"
#include "rcodes.h"
#include "kwrec.h"
#include "BankVerify.h"

//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(TCheckAuthDlgClient, TDialog)
//{{TCheckAuthDlgClientRSP_TBL_BEGIN}}
  EV_BN_CLICKED(IDC_STARTBUTTON, doNext),
  EV_BN_CLICKED(IDC_CANCELBUTTON, CancelClicked),
  EV_BN_CLICKED(IDC_VIEWBTN, ViewClicked),
  EV_WM_TIMER,
  EV_REGISTERED("VOICEAUTH", ProcessChkauthNotify ),
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
  TDialog(parent, resId, module),
  CurrentDialog(0),
  netioInProgress(false),
  needsCancelReply(false)
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

 Instruct = new TMsgWin(this,IDD_MSGWIN,MessageFileName,true);
 AuthMsg=new CheckAuth();
 AuthReqMsg=new IPCMessage();
 SetupControls();
}

//-----------------
// Class Destructor
//-----------------
TCheckAuthDlgClient::~TCheckAuthDlgClient()
{
 Destroy();
 delete AuthMsg;
 delete AuthReqMsg;
 delete DCMan;
}

//---------------
// Cancel Clicked
//---------------
void TCheckAuthDlgClient::CancelClicked()
{
 if ( needsCancelReply )
  sendCancel();

 if ( CurrentDialog )
  {
   delete CurrentDialog;
   CurrentDialog=0;
  }

 KillTimer(1);
 TimerBar->SetValue(0);
 WaitForStart();
}

void TCheckAuthDlgClient::ViewClicked()
{
 if ( ! CurrentDialog )
  return;

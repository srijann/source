//----------------------------------------------------------------------------
//  Project IPCSample
//  JHE Software, Inc.
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    IPCSample Application
//  FILE:         IPCSampleDialog.cpp
//  AUTHOR:       Jack H. Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  This Source file implements a Sample IPC Client Dialog that sends
//  ADMIN messages to an IPC Server and displays the response.
//----------------------------------------------------------------------------

#include <owl/pch.h>

#include "IPCSampleApp.h"
#include "IPCSampleDialog.h"
#include "IPCSample.rh"

#include "ipc.h"
#include "splib.h"
#include "TranType.h"
#include "CheckAuthFn.h"

#define PROCESSNAMELEN MAX_IPCLOGICALNAME+1
#define MSGTEXTLEN 512

//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(TIPCSampleDlgClient, TDialog)
//{{TIPCSampleDlgClientRSP_TBL_BEGIN}}
  // This is the RESPONSE TABLE Entry for OUR IPC Message Handler
  EV_BN_CLICKED(IDSEND, BNClicked),
//{{TIPCSampleDlgClientRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{TIPCSampleDlgClient Implementation}}


//------------------------------------------------------------------
// TIPCSampleDlgClient
// ~~~~~~~~~~
// Construction/Destruction handling.
//
// We Inherit from IPCMEssage so that our DIALOG includes a Message
// Object we can interact with to PUT/GET fields and SEND/RECV
// Messages.
//------------------------------------------------------------------
TIPCSampleDlgClient::TIPCSampleDlgClient(TWindow* parent, int argc, char* argv[])
:
  TDialog(parent, IDD_CLIENT),
  IPCMessage(),
  Argc(argc),
  Argv(argv)
{
  // INSERT>> Your constructor code here.
}


//-------------------------------------------------------------------
// In the Destructor we call IPCDestroy to clean up any IPC resources
// before the window closes.
//--------------------------------------------------------------------
TIPCSampleDlgClient::~TIPCSampleDlgClient()
{
 IPCDestroy(GetHandle());
 Destroy();
}

//------------------------------------
// Set up Interface to DIALOG Controls
//------------------------------------
void TIPCSampleDlgClient::SetupWindow()
{
 MsgReply = new TEdit(this,IDC_MSGREPLY,MSGTEXTLEN);
 Command = new TEdit(this,IDC_COMMAND,100);
 ProcessName = new TEdit(this,IDC_PROCESSNAME,PROCESSNAMELEN);
 SendButton = new TButton(this,IDSEND);
 TDialog::SetupWindow();
 if ( Argc > 2 )
  {
   ProcessName->Transfer(Argv[1],tdSetData);
   Command->Transfer(Argv[2],tdSetData);
   PostMessage(WM_COMMAND,IDSEND,BN_CLICKED);
  }
}

//-----------------------------------------------------------------------
// How ASYNCHRONOUS IPC Works --
//
// Use the GetFld, PutFld, and ClearMsg functions to build the
// IPCMessage.
//
// Use the SendMsg function to send the IPCMessage to a NAMED
// process. The SendMsg function will insert a SYNCID into the
// Caller's IPCMessage buffer and send a COPY of the Message to
// the destination process. The local IPCMessage should not be modified
// until the reply comes back or times out.
//
// The Reply message will be posted to your IPCNotify event handler
// Identifying the Incoming buffer via the LPARAM. The event
// handler should attempt to fetch the message using the RecvReply
// function, passing it the LPARAM and the SAME IPCMessage used to
// Send the Request. The RecvReply function will compare the SYNCID of
// the incoming message with the SYNCID in the caller's IPCMessage.
// If they match, the incoming IPCMessage will overlay the
// local IPCMessage and RecvReply will return true. If they do not match
// RecvReply will leave the local IPCMessage alone, discard the
// incoming message, and return false.
//
// Alternative:  You can handle multiple requests by using the RecvMsg
// function instead of the RecvReply function. In this case, SYNCID
// matching becomes YOUR responsibility. The function GetMsgSyncID
// is provided to allow you to query a Message for the embedded SYNCID.
//-----------------------------------------------------------------------


//---------------------------------------------------------------------
// The SEND function .. Executed when Control Button is Clicked.
// Transfer the Message From the Window to the IPCMessage and Send it
// to the Process Name Supplied by the user.
//---------------------------------------------------------------------
void TIPCSampleDlgClient::BNClicked()
{
 char processname[PROCESSNAMELEN];
 char msgtext[MSGTEXTLEN+5+1];
 bool ret;
 HWND captureWin;

 // Get the Process Name to SEND TO. It's Required
 ProcessName->Transfer(processname,tdGetData);
 if ( processname[0] == '\0' )
  {
   MessageBox("You Must Specify a Process Name","Error",MB_OK);
   return;
  }

 Command->Transfer(msgtext,tdGetData);
 ClearMsg();
 PutFld(IPCFN_ADMINMESSAGE,msgtext);
 PutFld(FN_TRANTYPE,T_ADMINMSG);

 captureWin=GetApplication()->GetMainWindow()->SetCapture();
 GetApplication()->GetMainWindow()->SetCursor(0,IDC_WAIT);
 ret=SendRecv(processname,15000,true);
 GetApplication()->GetMainWindow()->SetCursor(0,IDC_ARROW);
 ::SetCapture(captureWin);
 if ( ret )
  {
   GetFld(IPCFN_ADMINMESSAGE,sizeof(msgtext),msgtext);
   MsgReply->Transfer(msgtext,tdSetData);
  }

 if ( Argc > 2 )
  GetApplication()->GetMainWindow()->PostMessage(WM_SYSCOMMAND,SC_CLOSE); 
  
}



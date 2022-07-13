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
  EV_REGISTERED(IPCNotifyMessage, ProcessMessage ),
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
TIPCSampleDlgClient::TIPCSampleDlgClient(TWindow* parent, TResId resId, TModule* module)
:
  TDialog(parent, resId, module),
  IPCMessage()
{
  // INSERT>> Your constructor code here.
}


//-------------------------------------------------------------------
// In the Destructor we call IPCDestroy to clean up any IPC resources
// before the window closes.
//--------------------------------------------------------------------
TIPCSampleDlgClient::~TIPCSampleDlgClient()
{
 Destroy();
 IPCDestroy(GetHandle());
}

//------------------------------------
// Set up Interface to DIALOG Controls
//------------------------------------
void TIPCSampleDlgClient::SetupWindow()
{
 MsgText = new TEdit(this,IDC_MSGTEXT,MSGTEXTLEN);
 MsgReply = new TEdit(this,IDC_MSGREPLY,MSGTEXTLEN);
 DebugText = new TEdit(this,IDC_DEBUG,MSGTEXTLEN);
 ProcessName = new TEdit(this,IDC_PROCESSNAME,PROCESSNAMELEN);
 AltProcessName = new TEdit(this,IDC_ALTPROCESSNAME,PROCESSNAMELEN);
 SyncButton = new TRadioButton(this,IDC_SYNCRADIOBUTTON);
 ASyncButton = new TRadioButton(this,IDC_ASYNCRADIOBUTTON);
 SendButton = new TButton(this,IDSEND);
 TDialog::SetupWindow();
 ASyncButton->SetCheck(true);
}

//-----------------------------------------------------------
// Set/Reset the Waiting for Answer State.
//
// Set the Cursor of the dialog and control windows to either
// an Hourglass or arrow and disable/enable the SEND Button.
//------------------------------------------------------------
void TIPCSampleDlgClient::Waiting(bool wait)
{
 TResId id = wait ? IDC_WAIT : IDC_ARROW;
 SendButton->SetCursor(0,id);
 MsgText->SetCursor(0,id);
 MsgReply->SetCursor(0,id);
 DebugText->SetCursor(0,id);
 ProcessName->SetCursor(0,id);
 SetCursor(0,id);
 SendButton->EnableWindow( wait ? false : true );
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
 char altprocessname[PROCESSNAMELEN];
 char textbuf[MSGTEXTLEN+1];
 char replymsg[MSGTEXTLEN+5+1];
 bool ret;

 // Get the Process Name to SEND TO. It's Required
 ProcessName->Transfer(processname,tdGetData);
 if ( processname[0] == '\0' )
  {
   MessageBox("You Must Specify a Process Name","Error",MB_OK);
   return;
  }

 // Set up the ADMIN Message using MessageText from screen
 memset(textbuf,0,sizeof(textbuf));
 MsgText->Transfer(textbuf,tdGetData);

 // Clear the Reply Message From the screen
 MsgReply->Transfer("",tdSetData);

 // Clear out the IPCMessage So we start fresh
 ClearMsg();

 // Put the Message Text into the IPCMessage
 if ( strlen(textbuf) )
   PutFld(IPCFN_ADMINMESSAGE,textbuf);

 // If there is a DEBUG command, put it into the Message
 memset(textbuf,0,sizeof(textbuf));
 DebugText->Transfer(textbuf,tdGetData);
 PutFld(FN_TRANTYPE,T_ADMINMSG);

 if ( strlen(textbuf) )
  {
   // If the DEBUG command says FILL, fill the message with a
   // Variable Length field to it's maximum size and change
   // it to TRACEDUMP. This is a feature for testing the
   // network's ability to handle the largest message possible.

   bool fill=false;

   if ( stricmp(textbuf,"FILL") == 0 )
    {
     fill=true;
     strcpy(textbuf,"TRACEDUMP 1000");
    }

   PutFld(IPCFN_DEBUG,textbuf);

   if ( fill )
    {
     int   length = MAX_MSG - GetMsgLength() - 4;
     char* fillbuf = new char[length];
     memset(fillbuf,255,length);
     PutFld(255,fillbuf,length);
     delete fillbuf;
    }

  }

 // Sending Asynchronously
 if ( ASyncButton->GetCheck() )
  {
   // Send to Named Process, expecting an answer to be posted to this
   // window in 5 seconds. If SendMsg returns false, the process name
   // is unknown.
   if ( ! SendMsg(processname,GetHandle(),5000) )
     MessageBoxf(GetHandle(),"Error",MB_OK,
                 "Unable to Send Message to %s\n"
                 "See EVENT LOG for Details",processname);
   else
     Waiting(true);
  }
 // Sending Synchronously 
 else
  {
   AltProcessName->Transfer(altprocessname,tdGetData);
   Waiting(true);
   if ( altprocessname[0] )
     ret = SendRecv(processname,5000,true,0,altprocessname);
   else
     ret = SendRecv(processname,5000,true);
   Waiting(false);
   if ( ret )
    {
     // Display reply message on the screen
     if ( GetFld(IPCFN_ADMINMESSAGE,sizeof(replymsg),replymsg) )
      MsgReply->Transfer(replymsg,tdSetData);
     else
      MsgReply->Transfer("",tdSetData);
    }
   else
     MessageBoxf(GetHandle(),"Error",MB_OK,
                 "Unable to Send Message to %s\n"
                 "See EVENT LOG for Details",processname);
  }
}

//---------------------------------------------------------------------
// The IPC NOTIFICATION Event Handler. Called when an IPCNotify
// Message is Posted to this WINDOW.
//
//  WPARAM = can be either
//
//   IPCMessageReceived = A message was received. Call RecvReply
//                        to fetch the message.
//
//   IPCMessageTimedOut = The Message TIMER went off before a reply
//                        was received. In this case LPARAM is the
//                        SYNCID of the message.
//
//  LPARAM = This is either the BUFFER ID to be used to fetch the
//           message or the SYNCID of the message if it is a timeout
//           notification.
//--------------------------------------------------------------------

LRESULT TIPCSampleDlgClient::ProcessMessage(WPARAM MsgType, LPARAM Id)
{
 char replymsg[MSGTEXTLEN+5+1];

 // Is it a TIMEOUT?
 if ( MsgType==IPCMessageTimedOut )
  {
   // When a Message TIMES out make sure you reset the SYNCID of your
   // local IPCMessage either by calling ResetMsgSyncId or ClearMsg.
   // If you don't do this, a late response won't be discarded if/when
   // it arrives because the SYNCID's will match.
   ResetMsgSyncId();

   // Tell the USER that we didn't get an answer
   MessageBox("TIMED OUT","MESSAGE",MB_OK|MB_ICONEXCLAMATION|MB_APPLMODAL);

   // Get out of Wait for Answer Mode
   Waiting(false);
  }
 else
 // RecvReply returns true ONLY if this Reply matches the Request
 if ( RecvReply(Id) )
  {
   // Display reply message on the screen
   if ( GetFld(IPCFN_ADMINMESSAGE,sizeof(replymsg),replymsg) )
    MsgReply->Transfer(replymsg,tdSetData);
   else
    MsgReply->Transfer("",tdSetData);

   // Get out of Wait for Answer Mode
   Waiting(false);
  }

 return 1;
}




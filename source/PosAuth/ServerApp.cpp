//----------------------------------------------------------------------------
//  Project ServerApp
//  JHE Software, Inc.
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    ServerApp Application
//  FILE:         ServerApp.cpp
//  AUTHOR:       Jack H Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  This is the Framework for your Server Application. Grow this
//  object to build your Server Application. It is instantiated when
//  the main window is created.
//
//  You need to build two objects, both contained herein, ie.,
//  the ServerAppInfo and the ServerApp objects. See below for details.
//----------------------------------------------------------------------------

#include "win4me.h"
#pragma hdrstop
#include <stdio.h>
#include "ServerApp.h"
#include "messages.h"
#include "rcodes.h"
#include "DestName.h"

//-----------------------------------------------------------------
// The ServerAppInfo object. Provides Information about the Server.
//-----------------------------------------------------------------
ServerAppInfo::ServerAppInfo()
{
 // You Must supply a Process Name for the Server
 strcpy(ServerProcessName,PROCESSNAME_POSAUTH);

 // Supply A Version Number for the Dialog Window and ADMIN commands.
 strcpy(ProgramVersionInfo,"1.21");
}

//---------------------------------------------------------------------
// ServerApp Constructor .. Do your Server Initialization Here, opening
// files, etc.
//---------------------------------------------------------------------
ServerApp::ServerApp()
{
 // Set this flag to true if your initialization succeeds. If it fails
 // set it to false. The Main function will abort the process.
 ServerInit=true;
}

//----------------------------------------------------------------------
// ServerApp Destructor .. Shut down your ServerApp here, closing files,
// etc.
//----------------------------------------------------------------------
ServerApp::~ServerApp()
{
 if ( ServerInit )
  {
   // Do the Shutdown stuff here. You don't want to do it if your
   // initialization failed.
   ServerInit=false;
  }
}


//----------------------------------------------------------------------
// Handle Your Application Messages Here. All your member functions have
// access to the IPCMessage. Reply or Send the Message after you have
// processed it. The calling function does not do this for you.
//-----------------------------------------------------------------------
void ServerApp::DoApplicationMsg()
{
 FLDNUM InvalidField;
 bool   FieldMissing;
 BYTE DeclineRcode, ApprovalRcode;
 BYTE Rcode;

 // call the init function to get things set up
 Msg.initClassMembers();

 // Do we have all the stuff we need?
 if ( ! Msg.hasMerchantRec ||
      ! Msg.hasDataReq ||
      ! Msg.hasAuthRules )
  {
   Msg.doPosReply(RCODE_INVALIDMERCHANT);
   return;
  }

 // Do the Data Requirements/Validation stuff
 if ( ! Msg.fieldValidation(Msg.Pos,InvalidField,FieldMissing,Rcode) )
  {
   Msg.doPosReply(Rcode);
   return;
  }

 // if Post Authorization for PreAuth merchant, only rule that applies
 // is Single check
 if ( Msg.isPreauthMerchant() )
  {
   if ( Msg.CheckAmount() == 10 )
     Msg.isPreauth(true);
   else
   if ( (Rcode=Msg.doPostAuth()) != 0 )
    {
     Msg.doPosReply(Rcode);
     return;
    }
  }

 // Check for existence of Negative Data
 if ( Msg.isBadChecks() )
  {
   // Do a hard decline unless we have Mother's Maiden Name
   Msg.doPosReply(RCODE_BADCHECKS,
     Msg.GetFld(FN_MOTHERSNAME) ? 0 : "DECLINE CODE 1");
   return;
  }

 // Do the Rules
 Msg.doRules();

 // Check the Rcodes to see if we should decline it
 Msg.getRcodes(DeclineRcode,ApprovalRcode);
 if ( DeclineRcode && ! ApprovalRcode )
   if ( Msg.isPositive() )
    ApprovalRcode=RCODE_POSFILEAPPROVAL;
   else
    {
     Msg.doPosReply(DeclineRcode);
     return;
    }

 // If it's a Preauth, check for log the preauth (checking for
 // multiples)
 if ( Msg.isPreauth() &&
      (Rcode=Msg.doPreAuth()) != 0 )
  {
   Msg.doPosReply(Rcode);
   return;
  }

 // Approve it
 if ( ! ApprovalRcode )
  ApprovalRcode = Msg.getApprovalRcode();

 Msg.doPosReply(ApprovalRcode);
 Msg.PosFileUpdate();

 // If it's a Hold Check Approval, Put it on the referral queue so any
 // declines that follow can be matched.
 if ( Msg.isHoldCheckMerchant() &&
      ! Msg.GetFld(FN_REFERRALNUM) )
   Msg.QueueMsg();
}

//---------------------------------------------------------------------
// Do your ADMIN Message Support here. ADMIN messages are text messages
// found in the IPCFN_ADMINMESSAGE field.
//
// The MAIN Window also has an ADMIN Message Handler to take default
// actions for common commands. Before taking the default action,
// the admin message is sent here for processing.
//
// If the ADMIN Message is one of your own commands or you want to
// override and prevent the default action from taking place, return
// true.
//
// If you don't recognize the command, return false to allow the
// default action to take place.
//---------------------------------------------------------------------
bool ServerApp::DoAdminMsg()
{
 char MsgText[255];
 int  len=sizeof(MsgText);

 // The field will always be there
 Msg.GetFld(IPCFN_ADMINMESSAGE,len,MsgText);

 // Catch the HELP command and insert your own Command list. The
 // Default Handler will append the default commands to your list.
 if ( memcmp(MsgText,"HELP",4) == 0 )
  {
   strcat(MsgText,", NOHELP");
   Msg.PutFld(IPCFN_ADMINMESSAGE,MsgText);
   return false;
  }

 return false;
}





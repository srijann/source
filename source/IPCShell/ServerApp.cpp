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
#include "ServerApp.h"
#include "messages.h"
#include <process.h>
#include <errno.h>
#include <stdio.h>

//-----------------------------------------------------------------
// The ServerAppInfo object. Provides Information about the Server.
//-----------------------------------------------------------------
ServerAppInfo::ServerAppInfo()
{
 // You Must supply a Process Name for the Server
 strcpy(ServerProcessName,"IPCSHELL");

 // Supply A Version Number for the Dialog Window and ADMIN commands.
 strcpy(ProgramVersionInfo,"1.0");
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
 IPCLogEvent(ServerStarted);
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
   IPCLogEvent(ServerStopped);
  }
}


//----------------------------------------------------------------------
// Handle Your Application Messages Here. All your member functions have
// access to the IPCMessage. Reply or Send the Message after you have
// processed it. The calling function does not do this for you.
//-----------------------------------------------------------------------
void ServerApp::DoApplicationMsg()
{
 // In this Example we just reply to everything.
 Msg.SendMsg("REPLY");
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

 if (memcmp(MsgText,"SYS",3) == 0 )
  {
   system(&MsgText[4]);
   Msg.PutFld(IPCFN_ADMINMESSAGE,"OKAY");
   return true;
  }

 if ( memcmp(MsgText,"RUN",3) == 0 )
  {
   if ( (spawnl(P_NOWAIT,&MsgText[4],&MsgText[4],NULL)) == -1 )
    {
     sprintf(MsgText,"FAILED: %s",strerror(errno));
     Msg.PutFld(IPCFN_ADMINMESSAGE,MsgText);
    }
   else
     Msg.PutFld(IPCFN_ADMINMESSAGE,"OKAY");
   return true;
  }

 return false;
}





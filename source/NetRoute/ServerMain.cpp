//----------------------------------------------------------------------------
//  Project ServerApp
//  JHE Software, Inc.
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    ServerApp Application
//  FILE:         ServerMain.cpp
//  AUTHOR:       Jack H Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  This is the Main module for the Server Application. It Initializes
//  The IPC Interface and creates the main WINDOW as the primary IPC
//  MESSAGE QUEUE for this SERVER process.
//
//  Instantiates the Programmer-built ServerApp object and routes
//  all incoming messages to it for processing.
//----------------------------------------------------------------------------

#define  STRICT
#include <windows.h>
#include <stdio.h>
#include <time.h>
#pragma hdrstop
#include "ServerApp.h"
#include "ipc.h"         

static ServerApp* App;

void CALLBACK ApplFunc(void)
{
 App->DoApplicationMsg();
}

int CALLBACK AdminFunc(void)
{
 return App->DoAdminMsg();
}

#pragma argsused
#pragma option -w-aus
int PASCAL WinMain( HINSTANCE hInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszCmdLine,
                    int nCmdShow )
{
 ServerAppInfo Info;
 MSG msg;

 IPCInit(Info.ServerProcessName,0);
 App = new ServerApp;

 if ( ! App->ServerInit )
  {
   MessageBox(0,"PROGRAM INITIALIZATION FAILED\n"
                "SEE EVENT LOG FOR DETAILS","PROGRAM FAILURE",
              MB_ICONERROR|MB_OK);
   delete App;           
   return 0;
  }

 IPCServer Server(hInstance,
                  App->ServerProcessName,
                  App->ProgramVersionInfo,
                  &App->Msg,
                  ApplFunc,
                  AdminFunc);

 while( GetMessage( &msg, NULL, 0, 0 ) )
  {
   TranslateMessage( &msg );
   DispatchMessage( &msg );
  }

 delete App;
 return msg.wParam;
}




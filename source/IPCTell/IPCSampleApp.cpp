//----------------------------------------------------------------------------
//  Project IPCSample
//  JHE Software, Inc.
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    IPCSample Application
//  FILE:         IPCSampleApp.cpp
//  AUTHOR:       Jack H. Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  This is a Sample IPC Client Interface Program. This module
//  demonstrates how to initialize and shutdown the IPC interface.
//
//  The associated DIALOG module IPCSampleDialog demonstrates how
//  to send IPC messages and Receive Replies.
//----------------------------------------------------------------------------

#include <owl/pch.h>

#include <stdio.h>

#include "IPCSampleApp.h"
#include "IPCSampleDialog.h"                        // Definition of client class.

// Include this Header for direct calls to the IPC Interface.
#include "ipc.h"


//{{IPCSampleApp Implementation}}


//
// Build a response table for all messages/commands handled
// by the application.
//
DEFINE_RESPONSE_TABLE1(IPCSampleApp, TApplication)
//{{IPCSampleAppRSP_TBL_BEGIN}}
  EV_COMMAND(CM_HELPABOUT, CmHelpAbout),
//{{IPCSampleAppRSP_TBL_END}}
END_RESPONSE_TABLE;


//--------------------------------------------------------
// IPCSampleApp
//
IPCSampleApp::IPCSampleApp() : TApplication("IPCSample")
{

  // INSERT>> Your constructor code here.
}


IPCSampleApp::~IPCSampleApp()
{
  // INSERT>> Your destructor code here.
}


//--------------------------------------------------------
// IPCSampleApp
// ~~~~~
// Application intialization.
//
void IPCSampleApp::InitMainWindow()
{
  if (nCmdShow != SW_HIDE)
    nCmdShow = (nCmdShow != SW_SHOWMINNOACTIVE) ? SW_SHOWNORMAL : nCmdShow;

  TSDIDecFrame* frame = new TSDIDecFrame(0, GetName(), 0, false);
  frame->SetFlag(wfShrinkToClient);

  // Assign ICON w/ this application.
  //
  frame->SetIcon(this, IDI_SDIAPPLICATION);
  frame->SetIconSm(this, IDI_SDIAPPLICATION);

  SetMainWindow(frame);

}



//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(TSDIDecFrame, TDecoratedFrame)
//{{TSDIDecFrameRSP_TBL_BEGIN}}
//{{TSDIDecFrameRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{TSDIDecFrame Implementation}}


static int Argc;
static char** Argv;

TSDIDecFrame::TSDIDecFrame(TWindow* parent, const char far* title, TWindow* clientWnd, bool trackMenuSelection, TModule* module)
:
  TDecoratedFrame(parent, title, !clientWnd ? new TIPCSampleDlgClient(0,Argc,Argv) : clientWnd, trackMenuSelection, module)
{
  // INSERT>> Your constructor code here.

}


TSDIDecFrame::~TSDIDecFrame()
{
  // INSERT>> Your destructor code here.

}


//--------------------------------------------------------
// IPCSampleApp
// ~~~~~~~~~~~
// Menu Help About Wtell command
//
void IPCSampleApp::CmHelpAbout()
{
}

int OwlMain(int argc, char* argv[])
{
 Argc = argc;
 Argv = argv;
 // You Must initialize the IPC Interface before the application
 // Starts. This sets up access to the Shared Memory and obtains
 // necessary info from the registry.
 IPCSyncIOInit("IPCTell");

 IPCSampleApp   app;
 return app.Run();
}


//----------------------------------------------------------------------------
//  Project CheckAuth
//
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    CheckAuth Application
//  FILE:         checkauthapp.cpp
//  AUTHOR:
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of TCheckAuthApp (TApplication).
//
//----------------------------------------------------------------------------

#include "win4me.h"
#include "chkauth.h"
#include <stdio.h>
#include "checkauthapp.h"
#include "chkauthdlg.h"                        // Definition of client class.
#include "appnotify.h"


//{{TCheckAuthApp Implementation}}




//
// Build a response table for all messages/commands handled
// by the application.
//
DEFINE_RESPONSE_TABLE1(TCheckAuthApp, TApplication)
//{{TCheckAuthAppRSP_TBL_BEGIN}}
  EV_COMMAND(CM_HELPABOUT, CmHelpAbout),
//{{TCheckAuthAppRSP_TBL_END}}
END_RESPONSE_TABLE;


//--------------------------------------------------------
// TCheckAuthApp
//--------------------------------------------------------
TCheckAuthApp::TCheckAuthApp() : TApplication("Check Authorization")
{
}


TCheckAuthApp::~TCheckAuthApp()
{
  // INSERT>> Your destructor code here.
}


//--------------------------------------------------------
// TCheckAuthApp
// ~~~~~
// Application intialization.
//
void TCheckAuthApp::InitMainWindow()
{
  if (nCmdShow != SW_HIDE)
    nCmdShow = (nCmdShow != SW_SHOWMINNOACTIVE) ? SW_SHOW:
          nCmdShow;

  TSDIDecFrame* frame = new TSDIDecFrame(0, GetName(), 0, false);
  frame->SetFlag(wfShrinkToClient);

  // Override the default window style for the main window.
  //
  frame->Attr.Style |= WS_POPUPWINDOW;
  //  WS_BORDER | WS_CAPTION | WS_CLIPCHILDREN | WS_MINIMIZEBOX | WS_SYSMENU | WS_THICKFRAME | WS_VISIBLE;
  frame->Attr.Style &= ~(WS_CHILD);

  // Assign ICON w/ this application.
  //
  frame->SetIcon(this, IDI_SDIAPPLICATION);
  frame->SetIconSm(this, IDI_SDIAPPLICATION);

  SetMainWindow(frame);

}



//{{TSDIDecFrame Implementation}}


TSDIDecFrame::TSDIDecFrame(TWindow* parent, const char far* title, TWindow* clientWnd, bool trackMenuSelection, TModule* module)
:
  TDecoratedFrame(parent, title, !clientWnd ? new TCheckAuthDlgClient(0) : clientWnd, trackMenuSelection, module)
{
  // INSERT>> Your constructor code here.

}


TSDIDecFrame::~TSDIDecFrame()
{
  // INSERT>> Your destructor code here.

}


//--------------------------------------------------------
// TCheckAuthApp
// ~~~~~~~~~~~
// Menu Help About CheckAuth command
//
void TCheckAuthApp::CmHelpAbout()
{
}

int OwlMain(int , char* [])
{
 SetAppNotifyMsg("VOICEAUTH");
 IPCSyncIOInit("VOICAUTH");
 TCheckAuthApp   app;
 return app.Run();
}



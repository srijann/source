//----------------------------------------------------------------------------
//  Project Test
//  
//  Copyright © 1997. All Rights Reserved.
//
//  SUBSYSTEM:    Test Application
//  FILE:         testapp.cpp
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of TTestApp (TApplication).
//
//----------------------------------------------------------------------------

#include <owl/pch.h>

#include <stdio.h>

#include "testapp.h"
#include "tstdlgcl.h"                        // Definition of client class.


//{{TTestApp Implementation}}


//
// Build a response table for all messages/commands handled
// by the application.
//
DEFINE_RESPONSE_TABLE1(TTestApp, TApplication)
//{{TTestAppRSP_TBL_BEGIN}}
  EV_COMMAND(CM_HELPABOUT, CmHelpAbout),
//{{TTestAppRSP_TBL_END}}
END_RESPONSE_TABLE;


//--------------------------------------------------------
// TTestApp
//
TTestApp::TTestApp() : TApplication("Test")
{

  // INSERT>> Your constructor code here.
}


TTestApp::~TTestApp()
{
  // INSERT>> Your destructor code here.
}


//--------------------------------------------------------
// TTestApp
// ~~~~~
// Application intialization.
//
void TTestApp::InitMainWindow()
{
  if (nCmdShow != SW_HIDE)
    nCmdShow = (nCmdShow != SW_SHOWMINNOACTIVE) ? SW_SHOWMAXIMIZED : nCmdShow;

  TSDIDecFrame* frame = new TSDIDecFrame(0, GetName(), 0, false);
  frame->SetFlag(wfShrinkToClient);

  // Assign icons for this application.
  //
  frame->SetIcon(this, IDI_SDIAPPLICATION);
  frame->SetIconSm(this, IDI_SDIAPPLICATION);

  SetMainWindow(frame);

}



//{{TSDIDecFrame Implementation}}


TSDIDecFrame::TSDIDecFrame(TWindow* parent, const char far* title, TWindow* clientWnd, bool trackMenuSelection, TModule* module)
:
  TDecoratedFrame(parent, title, !clientWnd ? new TTestDlgClient(0) : clientWnd, trackMenuSelection, module)
{
  // INSERT>> Your constructor code here.

}


TSDIDecFrame::~TSDIDecFrame()
{
  // INSERT>> Your destructor code here.

}


//--------------------------------------------------------
// TTestApp
// ~~~~~~~~~~~
// Menu Help About Test command
//
void TTestApp::CmHelpAbout()
{
}



int OwlMain(int , char* [])
{
  TTestApp   app;
  return app.Run();
}

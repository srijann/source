//----------------------------------------------------------------------------
//  Project Kwtest
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    Kwtest Application
//  FILE:         kwtstapp.cpp
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of TKwtestApp (TApplication).
//
//----------------------------------------------------------------------------

#include "win4me.h"
#include <stdio.h>

#include "kwtstapp.h"
#include "kwtstdlc.h"                        // Definition of client class.


//{{TKwtestApp Implementation}}


//
// Build a response table for all messages/commands handled
// by the application.
//
DEFINE_RESPONSE_TABLE1(TKwtestApp, TApplication)
//{{TKwtestAppRSP_TBL_BEGIN}}
  EV_COMMAND(CM_HELPABOUT, CmHelpAbout),
//{{TKwtestAppRSP_TBL_END}}
END_RESPONSE_TABLE;


//--------------------------------------------------------
// TKwtestApp
//
TKwtestApp::TKwtestApp() : TApplication("POS Test")
{

  // INSERT>> Your constructor code here.
}


TKwtestApp::~TKwtestApp()
{
  // INSERT>> Your destructor code here.
}


//--------------------------------------------------------
// TKwtestApp
// ~~~~~
// Application intialization.
//
void TKwtestApp::InitMainWindow()
{
  if (nCmdShow != SW_HIDE)
    nCmdShow = (nCmdShow != SW_SHOWMINNOACTIVE) ? SW_SHOW : nCmdShow;

  TSDIDecFrame* frame = new TSDIDecFrame(0, GetName(), 0, false);
  frame->SetFlag(wfShrinkToClient);

    // Override the default window style for the main window.
  //
  frame->Attr.Style |= WS_POPUPWINDOW;
  //  WS_BORDER | WS_CAPTION | WS_CLIPCHILDREN | WS_MINIMIZEBOX | WS_SYSMENU | WS_THICKFRAME | WS_VISIBLE;
  frame->Attr.Style &= ~(WS_CHILD);

  // Assign icons for this application.
  //
  frame->SetIcon(this, IDI_SDIAPPLICATION);
  frame->SetIconSm(this, IDI_SDIAPPLICATION);

  SetMainWindow(frame);

}

                                                

//{{TSDIDecFrame Implementation}}


TSDIDecFrame::TSDIDecFrame(TWindow* parent, const char far* title, TWindow* clientWnd, bool trackMenuSelection, TModule* module)
:
  TDecoratedFrame(parent, title, !clientWnd ? new TKwtestDlgClient(0) : clientWnd, trackMenuSelection, module)
{
  // INSERT>> Your constructor code here.

}


TSDIDecFrame::~TSDIDecFrame()
{
  // INSERT>> Your destructor code here.

}


//--------------------------------------------------------
// TKwtestApp
// ~~~~~~~~~~~
// Menu Help About Kwtest command
//
void TKwtestApp::CmHelpAbout()
{
}



int OwlMain(int , char* [])
{
 IPCSyncIOInit("POSTEST");
 TKwtestApp   app;
 return app.Run();
}

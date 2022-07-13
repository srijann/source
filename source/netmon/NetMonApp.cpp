//----------------------------------------------------------------------------
//  Project NetMon
//  Secure Payment Systems
//  Copyright © 1997. All Rights Reserved.
//
//  SUBSYSTEM:    NetMon Application
//  FILE:         netmonapp.cpp
//  AUTHOR:       Jack H. Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of TNetMonApp (TApplication).
//
//----------------------------------------------------------------------------

#include "win4me.h"

#include <stdio.h>

#include "netmonapp.h"
#include "netmondlgclient.h"                        // Definition of client class.
#include "netmonaboutdlg.h"                        // Definition of about dialog.


//{{TNetMonApp Implementation}}


//
// Build a response table for all messages/commands handled
// by the application.
//
DEFINE_RESPONSE_TABLE1(TNetMonApp, TApplication)
//{{TNetMonAppRSP_TBL_BEGIN}}
  EV_COMMAND(CM_HELPABOUT, CmHelpAbout),
//{{TNetMonAppRSP_TBL_END}}
END_RESPONSE_TABLE;


//--------------------------------------------------------
// TNetMonApp
//
TNetMonApp::TNetMonApp() : TApplication("NetMon")
{

  // INSERT>> Your constructor code here.
}


TNetMonApp::~TNetMonApp()
{
  // INSERT>> Your destructor code here.
}


//--------------------------------------------------------
// TNetMonApp
// ~~~~~
// Application intialization.
//
void TNetMonApp::InitMainWindow()
{
  if (nCmdShow != SW_HIDE)
    nCmdShow = (nCmdShow != SW_SHOWMINNOACTIVE) ? SW_SHOWMINIMIZED : nCmdShow;

  TSDIDecFrame* frame = new TSDIDecFrame(0, GetName(), 0, false);
  frame->SetFlag(wfShrinkToClient);

  // Assign icons for this application.
  //
  frame->SetIcon(this, IDI_SDIAPPLICATION);
  frame->SetIconSm(this, IDI_SDIAPPLICATION);

  // Menu associated with window and accelerator table associated with table.
  //
  frame->AssignMenu(IDM_SDI);

  // Associate with the accelerator table.
  //
  frame->Attr.AccelTable = IDM_SDI;

  SetMainWindow(frame);

  frame->SetMenuDescr(TMenuDescr(IDM_SDI));
}



//{{TSDIDecFrame Implementation}}


TSDIDecFrame::TSDIDecFrame(TWindow* parent, const char far* title, TWindow* clientWnd, bool trackMenuSelection, TModule* module)
:
  TDecoratedFrame(parent, title, !clientWnd ? new TNetMonDlgClient(0) : clientWnd, trackMenuSelection, module)
{
  // INSERT>> Your constructor code here.

}


TSDIDecFrame::~TSDIDecFrame()
{
  // INSERT>> Your destructor code here.

}


//--------------------------------------------------------
// TNetMonApp
// ~~~~~~~~~~~
// Menu Help About NetMon command
//
void TNetMonApp::CmHelpAbout()
{
  // Show the modal dialog.
  //
  TNetMonAboutDlg(MainWindow).Execute();
}



int OwlMain(int , char* [])
{
  TNetMonApp   app;
  return app.Run();
}

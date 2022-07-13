//----------------------------------------------------------------------------
//  Project PosFileMan
//  Secure Payment Systems
//  Copyright © 1997. All Rights Reserved.
//
//  SUBSYSTEM:    PosFileMan Application
//  FILE:         posfilemanapp.cpp
//  AUTHOR:       Jack Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of TPosFileManApp (TApplication).
//
//----------------------------------------------------------------------------

#include "win4me.h"

#include <owl/buttonga.h>
#include <owl/statusba.h>
#include <stdio.h>

#include "FileEditDialog.h"
#include "MainDialog.h"
#include "BankPhoneDialog.h"
#include "posfilemandlgclient.h"                        // Definition of client class.
#include "posfilemanapp.h"
#include "posfilemanaboutdlg.h"                        // Definition of about dialog.

//{{TPosFileManApp Implementation}}


//
// Build a response table for all messages/commands handled
// by the application.
//
DEFINE_RESPONSE_TABLE1(TPosFileManApp, TApplication)
//{{TPosFileManAppRSP_TBL_BEGIN}}
  EV_COMMAND(CM_HELPABOUT, CmHelpAbout),
  EV_COMMAND_ENABLE(CM_VIEWBANKPHONE, CeViewbankphone),
  EV_COMMAND(CM_VIEWBANKPHONE, CmViewbankphone),
  EV_COMMAND(CM_VIEWPOSITIVE, CmViewpositive),
  EV_COMMAND_ENABLE(CM_VIEWPOSITIVE, CeViewpositive),
  EV_COMMAND_ENABLE(CM_GETRECORD, CeGetrecord),
  EV_COMMAND_ENABLE(CM_EDITCLEARALL, CeEditclearall),
  EV_COMMAND_ENABLE(CM_FILEADD, CeFileadd),
  EV_COMMAND_ENABLE(CM_FILECHANGE, CeFilechange),
  EV_COMMAND_ENABLE(CM_FILEDELETE, CeFiledelete),
  EV_COMMAND(CM_EDITCLEARALL, CmEditclearall),
  EV_COMMAND(CM_FILEADD, CmFileadd),
  EV_COMMAND(CM_FILECHANGE, CmFilechange),
  EV_COMMAND(CM_FILEDELETE, CmFiledelete),
  EV_COMMAND(CM_GETRECORD, CmGetrecord),
//{{TPosFileManAppRSP_TBL_END}}
END_RESPONSE_TABLE;


//--------------------------------------------------------
// TPosFileManApp
//
TPosFileManApp::TPosFileManApp() : TApplication("SPS File")
{

  // INSERT>> Your constructor code here.
}


TPosFileManApp::~TPosFileManApp()
{
  // INSERT>> Your destructor code here.
}


void TPosFileManApp::SetupSpeedBar(TDecoratedFrame* frame)
{
  ApxHarbor = new THarbor(*frame);

  // Create default toolbar New and associate toolbar buttons with commands.
  //
  TDockableControlBar* cb = new TDockableControlBar(frame);
  cb->Insert(*new TButtonGadget(IDB_GETRECORD, CM_GETRECORD));
  cb->Insert(*new TButtonGadget(IDB_FILECHANGE, CM_FILECHANGE));
  cb->Insert(*new TButtonGadget(IDB_FILEADD, CM_FILEADD));
  cb->Insert(*new TButtonGadget(IDB_FILEDELETE, CM_FILEDELETE));
  cb->Insert(*new TButtonGadget(IDB_EDITCLEAR, CM_EDITCLEARALL));

  // Add caption and fly-over help hints.
  //
  cb->SetCaption("Toolbar");
  cb->SetHintMode(TGadgetWindow::EnterHints);

  ApxHarbor->Insert(*cb, alTop);
}


//--------------------------------------------------------
// TPosFileManApp
// ~~~~~
// Application intialization.
//
void TPosFileManApp::InitMainWindow()
{
  if (nCmdShow != SW_HIDE)
    nCmdShow = (nCmdShow != SW_SHOWMINNOACTIVE) ? SW_SHOWNORMAL : nCmdShow;

  TSDIDecFrame* frame = new TSDIDecFrame(0, GetName(), 0, true);
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

  TStatusBar* sb = new TStatusBar(frame, TGadget::Recessed,
                                  TStatusBar::CapsLock        |
                                  TStatusBar::NumLock         |
                                  TStatusBar::ScrollLock);
  frame->Insert(*sb, TDecoratedFrame::Bottom);

  SetupSpeedBar(frame);

  SetMainWindow(frame);

  frame->SetMenuDescr(TMenuDescr(IDM_SDI));

  currentDialog=0;
  posFileDialog=0;
  bankPhoneDialog=0;
}


//{{TSDIDecFrame Implementation}}


TSDIDecFrame::TSDIDecFrame(TWindow* parent, const char far* title, TWindow* clientWnd, bool trackMenuSelection, TModule* module)
:
  TDecoratedFrame(parent, title, !clientWnd ? new MainDialog(0) : clientWnd, trackMenuSelection, module)
{
  // INSERT>> Your constructor code here.

}


TSDIDecFrame::~TSDIDecFrame()
{
  // INSERT>> Your destructor code here.

}


void TSDIDecFrame::SetupWindow()
{
  TDecoratedFrame::SetupWindow();
  TRect  r;
  GetWindowRect(r);

  SetWindowPos(0, 0, 0, r.right-r.left + 2, r.bottom-r.top + 60, 0 );

}


//--------------------------------------------------------
// TPosFileManApp
// ~~~~~~~~~~~
// Menu Help About PosFileMan command
//
void TPosFileManApp::CmHelpAbout()
{
  // Show the modal dialog.
  //
  TPosFileManAboutDlg(MainWindow).Execute();
}

int OwlMain(int , char* [])
{
 IPCSyncIOInit("SPSFILE");
 TPosFileManApp   app;
 return app.Run();
}




void TPosFileManApp::CeViewbankphone(TCommandEnabler &tce)
{
 tce.Enable( currentDialog == 0 ||
             currentDialog != bankPhoneDialog );
}


void TPosFileManApp::CmViewbankphone()
{
 if ( currentDialog )
   currentDialog->ShowWindow(SW_HIDE);
 if ( ! bankPhoneDialog )
  {
   bankPhoneDialog = new BankPhoneDialog(GetMainWindow());
   bankPhoneDialog->Create();
  }
 currentDialog = bankPhoneDialog;
 currentDialog->ShowWindow(SW_NORMAL);
 currentDialog->BringWindowToTop();
}


void TPosFileManApp::CmViewpositive()
{
 if ( currentDialog )
   currentDialog->ShowWindow(SW_HIDE);
 if ( ! posFileDialog )
  {
   posFileDialog = new TPosFileManDlgClient(GetMainWindow());
   posFileDialog->Create();
  }
 currentDialog = posFileDialog;
 currentDialog->ShowWindow(SW_NORMAL);
 currentDialog->BringWindowToTop();
}


void TPosFileManApp::CeViewpositive(TCommandEnabler &tce)
{
 tce.Enable( currentDialog == 0 ||
             currentDialog != posFileDialog );
}


void TPosFileManApp::CeGetrecord(TCommandEnabler &tce)
{
 if ( ! currentDialog )
   tce.Enable(false);
 else
  currentDialog->CeGetrecord(tce);
}


void TPosFileManApp::CeEditclearall(TCommandEnabler &tce)
{
 if ( ! currentDialog )
   tce.Enable(false);
 else
 currentDialog->CeEditclearall(tce);
}


void TPosFileManApp::CeFileadd(TCommandEnabler &tce)
{
 if ( ! currentDialog )
   tce.Enable(false);
 else
 currentDialog->CeFileadd(tce);
}

void TPosFileManApp::CeFilechange(TCommandEnabler &tce)
{
 if ( ! currentDialog )
   tce.Enable(false);
 else
 currentDialog->CeFilechange(tce);
}


void TPosFileManApp::CeFiledelete(TCommandEnabler &tce)
{
 if ( ! currentDialog )
   tce.Enable(false);
 else
 currentDialog->CeFiledelete(tce);
}


void TPosFileManApp::CmEditclearall()
{
 if ( currentDialog )
  currentDialog->CmEditclearall();
}


void TPosFileManApp::CmFileadd()
{
 if ( currentDialog )
  currentDialog->CmFileadd();
}


void TPosFileManApp::CmFilechange()
{
 if ( currentDialog )
  currentDialog->CmFilechange();
}


void TPosFileManApp::CmFiledelete()
{
 if ( currentDialog )
  currentDialog->CmFiledelete();
}


void TPosFileManApp::CmGetrecord()
{
 if ( currentDialog )
  currentDialog->CmGetrecord();
}


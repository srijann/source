//----------------------------------------------------------------------------
//  Project LogSearch
//  Secure Payment Systems
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    LogSearch Application
//  FILE:         logsearchapp.cpp
//  AUTHOR:       Jack Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of TLogSearchApp (TApplication).
//
//----------------------------------------------------------------------------

#include "win4me.h"

#include <owl/buttonga.h>
#include <owl/statusba.h>
#include <stdio.h>

#include "logsearchapp.h"
#include "logsearchdlgclient.h"                        // Definition of client class.
#include "logsearchaboutdlg.h"                        // Definition of about dialog.
#include "SearchKeyDialog.h"
#include "PickListDialog.h"
#include "AppNotify.h"

static UINT AppNotifyMsgNum;
#define MAX_MSGSTACK 100
static IPCMessage MsgStack[MAX_MSGSTACK];

//{{TLogSearchApp Implementation}}


//
// Build a response table for all messages/commands handled
// by the application.
//
DEFINE_RESPONSE_TABLE1(TLogSearchApp, TApplication)
//{{TLogSearchAppRSP_TBL_BEGIN}}
  EV_COMMAND(CM_HELPABOUT, CmHelpAbout),
  EV_COMMAND_ENABLE(CM_EDITFIND, CeEditfind),
  EV_COMMAND_ENABLE(CM_EDITFINDNEXT, CeEditfindnext),
  EV_COMMAND_ENABLE(CM_EDITFINDPREV, CeEditfindprev),
  EV_COMMAND_ENABLE(CM_FILEPRINT, CeFileprint),
  EV_COMMAND(CM_EDITFIND, CmEditfind),
  EV_COMMAND_ENABLE(CM_PICKLIST, CePicklist),
  EV_COMMAND(CM_PICKLIST, CmPicklist),
  EV_REGISTERED("LOGSEARCH", ProcessAppNotify),
  EV_COMMAND(CM_EDITFINDNEXT, CmEditfindnext),
  EV_COMMAND(CM_EDITFINDPREV, CmEditfindprev),
  EV_COMMAND(CM_PRINTOPTIONS, CmPrintoptions),
//{{TLogSearchAppRSP_TBL_END}}
END_RESPONSE_TABLE;


//--------------------------------------------------------
// TLogSearchApp
//
TLogSearchApp::TLogSearchApp() : TApplication("LogSearch"),
 MsgCount(0),
 PickList(0)
{
 memset(&searchInfo,0,sizeof(searchInfo));
}


TLogSearchApp::~TLogSearchApp()
{
  // INSERT>> Your destructor code here.
}


void TLogSearchApp::SetupSpeedBar(TDecoratedFrame* frame)
{
  ApxHarbor = new THarbor(*frame);

  // Create default toolbar New and associate toolbar buttons with commands.
  //
  TDockableControlBar* cb = new TDockableControlBar(frame);
  cb->Insert(*new TButtonGadget(CM_EDITFIND, CM_EDITFIND));
  cb->Insert(*new TButtonGadget(CM_EDITFINDNEXT, CM_EDITFINDNEXT));
  cb->Insert(*new TButtonGadget(CM_EDITFINDPREV, CM_EDITFINDPREV));
  cb->Insert(*new TButtonGadget(CM_PICKLIST,CM_PICKLIST));
  cb->Insert(*new TButtonGadget(CM_FILEPRINT,CM_PRINTCURRENT));

  // Add caption and fly-over help hints.
  //
  cb->SetCaption("Toolbar");
  cb->SetHintMode(TGadgetWindow::EnterHints);

  ApxHarbor->Insert(*cb, alTop);
}


//--------------------------------------------------------
// TLogSearchApp
// ~~~~~
// Application intialization.
//
void TLogSearchApp::InitMainWindow()
{
  if (nCmdShow != SW_HIDE)
    nCmdShow = (nCmdShow != SW_SHOWMINNOACTIVE) ? SW_SHOWMAXIMIZED : nCmdShow;

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
}



//{{TSDIDecFrame Implementation}}


TSDIDecFrame::TSDIDecFrame(TWindow* parent, const char far* title, TWindow* clientWnd, bool trackMenuSelection, TModule* module)
:
  TDecoratedFrame(parent, title, !clientWnd ? new TLogSearchDlgClient(0) : clientWnd, trackMenuSelection, module)
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

  r.bottom += 60;
  SetWindowPos(0, r, SWP_NOZORDER | SWP_NOMOVE);

  // INSERT>> Your code here.
}


//--------------------------------------------------------
// TLogSearchApp
// ~~~~~~~~~~~
// Menu Help About LogSearch command
//
void TLogSearchApp::CmHelpAbout()
{
  // Show the modal dialog.
  //
  TLogSearchAboutDlg(MainWindow).Execute();
}



int OwlMain(int , char* [])
{
 IPCSyncIOInit("LOGSEARCH");
 AppNotifyMsgNum = SetAppNotifyMsg("LOGSEARCH");

 TLogSearchApp   app;
 return app.Run();
}

void TLogSearchApp::CeEditfind(TCommandEnabler &tce)
{
 tce.Enable(true);
}


void TLogSearchApp::CeEditfindnext(TCommandEnabler &tce)
{
 tce.Enable( MsgCount > 0 && CurrentMsgIndex < MsgCount-1 );
}


void TLogSearchApp::CeEditfindprev(TCommandEnabler &tce)
{
 tce.Enable( MsgCount > 0 && CurrentMsgIndex != 0 );
}


void TLogSearchApp::CeFileprint(TCommandEnabler &tce)
{
 tce.Enable(false);
}


void TLogSearchApp::CePicklist(TCommandEnabler &tce)
{
 tce.Enable(PickList && MsgCount);
}

void TLogSearchApp::CmEditfind()
{
 if ( SearchKeyDialog(GetMainWindow(),searchInfo).Execute() != IDOK )
  return;

 // Create the Pick List and Display Dialog Windows the first time
 // a search is executed.
 if ( ! PickList )
  {
   PickList = new PickListDialog(GetMainWindow());
   DisplayDialog = new MainDisplayDialog(GetMainWindow());
   PickList->Create();
   DisplayDialog->Create();
  }

 DisplayDialog->Show(SW_HIDE);
 PickList->Show(SW_NORMAL);
 PickList->Build(&searchInfo,MsgStack,&MsgCount,MAX_MSGSTACK);
}

void TLogSearchApp::CmPicklist()
{
 PickList->Show(SW_NORMAL);
}

//----------------------------------------------------------------
// App Notify Message Handler. All Dialogs are Popups with system
// menus. All system commands are referred here for processing so
// we can maintain the Dialog List and set the Z-order as windows
// are closed/resized, etc.
//
// See FormDialog for more details
//----------------------------------------------------------------
LRESULT TLogSearchApp::ProcessAppNotify(WPARAM MsgType, LPARAM Id)
{
 switch( MsgType )
  {
   case IDOK: PickList->ShowWindow(SW_HIDE);
              if ( MsgCount > 0 )
               {
                DisplayDialog->Display(&MsgStack[CurrentMsgIndex=Id]);
                DisplayDialog->Show(SW_NORMAL);
               }
              else
               DisplayDialog->Show(SW_HIDE);
              break;
   case IDCANCEL:
              PickList->ShowWindow(SW_HIDE);
              break;

  }

 return 0;
}

void TLogSearchApp::CmEditfindnext()
{
 if ( MsgCount==0 || CurrentMsgIndex >= MsgCount-1 )
  return;

 DisplayDialog->Display(&MsgStack[++CurrentMsgIndex]);
 PickList->SetSelection(CurrentMsgIndex);
}


void TLogSearchApp::CmEditfindprev()
{
 if ( MsgCount==0 || CurrentMsgIndex == 0 )
  return;

 DisplayDialog->Display(&MsgStack[--CurrentMsgIndex]);
 PickList->SetSelection(CurrentMsgIndex);
}


void TLogSearchApp::CmPrintoptions()
{
  // INSERT>> Your code here.

}


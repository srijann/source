//----------------------------------------------------------------------------
//  Project KDSTest
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    KDSTest Application
//  FILE:         kdstestapp.cpp
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of TKDSTestApp (TApplication).
//
//----------------------------------------------------------------------------

#include "win4me.h"

#include <stdio.h>

#include "kdstestapp.h"
#include "kdstestdlgclient.h"                       // Definition of client class.
#include "kdstestaboutdlg.h"                        // Definition of about dialog.
#include "TranType.h"
#include "CheckAuthFn.h"

//{{TKDSTestApp Implementation}}


//
// Build a response table for all messages/commands handled
// by the application.
//
DEFINE_RESPONSE_TABLE1(TKDSTestApp, TApplication)
//{{TKDSTestAppRSP_TBL_BEGIN}}
  EV_COMMAND(CM_HELPABOUT, CmHelpAbout),
//{{TKDSTestAppRSP_TBL_END}}
END_RESPONSE_TABLE;


//--------------------------------------------------------
// TKDSTestApp
//
TKDSTestApp::TKDSTestApp() : TApplication("KDSTest")
{

  // INSERT>> Your constructor code here.
}


TKDSTestApp::~TKDSTestApp()
{
  // INSERT>> Your destructor code here.
}


//--------------------------------------------------------
// TKDSTestApp
// ~~~~~
// Application intialization.
//
void TKDSTestApp::InitMainWindow()
{
  if (nCmdShow != SW_HIDE)
    nCmdShow = (nCmdShow != SW_SHOWMINNOACTIVE) ? SW_SHOWMAXIMIZED : nCmdShow;

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



//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(TSDIDecFrame, TDecoratedFrame)
//{{TSDIDecFrameRSP_TBL_BEGIN}}
  EV_COMMAND_ENABLE(CM_VIEWLOG, CeViewlog),
  EV_REGISTERED(IPCNotifyMessage, ProcessMessage ),
  EV_COMMAND_ENABLE(CM_VIEWRESPONSE, CeViewresponse),
  EV_COMMAND(CM_VIEWLOG, CmViewlog),
  EV_COMMAND(CM_VIEWRESPONSE, CmViewresponse),
//{{TSDIDecFrameRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{TSDIDecFrame Implementation}}


TSDIDecFrame::TSDIDecFrame(TWindow* parent, const char far* title, TWindow* clientWnd, bool trackMenuSelection, TModule* module)
:
  TDecoratedFrame(parent, title, !clientWnd ? new TKDSTestDlgClient(0) : clientWnd, trackMenuSelection, module)
{
  // INSERT>> Your constructor code here.

}


TSDIDecFrame::~TSDIDecFrame()
{
  // INSERT>> Your destructor code here.

}


//--------------------------------------------------------
// TKDSTestApp
// ~~~~~~~~~~~
// Menu Help About KDSTest command
//
void TKDSTestApp::CmHelpAbout()
{
  // Show the modal dialog.
  //
  TKDSTestAboutDlg(MainWindow).Execute();
}



int OwlMain(int , char* [])
{
  IPCInit("KDSTest");
  TKDSTestApp   app;
  return app.Run();
}

void TSDIDecFrame::SetupWindow()
{
 TDecoratedFrame::SetupWindow();
 IPCRegister("KDSTest",GetHandle());
 responseDialog=new ResponseDialog(this);
 logDialog=new TranLogDialog(this);
 responseDialog->Create();
 logDialog->Create();
 logDialog->Show(SW_SHOW);
}

void TSDIDecFrame::CeViewlog(TCommandEnabler &tce)
{
 tce.Enable( responseDialog->IsWindowVisible() );
}

LRESULT TSDIDecFrame::ProcessMessage(WPARAM, LPARAM Id)
{
 char TranType[10];

 if ( ! Msg.RecvMsg(Id) ||
      ! Msg.GetFld(FN_TRANTYPE,sizeof(TranType),TranType) )
   return 0;

 if ( strcmp(TranType,T_VOICEREQ) == 0 )
  {
   KENWOOD_TERMINFO TermInfo;

   TermInfo.IPCKey =0;
   TermInfo.SyncId=1;
   sprintf(TermInfo.TTYName,"VC%d",1);
   Msg.PutFld(FN_TERMINFO,&TermInfo,sizeof(TermInfo));

   responseDialog->ProcessMessage(Msg);
   Msg.SendMsg("REPLY");
   logDialog->ProcessInqMessage(Msg);
  }
 else
 if ( strcmp(TranType,T_AUTHREPLY) == 0 )
  {
   if ( ! Msg.GetFld(FN_TERMINFO) )
    {
     ::MessageBox(0,"NO TERMINFO","ERROR",MB_OK|MB_ICONERROR);
     return 0;
    }
   logDialog->ProcessReplyMessage(Msg);
   Msg.SendMsg("REPLY");
  }

 return 1;
}

void TSDIDecFrame::CeViewresponse(TCommandEnabler &tce)
{
 tce.Enable( logDialog->IsWindowVisible() );
}


void TSDIDecFrame::CmViewlog()
{
 responseDialog->Show(SW_HIDE);
 logDialog->Show(SW_SHOWNA);
}


void TSDIDecFrame::CmViewresponse()
{
 logDialog->Show(SW_HIDE);
 responseDialog->Show(SW_NORMAL);
}


//----------------------------------------------------------------------------
//  Project SpsManager
//
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    SpsManager Application
//  FILE:         spsmanagerapp.cpp
//  AUTHOR:
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of TSpsManagerApp (TApplication).
//
//----------------------------------------------------------------------------

#include "win4me.h"
#pragma hdrstop
#include <classlib/dlistimp.h>
#include <owl/opensave.h>
#include <owl/statusba.h>
#include <owl/inputdia.h>
#include "AppNotify.h"
#include "SaveFile.h"
#include "spsmanagerapp.h"
#include "spsmanagerdlgclient.h"                        // Definition of client class.
#include "spsmanageraboutdlg.h"                        // Definition of about dialog.

#include "MerchantDialog.h"
#include "DataReqDialog.h"
#include "UnvFundsDialog.h"
#include "AuthRulesDialog.h"
#include "PosFileRulesDialog.h"
#include "ResponseMsgDialog.h"
#include "ProductClassDialog.h"
#include "ProductClassList.h"
#include "ExitDialog.h"
#include "OpenTreeDialog.h"
#include "OpenClassDialog.h"
#include "CheckAuthfn.h"
#include "DbmanMsg.h"
#include "OpenMerchDialog.h"
#include "MerchNOFDialog.h"
#include "TranType.h"

// This is our dialog list
static TIDoubleListImp<FormDialog> Dialogs;

void SetTop(FormDialog* Win=0);
static UINT AppNotifyMsgNum;
static bool SaveAllEnabled;
static int CheckModify(const FormDialog& Win,void*);
static void ResetModified(FormDialog& Win,void*);

//
// Generated help file.
//
const char HelpFileName[] = "SpsMan.hlp";


//{{TSpsManagerApp Implementation}}

//Starting ID number for Window choice menu
#define CM_WINMENU 10000
// Maximum number of window choices to display
#define MAX_WINMENU 20

//
// Build a response table for all messages/commands handled
// by the application.
//
DEFINE_RESPONSE_TABLE1(TSpsManagerApp, TApplication)
//{{TSpsManagerAppRSP_TBL_BEGIN}}
  EV_COMMAND(CM_HELPCONTENTS, CmHelpContents),
  EV_COMMAND(CM_HELPUSING, CmHelpUsing),
  EV_COMMAND(CM_HELPABOUT, CmHelpAbout),
  EV_COMMAND_ENABLE(CM_FILESAVE, CeFilesave),
  EV_REGISTERED("SPSMAN", ProcessAppNotify),
  EV_COMMAND_ENABLE(CM_SAVEALL, CeSaveall),
  EV_COMMAND_ENABLE(CM_SAVETOWORK, CeSavetoWork),
  EV_COMMAND(CM_RELOADCHANGES, CmReloadchanges),
  EV_COMMAND(CM_OPENAUTHRULES, CmOpenauthrules),
  EV_COMMAND(CM_OPENDATAREQ, CmOpendatareq),
  EV_COMMAND(CM_OPENPOSFILERULES, CmOpenposfilerules),
  EV_COMMAND(CM_OPENUNVFUNDSRULES, CmOpenUnvFundsrules),
  EV_COMMAND_ENABLE(CM_CLOSEALLWINDOWS, CeCloseallwindows),
  EV_COMMAND_ENABLE(CM_WINMENU+0, CeChooseWindow),
  EV_COMMAND_ENABLE(CM_WINMENU+1, CeChooseWindow),
  EV_COMMAND_ENABLE(CM_WINMENU+2, CeChooseWindow),
  EV_COMMAND_ENABLE(CM_WINMENU+3, CeChooseWindow),
  EV_COMMAND_ENABLE(CM_WINMENU+4, CeChooseWindow),
  EV_COMMAND_ENABLE(CM_WINMENU+5, CeChooseWindow),
  EV_COMMAND_ENABLE(CM_WINMENU+6, CeChooseWindow),
  EV_COMMAND_ENABLE(CM_WINMENU+7, CeChooseWindow),
  EV_COMMAND_ENABLE(CM_WINMENU+8, CeChooseWindow),
  EV_COMMAND_ENABLE(CM_WINMENU+9, CeChooseWindow),
  EV_COMMAND_ENABLE(CM_WINMENU+10, CeChooseWindow),
  EV_COMMAND_ENABLE(CM_WINMENU+11, CeChooseWindow),
  EV_COMMAND_ENABLE(CM_WINMENU+12, CeChooseWindow),
  EV_COMMAND_ENABLE(CM_WINMENU+13, CeChooseWindow),
  EV_COMMAND_ENABLE(CM_WINMENU+14, CeChooseWindow),
  EV_COMMAND_ENABLE(CM_WINMENU+15, CeChooseWindow),
  EV_COMMAND_ENABLE(CM_WINMENU+16, CeChooseWindow),
  EV_COMMAND_ENABLE(CM_WINMENU+17, CeChooseWindow),
  EV_COMMAND_ENABLE(CM_WINMENU+18, CeChooseWindow),
  EV_COMMAND_ENABLE(CM_WINMENU+19, CeChooseWindow),
  EV_COMMAND_ENABLE(CM_MINIMIZEALL, CeMinimizeall),
  EV_COMMAND(CM_OPENMERCHANT, CmOpenmerchant),
  EV_COMMAND(CM_OPENRESPMSG, CmOpenrespmsg),
  EV_COMMAND(CM_OPENPRODUCTCLASSES, CmOpenproductclasses),
//{{TSpsManagerAppRSP_TBL_END}}
END_RESPONSE_TABLE;


//--------------------------------------------------------
// TSpsManagerApp
//
TSpsManagerApp::TSpsManagerApp() : TApplication("SPS Manager")
{
  HelpState = false;
  ContextHelp = false;
  HelpCursor = 0;

  // INSERT>> Your constructor code here.
}


TSpsManagerApp::~TSpsManagerApp()
{
}

//-------------------------------------------------------------
// Handle the Can Close of the main application. See if we have
// any modifications to deal with.
//-------------------------------------------------------------
bool TSpsManagerApp::CanClose()
{
 bool result;
 int  Select;

 if ( Dialogs.FirstThat(CheckModify,NULL) )
  {
   ExitDialog(GetMainWindow(),&Select).Execute();
   switch( Select )
    {
     case 0: return false;
     case 1: CmFilesaveall(); break;
     case 2: CmFilesavetoWork(); break;
     case 3: break;
     case 4: Dialogs.ForEach(ResetModified,NULL); break;
    }
  }

 result = TApplication::CanClose();

  // Close the help engine if we used it.
  //
  if (result && HelpState)
    GetMainWindow()->WinHelp(HelpFileName, HELP_QUIT, 0);

 return result;
}


//--------------------------------------------------------
// TSpsManagerApp
// ~~~~~
// Application intialization.
//
void TSpsManagerApp::InitMainWindow()
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

  SetMainWindow(frame);

  frame->SetMenuDescr(TMenuDescr(IDM_SDI));
}



//{{TSDIDecFrame Implementation}}


TSDIDecFrame::TSDIDecFrame(TWindow* parent, const char far* title, TWindow* clientWnd, bool trackMenuSelection, TModule* module)
:
  TDecoratedFrame(parent, title, !clientWnd ? new TSpsManagerDlgClient(0) : clientWnd, trackMenuSelection, module)
{
  // INSERT>> Your constructor code here.

}


TSDIDecFrame::~TSDIDecFrame()
{
 // We get a GPF if the Dialogs Object tries to delete it's objects so
 // we flush it here to prevent this.
 Dialogs.Flush(0);
}


void TSDIDecFrame::SetupWindow()
{
  TDecoratedFrame::SetupWindow();
  TRect  r;
  GetWindowRect(r);

  SetWindowPos(0, r, SWP_NOZORDER | SWP_NOMOVE);

  // INSERT>> Your code here.

}


//--------------------------------------------------------
// TSpsManagerApp
// ~~~~~
// Menu Help Contents command
//
void TSpsManagerApp::CmHelpContents()
{
  // Show the help table of contents.
  //
  HelpState = GetMainWindow()->WinHelp(HelpFileName, HELP_CONTENTS, 0);
}


//--------------------------------------------------------
// TSpsManagerApp
// ~~~~~
// Menu Help Using Help command
//
void TSpsManagerApp::CmHelpUsing()
{
  // Display the contents of the Windows help file.
  //
  HelpState = GetMainWindow()->WinHelp(HelpFileName, HELP_HELPONHELP, 0);
}


//--------------------------------------------------------
// TSpsManagerApp
// ~~~~~~~~~~~
// Menu Help About SpsManager command
//
void TSpsManagerApp::CmHelpAbout()
{
  // Show the modal dialog.
  //
  TSpsManagerAboutDlg(MainWindow).Execute();
}



//
// Process application messages to provide context sensitive help
//
bool TSpsManagerApp::ProcessAppMsg(MSG& msg)
{
  if (msg.message == WM_COMMAND) {
    if (ContextHelp || ::GetKeyState(VK_F1) < 0) {
      ContextHelp = false;
      GetMainWindow()->WinHelp(HelpFileName, HELP_CONTEXT, msg.wParam);
      return true;
    }
   switch( msg.wParam )
    {
     case CM_FILESAVE:   CmFilesave(); return true;
     case CM_SAVEALL:    CmFilesaveall(); return true;
     case CM_SAVETOWORK:  CmFilesavetoWork(); return true;
     case CM_CLOSEALLWINDOWS: CmCloseAllWindows(); return true;
     case CM_MINIMIZEALL: CmMinimizeAllWindows(); return true;
     default: if ( msg.wParam >= CM_WINMENU &&
                   msg.wParam < CM_WINMENU+MAX_WINMENU )
               {
                CmSelectWindow(msg.wParam);
                return true;
               }
              break;
    }
  }
  else
    switch(msg.message) {
    case WM_KEYDOWN:
      if (msg.wParam == VK_F1) {
        // If the Shift/F1 then set the help cursor and turn on the modal help state.
        //
        if (::GetKeyState(VK_SHIFT) < 0) {
          ContextHelp = true;
          HelpCursor = new TCursor(GetMainWindow()->GetModule()->GetInstance(), TResId(IDC_HELPCURSOR));
          ::SetCursor(*HelpCursor);
          return true;        // Gobble up the message.
        }
        else {
          // If F1 w/o the Shift key then bring up help's main index.
          //
          GetMainWindow()->WinHelp(HelpFileName, HELP_INDEX, 0);
          return true;        // Gobble up the message.
        }
      }
      else {
        if (ContextHelp && msg.wParam == VK_ESCAPE) {
          if (HelpCursor)
            delete HelpCursor;
          HelpCursor = 0;
          ContextHelp = false;
          GetMainWindow()->SetCursor(0, IDC_ARROW);
          return true;    // Gobble up the message.
        }
      }
      break;

    case WM_MOUSEMOVE:
    case WM_NCMOUSEMOVE:
      if (ContextHelp) {
        ::SetCursor(*HelpCursor);
        return true;        // Gobble up the message.
      }
      break;

    case WM_INITMENU:
      if (ContextHelp) {
        ::SetCursor(*HelpCursor);
        return true;    // Gobble up the message.
      }
      break;
    case WM_ENTERIDLE:
      if (msg.wParam == MSGF_MENU)
        if (GetKeyState(VK_F1) < 0) {
          ContextHelp = true;
          GetMainWindow()->PostMessage(WM_KEYDOWN, VK_RETURN, 0);
          return true;     // Gobble up the message.
        }
      break;
    default:
      ;
    }  // End of message switch

  // Continue normal processing.
  //
  return TApplication::ProcessAppMsg(msg);
}

int OwlMain(int , char* [])
{
 IPCSyncIOInit("SPSMAN");
 AppNotifyMsgNum = SetAppNotifyMsg("SPSMAN");
 TSpsManagerApp   app;
 return app.Run();
}

//-------------------------------
// Handle the Save menu selection
//-------------------------------
void TSpsManagerApp::CmFilesave()
{
 FormDialog* Top=Dialogs.PeekHead();
 if ( Top && ! Top->IsIconic() && Top->IsModified() )
  {
   Waiting(true);
   Top->Save();
   Waiting(false);
  }
}

//-------------------------------------------------------------------
// Save a Dialog if modified. Returns true if the save fails, causing
// the iteration to stop.
//-------------------------------------------------------------------
static int save(const FormDialog& Win,void*)
{
 FormDialog& W=(FormDialog&)Win;
 if ( W.IsModified() &&
      ! W.Save() )
  return 1;
 else
  return 0;
}

//----------------------------------
// Handle the saveall menu selection
//----------------------------------
void TSpsManagerApp::CmFilesaveall()
{
 Waiting(true);
 Dialogs.FirstThat(save,NULL);
 Waiting(false);
}

//----------------------------------------------------
// Dialogs FirstThat iterator to save modified windows
//----------------------------------------------------
static int TempSave(const FormDialog& Win,void* FilePtr)
{
 FormDialog& W=(FormDialog&)Win;

 if ( ! W.IsModified() )
   return 0;

 ((TFile*)FilePtr)->SeekToEnd();

  if ( ! W.SaveTemp((TFile*)FilePtr) )
   return 1;

 return 0;
}

//--------------------------------------------------------------------
// Dialogs ForEach iterator to reset the Modified flags on all windows
//--------------------------------------------------------------------
static void ResetModified(FormDialog& Win,void*)
{
 Win.ResetModified();
}

//------------------------------------
// Handle the savetoWork menu selection
//------------------------------------
void TSpsManagerApp::CmFilesavetoWork()
{
 TFile* File = 0;
 short Version;

 TOpenSaveDialog::TData FileData
    (OFN_HIDEREADONLY|OFN_PATHMUSTEXIST|OFN_NOCHANGEDIR,
    "SPS Manager Work Files (*.WRK)|*.wrk|", 0, "", "WRK");

 TFileSaveDialog dlg( GetMainWindow(), FileData );

 Waiting(true);
 if( dlg.Execute() == IDOK &&
     (File=OpenSaveFile(FileData.FileName)) != 0  )
  {
   if ( Dialogs.FirstThat(TempSave,File) )
     FileError("ERROR WRITING TO FILE");
   else
    {
     Version=SAVEFILE_CURRENT_VERSION;
     File->SeekToBegin();
     if ( File->Write(&Version,sizeof(Version)) != sizeof(Version) ||
          ! File->Close() )
       FileError("ERROR WRITING TO FILE");
     else
       Dialogs.ForEach(ResetModified,NULL);
    }
   if ( File->IsOpen() )
     File->Close();
  }

 if ( File )
   delete File;
 Waiting(false);
}

//----------------------------------------------
// Bring the first window in the list to the top
//----------------------------------------------
void SetTop(FormDialog* Win)
{
 FormDialog* Top;

 if ( Win  )
  {
   if ( Win->IsIconic() )
    Win->ShowWindow(SW_NORMAL);
   Win->BringWindowToTop();
   if ( Win!=Dialogs.PeekHead() )
    {
     Dialogs.Detach(Win,0);
     Dialogs.AddAtHead(Win);
    }
  }
 else
   if ( ((Top=Dialogs.PeekHead()) != 0) && ! Top->IsIconic() )
    Top->BringWindowToTop();
}

//-----------------------------------------
// ForEach iterator callback to do minimize
//-----------------------------------------
static void Minimize(FormDialog& Dialog,void*)
{
 Dialog.ShowWindow(SW_MINIMIZE);
}

//----------------------------------------
// Handle the minimize all windows command
//-----------------------------------------
void TSpsManagerApp::CmMinimizeAllWindows()
{
 Dialogs.ForEach(Minimize,NULL);
}

//-------------------------------------
// Handle the Close All Windows command
//-------------------------------------
void TSpsManagerApp::CmCloseAllWindows()
{
 int  Select;
 FormDialog* Dialog;

 if ( Dialogs.FirstThat(CheckModify,NULL) )
  {
   ExitDialog(GetMainWindow(),&Select).Execute();
   switch( Select )
    {
     case 0: return;
     case 1: CmFilesaveall(); break;
     case 2: CmFilesavetoWork(); break;
     case 3: break;
     case 4: Dialogs.ForEach(ResetModified,NULL); break;
    }
  }

 while( (Dialog=Dialogs.PeekHead()) != 0 )
  {
   if ( ! Dialog->CanClose() )
     return;
   Dialog->Destroy();
   Dialogs.DetachAtHead(1);
  }

 Dialogs.Flush(1);
}

//----------------------------------------------------------------
// App Notify Message Handler. All Dialogs are Popups with system
// menus. All system commands are referred here for processing so
// we can maintain the Dialog List and set the Z-order as windows
// are closed/resized, etc.
//
// See FormDialog for more details
//----------------------------------------------------------------
LRESULT TSpsManagerApp::ProcessAppNotify(WPARAM MsgType, LPARAM Id)
{
 if ( Dialogs.GetItemsInContainer() == 0 )
   return 0;

 FormDialog* Win = (FormDialog*)Id;

 switch( MsgType )
  {
   case SC_CLOSE: if ( Win->IsModified() )
                    if ( ! Win->CanClose() )
                     {
                      if ( Win != Dialogs.PeekHead() )
                       {
                        Dialogs.Detach(Win,0);
                        Dialogs.AddAtHead(Win);
                       }
                      SetTop();
                      return 0;
                     }
                  Win->Destroy();
                  Dialogs.Detach(Win,1);
                  SetTop();
                  break;

   case SC_MINIMIZE:
                  Dialogs.Detach(Win,0);
                  Dialogs.AddAtTail(Win);
                  SetTop();
                  break;

   case SC_RESTORE:
   case SC_MAXIMIZE:
                  SetTop(Win);
                  break;
  }

 return 0;
}

//--------------------------------------------------------
// FirstThat Iterator to Checkmodify status of all windows
//--------------------------------------------------------
static int CheckModify(const FormDialog& Win,void*)
{
 FormDialog& W=(FormDialog&)Win;
 return W.IsModified();
}

//--------------------
// Enabler for SaveAll
//--------------------
void TSpsManagerApp::CeSaveall(TCommandEnabler &tce)
{
 if ( Dialogs.FirstThat(CheckModify,NULL) )
  {
   tce.Enable(true);
   SaveAllEnabled=true;
  }
 else
   SaveAllEnabled=false;
}

//----------------------
// Enabler for savetoWork
//----------------------
void TSpsManagerApp::CeSavetoWork(TCommandEnabler &tce)
{
 // This gets the same response as the SaveAll Enabler.. The SaveAll
 // is always called first, storing it's result so we can use it too.
 if ( SaveAllEnabled )
  tce.Enable(true);
}

//-----------------
// Enabler for Save
//-----------------
void TSpsManagerApp::CeFilesave(TCommandEnabler &tce)
{
 FormDialog* Top=Dialogs.PeekHead();
 if ( Top &&
      ! Top->IsIconic() &&
      Top->IsModified() )
   tce.Enable(true);
}


//------------------------------------------------------------------
// Attempt to create a Dialog and bring it to the top of the Z-order
//------------------------------------------------------------------
void TSpsManagerApp::DoCreateDialog(FormDialog* Win)
{
 Waiting(true);

 if ( Win->Create() )
   Dialogs.AddAtHead(Win);
 else
   delete Win;

 Waiting(false);
}

//-------------------------------------------
// Set/Reset the Wait mode (Do the HourGlass)
//-------------------------------------------
void TSpsManagerApp::Waiting(bool wait)
{
 if ( wait )
  {
   CaptureWin=GetMainWindow()->SetCapture();
   GetMainWindow()->SetCursor(0,IDC_WAIT);
  }
 else
  {
   GetMainWindow()->SetCursor(0,IDC_ARROW);
   ::SetCapture(CaptureWin);
   SetTop();
  }
}

//------------------------------
// Open Data Requirements Record
//------------------------------
void TSpsManagerApp::CmOpendatareq()
{
 OpenClassInfo Info;
 DataRequirements* Rec;
 FormDialog* Dialog;

 Waiting(true);
 if ( OpenClassDialog(GetMainWindow(),"Open Data Requirements Record",
       &Info,DATAREQ_LIST).Execute() == IDCANCEL )
  {
   Waiting(false);
   return;
  }

 Waiting(false);

 // Get existing record or a template for a new record
 DBManMessage Msg;
 if ( Msg.DBGet(T_GETDATAREQ,&Info,sizeof(Info)) != DB_OKAY )
  {
   Waiting(false);
   return;
  }
 Waiting(false);
 Rec = new DataRequirements(FN_DATAREQRECORD,&Msg);
 Dialog = new DataReqDialog(GetMainWindow(),Rec);
 if ( Info.IsNew )
  Dialog->SetModified();
 DoCreateDialog( Dialog );
}

//--------------------
// Open POS file rules
//--------------------
void TSpsManagerApp::CmOpenposfilerules()
{
 OpenClassInfo Info;
 PositiveRules* Rec;
 PosFileRulesDialog* Dialog;

 Waiting(true);
 if ( OpenClassDialog(GetMainWindow(),"Open Positive File Rules Record",
       &Info,POSRULES_LIST).Execute() == IDCANCEL )
  {
   Waiting(false);
   return;
  }
 Waiting(false);

 //Get existing record or a template for a new record
 DBManMessage Msg;
 if ( Msg.DBGet(T_GETPOSFILERULES,&Info,sizeof(Info)) != DB_OKAY )
  {
   Waiting(false);
   return;
  }
 Waiting(false);
 Rec = new PositiveRules(FN_POSITIVERULESRECORD,&Msg);
 Dialog = new PosFileRulesDialog(GetMainWindow(),Rec);
 if ( Info.IsNew )
  Dialog->SetModified();
 DoCreateDialog( Dialog );
}


#include "splib.h"
//--------------------------
// Open Unveried Funds Rules
//--------------------------
void TSpsManagerApp::CmOpenUnvFundsrules()
{
 OpenClassInfo Info;
 UnvFundsRules* Rec;
 FormDialog* Dialog;

 Waiting(true);
 if ( OpenClassDialog(GetMainWindow(),"Open Unverified Funds Rules Record",
       &Info,UNVFUNDSRULES_LIST).Execute() == IDCANCEL )
  {
   Waiting(false);
   return;
  }

 Waiting(false);

 // Get existing record or a template for a new record
 DBManMessage Msg;
 if ( Msg.DBGet(T_GETUNVFUNDSRULES,&Info,sizeof(Info)) != DB_OKAY )
  {
   Waiting(false);
   return;
  }
 Waiting(false);
 Rec = new UnvFundsRules(FN_UNVFUNDSRULESRECORD,&Msg);
 Dialog = new UnvFundsDialog(GetMainWindow(),Rec);
 if ( Info.IsNew )
  Dialog->SetModified();
 DoCreateDialog( Dialog );
 Waiting(false);
}

//-----------------------
// Open Auth Rules Dialog
//-----------------------
void TSpsManagerApp::CmOpenauthrules()
{
 OpenTreeInfo Info;
 AuthRules* globalRules=0;
 AuthRules* classRules=0;
 AuthRules* subClassRules=0;

 Waiting(true);
 if (  OpenTreeDialog(GetMainWindow(),&Info).Execute() == IDCANCEL )
  {
   Waiting(false);
   return;
  }
 DBManMessage Msg;
 if ( Msg.DBGet(T_GETAUTHRULES,&Info,sizeof(Info)) != DB_OKAY )
  {
   Waiting(false);
   return;
  }
 Waiting(false);

 if ( Msg.GetFld(FN_GLOBALAUTHRULES) )
  globalRules=new AuthRules(FN_GLOBALAUTHRULES,&Msg);
 if ( Msg.GetFld(FN_CLASSAUTHRULES) )
   classRules=new AuthRules(FN_CLASSAUTHRULES,&Msg);
 if ( Msg.GetFld(FN_SUBCLASSAUTHRULES) )
   subClassRules=new AuthRules(FN_SUBCLASSAUTHRULES,&Msg);

 DoCreateDialog( new AuthRulesDialog(GetMainWindow(),
                   globalRules, classRules, subClassRules, Info.IsNew));
}

//--------------------------------------
// Command Enabler for Close All Windows
//--------------------------------------

typedef struct
 {
  HMENU hMenu;
  uint  Id;
 } APPENDMENUINFO;

static void AppendMenuTitle(FormDialog& Win,void* AppendInfo)
{
 char Title[35];
 uint Flags=MF_STRING;

 APPENDMENUINFO* Info=(APPENDMENUINFO*)AppendInfo;
 Win.GetWindowText(Title,sizeof(Title));
 if ( Win.IsModified() )
  Flags |= MF_CHECKED;
 AppendMenu(Info->hMenu,Flags,Info->Id,Title);
 ++Info->Id;
}

//------------------------------
// Enabler for Close All Windows
//------------------------------
void TSpsManagerApp::CeCloseallwindows(TCommandEnabler &tce)
{
 HMENU hMenu=GetSubMenu(GetMainWindow()->GetMenu(),1);
 int numItems=GetMenuItemCount(hMenu);
 int i;
 APPENDMENUINFO AppendInfo;

 // Delete the separator and all of the window choices
 if ( numItems > 2 )
  {
   DeleteMenu(hMenu,2,MF_BYPOSITION);
   numItems-=3;
   for (i=0; i<numItems; ++i)
     DeleteMenu(hMenu,i+CM_WINMENU,MF_BYCOMMAND);
  }

 // No Windows -- don't enable
 if ( Dialogs.IsEmpty() )
   return;

 // Set up the Window menu
 AppendMenu(hMenu,MF_SEPARATOR|MF_ENABLED,0,0);
 AppendInfo.hMenu=hMenu;
 AppendInfo.Id=CM_WINMENU;
 Dialogs.ForEach(AppendMenuTitle,&AppendInfo);
 tce.Enable(true);
}

//------------------------------
// Enabler for Window menu items
//------------------------------
void TSpsManagerApp::CeChooseWindow(TCommandEnabler &tce)
{
 tce.Enable(true);
}

typedef struct
 {
  int start;
  int stop;
 } SRCHINFO;

//-----------------------
// Find a window by index
//-----------------------
static int FindByIndex(const FormDialog&,void* SrchInfo)
{
 SRCHINFO* Info=(SRCHINFO*)SrchInfo;
 if ( Info->start==Info->stop )
  return 1;
 ++Info->start;
 return 0;
}

//------------------------------
// Select a Window from the menu
//------------------------------
void TSpsManagerApp::CmSelectWindow(int Id)
{
 FormDialog* Win;
 SRCHINFO Info;

 Info.start = 0;
 Info.stop = Id - CM_WINMENU;
 if ( (Win=Dialogs.FirstThat(FindByIndex,&Info)) != 0 )
  {
   Win->ShowWindow(SW_NORMAL);
   SetTop(Win);
  }
}

//---------------------
// Minimize all windows
//---------------------
void TSpsManagerApp::CeMinimizeall(TCommandEnabler &tce)
{
 if ( ! Dialogs.IsEmpty() )
  tce.Enable(true);
}


//-----------------------
// Open a Merchant Record
//-----------------------
void TSpsManagerApp::CmOpenmerchant()
{
 int  Ret;
 MerchantRec* stationRec=0;
 MerchantRec* accountRec=0;
 FormDialog* Dialog;
 bool isNew=false;
 char Key[sizeof(stationRec->Data.MerchantName)];

 if ( OpenMerchDialog(GetMainWindow(),Key).Execute() == IDCANCEL )
   return;

 Waiting(true);
 DBManMessage Msg;
 Ret = Msg.DBGet(T_GETMERCHANTREC,Key,strlen(Key));
 Waiting(false);
 switch( Ret )
  {
   case DB_OKAY:
     if ( Msg.GetFld(FN_ACCOUNTRECORD) )
      accountRec=new MerchantRec(FN_ACCOUNTRECORD,&Msg);
     if ( Msg.GetFld(FN_MERCHANTRECORD) )
      stationRec=new MerchantRec(FN_MERCHANTRECORD,&Msg);
     break;

   case DB_NOTFOUND:
     if ( strlen(Key) > 5 )
      {
       char Buf[80];
       sprintf(Buf,"MERCHANT NAME '%s' DOES NOT EXIST",Key);
       GetMainWindow()->MessageBox(Buf);
       return;
      }
     Msg.GetFld(FN_DBKEY,sizeof(Key),Key);
     if ( MerchNofDialog(GetMainWindow(),Key).Execute() == IDCANCEL )
       return;
     if ( Msg.GetFld(FN_ACCOUNTRECORD) )
      {
       accountRec=new MerchantRec(FN_ACCOUNTRECORD,&Msg);
       stationRec=new MerchantRec;
       stationRec->FormatNew(Key);
      }
     else
      {
       accountRec=new MerchantRec;
       accountRec->FormatNew(Key);
      }
     isNew=true;
     break;

   default: return;
  }

 Dialog = new MerchantDialog(GetMainWindow(),accountRec,stationRec);
 if ( isNew )
   Dialog->SetModified();
 DoCreateDialog( Dialog );
}

//-------------------------------
// Open a Response Message Record
//-------------------------------
void TSpsManagerApp::CmOpenrespmsg()
{
 OpenClassInfo Info;
 PosResponse* Rec;
 FormDialog* Dialog;

 Waiting(true);
 if ( OpenClassDialog(GetMainWindow(),"Open POS Response Record",
       &Info,POSRESPONSE_LIST).Execute() == IDCANCEL )
  {
   Waiting(false);
   return;
  }

 Waiting(false);

 // Get existing record or a template for a new record
 DBManMessage Msg;
 if ( Msg.DBGet(T_GETPOSRESPONSEREC,&Info,sizeof(Info)) != DB_OKAY )
  {
   Waiting(false);
   return;
  }
 Waiting(false);
 Rec = new PosResponse(FN_POSRESPONSERECORD,&Msg);
 Dialog = new ResponseMsgDialog(GetMainWindow(),Rec);
 if ( Info.IsNew )
  Dialog->SetModified();
 DoCreateDialog( Dialog );
 Waiting(false);
}

//----------------------------
// Open Product Classes Record
//-----------------------------
void TSpsManagerApp::CmOpenproductclasses()
{
 ProductClassList* List;
 FormDialog* Dialog;

 Waiting(true);
 List = new ProductClassList;
 Waiting(false);
 Dialog = new ProductClassDialog(GetMainWindow(),List);
 DoCreateDialog( Dialog );
}

//--------------------------------
// Reload Changes from a SAVE file
//--------------------------------
void TSpsManagerApp::CmReloadchanges()
{
 TFile* File;
 SAVERECORD_SENTINEL Sentinel;
 char *Buf;
 FormDialog* Win;
 bool Quit=false;

 TFileOpenDialog::TData FileData(
  OFN_HIDEREADONLY|OFN_PATHMUSTEXIST|OFN_NOCHANGEDIR|OFN_FILEMUSTEXIST,
  "SPS Manager Work Files (*.WRK)|*.wrk|", 0, "", "WRK");

 TFileOpenDialog dlg( GetMainWindow(), FileData );
 if ( dlg.Execute() != IDOK ||
      (File = OpenFile(FileData.FileName)) == 0 )
  return;

 Waiting(true);
 File->Seek(sizeof(SAVEFILE_SENTINEL));
 while( ! Quit &&
        File->Read(&Sentinel,sizeof(Sentinel)) == sizeof(Sentinel) )
  {
   Win=0;
   Buf=new char[Sentinel.Len];
   if ( File->Read(Buf,Sentinel.Len) != Sentinel.Len )
    {
     FileError("ERROR READING FROM FILE");
     break;
    }

   if ( Sentinel.Version < NUMRECTYPES &&
        Sentinel.Version != SaveRecordVersion[Sentinel.Type] )
    {
     if ( GetMainWindow()->MessageBox("A RECORD IN THIS FILE CANNOT BE\n"
                 "RESTORED DUE TO A VERSION MISMATCH.\n\n"
                 "RESPOND OK TO SKIP THE RECORD OR CANCEL TO QUIT.","",
                 MB_OKCANCEL|MB_ICONERROR) == IDCANCEL )
      Quit=true;
    }
   else
    switch ( Sentinel.Type )
     {
      case RecTypePosResponseMsg:
        {
         PosResponse* messages =
              new PosResponse((struct PosResponseRecord*) Buf );
         Win=new ResponseMsgDialog(GetMainWindow(),messages);
         delete[] Buf;
         break;
        } 


      case RecTypeDataRequirements:
        {
         DataRequirements* DataReq =
              new DataRequirements((struct DataReqRecord*) Buf );
         Win=new DataReqDialog(GetMainWindow(),DataReq);
         delete[] Buf;
         break;
        }

      case RecTypePosfileRules:
       {
        PositiveRules* PosRules=
              new PositiveRules( (struct PositiveRulesRecord*) Buf );
        Win=new PosFileRulesDialog(GetMainWindow(),PosRules);
        delete[] Buf;
        break;
       }

      case RecTypeUnvFundsRules:
        {
         UnvFundsRules* Rules =
              new UnvFundsRules((struct UnvFundsRulesRecord*) Buf );
         Win=new UnvFundsDialog(GetMainWindow(),Rules);
         delete[] Buf;
         break;
        }

      case RecTypeAuthRules:
        {
         AuthRules* globalRules=0;
         AuthRules* classRules=0;
         AuthRules* subClassRules=0;
         int NumRecs = Sentinel.Len/sizeof(globalRules->Data);
         struct AuthRulesRecord* Rec=(struct AuthRulesRecord*)Buf;
         switch( NumRecs )
          {
           case 3:  subClassRules=new AuthRules(&Rec[2]);
           case 2:  classRules=new AuthRules(&Rec[1]);
           case 1:  globalRules=new AuthRules(&Rec[0]);
           default: break;
          }

         Win = new AuthRulesDialog(GetMainWindow(),
                    globalRules,classRules,subClassRules,true);
         delete[] Buf;
        }
        break;

      case RecTypeMerchantRecord:
        {
         MerchantRec* accountRec;
         MerchantRec* stationRec=0;
         struct MerchantRecord* Rec=(struct MerchantRecord*)Buf;

         accountRec=new MerchantRec(&Rec[0]);
         if ( Sentinel.Len == 2 * sizeof(accountRec->Data) )
           stationRec=new MerchantRec(&Rec[1]);

         Win = new MerchantDialog(GetMainWindow(),
                    accountRec,stationRec);                     
         delete[] Buf;
        }
        break;

      case RecTypeProductClasses:
        Win=new ProductClassDialog(GetMainWindow(),
                       new ProductClassList(Buf,Sentinel.Len));
        break;

      default:
        GetMainWindow()->MessageBox(
          "THIS SAVE FILE CONTAINS UNRECOGNIZED DATA.\n"
          "THE RELOAD WILL BE ABORTED.","",MB_OK);
        Quit=true;
        break;
     }

   if ( Win )
    {
     Win->Create();
     Win->SetModified();
     Win->Show(SW_MINIMIZE);
     Dialogs.AddAtTail(Win);
    }
   else
    delete[] Buf;
  }

 File->Close();
 delete File;
 Waiting(false);
}



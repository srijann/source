//----------------------------------------------------------------------------
//  Project ClassLib
//
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    ClassLib.apx Application
//  FILE:         msgwin.cpp
//  AUTHOR:
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of TMsgWin (TDialog).
//
//----------------------------------------------------------------------------
#include "win4me.h"
#include <owl/updown.h>
#include <stdio.h>
#include "MsgWin.rh"
#include "MsgWin.h"
#include "OperatorId.h"

//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(TMsgWin, TDialog)
//{{TMsgWinRSP_TBL_BEGIN}}
  EV_WM_RBUTTONDOWN,
  EV_WM_LBUTTONDOWN,
  EV_COMMAND(CM_RELOADMESSAGES,CmReloadMessages),
#ifndef NOEDIT
  EV_COMMAND(CM_EDITMESSAGES,CmEditMessages),
  EV_COMMAND(CM_SAVEMESSAGES,CmSaveMessages),
#endif
//{{TMsgWinRSP_TBL_END}}
END_RESPONSE_TABLE;

//{{TMsgWin Implementation}}

#ifndef NOEDIT

//{{TDialog = TMsgEdit}}
class TMsgEdit : public TDialog {
  TEdit* Msg;
  TEdit* Msgnum;
  StringDB* StringHandler;
  int CurrentMsgNum;
  int StartMsgNum;
  TMsgWin* Creator;
  
  void TextOut();
  void SaveText();
  bool ModificationsWereMade;
  char* TransferBuf;

  public:
    TMsgEdit(TMsgWin*,StringDB*,int);
    virtual ~TMsgEdit();

//{{TMsgEditVIRTUAL_BEGIN}}
  public:
    virtual void SetupWindow();
    virtual TResult EvCommand(uint id, THandle hWndCtl, uint notifyCode);

//{{TMsgEditVIRTUAL_END}}

//{{TMsgEditRSP_TBL_BEGIN}}
  protected:
  bool UDNDeltapos(TNmUpDown& udn);
  void BNClickedCancel();
  void BNClickedDone();
  void EvClose();
//{{TMsgEditRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(TMsgEdit);
};    //{{TMsgEdit}}
#endif // ifndef NOEDIT


//------------
// Constructor
//------------
#pragma argsused
TMsgWin::TMsgWin(TWindow* parent,int Id,char* MsgFile,
                 TModule* Module)
:
    TDialog(parent,Id,Module)
{
  // Change the window's background color
  //
  SetBkgndColor(GetSysColor(COLOR_WINDOW));
  // Create a STatic control Bound inside the parent
  TextWin = new TEdit(this,IDCS_MSG);
  StringHandler = new StringDB(MsgFile);
  ShouldDeleteStringHandler=true;
  PopUpMenu=0;
  MoveInfo.increment=0;
  isBuddy=false;
}

//----------------------------------------------------
// Construct as Buddy window to another Message Window
//----------------------------------------------------
TMsgWin::TMsgWin(TMsgWin* cWin,int Id,int MoveIncrement) :
 TDialog(cWin->Parent,Id)
{
  // Change the window's background color
  //
  SetBkgndColor(GetSysColor(COLOR_WINDOW));
  // Create a STatic control Bound inside the parent
  TextWin = new TEdit(this,IDCS_MSG);
  StringHandler = cWin->StringHandler;
  ShouldDeleteStringHandler=false;
  PopUpMenu=0;
  MoveInfo.increment = MoveIncrement;
  MoveInfo.curPosition=Up;
  cWin->Buddy=this;
  Buddy=cWin;
  isBuddy=true;
}

//-----------
// Destructor
//-----------
TMsgWin::~TMsgWin()
{
 if ( ShouldDeleteStringHandler )
   delete StringHandler;
 if ( PopUpMenu )
   delete PopUpMenu;
 if ( isBuddy )
  Buddy->Buddy=0;
 Destroy();
}

//------------------------------------------------------
// Get message from string Handler and display in Window
//------------------------------------------------------
void TMsgWin::Say(int SayId)
{
 // Move the Buddy window Down
 if ( ! isBuddy && Buddy )
  Buddy->Move(Down);
 TextWin->Transfer( StringHandler->GetString(SayId), tdSetData);
 CurrentMsgNum=SayId;
}

void TMsgWin::Say(char* MessageText)
{
 // Move the Buddy window Down
 if ( ! isBuddy && Buddy )
  Buddy->Move(Down);
 TextWin->Transfer(MessageText,tdSetData);
}

//---------------------------------------------------------
// Handle the Right Click PoP-UP Menu in the Message Window
// Let's User ReLoad/Edit/Save Message File
//---------------------------------------------------------
#pragma argsused
void TMsgWin::EvRButtonDown(uint modKeys, TPoint& point)
{
 UINT flags;

 //TWindow::EvRButtonDown(modKeys, point);
 if (PopUpMenu)
  delete PopUpMenu;

 PopUpMenu = new TPopupMenu();
 PopUpMenu->AppendMenu(MF_STRING,CM_RELOADMESSAGES,
                       "&Reload Messages");
 COperatorId Id;
 flags = (Id.Get() < 100) ? MF_STRING : MF_STRING|MF_DISABLED|MF_GRAYED;
 PopUpMenu->AppendMenu(MF_SEPARATOR,0,"");
 PopUpMenu->AppendMenu(flags,CM_EDITMESSAGES,"&Edit Messages");
 flags = MF_STRING;
 if ( ! StringHandler->IsModified() )
   flags |= MF_DISABLED|MF_GRAYED;
 PopUpMenu->AppendMenu(flags,CM_SAVEMESSAGES,"&Save Messages");
 PopUpMenu->TrackPopupMenu(TPM_LEFTBUTTON|TPM_CENTERALIGN,point,0,HWindow);
}

//----------------------
// Handle the Left Click
//----------------------
#pragma argsused
void TMsgWin::EvLButtonDown(uint modKeys, TPoint& point)
{
 BringWindowToTop();
}

//------------------------
// Reload the Message File
//------------------------
void TMsgWin::CmReloadMessages(void)
{
 delete PopUpMenu;
 PopUpMenu=0;
 Say("Loading...");
 TextWin->SetCursor(0,IDC_WAIT);
 StringHandler->Load();
 Say(CurrentMsgNum);
 TextWin->SetCursor(0,IDC_ARROW);
}

#ifndef NOEDIT
//----------------------
// Invoke Message Editor
//----------------------
void TMsgWin::CmEditMessages(void)
{
 delete PopUpMenu;
 PopUpMenu=0;
 TMsgEdit EditWin(this,StringHandler,CurrentMsgNum);
 EditWin.Execute();
}

//----------------------
// Save The message File
//----------------------
void TMsgWin::CmSaveMessages(void)
{
 delete PopUpMenu;
 PopUpMenu=0;
 Say("Saving...");
 TextWin->SetCursor(0,IDC_WAIT);
 StringHandler->Save();
 Say(CurrentMsgNum);
 TextWin->SetCursor(0,IDC_ARROW);
}
#endif

//-----------------------------------------
// Check to see if the Window can be closed
//-----------------------------------------
bool TMsgWin::CanClose()
{
#ifndef NOEDIT
 if ( StringHandler->IsModified() )
   switch( MessageBox("YOU HAVE MADE CHANGES TO THE MESSAGE FILE.\n"
                   "SAVE BEFORE QUITTING?",
                   "WARNING",MB_YESNOCANCEL|MB_ICONEXCLAMATION) )
    {
     case IDCANCEL:
       return false;
     case IDYES:
       CmSaveMessages();
     default:
       break;
    }
#endif
 return true;
}

//----------------------------------------------------------------
// Handle Command notification so we can avoid closing the window
// on Escape.
//----------------------------------------------------------------
TResult TMsgWin::EvCommand(uint id, THandle hWndCtl, uint notifyCode)
{
  TResult result;

  // Filter OUT Escape
  if ( hWndCtl==NULL && id == IDCANCEL )
   {
    Move();
    return 0;
   }

  result = TDialog::EvCommand(id, hWndCtl, notifyCode);

  // INSERT>> Your code here.

  return result;
}

void TMsgWin::SetupWindow()
{
 TDialog::SetupWindow();
 MoveInfo.origY=TWindow::Attr.Y;
 MoveInfo.origX=TWindow::Attr.X;
}

//-----------------------------------------------------------
// Toggle the Position of the Special Handling Message Window
//-----------------------------------------------------------
void TMsgWin::Move(int How)
{
 if ( ! isBuddy && Buddy )
  {
   Buddy->Move(How);
   return;
  }

 if ( ! MoveInfo.increment )
  return;

 if (How==Toggle)
  if ( MoveInfo.curPosition==Up )
   How=Down;
  else
   How=Up;

 if ( How==MoveInfo.curPosition )
   return;

 if ( How==Down )
   MoveWindow(TWindow::Attr.X,
              TWindow::Attr.Y+MoveInfo.increment,
              TWindow::Attr.W,TWindow::Attr.H,true);
 else
   MoveWindow(MoveInfo.origX,MoveInfo.origY,
              TWindow::Attr.W,TWindow::Attr.H,true);

 MoveInfo.curPosition=How;
}


#ifndef NOEDIT

// This number is the TextLimit parameter used to construct the
// TEdit interface element. If the Edit window is set to NO scroll,
// then this number doesn't affect how much text fits inside the
// window. It only affects how much can be transferred OUT. If
// 1000 characters is not enough, you can provide your own MACRO.
#ifndef MAXEDITTEXT
#define MAXEDITTEXT 1000
#endif

//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(TMsgEdit, TDialog)
//{{TMsgEditRSP_TBL_BEGIN}}
  EV_UDN_DELTAPOS(IDC_MSCTLS_UPDOWN1, UDNDeltapos),
  EV_BN_CLICKED(IDCANCEL, BNClickedCancel),
  EV_BN_CLICKED(IDOK, BNClickedDone),
  EV_WM_CLOSE,
//{{TMsgEditRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{TMsgEdit Implementation}}

//-------------------
// Window Constructor
//-------------------
TMsgEdit::TMsgEdit(TMsgWin* parent,StringDB* StrDB,int MsgNum)
:
    TDialog(parent, IDD_MSGEDIT)
{
 Creator=parent;
 Msg=new TEdit(this,IDC_MSG,MAXEDITTEXT);
 Msg->FormatLines(true);
 Msgnum=new TEdit(this,IDCS_MSGNUM,5);
 // The UpDown Control Interface doesn't work right. See below
 //UpDown=new TUpDown(this,IDC_MSCTLS_UPDOWN1);
 StringHandler=StrDB;
 CurrentMsgNum=StartMsgNum=MsgNum;
 ModificationsWereMade=false;
 TransferBuf = new char[MAXEDITTEXT+1];
}

//------------------
// Window Destructor
//------------------
TMsgEdit::~TMsgEdit()
{
 delete[] TransferBuf;
 Destroy();
}

//-----------------------------------------------------------
// Overriding vitual function to do some setup we can't do in
// the constructor.
//-----------------------------------------------------------
void TMsgEdit::SetupWindow()
{
 TDialog::SetupWindow();
 // This stuff doesn't work right so we're not using it
 // UpDown->SetRange(0,UD_MAXVAL);
 // UpDown->SetPos(CurrentMsgNum);
 TextOut();
}

//-------------------------------------------------------------------
// Get Message Text from StringHandler and display in Text window.
// Also format Static control indicating the Current Message Number.
//-------------------------------------------------------------------
void TMsgEdit::TextOut()
{
 char buf[20];
 char *str=StringHandler->GetString(CurrentMsgNum);
 itoa(CurrentMsgNum,buf,10);
 Msgnum->Transfer(buf,tdSetData);
 Msg->Transfer(str,tdSetData);
 Msg->SetSelection(0,strlen(str));
}

//---------------------------------------------------------------
// Check the Text Window to see if the message has been modified.
// If it has, refer it to the StringHandler function.
//---------------------------------------------------------------
void TMsgEdit::SaveText()
{
 if ( ! Msg->IsModified() )
   return;
 Msg->Transfer(TransferBuf,tdGetData);
 StringHandler->PutString(TransferBuf,CurrentMsgNum);
 ModificationsWereMade=true;
 if ( CurrentMsgNum==StartMsgNum )
  Creator->Say(CurrentMsgNum);
}

//----------------------------------------------------------------
// Handle the UPDOWN control, iterating through Messages with each
// click.
//----------------------------------------------------------------
bool TMsgEdit::UDNDeltapos(TNmUpDown& udn)
{
// Save the current Message window, if modified.
 SaveText();

// Unfortunately, we cannot use the pos indicator of the UPDown
// control to remember the position for us because, like much of
// Windows, it doesn't behave as expected. We are using the iDelta
// here to interpret it ourselves. Hope it stays the same in other
// versions of Windows.
// CurrentMsgNum=UpDown->GetPos(); Doesn't work properly. First click
//    in opposite direction goes in same direction. Don't know if
//    it's Windows or OWL.

 switch( udn.iDelta )
  {
   // 1 Means UP
   case  1:   ++CurrentMsgNum; break;
   // -1 Means Down
   case -1:   if ( CurrentMsgNum )
                --CurrentMsgNum;
   default:   break;
  }

 // Display the next Message in UP/DOWN sequence
 TextOut();
 return false;
}

//-------------------------------------------------------------
// Cancel Button CLICKED. IGNORE changes to Current Message but
// can't UNDO prior changes so we have to warn the user about
// the temporary condition of his message file.
//-------------------------------------------------------------
void TMsgEdit::BNClickedCancel()
{
 if ( ModificationsWereMade )
   MessageBox("CANCEL only discards the modifications\n"
              "you have made in the currently displayed window.\n\n"
              "If you wish to UNDO all modifications, select\n"
              "'Reload Messages' from the EDIT MENU of the\n"
              "MESSAGE WINDOW after exiting.","",MB_OK|MB_ICONWARNING);

 CmCancel();
}


//--------------------------------------------------------------
// Done Button Clicked. Save the current Message before exiting.
//--------------------------------------------------------------
void TMsgEdit::BNClickedDone()
{
 SaveText();
 CmOk();
}

//--------------------------------------------------------------
// Close via System Menu or ESC. Treat same as Cancel.
//--------------------------------------------------------------
void TMsgEdit::EvClose()
{
 BNClickedCancel();
}

TResult TMsgEdit::EvCommand(uint id, THandle hWndCtl, uint notifyCode)
{
  TResult result;

  result = TDialog::EvCommand(id, hWndCtl, notifyCode);

  // When focus leaves the message number field, get the message number
  // and redisplay it.
  if ( notifyCode==EN_KILLFOCUS && id == IDCS_MSGNUM )
   {
    char Buf[6];
    Msgnum->Transfer(Buf,tdGetData);
    CurrentMsgNum=atoi(Buf);
    TextOut();
   }

  return result;
}


#endif // ifndef NOEDIT



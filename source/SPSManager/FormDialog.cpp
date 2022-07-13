//----------------------------------------------------------------------------
//  Project spsmanager
//
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    spsmanager.apx Application
//  FILE:         FormDialog.cpp
//  AUTHOR:       Jack Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of FormDialog (TDialog).
//
//  This is the Base Class for all of the SPSManager Dialogs.
//----------------------------------------------------------------------------
#include "win4me.h"
#pragma hdrstop
#include "FormDialog.h"
#include "AppNotify.h"

//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(FormDialog, TDialog)
//{{FormDialogRSP_TBL_BEGIN}}
//{{FormDialogRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{FormDialog Implementation}}

extern void SetTop(FormDialog* Win);

FormDialog::FormDialog(TWindow* parent, TResId resId, short Type)
:
 TDialog(parent, resId, 0),
 RecType(Type),
 ModifiedFlag(false),
 FailFlag(false)
{
}


FormDialog::~FormDialog()
{
 Destroy(IDCANCEL);
}

//--------------------------
// Our Dialogs Do Not ESCAPE
//--------------------------
TResult FormDialog::EvCommand(uint id, THandle hWndCtl, uint notifyCode)
{
 TResult result;
  switch( notifyCode )
   {
    case 0:
       if ( id==IDCANCEL || id==IDOK )
         return 0;
      break;
    default:
      break;
   }

  result = TDialog::EvCommand(id, hWndCtl, notifyCode);
  return result;
}

//--------------------------------------------
// Override CanClose to insure Window is saved
//--------------------------------------------
bool FormDialog::CanClose()
{
 if ( ! IsModified() )
  return true;

 SetTop();

 switch (MessageBox("YOU HAVEN'T SAVED THE CHANGES YOU MADE\n"
              "TO THIS WINDOW.\n\n"
              "DO YOU WANT TO SAVE THEM NOW?\n\n"
              "WARNING: IF YOU REPLY NO, YOUR CHANGES\n"
              "WILL BE DISCARDED!\n","",MB_YESNOCANCEL|MB_ICONEXCLAMATION) )
  {
   case IDYES:    return Save();
   case IDNO:     return true;
   case IDCANCEL: return false;
  }

 return true;
}

//------------------------------
// Intercept the System Commands
//------------------------------
TResult FormDialog::DefWindowProc(uint msg, TParam1 p1, TParam2 p2)
{
  TResult result;

  // Refer all SYSTEM commands to the main window for action. If it's
  // a close, we let the main window do the whole business.
  if ( msg==WM_SYSCOMMAND )
   {
    // Don't allow minimize or close with any bad data on the screen
    if ( ( (p1&0xfff0) == SC_MINIMIZE ||
           (p1&0xfff0) == SC_CLOSE )  &&
        ! Validate() )
     return 0;

    AppNotifyMsg( Parent, p1 & 0xfff0, (LPARAM) this);

    // Don't do the Close.. We let the main window do it
    if ( (p1 & 0xfff0) == SC_CLOSE )
      return 0;

    // Don't Allow Maximize to full screen
    if ( (p1 & 0xfff0) == SC_MAXIMIZE )
      p1 = SC_RESTORE;
   }

  result = TDialog::DefWindowProc(msg, p1, p2);
  return result;
}

//----------------------------------------------------------------
// Override the Create function to allow us to abort the dialog in
// case of failure.
//----------------------------------------------------------------
bool FormDialog::Create()
{
  bool result;

  result = TDialog::Create();
  if ( FailFlag )
   return false;
  else
   return result;
}

//----------------------------------------
// Provide a member to Bring Window to Top
//----------------------------------------
void FormDialog::SetTop()
{
 ::SetTop(this);
}



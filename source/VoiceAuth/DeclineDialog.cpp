//----------------------------------------------------------------------------
//  Project VoiceAuth
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    VoiceAuth.apx Application
//  FILE:         declinedialog.cpp
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of DeclineDialog (TDialog).
//
//----------------------------------------------------------------------------
#include "win4me.h"

#include "declinedialog.h"
#include "chkauth.h"
#include "Appnotify.h"


//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(DeclineDialog, TDialog)
//{{DeclineDialogRSP_TBL_BEGIN}}
  EV_BN_CLICKED(IDOK, BNClicked),
//{{DeclineDialogRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{DeclineDialog Implementation}}


DeclineDialog::DeclineDialog(TWindow* parent)
:
  TDialog(parent, IDD_DECLINED)
{
}


DeclineDialog::~DeclineDialog()
{
  Destroy(IDCANCEL);
}


void DeclineDialog::BNClicked()
{
 AppNotifyMsg(Parent,NOTIFY_DIALOGCOMPLETE,0);
 CmOk();
}


TResult DeclineDialog::EvCommand(uint id, THandle hWndCtl, uint notifyCode)
{
 TResult result;
  switch( notifyCode )
   {
    case 0:
       if ( id==IDCANCEL )
         return 0;
      break;

    default:
      break;
   }

  result = TDialog::EvCommand(id, hWndCtl, notifyCode);
  return result;
}


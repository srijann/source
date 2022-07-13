//----------------------------------------------------------------------------
//  Project VoiceAuth
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    VoiceAuth.apx Application
//  FILE:         approvaldialog.cpp
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of ApprovalDialog (TDialog).
//
//----------------------------------------------------------------------------
#include "win4me.h"

#include <stdio.h>
#include "approvaldialog.h"
#include "chkauth.h"
#include "appnotify.h"
#include "Checkauthfn.h"

//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(ApprovalDialog, TDialog)
//{{ApprovalDialogRSP_TBL_BEGIN}}
  EV_BN_CLICKED(IDOK, BNClicked),
//{{ApprovalDialogRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{ApprovalDialog Implementation}}


ApprovalDialog::ApprovalDialog(TWindow* parent, CheckAuth& AuthMsg)
:
  TDialog(parent, IDD_APPROVAL),
  authMsg(AuthMsg)
{
 APPNUM=new TEdit(this,IDC_APPNUM);
}


ApprovalDialog::~ApprovalDialog()
{
  Destroy(IDCANCEL);
}


void ApprovalDialog::BNClicked()
{
 AppNotifyMsg(Parent,NOTIFY_DIALOGCOMPLETE,0);
 CmOk();
}




void ApprovalDialog::SetupWindow()
{
 char Buf[10];
 WORD ApprovalNum;

 TDialog::SetupWindow();

 if ( authMsg.GetFld(FN_APPROVALNUM,ApprovalNum) )
  {
   sprintf(Buf,"%04d",ApprovalNum);
   APPNUM->Transfer(Buf,tdSetData);
  }

}


TResult ApprovalDialog::EvCommand(uint id, THandle hWndCtl, uint notifyCode)
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


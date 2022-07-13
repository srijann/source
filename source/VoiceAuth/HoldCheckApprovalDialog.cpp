//----------------------------------------------------------------------------
//  Project VoiceAuth
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    VoiceAuth.apx Application
//  FILE:         holdcheckHoldCheckApprovalDialog.cpp
//  AUTHOR:
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of HoldCheckHoldCheckApprovalDialog (TDialog).
//
//----------------------------------------------------------------------------
#include "win4me.h"

#include <stdio.h>
#include "HoldCheckApprovalDialog.h"
#include "chkauth.h"
#include "appnotify.h"

//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(HoldCheckApprovalDialog, TDialog)
//{{ApprovalDialogRSP_TBL_BEGIN}}
  EV_BN_CLICKED(IDOK, BNClicked),
//{{ApprovalDialogRSP_TBL_END}}
END_RESPONSE_TABLE;

#define NUMCONTROLS 8

//{{HoldCheckHoldCheckApprovalDialog Implementation}}


HoldCheckApprovalDialog::HoldCheckApprovalDialog(TWindow* parent, CheckAuth& AuthMsg)
:
  TDialog(parent, IDD_HOLDCHECKAPPROVAL),
  authMsg(AuthMsg)
{
 DCMan = new DialogControlManager(this,8,&AuthMsg);

 APPNUM1=new TEdit(this,IDC_HCAPPNUM1);
 APPNUM2=new TEdit(this,IDC_HCAPPNUM2);
 APPNUM3=new TEdit(this,IDC_HCAPPNUM3);
 APPNUM4=new TEdit(this,IDC_HCAPPNUM4);
 DCMan->Add(IDC_HCAPAMOUNT1,10,FN_HOLDCHECKAMOUNT1,-1);
 DCMan->Add(IDC_HCAPAMOUNT2,10,FN_HOLDCHECKAMOUNT2,-1);
 DCMan->Add(IDC_HCAPAMOUNT3,10,FN_HOLDCHECKAMOUNT3,-1);
 DCMan->Add(IDC_HCAPAMOUNT4,10,FN_HOLDCHECKAMOUNT4,-1);
 DCMan->Add(IDC_HCAPCHECKNUM1,10,FN_HOLDCHECKCHECKNUM1,-1);
 DCMan->Add(IDC_HCAPCHECKNUM2,10,FN_HOLDCHECKCHECKNUM2,-1);
 DCMan->Add(IDC_HCAPCHECKNUM3,10,FN_HOLDCHECKCHECKNUM3,-1);
 DCMan->Add(IDC_HCAPCHECKNUM4,10,FN_HOLDCHECKCHECKNUM4,-1);
 AuthMsg.GetFld(FN_HOLDCHECKSOURCETYPES,sizeof(sourceTypes),sourceTypes);
}

HoldCheckApprovalDialog::~HoldCheckApprovalDialog()
{
  Destroy(IDCANCEL);
}

void HoldCheckApprovalDialog::BNClicked()
{
 AppNotifyMsg(Parent,NOTIFY_DIALOGCOMPLETE,0);
 CmOk();
}


void HoldCheckApprovalDialog::SetupWindow()
{
 char Buf[10];
 WORD ApprovalNum;

 TDialog::SetupWindow();

 DCMan->CopyAllFromMsg();

 if ( authMsg.GetFld(FN_HOLDCHECKAPPROVALNUM1,ApprovalNum) )
  {
   sprintf(Buf,"%04d",ApprovalNum);
   APPNUM1->Transfer(Buf,tdSetData);
  }
 if ( authMsg.GetFld(FN_HOLDCHECKAPPROVALNUM2,ApprovalNum) )
  {
   sprintf(Buf,"%04d",ApprovalNum);
   APPNUM2->Transfer(Buf,tdSetData);
  }
 if ( authMsg.GetFld(FN_HOLDCHECKAPPROVALNUM3,ApprovalNum) )
  {
   sprintf(Buf,"%04d",ApprovalNum);
   APPNUM3->Transfer(Buf,tdSetData);
  }
 if ( authMsg.GetFld(FN_HOLDCHECKAPPROVALNUM4,ApprovalNum) )
  {
   sprintf(Buf,"%04d",ApprovalNum);
   APPNUM4->Transfer(Buf,tdSetData);
  }

 if ( sourceTypes[0] == 'P' )
  {
   APPNUM1->EnableWindow(false);
   DCMan->Disable(FN_HOLDCHECKAMOUNT1);
   DCMan->Disable(FN_HOLDCHECKCHECKNUM1);
  }
 if ( sourceTypes[1] == 'P' )
  {
   APPNUM2->EnableWindow(false);
   DCMan->Disable(FN_HOLDCHECKAMOUNT2);
   DCMan->Disable(FN_HOLDCHECKCHECKNUM2);
  }
 if ( sourceTypes[2] == 'P' )
  {
   APPNUM3->EnableWindow(false);
   DCMan->Disable(FN_HOLDCHECKAMOUNT3);
   DCMan->Disable(FN_HOLDCHECKCHECKNUM3);
  }
 if ( sourceTypes[3] == 'P' )
  {
   APPNUM4->EnableWindow(false);
   DCMan->Disable(FN_HOLDCHECKAMOUNT4);
   DCMan->Disable(FN_HOLDCHECKCHECKNUM4);
  }
}


TResult HoldCheckApprovalDialog::EvCommand(uint id, THandle hWndCtl, uint notifyCode)
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


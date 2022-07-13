//----------------------------------------------------------------------------
//  Project VoiceAuth
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    VoiceAuth.apx Application
//  FILE:         yesnodialog.cpp
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of YesNoDialog (TDialog).
//
//----------------------------------------------------------------------------
#include "win4me.h"

#include "yesnodialog.h"
#include "appnotify.h"
#include "chkauth.h"


//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(YesNoDialog, TDialog)
//{{YesNoDialogRSP_TBL_BEGIN}}
  EV_BN_CLICKED(IDCANCEL, BNCancelClicked),
  EV_BN_CLICKED(IDOK, BNOkClicked),
//{{YesNoDialogRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{YesNoDialog Implementation}}


YesNoDialog::YesNoDialog(TWindow* parent, int& YesNo,char* Text )
:
 TDialog(parent, IDD_YESNO),
 yesNo(YesNo),
 text(Text)
{
 yesNo=false;
 if ( Text )
  TEXT=new TStatic(this,IDC_YESNOTEXT);
}


YesNoDialog::~YesNoDialog()
{
  Destroy(IDCANCEL);
}


void YesNoDialog::BNCancelClicked()
{
 AppNotifyMsg(Parent,NOTIFY_DIALOGCOMPLETE,0);
 yesNo=false;
 CmOk();
}


void YesNoDialog::BNOkClicked()
{
 AppNotifyMsg(Parent,NOTIFY_DIALOGCOMPLETE,0);
 yesNo=true;
 CmOk();
}


void YesNoDialog::SetupWindow()
{
  TDialog::SetupWindow();
  if ( text )
   TEXT->Transfer(text,tdSetData);
}


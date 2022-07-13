//----------------------------------------------------------------------------
//  Project spsmanager
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    spsmanager.apx Application
//  FILE:         exitdialog.cpp
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of ExitDialog (TDialog).
//
//----------------------------------------------------------------------------
#include "win4me.h"
#pragma hdrstop
#include "exitdialog.h"

//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(ExitDialog, TDialog)
//{{ExitDialogRSP_TBL_BEGIN}}
  EV_BN_CLICKED(IDCANCEL, BNCancelClicked),
  EV_BN_CLICKED(IDC_SAVENOW, BNSaveNowClicked),
  EV_BN_CLICKED(IDC_SAVEWORK, BNSaveWorkClicked),
  EV_BN_CLICKED(IDC_REVIEW, BNReviewClicked),
  EV_BN_CLICKED(IDC_DISCARD, BNDiscardClicked),
//{{ExitDialogRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{ExitDialog Implementation}}


ExitDialog::ExitDialog(TWindow* parent, int* Sel, TResId resId, TModule* module)
:
 TDialog(parent, resId, module),
 Selection(Sel)
{
 SAVENOW=new TRadioButton(this,IDC_SAVENOW);
 SAVEWORK=new TRadioButton(this,IDC_SAVEWORK);
 REVIEW=new TRadioButton(this,IDC_REVIEW);
 DISCARD=new TRadioButton(this,IDC_DISCARD);
}


ExitDialog::~ExitDialog()
{
  Destroy(IDCANCEL);
}


void ExitDialog::SetupWindow()
{
 TDialog::SetupWindow();
 SAVENOW->SetCheck(true);
 *Selection=1;
}

void ExitDialog::BNCancelClicked()
{
 *Selection=0;
 CmCancel();
}


void ExitDialog::BNSaveNowClicked()
{
 *Selection=1;
}


void ExitDialog::BNSaveWorkClicked()
{
 *Selection=2;
}


void ExitDialog::BNReviewClicked()
{
 *Selection=3;
}


void ExitDialog::BNDiscardClicked()
{
 *Selection=4;
}


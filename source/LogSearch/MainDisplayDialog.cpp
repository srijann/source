//----------------------------------------------------------------------------
//  Project LogSearch
//  Secure Payment Systems
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    LogSearch.apx Application
//  FILE:         maindisplaydialog.cpp
//  AUTHOR:       Jack Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of MainDisplayDialog (TDialog).
//
//----------------------------------------------------------------------------
#include "win4me.h"

#include "maindisplaydialog.h"

//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(MainDisplayDialog, TDialog)
//{{MainDisplayDialogRSP_TBL_BEGIN}}
  EV_TCN_SELCHANGE(IDC_MAINTAB, TCNSelchange),
  EV_TCN_SELCHANGING(IDC_MAINTAB, TCNSelchanging),
//{{MainDisplayDialogRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{MainDisplayDialog Implementation}}


MainDisplayDialog::MainDisplayDialog(TWindow* parent)
:
 TDialog(parent, IDD_MAINDISPLAY)
{
  // INSERT>> Your constructor code here.
 MAINTAB = new TTabControl(this,IDC_MAINTAB);
 productList = new ProductClassList;
}


MainDisplayDialog::~MainDisplayDialog()
{
  Destroy(IDCANCEL);
  delete productList;
}


void MainDisplayDialog::SetupWindow()
{
 TDialog::SetupWindow();
 MAINTAB->Add("Auth");
 MAINTAB->Add("Decision");
 MAINTAB->Add("Hold Check");
 MAINTAB->Add("Employer/Spouse");
 MAINTAB->Add("Merchant");
 MAINTAB->Add("Rules 1-6");
 MAINTAB->Add("Rules 7-8");
 MAINTAB->Add("Posfile Rules");
 MAINTAB->Add("Posfile Record");
 chkauthDialog = new ChkauthDialog(this,productList);
 chkauthDialog->Create();
 chkauthDialog->Show(SW_NORMAL);
 CurrentDialog=chkauthDialog;
 decisionDialog = new DecisionDialog(this);
 decisionDialog->Create();
 employerDialog = new EmployerDialog(this);
 employerDialog->Create();
 holdCheckDialog = new HoldCheckDialog(this);
 holdCheckDialog->Create();
 merchantDialog = new MerchantDialog(this,productList);
 merchantDialog->Create();
 rulesDialog = new RulesDialog(this);
 rulesDialog->Create();
 rules2Dialog = new Rules2Dialog(this);
 rules2Dialog->Create();
 posfileDialog = new PosfileDialog(this);
 posfileDialog->Create();
 posRulesDialog = new PosRulesDialog(this);
 posRulesDialog->Create();
}


void MainDisplayDialog::TCNSelchange(TNotify&)
{
 CurrentDialog->Show(SW_HIDE);
 switch( MAINTAB->GetSel() )
  {
   case 0: CurrentDialog = chkauthDialog; break;
   case 1: CurrentDialog = decisionDialog; break;
   case 2: CurrentDialog = holdCheckDialog; break;
   case 3: CurrentDialog = employerDialog; break;
   case 4: CurrentDialog = merchantDialog; break;
   case 5: CurrentDialog = rulesDialog; break;
   case 6: CurrentDialog = rules2Dialog; break;
   case 7: CurrentDialog = posRulesDialog; break;
   case 8: CurrentDialog = posfileDialog; break;
  }

 CurrentDialog->Show(SW_SHOW);
 CurrentDialog->SetFocus();
}


bool MainDisplayDialog::TCNSelchanging(TNotify&)
{
 return false;
}

void MainDisplayDialog::Display(IPCMessage* Msg)
{
 chkauthDialog->Display(Msg);
 decisionDialog->Display(Msg);
 holdCheckDialog->Display(Msg);
 employerDialog->Display(Msg);
 merchantDialog->Display(Msg);
 rulesDialog->Display(Msg);
 rules2Dialog->Display(Msg);
 posfileDialog->Display(Msg);
 posRulesDialog->Display(Msg);
}

bool MainDisplayDialog::ShowWindow(int cmdShow)
{
  bool result;

  result = TDialog::ShowWindow(cmdShow);
  CurrentDialog->ShowWindow(cmdShow);
  // INSERT>> Your code here.

  return result;
}


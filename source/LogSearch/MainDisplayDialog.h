//----------------------------------------------------------------------------
//  Project LogSearch
//  Secure Payment Systems
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    LogSearch.apx Application
//  FILE:         maindisplaydialog.h
//  AUTHOR:       Jack Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for MainDisplayDialog (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(maindisplaydialog_h)              // Sentry, use file only if it's not already included.
#define maindisplaydialog_h

#include <owl/dialog.h>
#include <owl/tabctrl.h>

#include <owl/commctrl.h>

#include "MainDisplayDialog.rh"            // Definition of all resources.
#include "ChkauthDialog.h"
#include "DecisionDialog.h"
#include "EmployerDialog.h"
#include "HoldCheckDialog.h"
#include "RulesDialog.h"
#include "Rules2Dialog.h"
#include "MerchantDialog.h"
#include "PosfileDialog.h"
#include "PosRulesDialog.h"
#include "ProductClassList.h"

//{{TDialog = MainDisplayDialog}}
class MainDisplayDialog : public TDialog {
  TTabControl* MAINTAB;
  TDialog* CurrentDialog;

  ChkauthDialog* chkauthDialog;
  DecisionDialog* decisionDialog;
  EmployerDialog* employerDialog;
  HoldCheckDialog* holdCheckDialog;
  MerchantDialog* merchantDialog;
  RulesDialog* rulesDialog;
  Rules2Dialog* rules2Dialog;
  PosfileDialog* posfileDialog;
  PosRulesDialog* posRulesDialog;
  ProductClassList* productList;

  public:
    MainDisplayDialog(TWindow* parent);
    virtual ~MainDisplayDialog();
    void Display(IPCMessage*);

//{{MainDisplayDialogVIRTUAL_BEGIN}}
  public:
    virtual void SetupWindow();
    virtual bool ShowWindow(int cmdShow);
//{{MainDisplayDialogVIRTUAL_END}}

//{{MainDisplayDialogRSP_TBL_BEGIN}}
  protected:
    void TCNSelchange(TNotify& tcn);
    bool TCNSelchanging(TNotify& tcn);
//{{MainDisplayDialogRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(MainDisplayDialog);
};    //{{MainDisplayDialog}}


#endif  // maindisplaydialog_h sentry.


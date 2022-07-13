//----------------------------------------------------------------------------
//  Project LogSearch
//  Secure Payment Systems
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    LogSearch.apx Application
//  FILE:         merchantdialog.h
//  AUTHOR:       Jack Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for MerchantDialog (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(merchantdialog_h)              // Sentry, use file only if it's not already included.
#define merchantdialog_h

#include <owl/dialog.h>
#include <owl/static.h>
#include <owl/radiobut.h>
#include <owl/checkbox.h>

#include "MerchantDialog.rh"
#include "TimeZoneInfo.h"
#include "ProductClassList.h"


//{{TDialog = MerchantDialog}}
class MerchantDialog : public TDialog {

  TStatic* MERCHANTNAM;
  TStatic* MERCHANTNUM;
  TStatic* VFYSECRULES;
  TStatic* GUSECRULES;
  TStatic* VFYPOSFILERULES;
  TStatic* VFYRESPMSG;
  TStatic* VFYDATAREQ;
  TStatic* VFYAUTHRULESSUBCLASS;
  TStatic* VFYAUTHRULESCLASS;
  TStatic* GUPOSFILERULES;
  TStatic* GURESPMSG;
  TStatic* GUDATAREQ;
  TStatic* GUAUTHRULESSUBCLASS;
  TStatic* GUAUTHRULESCLASS;
  TStatic* MERCHANTTIMEZONE;
  TStatic* PRODUCTGROUP;

  TRadioButton* VERIFYSERVICE;
  TRadioButton* HOLDCHECKSERVICE;
  TRadioButton* PREAUTHSERVICE;
  TRadioButton* GUARANTEESERVICE;
  TRadioButton* GUARANTEEVERIFYSERVICE;

  // FED RESERVE REGION CHECKBOXES
  TCheckBox* FEDREG1;
  TCheckBox* FEDREG2;
  TCheckBox* FEDREG3;
  TCheckBox* FEDREG4;
  TCheckBox* FEDREG5;
  TCheckBox* FEDREG6;
  TCheckBox* FEDREG7;
  TCheckBox* FEDREG8;
  TCheckBox* FEDREG9;
  TCheckBox* FEDREG10;
  TCheckBox* FEDREG11;
  TCheckBox* FEDREG12;

  TStatic* MERCHANTZIP;
  TStatic* MERCHANTSTATE;
  TStatic* MERCHANTSIC;

  TStatic* VERIFYSERVICEAMOUNT;
  TStatic* PREAUTHDAYS;
  TStatic* PREAUTHAMOUNT;
  TStatic* HOLDCHECKAMOUNT;
  TStatic* HOLDCHECKVALUEPERCENT;
  TStatic* HOLDCHECK1PERCENT;

  void clearAll();
  TimeZoneInfo timeZoneInfo;
  ProductClassList* productList;

  public:
    MerchantDialog(TWindow* parent,ProductClassList*);
    virtual ~MerchantDialog();
    void Display(IPCMessage*);
};    //{{MerchantDialog}}


#endif  // merchantdialog_h sentry.


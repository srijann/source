//----------------------------------------------------------------------------
//  Project spsmanager
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    spsmanager.apx Application
//  FILE:         merchantdialog.h
//  AUTHOR:
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for MerchantDialog (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(merchantdialog_h)              // Sentry, use file only if it's not already included.
#define merchantdialog_h

#include <owl/dialog.h>
#include <owl/edit.h>
#include <owl/combobox.h>
#include <owl/checkbox.h>
#include <owl/radiobut.h>

#include "FormDialog.h"
#include "MerchantDialog.rh"
#include "MerchantRec.h"
#include "ListDefs.h"
#include "ProductClassList.h"
#include "EditUns.h"
#include "TimeZoneInfo.h"
#include "EditStateCodes.h"

// These names conflict with VELOCIS macros in Spsmandb.h
#undef MERCHANTID
#undef MERCHANTNAME

//{{TDialog = MerchantDialog}}
class MerchantDialog : public FormDialog {

  MerchantRec* accountRec;
  MerchantRec* stationRec;
  MerchantRec* workingRec;
  MerchantRec  origRec;
  TimeZoneInfo timeZoneInfo;

  StringTreeList* AuthRulesList;
  ProductClassList* PCList;

  TComboBox* VFYUNVFUNDSRULES;
  TComboBox* GUUNVFUNDSRULES;
  TComboBox* VFYPOSFILERULES;
  TComboBox* VFYRESPMSG;
  TComboBox* VFYDATAREQ;
  TComboBox* VFYAUTHRULESSUBCLASS;
  TComboBox* VFYAUTHRULESCLASS;
  TComboBox* GUPOSFILERULES;
  TComboBox* GURESPMSG;
  TComboBox* GUDATAREQ;
  TComboBox* GUAUTHRULESSUBCLASS;
  TComboBox* GUAUTHRULESCLASS;
  TComboBox* MERCHANTTIMEZONE;
  TComboBox* PRODUCTGROUP;

  // ENABLER BUTTONS
  TCheckBox* VFYDATAREQBTN;
  TCheckBox* VFYPOSFILERULESBTN;
  TCheckBox* GUPOSFILERULESBTN;
  TCheckBox* VFYRESPMSGBTN;
  TCheckBox* GURESPMSGBTN;
  TCheckBox* VFYUNVFUNDSRULESBTN;
  TCheckBox* GUUNVFUNDSRULESBTN;
  TCheckBox* GUDATAREQBTN;
  TCheckBox* GUAUTHRULESBTN;
  TCheckBox* VFYAUTHRULESBTN;
  TCheckBox* MERCHANTTIMEZONEBTN;
  TCheckBox* MERCHANTSICBTN;
  TCheckBox* MERCHANTZIPBTN;
  TCheckBox* MERCHANTSTATEBTN;
  TCheckBox* FEDREGBTN;
  TCheckBox* SERVICESBTN;
  TCheckBox* PRODUCTGROUPBTN;
  TCheckBox* MESSAGENUMBERBTN;

  // SERVICES

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

  // EDIT FIELDS
  TEdit* MERCHANTZIP;
  TEditStateCodes* MERCHANTSTATE;
  TEdit* MERCHANTNAME;
  TEdit* MERCHANTID;
  TEdit* MERCHANTSIC;
  TEdit* MESSAGENUMBER;

  TEditUns* VERIFYSERVICEAMOUNT;
  TEditUns* PREAUTHDAYS;
  TEditUns* PREAUTHAMOUNT;
  TEditUns* HOLDCHECKAMOUNT;
  TEditUns* HOLDCHECKVALUEPERCENT;
  TEditUns* HOLDCHECK1PERCENT;

  bool SetupComplete;
  void EditChange(uint Id);
  void BTNChange(uint Id);
  void BoxChange(uint Id);

  void InitSubClassBox(TComboBox* Box,char *Class);
  MerchantRec* GetRec(int Enabler);
  void DisplayFedRegions(void);
  void DisplayServices(void);
  void DisplayEdit(TEdit* Ctl,char* Field,int Enabler);
  void DisplayBox(TComboBox* Ctl,char* Field,int Enabler);
  void DisplayTimeZone(int Value);
  void DisplayProductGroup();
  void EnableByService();
  bool Validator();

  public:
    MerchantDialog(TWindow* parent, MerchantRec* AccountRec,
                   MerchantRec* StationRec);
    virtual ~MerchantDialog();
    virtual bool IsModified();
    virtual bool Save();
    virtual bool SaveTemp(TFile* File);
    virtual void ResetModified();
    virtual bool Validate();

//{{MerchantDialogVIRTUAL_BEGIN}}
  public:
    virtual void SetupWindow();
    virtual bool Create();
    virtual TResult EvCommand(uint id, THandle hWndCtl, uint notifyCode);
//{{MerchantDialogVIRTUAL_END}}
};    //{{MerchantDialog}}


#endif  // merchantdialog_h sentry.


//----------------------------------------------------------------------------
//  Project spsmanager
//
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    spsmanager.apx Application
//  FILE:         SecRulesdialog.h
//  AUTHOR:
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for SecRulesDialog (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(SecRulesdialog_h)              // Sentry, use file only if it's not already included.
#define SecRulesdialog_h

#include <owl/dialog.h>
#include <owl/tabctrl.h>
#include <owl/checkbox.h>

#include "SecRulesDialog.rh"
#include "SecRules.h"
#include "FormDialog.h"
#include "edituns.h"

//{{TDialog = SecRulesDialog}}
class SecRulesDialog : public FormDialog {
   SecRules *origRec;
   SecRules  newRec;

   TCheckBox* MAXBANKVERIFY;
   TCheckBox* HIRISKPRODUCT2;
   TCheckBox* HIRISKPRODUCT1;
   TCheckBox* HIRISKCHECKNUMBER;
   TCheckBox* HIRISKAMOUNT;
   TCheckBox* HIRISKDEMOGRAPHIC;
   TCheckBox* NOAPARTMENT;
   TCheckBox* MAXRCODES;
   TCheckBox* HIRISKCALICENSE;
   TCheckBox* SSNFRAUD;
   TCheckBox* LOWCREDITSCORE;
   TCheckBox* ADDRESSUNVERIFIABLE;
   TCheckBox* SSNINVALID;
   TCheckBox* PHONEUNVERIFIABLE;
   TCheckBox* CHECKMUSTBESIGNED;
   TCheckBox* NOCARRYOUT;
   TCheckBox* HIRISKAGE;


   TEditUns* HiRiskProduct1ToTime;
   TEditUns* HiRiskProduct1FromTime;
   TEditUns* HiRiskProduct2ToTime;
   TEditUns* HiRiskProduct2FromTime;
   TEditUns* HiRiskProduct2DayOfWeek;
   TEditUns* HiRiskProduct2Amount;
   TEditUns* HiRiskProduct2Value;
   TEditUns* HiRiskProdcut2FromTime;
   TEditUns* HiRiskProduct1DayOfWeek;
   TEditUns* HiRiskProduct1Amount;
   TEditUns* HiRiskProduct1Value;
   TEditUns* HiRiskCheckNumberValue;
   TEditUns* HiRiskAmountValue;
   TEditUns* MaxRcodesNum;
   TEditUns* MaxBankVerifyDays;
   TEditUns* MaxBankVerifyNumber;
   TEditUns* LowCreditScoreValue;
   TEditUns* HiRiskDemographicClass;
   TEditUns* HiRiskAgeValue;

   TEdit* HiRiskCALicense10;
   TEdit* HiRiskCALicense8;
   TEdit* HiRiskCALicense7;
   TEdit* HiRiskCALicense6;
   TEdit* HiRiskCALicense5;
   TEdit* HiRiskCALicense4;
   TEdit* HiRiskCALicense3;
   TEdit* HiRiskCALicense2;
   TEdit* HiRiskCALicense1;
   TEdit* HiRiskCALicense9;

  void UpdateWin();
  void ChangeHandler(uint Id);
  bool SetupComplete;

  void SetStateRanges(bool Enable);

  public:
    SecRulesDialog(TWindow* parent, SecRules* OrigRec );
    virtual ~SecRulesDialog();
    virtual bool IsModified();
    virtual bool Save();
    virtual bool SaveTemp(TFile* File);
    virtual void ResetModified();
    virtual bool Validate();
//{{SecRulesDialogVIRTUAL_BEGIN}}
  public:
    virtual void SetupWindow();
    TResult EvCommand(uint id, THandle hWndCtl, uint notifyCode);
//{{SecRulesDialogVIRTUAL_END}}

//{{SecRulesDialogRSP_TBL_BEGIN}}
  protected:
//{{SecRulesDialogRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(SecRulesDialog);
};    //{{SecRulesDialog}}


#endif  // SecRulesdialog_h sentry.


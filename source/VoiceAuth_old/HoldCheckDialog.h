//----------------------------------------------------------------------------
//  Project VoiceAuth
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    VoiceAuth.apx Application
//  FILE:         holdcheckdialog.h
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for HoldCheckDialog (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(holdcheckdialog_h)              // Sentry, use file only if it's not already included.
#define holdcheckdialog_h

#include <owl/dialog.h>

#include <owl/commctrl.h>
#include <owl/button.h>
#include <owl/checkbox.h>
#include "HoldCheckDialog.rh"
#include "CheckAuth.h"
#include "MsgWin.h"
#include "DCManage.h"

//{{TDialog = HoldCheckDialog}}
class HoldCheckDialog : public TDialog {
  TStatic* HCMAXTEXT;
  TStatic* HCMINTEXT;
  TEdit* HCMAX;
  TEdit* HCSUM;
  TEdit* HCMIN1;
  TStatic* ISAPPROVAL;
  TCheckBox* CHECKBOX[3];

  uint   MaxSum;
  uint   MinAmount1;
  uint   getAmount(TEdit*);
  void   computeMaxSum();
  CheckAuth& authMsg;
  void  adjustAmount(int Id);
  void  adjustAmounts();
  bool   sumAmounts();
  void  handleApprovedButton(int n,FLDNUM amountFN,FLDNUM checknumFN);

  bool   displayMode;

  struct
   {
    char  Amount[12];
    char  CheckNumber[7];
   } origInfo[4];


  TMsgWin* msgWin; 
  public:
    HoldCheckDialog(TWindow* parent,CheckAuth& Msg,TMsgWin* MsgWin);
    virtual ~HoldCheckDialog();
    bool editData();
    DialogControlManager* DCMan;

//{{HoldCheckDialogVIRTUAL_BEGIN}}
  public:
    virtual TResult EvCommand(uint id, THandle hWndCtl, uint notifyCode);
    virtual void SetupWindow();
//{{HoldCheckDialogVIRTUAL_END}}

//{{HoldCheckDialogRSP_TBL_BEGIN}}
  protected:
    void BNOkayClicked();
    void ENKillfocusVehicleValue();
    void ENKillfocusAmount1();
    void ENKillfocusAmount2();
    void ENKillfocusAmount3();
    void ENKillfocusAmount4();
    void BNClicked1();
    void BNClicked2();
    void BNClicked3();
    void BNClicked4();
//{{HoldCheckDialogRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(HoldCheckDialog);
};    //{{HoldCheckDialog}}


#endif  // holdcheckdialog_h sentry.


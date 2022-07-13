//---------------------------------------------------------------------------
//  Project CheckAuth
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    CheckAuth Application
//  FILE:         checkauthdlgclient.h
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for TCheckAuthDlgClient (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(checkauthdlgclient_h)              // Sentry, use file only if it's not already included.
#define checkauthdlgclient_h
#include <owl/gauge.h>
#include <owl/button.h>
#include <time.h>
#include "checkauthapp.rh"                  // Definition of all resources.
#include "checkauth.rh"
#include "msgwin.h"
#include "DCManage.h"
#include "CheckAuthFn.h"
#include "CheckAuth.h"
#include "HoldCheckDialog.h"
#include "DecisionDialog.h"
#include "EmployerDialog.h"
#include "ProductClassList.h"

//{{TDialog = TCheckAuthDlgClient}}
class TCheckAuthDlgClient : public TDialog {
  TGauge* TimerBar;
  TButton* CancelButton;
  TButton* StartButton;
  TButton* ViewButton;
  TMsgWin* Instruct;
  TDialog* CurrentDialog;
  TDialog* CurrentView;
  HoldCheckDialog* holdCheckDialog;
  DecisionDialog*  decisionDialog;
  EmployerDialog*  employerDialog;
  DialogControlManager* DCMan;
  ProductClassList* ProductList;
  CheckAuth* AuthMsg;
  IPCMessage* AuthReqMsg;
  time_t StartTime;
  time_t StopTime;
  void StepTimer(void);
  LRESULT ProcessChkauthNotify(WPARAM MsgType, LPARAM Id);
  void getMerchantData();
  void verifyMerchantData();
  void doNext();
  void WaitForStart();
  void SetupControls();
  void SetupLists();
  void enableFields(int Type);
  int  authTypeEnabled;
  void doReferral();
  bool collectAuthData(int Type);
  int  getInvalidMsgID(FLDNUM Fn,BYTE Rcode);
  void doAuthorization();
  void isPostAuthRequest();
  void doDecision();
  void sendApproval();
  bool doPostAuth();
  bool doPreAuth();
  bool sendHoldCheckApprovals();
  bool sendHoldCheckDeclines();
  void sendCancel();
  bool logTransaction();
  int  authStep;
  void setMerchantCaption();
  bool replyPending;
  bool logPending;
  bool lockFocus;
  int  YesNoAnswer;

  enum steps
   {
    STEP_STARTING,
    STEP_FETCHINGREFERRAL,
    STEP_VERIFYINGREFERRAL,
    STEP_FETCHINGMERCHANTDATA,
    STEP_COLLECTINGAMOUNT,
    STEP_PREAUTHQUERY,
    STEP_COLLECTINGHOLDCHECKDATA,
    STEP_COLLECTINGAUTHDATA,
    STEP_COLLECTINGBVDATA,
    STEP_COLLECTINGEMPLOYERDATA,
    STEP_DECIDING,
    STEP_APPROVED,
    STEP_DECLINED,
   };

  char MessageFileName[MAX_PATH+1];

  public:
    TCheckAuthDlgClient(TWindow* parent, TResId resId = IDD_CHKAUTH, TModule* module = 0);
    virtual ~TCheckAuthDlgClient();


//{{TCheckAuthDlgClientVIRTUAL_BEGIN}}
  public:
    virtual void SetupWindow();
    virtual TResult EvCommand(uint id, THandle hWndCtl, uint notifyCode);
//{{TCheckAuthDlgClientVIRTUAL_END}}

//{{TCheckAuthDlgClientRSP_TBL_BEGIN}}
  protected:
  void cancelTransaction();
  void endTransaction();
  void EvTimer(uint timerId);
  void ViewClicked();
  void AdjustAmount();
  void enableBusinessName();
//{{TCheckAuthDlgClientRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(TCheckAuthDlgClient);
};    //{{TCheckAuthDlgClient}}

#endif  // checkauthdlgclient_h sentry.

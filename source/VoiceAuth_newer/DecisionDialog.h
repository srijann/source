//----------------------------------------------------------------------------
//  Project VoiceAuth
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    VoiceAuth.apx Application
//  FILE:         decisiondialog.h
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for DecisionDialog (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(decisiondialog_h)              // Sentry, use file only if it's not already included.
#define decisiondialog_h

#include <owl/dialog.h>


#include <owl/commctrl.h>
#include <owl/checkbox.h>
#include <owl/updown.h>
#include <owl/edit.h>
#include <owl/button.h>
#include "DecisionDialog.rh"            // Definition of all resources.
#include "checkauth.h"
#include "kwrec.h"
#include "msgwin.h"
#include "RcodeInfo.h"
#include "BankPhoneDB.h"

#define MAXACTIVITY 5
#define MAXCHECKS   3
#define NUMRCODES   8
#define PERIOD1DAYS   3
#define PERIOD2DAYS   8
#define PERIOD3DAYS  30

//{{TDialog = DecisionDialog}}
class DecisionDialog : public TDialog {
  TCheckBox* RcodeCheck[NUMRCODES];
  TUpDown* ActivityUpDn;
  TUpDown* ChecksUpDn;
  struct BankPhoneRecord BankPhoneRec;

  struct
   {
    TEdit* DATE;
    TEdit* MERCH;
    TEdit* AMT;
    TEdit* CKNUM;
    TEdit* BV;
    TEdit* RC;
    TEdit* IDS;
   } ActRows[MAXACTIVITY];

  struct
   {
    TEdit* DATE;
    TEdit* MERCH;
    TEdit* AMT;
    TEdit* CKNUM;
    TEdit* TYPE;
    TEdit* IDS;
   } CheckRows[MAXCHECKS];

  TEdit* BVABA;
  TEdit* BVACCOUNT;
  TEdit* BVACCTOPENDATE;
  TEdit* BVAVGBAL;
  TEdit* BVACCTCLOSEDDATE;
  TEdit* BVPER1NUM;
  TEdit* BVPER2NUM;
  TEdit* BVPER3NUM;
  TEdit* BVPER1AMT;
  TEdit* BVPER2AMT;
  TEdit* BVPER3AMT;
  TEdit* BVAGE;
  TEdit* BVCHKNUM;
  TEdit* BVRESPTEXT;
  TEdit* BVAMOUNT;

  TEdit* BVBANKNAME;
  TEdit* BVBANKPHONE;
  TEdit* BVBANKCOMMENT;
  TEdit* COMMENTBOX;
  TEdit* MOTHERSNAME;

  TButton* CHANGEBUTTON;

  char   BVStatus[3];
  char   CommentBuf[250];
  CheckAuth& AuthMsg;
  IPCMessage& replyMsg;
  TMsgWin* msgWin;
  TMsgWin* msgWin2;
  DWORD    msgNum;

  ACTIVITY* Act;
  int numActivity;
  int ActivityIndex;
  BADCHECK*  BadChecks;
  int numChecks;
  int ChecksIndex;
  void displayActivity();
  void displayChecks();
  void displayBVDays();
  void formatActivityRow(int Row,int ActIndex);
  void formatChecksRow(int Row,int ChecksIndex);
  void displayDecisionMsg();
  bool sendReply();
  int  BVDays;
  bool doUnverifiedFundsRules();
  bool isVerifyButtonChecked();
  bool verifyButtonClicked;

  RcodeInfo rcodeInfo;
  IPCMessage BankPhoneMsg;

  public:
    DecisionDialog(TWindow* parent, CheckAuth& Msg,
                    IPCMessage& ReplyMsg,TMsgWin* MsgWin,DWORD MsgNum);
    virtual ~DecisionDialog();

//{{DecisionDialogVIRTUAL_BEGIN}}
  public:
    virtual void SetupWindow();
    virtual TResult EvCommand(uint id, THandle hWndCtl, uint notifyCode);
//{{DecisionDialogVIRTUAL_END}}

//{{DecisionDialogRSP_TBL_BEGIN}}
  protected:
    void BNOkayClicked();
    void BNDeclineClicked();
    void BNUnverifyClicked();
    bool UDNACTDeltapos(TNmUpDown& udn);
    bool UDNCHKDeltapos(TNmUpDown& udn);
    void BNIFClicked();
    void BNGoodClicked();
    void BNClosedClicked();
    void BNUnverifableClicked();
    void BNUnvClicked();
    void BNChangeClicked();
//{{DecisionDialogRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(DecisionDialog);
};    //{{DecisionDialog}}


#endif  // decisiondialog_h sentry.


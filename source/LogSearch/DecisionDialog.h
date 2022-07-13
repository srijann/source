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
#include <owl/radiobut.h>
#include "DecisionDialog.rh"            // Definition of all resources.
#include "checkauth.h"
#include "kwrec.h"
#include "RcodeInfo.h"

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

  CheckAuth AuthMsg;

  struct
   {
    TStatic* DATE;
    TStatic* MERCH;
    TStatic* AMT;
    TStatic* CKNUM;
    TStatic* BV;
    TStatic* RC;
    TStatic* IDS;
   } ActRows[MAXACTIVITY];

  struct
   {
    TStatic* DATE;
    TStatic* MERCH;
    TStatic* AMT;
    TStatic* CKNUM;
    TStatic* TYPE;
    TStatic* IDS;
   } CheckRows[MAXCHECKS];

  TStatic* BVABA;
  TStatic* BVACCOUNT;
  TStatic* BVACCTOPENDATE;
  TStatic* BVAVGBAL;
  TStatic* BVACCTCLOSEDDATE;
  TStatic* BVPER1NUM;
  TStatic* BVPER2NUM;
  TStatic* BVPER3NUM;
  TStatic* BVPER1AMT;
  TStatic* BVPER2AMT;
  TStatic* BVPER3AMT;
  TStatic* BVAGE;
  TStatic* BVCHKNUM;
  TStatic* BVRESPTEXT;
  TStatic* BVAMOUNT;
  TStatic* TRANSCORE;
  TStatic* SSNSCORE;
  TStatic* CONSPHOK;
  TStatic* EMPPHOK;
  TStatic* COMMENTBOX;
  TStatic* MOTHERSNAME;

  TRadioButton* BVCLOSEDBTN;
  TRadioButton* BVIFBTN;
  TRadioButton* BVGOODBTN;
  TRadioButton* BVUNVERIFYBTN;

  char   BVStatus[3];

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
  int  BVDays;
  void clearAll();

  RcodeInfo rcodeInfo;

  public:
    DecisionDialog(TWindow* parent);
    virtual ~DecisionDialog();
    void Display(IPCMessage*);

//{{DecisionDialogVIRTUAL_BEGIN}}
//{{DecisionDialogVIRTUAL_END}}

//{{DecisionDialogRSP_TBL_BEGIN}}
  protected:
    bool UDNACTDeltapos(TNmUpDown& udn);
    bool UDNCHKDeltapos(TNmUpDown& udn);
//{{DecisionDialogRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(DecisionDialog);
};    //{{DecisionDialog}}


#endif  // decisiondialog_h sentry.


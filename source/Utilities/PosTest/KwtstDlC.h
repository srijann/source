//----------------------------------------------------------------------------
//  Project Kwtest
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    Kwtest Application
//  FILE:         kwtstdlc.h
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for TKwtestDlgClient (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(kwtstdlc_h)              // Sentry, use file only if it's not already included.
#define kwtstdlc_h

#include "kwtstapp.rh"                  // Definition of all resources.


#include <owl/commctrl.h>
#include <owl/edit.h>
#include <owl/button.h>
#include <owl/radiobut.h>
#include <owl/checkbox.h>
#include "EditUns.h"
#include "kwrec.h"

#define MAXACT 9

//{{TDialog = TKwtestDlgClient}}
class TKwtestDlgClient : public TDialog {

   TCheckBox* CHECKSBOX;
   TCheckBox* BIG;

   TButton* SENDBUTTON;
   TEdit* AMOUNT;
   TEdit* STATE;
   TEdit* CHECKNUM;
   TEdit* PHONE;
   TEdit* ABA;
   TEdit* ACCOUNT;  
   TEdit* MERCHNUM;
   TEdit* RESPONSE;
   TButton* SEND;
   TEdit* DL;
   TEdit* DOB;
   TEdit* RCODE;
   TEdit* ZIPCODE;
   TEdit* SSN;

   struct
    {
     TEdit* MERCHID;
     TEditUns* AGE;
     TEditUns* AMOUNT;
     TEditUns* CHECKNUM;
     TEdit* SIC;
     TEditUns* RCODE;
    } ACTEDIT[MAXACT];

   ACTIVITY Act[20];
   CHECK BadCheck[20];

   void EnableEdit(bool);
   void ClearEdit();
   void MsgToWin(TEdit*,FLDNUM);
   void WinToMsg(TEdit*,FLDNUM);
   void ActToMsg();
   char TransBuf[5000];

   IPCMessage Msg;



  public:
    TKwtestDlgClient(TWindow* parent, TResId resId = IDD_CLIENT, TModule* module = 0);
    virtual ~TKwtestDlgClient();
    LRESULT ProcessMessage(WPARAM w, LPARAM lParam);

//{{TKwtestDlgClientVIRTUAL_BEGIN}}
  public:
    virtual void SetupWindow();
//{{TKwtestDlgClientVIRTUAL_END}}

//{{TKwtestDlgClientRSP_TBL_BEGIN}}
  protected:
    void BNClicked();
    void BNSendClicked();
    void BNRecvClicked();
    void BNAutoClicked();
    void BNClickedActClear();
//{{TKwtestDlgClientRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(TKwtestDlgClient);
};    //{{TKwtestDlgClient}}


#endif  // kwtstdlc_h sentry.

//----------------------------------------------------------------------------
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
#include <owl/validate.h>
#include <owl/edit.h>
#include <time.h>
#include "checkauth.rh"                  // Definition of all resources.
#include "msgwin.h"

//{{TDialog = TCheckAuthDlgClient}}
class TCheckAuthDlgClient : public TDialog, IPCMessage {
  TGauge* TimerBar;
  TButton* CancelButton;
  TButton* StartButton;

  TEdit* TRANSPORT;
  TEdit* ADRSTATE;
  TEdit* ADDRESS;
  TEdit* CHECKTYPE;
  TEdit* CHECKNUMBER;
  TEdit* CITY;
  TEdit* RESIDENCE;
  TEdit* ZIP2;
  TEdit* ZIP1;
  TEdit* PRODUCTCLASS;
  TEdit* BUSINESSNAME;
  TEdit* PRIVATELABEL;
  TEdit* PHONE;
  TEdit* AREACODE;
  TEdit* SSN;
  TEdit* FIRSTNAME;
  TEdit* LASTNAME;
  TEdit* MICR;
  TEdit* DOB;
  TEdit* LICENSE;
  TEdit* STATE;
  TEdit* AMOUNT;
  TEdit* MERCHNUM;

  TStatic* ST_TRANSPORT;
  TStatic* ST_ADRSTATE;
  TStatic* ST_ADDRESS;
  TStatic* ST_CHECKTYPE;
  TStatic* ST_CHECKNUMBER;
  TStatic* ST_CITY;
  TStatic* ST_RESIDENCE;
  TStatic* ST_ZIP;
  TStatic* ST_PRODUCTCLASS;
  TStatic* ST_BUSINESSNAME;
  TStatic* ST_PRIVATELABEL;
  TStatic* ST_PHONE;
  TStatic* ST_SSN;
  TStatic* ST_NAME;
  TStatic* ST_MICR;
  TStatic* ST_DOB;
  TStatic* ST_LICENSE;
  TStatic* ST_STATE;
  TStatic* ST_AMOUNT;
  TStatic* ST_MERCHNUM;

  TMsgWin* Instruct;

  time_t StartTime;
  time_t StopTime;
  void StepTimer(void);
  LRESULT ProcessNetReply(WPARAM MsgType, LPARAM Id);
  LRESULT ProcessChkauthNotify(WPARAM MsgType, LPARAM Id);

  // If we have raised a modeless dialog from our main window this
  // is it.
  TDialog* CurrentDialog;

  void FetchStation();
  void ClearAllFields();
  int mode;

  enum modes
   {
    MODE_START,
    MODE_GETREFERRAL,
    MODE_GETSTATION,
    MODE_GETBASIC,
    MODE_GETEXTENDED
   };

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
  void NextAction();
  void CancelClicked();
  void EvTimer(uint timerId);
//{{TCheckAuthDlgClientRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(TCheckAuthDlgClient);
};    //{{TCheckAuthDlgClient}}


#endif  // checkauthdlgclient_h sentry.

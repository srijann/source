//----------------------------------------------------------------------------
//  Project Kwsimv
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    Kwsimv Application
//  FILE:         kwsimvdlgclient.h
//  AUTHOR:
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for ResponseDialog (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(kwsimvdlgclient_h)              // Sentry, use file only if it's not already included.
#define kwsimvdlgclient_h

#include <owl/edit.h>
#include "ResponseDialog.rh"             // Definition of all resources.

#include <owl/commctrl.h>
#include "kwrec.h"
#include "ipcmsg.h"

#define MAXACTIVITY 10
#define MAXCHECKS    5

//{{TDialog = ResponseDialog}}
class ResponseDialog : public TDialog {

  struct
   {
    TEdit* DATE;
    TEdit* TIME;
    TEdit* MERCH;
    TEdit* AMT;
    TEdit* CKNUM;
    TEdit* SIC;
    TEdit* RC;
    TEdit* BV;
    TEdit* BVDT;
    TEdit* BVBAL;
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

  TEdit* KWRCODE;
  TEdit* KWTEXT;
  TEdit* MOTHERSNAME;

  ACTIVITY Act[MAXACTIVITY];
  BADCHECK Check[MAXCHECKS];

  void ActivityToMsg(IPCMessage& Msg);
  void ChecksToMsg(IPCMessage& Msg);
  void ResponseToMsg(IPCMessage& Msg);

  public:
    ResponseDialog(TWindow* parent, TResId resId = IDD_RESPONSE, TModule* module = 0);
    virtual ~ResponseDialog();
    void ProcessMessage(IPCMessage& Msg);              

};    //{{ResponseDialog}}


#endif  // kwsimvdlgclient_h sentry.

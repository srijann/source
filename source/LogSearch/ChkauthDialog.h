//----------------------------------------------------------------------------
//  Project LogSearch
//  Secure Payment Systems
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    LogSearch.apx Application
//  FILE:         chkauthdialog.h
//  AUTHOR:       Jack Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for ChkauthDialog (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(chkauthdialog_h)              // Sentry, use file only if it's not already included.
#define chkauthdialog_h

#include <owl/dialog.h>
#include <owl/static.h>

#include "CheckAuth.rh"
#include "IPCTransfer.h"
#include "ProductClassList.h"


//{{TDialog = ChkauthDialog}}
class ChkauthDialog : public TDialog {
   TStatic* OPERATORID;
   TStatic* CALLTIME;
   TStatic* CALLDATE;
   TStatic* CALLMINUTES;
   TStatic* CALLSECONDS;
   TStatic* CONSUMERNAME;
   TStatic* BANKACCOUNT;
   TStatic* BANKNUMBER;
   TStatic* MERCHID;
   TStatic* PRIVATELABEL;
   TStatic* BUSINESSNAME;
   TStatic* TRANSPORTMETHOD;
   TStatic* PRODUCTCLASS;
   TStatic* SSN;
   TStatic* PHONE;
   TStatic* APARTMENT;
   TStatic* ZIPCODE;
   TStatic* CONSUMERSTATE;
   TStatic* CITY;
   TStatic* ADDRESS;
   TStatic* TITLE;
   TStatic* CHECKNUMBER;
   TStatic* DOB;
   TStatic* LICENSE;
   TStatic* LICENSESTATE;
   TStatic* CHECKTYPE;
   TStatic* AMOUNT;
   TStatic* APPROVALNUM;

   IPCTransfer Transfer;
   ProductClassList* productClassList;

  public:
    ChkauthDialog(TWindow* parent,ProductClassList*);
    virtual ~ChkauthDialog();
    void Display(IPCMessage*);

//{{ChkauthDialogVIRTUAL_BEGIN}}
  public:
    virtual void SetupWindow();
//{{ChkauthDialogVIRTUAL_END}}
};    //{{ChkauthDialog}}


#endif  // chkauthdialog_h sentry.


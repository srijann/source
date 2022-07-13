//----------------------------------------------------------------------------
//  Project LogSearch
//  Secure Payment Systems
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    LogSearch.apx Application
//  FILE:         posfiledialog.h
//  AUTHOR:       Jack Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for PosfileDialog (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(posfiledialog_h)              // Sentry, use file only if it's not already included.
#define posfiledialog_h

#include <owl/dialog.h>
#include <owl/static.h>

#include "posfiledialog.rh"            // Definition of all resources.


//{{TDialog = PosfileDialog}}
class PosfileDialog : public TDialog {
  TStatic*  BANKNUMBER;
  TStatic*  BANKACCOUNT;
  TStatic*  LICENSESTATE;
  TStatic*  LICENSE;
  TStatic*  CONSUMERNAME;
  TStatic*  BUSINESSNAME;
  TStatic*  OVERRIDESALLOWED;
  TStatic*  OVERRIDEDAYS;
  TStatic*  MAXOVERRIDEAMOUNT;
  TStatic*  MAXOVERRIDEACCUM;
  TStatic*  ACCOUNT1;
  TStatic*  ACCOUNT2;
  TStatic*  ACCOUNT3;
  TStatic*  SIC1;
  TStatic*  SIC2;
  TStatic*  SIC3;
  TStatic*  DATEADDED;
  TStatic*  DATELASTAPPROVAL;
  TStatic*  APPROVEDTRANSACTIONS;
  TStatic*  APPROVEDDOLLARS;
  TStatic*  DOB;
  TStatic*  PHONE;

  void DateToStatic(TStatic*,unsigned long);
  void NumToStatic(TStatic*,unsigned long,int Len=0);
  void clearAll();

  public:
    PosfileDialog(TWindow* parent);
    virtual ~PosfileDialog();
    void Display(IPCMessage*);    
};    //{{PosfileDialog}}


#endif  // posfiledialog_h sentry.


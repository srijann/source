//----------------------------------------------------------------------------
//  Project LogSearch
//  Secure Payment Systems
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    LogSearch.apx Application
//  FILE:         employerdialog.h
//  AUTHOR:       Jack Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for EmployerDialog (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(employerdialog_h)              // Sentry, use file only if it's not already included.
#define employerdialog_h

#include <owl/dialog.h>
#include <owl/static.h>

#include "EmployerDialog.rh"
#include "IPCTransfer.h"

//{{TDialog = EmployerDialog}}
class EmployerDialog : public TDialog {
  TStatic* HCPOSITION;
  TStatic* HCYEARSEMPLOYED;
  TStatic* HCSPOUSEPOSITION;
  TStatic* HCSPOUSEYEARSEMPLOYED;
  TStatic* HCSPOUSESSN;
  TStatic* HCSPOUSENAME;
  TStatic* HCSPOUSESALARY;
  TStatic* HCSPOUSEEMPLOYERPHONE;
  TStatic* HCSPOUSEEMPLOYERADR3;
  TStatic* HCSPOUSEEMPLOYERADR2;
  TStatic* HCSPOUSEEMPLOYERADR1;
  TStatic* HCSPOUSEEMPLOYERNAME;
  TStatic* HCSALARY;
  TStatic* HCEMPLOYERPHONE;
  TStatic* HCEMPLOYERADR3;
  TStatic* HCEMPLOYERADR2;
  TStatic* HCEMPLOYERADR1;
  TStatic* HCEMPLOYERNAME;
  TStatic* HCFINANCEAMOUNT;

  IPCTransfer Transfer;

  public:
    EmployerDialog(TWindow* parent);
    virtual ~EmployerDialog();
    void Display(IPCMessage*);
};    //{{EmployerDialog}}


#endif  // employerdialog_h sentry.


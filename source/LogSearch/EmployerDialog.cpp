//----------------------------------------------------------------------------
//  Project LogSearch
//  Secure Payment Systems
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    LogSearch.apx Application
//  FILE:         employerdialog.cpp
//  AUTHOR:       Jack Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of EmployerDialog (TDialog).
//
//----------------------------------------------------------------------------
#include "win4me.h"

#include "employerdialog.h"
#include "CheckAuthFN.h"


//{{EmployerDialog Implementation}}
#define STATIC(name) name=new TStatic(this,IDC_##name)


EmployerDialog::EmployerDialog(TWindow* parent)
:
    TDialog(parent, IDD_EMPLOYER)
{
  STATIC(HCPOSITION);
  STATIC(HCYEARSEMPLOYED);
  STATIC(HCSPOUSEPOSITION);
  STATIC(HCSPOUSEYEARSEMPLOYED);
  STATIC(HCSPOUSESSN);
  STATIC(HCSPOUSENAME);
  STATIC(HCSPOUSESALARY);
  STATIC(HCSPOUSEEMPLOYERPHONE);
  STATIC(HCSPOUSEEMPLOYERADR3);
  STATIC(HCSPOUSEEMPLOYERADR2);
  STATIC(HCSPOUSEEMPLOYERADR1);
  STATIC(HCSPOUSEEMPLOYERNAME);
  STATIC(HCSALARY);
  STATIC(HCEMPLOYERPHONE);
  STATIC(HCEMPLOYERADR3);
  STATIC(HCEMPLOYERADR2);
  STATIC(HCEMPLOYERADR1);
  STATIC(HCEMPLOYERNAME);
  STATIC(HCFINANCEAMOUNT);
}


EmployerDialog::~EmployerDialog()
{
  Destroy(IDCANCEL);

  // INSERT>> Your destructor code here.

}

#define OUTPUT(name) Transfer.ToCtl(FN_##name,HC##name)

void EmployerDialog::Display(IPCMessage* Msg)
{
 Transfer.SetIPCMsg(Msg);
  OUTPUT(POSITION);
  OUTPUT(YEARSEMPLOYED);
  OUTPUT(SPOUSEPOSITION);
  OUTPUT(SPOUSEYEARSEMPLOYED);
  OUTPUT(SPOUSESSN);
  OUTPUT(SPOUSENAME);
  OUTPUT(SPOUSESALARY);
  OUTPUT(SPOUSEEMPLOYERPHONE);
  OUTPUT(SPOUSEEMPLOYERADR3);
  OUTPUT(SPOUSEEMPLOYERADR2);
  OUTPUT(SPOUSEEMPLOYERADR1);
  OUTPUT(SPOUSEEMPLOYERNAME);
  OUTPUT(SALARY);
  OUTPUT(EMPLOYERPHONE);
  OUTPUT(EMPLOYERADR3);
  OUTPUT(EMPLOYERADR2);
  OUTPUT(EMPLOYERADR1);
  OUTPUT(EMPLOYERNAME);
  OUTPUT(FINANCEAMOUNT);
}


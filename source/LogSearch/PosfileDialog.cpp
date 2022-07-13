//----------------------------------------------------------------------------
//  Project LogSearch
//  Secure Payment Systems
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    LogSearch.apx Application
//  FILE:         posfiledialog.cpp
//  AUTHOR:       Jack Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of PosfileDialog (TDialog).
//
//----------------------------------------------------------------------------
#include "win4me.h"

#include "posfiledialog.h"
#include "checkauthfn.h"
#include <time.h>
#include <stdio.h>
#define NOMACROS
#include "posfile.h"

//{{PosfileDialog Implementation}}

#define STATIC(name) name = new TStatic(this,IDC_PF##name)

PosfileDialog::PosfileDialog(TWindow* parent)
:
 TDialog(parent, IDD_POSFILE )
{
 STATIC(BANKNUMBER);
 STATIC(BANKACCOUNT);
 STATIC(LICENSESTATE);
 STATIC(LICENSE);
 STATIC(CONSUMERNAME);
 STATIC(BUSINESSNAME);
 STATIC(OVERRIDESALLOWED);
 STATIC(OVERRIDEDAYS);
 STATIC(MAXOVERRIDEAMOUNT);
 STATIC(MAXOVERRIDEACCUM);
 STATIC(ACCOUNT1);
 STATIC(ACCOUNT2);
 STATIC(ACCOUNT3);
 STATIC(SIC1);
 STATIC(SIC2);
 STATIC(SIC3);
 STATIC(DATEADDED);
 STATIC(DATELASTAPPROVAL);
 STATIC(APPROVEDTRANSACTIONS);
 STATIC(APPROVEDDOLLARS);
 STATIC(DOB);
 STATIC(PHONE);
}


PosfileDialog::~PosfileDialog()
{
  Destroy(IDCANCEL);
}

//-----------------
// Clear All fields
//-----------------
void PosfileDialog::clearAll()
{
 BANKNUMBER->Clear();
 BANKACCOUNT->Clear();
 LICENSESTATE->Clear();
 LICENSE->Clear();
 CONSUMERNAME->Clear();
 BUSINESSNAME->Clear();
 OVERRIDESALLOWED->Clear();
 OVERRIDEDAYS->Clear();
 MAXOVERRIDEAMOUNT->Clear();
 MAXOVERRIDEACCUM->Clear();
 ACCOUNT1->Clear();
 ACCOUNT2->Clear();
 ACCOUNT3->Clear();
 SIC1->Clear();
 SIC2->Clear();
 SIC3->Clear();
 DATEADDED->Clear();
 DATELASTAPPROVAL->Clear();
 APPROVEDTRANSACTIONS->Clear();
 APPROVEDDOLLARS->Clear();
 DOB->Clear();
 PHONE->Clear();
}

//--------------------------------------
// Send date to control in form mm/dd/yy
//--------------------------------------
void PosfileDialog::DateToStatic(TStatic* Ctl,unsigned long Ts)
{
 struct tm* t=localtime( (time_t*)&Ts);
 char Buf[20];

 sprintf(Buf,"%02d/%02d/%02d",t->tm_mon+1,t->tm_mday,t->tm_year%100);
 Ctl->Transfer(Buf,tdSetData);
}

//---------------------------
// Transfer control to binary
//---------------------------
void PosfileDialog::NumToStatic(TStatic* Ctl,unsigned long Num,int Len)
{
 char Buf[20];

 if ( Len )
  sprintf(Buf,"%0*d",Len,Num);
 else
  sprintf(Buf,"%d",Num);
 Ctl->Transfer(Buf,tdSetData);
}


//---------------------------
// Display the Current Record
//---------------------------
#define TEXTOUT(name,fldname) name->Transfer(Rec->fldname,tdSetData)
#define NUMOUT(name,fldname) NumToStatic(name,Rec->fldname)
#define FNUMOUT(name,fldname,len) NumToStatic(name,Rec->fldname,len)
void PosfileDialog::Display(IPCMessage* Msg)
{
 PosfileRecord* Rec= (PosfileRecord*)Msg->GetFldPtr(FN_POSITIVEFILERECORD);
 clearAll();

 if ( Rec==NULL )
  return;

 TEXTOUT(BANKNUMBER,BankNumber);
 TEXTOUT(BANKACCOUNT,BankAccount);
 TEXTOUT(LICENSESTATE,LicenseState);
 TEXTOUT(LICENSE,License);
 if ( Rec->DateOfBirth )
  FNUMOUT(DOB,DateOfBirth,6);
 DateToStatic(DATEADDED,Rec->FirstApproval);

 if ( Rec->LastApproval )
   DateToStatic(DATELASTAPPROVAL,Rec->LastApproval);

 NUMOUT(APPROVEDTRANSACTIONS,NumApprovals);
 NUMOUT(APPROVEDDOLLARS,AmountApprovals);

 TEXTOUT(CONSUMERNAME,ConsumerInfo.ConsumerName);
 TEXTOUT(BUSINESSNAME,ConsumerInfo.BusinessName);
 TEXTOUT(PHONE,PhoneKey);

 if ( Rec->Parms.OverridePeriod )
  {
   TEXTOUT(ACCOUNT1,Parms.AccountRestrictions[0]);
   TEXTOUT(ACCOUNT2,Parms.AccountRestrictions[1]);
   TEXTOUT(ACCOUNT3,Parms.AccountRestrictions[2]);
   NUMOUT(OVERRIDESALLOWED,Parms.NumberOfOverridesAllowed);
   NUMOUT(OVERRIDEDAYS,Parms.OverridePeriod);
   NUMOUT(MAXOVERRIDEAMOUNT,Parms.MaxOverrideAmount);
   NUMOUT(MAXOVERRIDEACCUM,Parms.MaxOverrideAccum);
   if ( Rec->Parms.SicCodeRestrictions[0] )
     NUMOUT(SIC1,Parms.SicCodeRestrictions[0]);
   if ( Rec->Parms.SicCodeRestrictions[1] )
     NUMOUT(SIC2,Parms.SicCodeRestrictions[1]);
   if ( Rec->Parms.SicCodeRestrictions[2] )
     NUMOUT(SIC3,Parms.SicCodeRestrictions[2]);
  }

}


//----------------------------------------------------------------------------
//  Project LogSearch
//  Secure Payment Systems
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    LogSearch.apx Application
//  FILE:         holdcheckdialog.cpp
//  AUTHOR:       Jack Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of HoldCheckDialog (TDialog).
//
//----------------------------------------------------------------------------
#include "win4me.h"

#include <stdio.h>
#include "holdcheckdialog.h"
#include "CheckAuthFn.h"


//{{HoldCheckDialog Implementation}}

#define STATIC(name) name=new TStatic(this,IDC_##name)

HoldCheckDialog::HoldCheckDialog(TWindow* parent)
:
    TDialog(parent, IDD_HOLDCHECK)
{
 STATIC(HCVEHICLEVALUE);
 STATIC(HCAMOUNT1);
 STATIC(HCCHECKNUM1);
 STATIC(HCAPPNUM4);
 STATIC(HCAPPNUM3);
 STATIC(HCAPPNUM2);
 STATIC(HCAPPNUM1);
 STATIC(HCAMOUNT2);
 STATIC(HCCHECKNUM2);
 STATIC(HCAMOUNT3);
 STATIC(HCCHECKNUM3);
 STATIC(HCAMOUNT4);
 STATIC(HCCHECKNUM4);
 STATIC(HCSUM);
}


HoldCheckDialog::~HoldCheckDialog()
{
  Destroy(IDCANCEL);

  // INSERT>> Your destructor code here.

}

void HoldCheckDialog::Display(IPCMessage* Msg)
{
 DWORD Sum;
 char fmtBuf[10];
 Transfer.SetIPCMsg(Msg);
 Transfer.ToCtl(FN_VEHICLEVALUE,HCVEHICLEVALUE);
 Transfer.ToCtl(FN_HOLDCHECKCHECKNUM1,HCCHECKNUM1);
 Transfer.ToCtl(FN_HOLDCHECKCHECKNUM2,HCCHECKNUM2);
 Transfer.ToCtl(FN_HOLDCHECKCHECKNUM3,HCCHECKNUM3);
 Transfer.ToCtl(FN_HOLDCHECKCHECKNUM4,HCCHECKNUM4);
 Transfer.ToCtl(FN_HOLDCHECKAMOUNT1,HCAMOUNT1);
 Transfer.ToCtl(FN_HOLDCHECKAMOUNT2,HCAMOUNT2);
 Transfer.ToCtl(FN_HOLDCHECKAMOUNT3,HCAMOUNT3);
 Transfer.ToCtl(FN_HOLDCHECKAMOUNT4,HCAMOUNT4);
 Transfer.ToCtl(FN_HOLDCHECKAPPROVALNUM1,HCAPPNUM1);
 Transfer.ToCtl(FN_HOLDCHECKAPPROVALNUM2,HCAPPNUM2);
 Transfer.ToCtl(FN_HOLDCHECKAPPROVALNUM3,HCAPPNUM3);
 Transfer.ToCtl(FN_HOLDCHECKAPPROVALNUM4,HCAPPNUM4);
 Sum=getAmount(Msg,FN_HOLDCHECKAMOUNT1) +
     getAmount(Msg,FN_HOLDCHECKAMOUNT2) +
     getAmount(Msg,FN_HOLDCHECKAMOUNT3) +
     getAmount(Msg,FN_HOLDCHECKAMOUNT4);
 if ( Sum )
  {
   sprintf(fmtBuf,"%d.%02d",Sum/100,Sum%100);
   HCSUM->Transfer(fmtBuf,tdSetData);
  }
 else
  HCSUM->Clear();

}

//---------------------------------------------------------------
// Get Amount from edit control as an integer value representing
// cents.
// Return 0 if no value
//---------------------------------------------------------------
uint HoldCheckDialog::getAmount(IPCMessage* Msg,FLDNUM Fn)
{
 char Buf[10];
 uint Amount;
 char* dpt;

 if ( ! Msg->GetFld(Fn,sizeof(Buf),Buf) )
  return 0;
 if ( ! Buf[0] )
  return 0;

 Amount = atoi(Buf) * 100;
 if ( (dpt=strchr(Buf,'.')) != NULL )
   Amount += atoi(&dpt[1]);

 return Amount;
}



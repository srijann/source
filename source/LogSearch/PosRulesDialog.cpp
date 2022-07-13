//----------------------------------------------------------------------------
//  Project LogSearch
//  Secure Payment Systems
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    LogSearch.apx Application
//  FILE:         posrulesdialog.cpp
//  AUTHOR:       Jack Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of PosRulesDialog (TDialog).
//
//----------------------------------------------------------------------------
#include "win4me.h"

#include "posrulesdialog.h"
#include <stdio.h>
#include "unsnum.h"
#include "Spsmandb.h"
#include "CheckAuthfn.h"

//{{PosRulesDialog Implementation}}

#define DEFROW(n) Row[n-1].AMT = new TStatic(this,IDC_POSLIMITAMT##n,5);\
                   for (i=1; i<=NUMCOLS; ++i)\
                     Row[n-1].MULTI[i-1] = new TStatic(this,IDC_POSLIMITAMT##n +i)


PosRulesDialog::PosRulesDialog(TWindow* parent)
:
    TDialog(parent, IDD_POSFILELIMITS )
{
 int i;
 
 POSMAXOVERRIDES=new TStatic(this,IDC_POSMAXOVERRIDES);
 POSPERIOD=new TStatic(this,IDC_POSPERIOD);
 WAITBEFOREPOSITIVE = new TStatic(this,IDC_WAITBEFOREPOSITIVE);
 MULTIPLYNUMAPPROVED = new TStatic(this,IDC_MULTIPLYNUMAPPROVED);
 MULTIPLYDOLLARSAPPROVED = new TStatic(this,IDC_MULTIPLYDOLLARSAPPROVED);
 MULTIPLYMONTHS = new TStatic(this,IDC_MULTIPLYMONTHS);
 OVERRIDEACCUM = new TStatic(this,IDC_OVERRIDEACCUM);

 DEFROW(1);
 DEFROW(2);
 DEFROW(3);
 DEFROW(4);
 DEFROW(5);
 DEFROW(6);
 DEFROW(7);
 DEFROW(8);
 DEFROW(9);
}


PosRulesDialog::~PosRulesDialog()
{
  Destroy(IDCANCEL);

  // INSERT>> Your destructor code here.

}

#define NUMOUT(ctl,val) if ( isEmpty(val) )\
                          ctl->Clear();\
                        else\
                          NumOut(ctl,val)
void PosRulesDialog::Display(IPCMessage* Msg)
{
 PositiveRulesRecord* Rec=
          (PositiveRulesRecord *)Msg->GetFldPtr(FN_POSITIVERULESRECORD);
 int r,c;

 if ( Rec==NULL )
  {
   POSMAXOVERRIDES->Clear();
   POSPERIOD->Clear();
   WAITBEFOREPOSITIVE->Clear();
   MULTIPLYNUMAPPROVED->Clear();
   MULTIPLYDOLLARSAPPROVED->Clear();
   MULTIPLYMONTHS->Clear();
   OVERRIDEACCUM->Clear();
   for (r=0; r<NUMROWS; ++r )
    {
     Row[r].AMT->Clear();
     for (c=0; c<NUMCOLS; ++c)
       Row[r].MULTI[c]->Clear();
    }
   return;
  }

 NUMOUT(POSMAXOVERRIDES,Rec->MaxOverrides);
 NUMOUT(POSPERIOD,Rec->OverridePeriod);
 NUMOUT(WAITBEFOREPOSITIVE,Rec->WaitForPositiveDays);
 NUMOUT(MULTIPLYNUMAPPROVED,Rec->MultiplyIfNumApprovals);
 NUMOUT(MULTIPLYDOLLARSAPPROVED,Rec->MultiplyIfAmountApprovals);
 NUMOUT(MULTIPLYMONTHS,Rec->MultiplyIfLastWithinMonths);
 NUMOUT(OVERRIDEACCUM,Rec->MaxOverrideAccum);

 for (r=0; r<NUMROWS; ++r )
  {
   NUMOUT(Row[r].AMT,Rec->AgeTbl[r].Limit);
   for (c=0; c<NUMCOLS; ++c)
    multiplierOut(Row[r].MULTI[c],Rec->AgeTbl[r].Multiplier[c][0]);
  }
}

//--------------------------------------
// Transfer a number to a static control
//--------------------------------------
void PosRulesDialog::NumOut(TStatic* Ctl,unsigned long Val)
{
 char Buf[20];
 sprintf(Buf,"%d",Val);
 Ctl->Transfer(Buf,tdSetData);
}

//---------------------------------------------
// Transfer a multiplier value to an edit field
//---------------------------------------------
void PosRulesDialog::multiplierOut(TStatic* Ctl,unsigned char Val)
{
 char Buf[10];

 if ( isEmpty(Val) )
  Buf[0] = 0;
 else
  sprintf(Buf,"%d.%0d",Val/10,Val%10);
 Ctl->Transfer(Buf,tdSetData);
}


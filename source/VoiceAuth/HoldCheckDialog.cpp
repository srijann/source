//----------------------------------------------------------------------------
//  Project VoiceAuth
//
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    VoiceAuth.apx Application
//  FILE:         holdcheckdialog.cpp
//  AUTHOR:
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of HoldCheckDialog (TDialog).
//
//----------------------------------------------------------------------------
#include "win4me.h"

#include <stdio.h>
#include "holdcheckdialog.h"
#include "AppNotify.h"
#include "Chkauth.h"
#include "decimalvalidator.h"
#include "Msgid.h"
#include "CheckAuthFn.h"
#include "UnsNum.h"

//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(HoldCheckDialog, TDialog)
//{{HoldCheckDialogRSP_TBL_BEGIN}}
  EV_BN_CLICKED(IDC_COMPLETEBTN,BNOkayClicked),
  EV_EN_KILLFOCUS(IDC_HCVEHICLEVALUE, ENKillfocusVehicleValue),
  EV_EN_KILLFOCUS(IDC_HCAMOUNT1, ENKillfocusAmount1),
  EV_EN_KILLFOCUS(IDC_HCAMOUNT2, ENKillfocusAmount2),
  EV_EN_KILLFOCUS(IDC_HCAMOUNT3, ENKillfocusAmount3),
  EV_EN_KILLFOCUS(IDC_HCAMOUNT4, ENKillfocusAmount4),
  EV_BN_CLICKED(IDC_CHECK1, BNClicked1),
  EV_BN_CLICKED(IDC_CHECK2, BNClicked2),
  EV_BN_CLICKED(IDC_CHECK3, BNClicked3),
//{{HoldCheckDialogRSP_TBL_END}}
END_RESPONSE_TABLE;

//{{HoldCheckDialog Implementation}}

#define NUMCONTROLS 40

#define EDITCTL(n,len) DCMan->Add(IDC_HC##n,len,FN_##n,MID_HC##n)
#define EDITAMT(n,fn) DCMan->Add(IDC_HC##n,10,fn,MID_HC##n)->\
   SetValidator(new TDecimalValidator(6,2))
#define EDITCKNUM(n,fn) DCMan->Add(IDC_HC##n,7,fn,MID_HC##n)

HoldCheckDialog::HoldCheckDialog(TWindow* parent,CheckAuth& Msg,TMsgWin* MsgWin)
:
 TDialog(parent, IDD_HOLDCHECK),
 authMsg(Msg),
 displayMode(false),
 msgWin(MsgWin)
{
  DCMan = new DialogControlManager(this,NUMCONTROLS,
               (IPCMessage*)&Msg,MsgWin);

  EDITAMT(AMOUNT4,FN_HOLDCHECKAMOUNT4);
  EDITAMT(AMOUNT3,FN_HOLDCHECKAMOUNT3);
  EDITAMT(AMOUNT2,FN_HOLDCHECKAMOUNT2);
  EDITAMT(AMOUNT1,FN_HOLDCHECKAMOUNT1);
  EDITCKNUM(CHECKNUM1,FN_HOLDCHECKCHECKNUM1);
  EDITCKNUM(CHECKNUM2,FN_HOLDCHECKCHECKNUM2);
  EDITCKNUM(CHECKNUM3,FN_HOLDCHECKCHECKNUM3);
  EDITCKNUM(CHECKNUM4,FN_HOLDCHECKCHECKNUM4);
  DCMan->Add(IDC_HCVEHICLEVALUE,10,FN_VEHICLEVALUE,MID_HCVEHICLEVALUE)->
    SetValidator(new TDecimalValidator(6,2));
  HCMAXTEXT = new TStatic(this,IDC_HCMAXTEXT);
  HCMAX = new TEdit(this,IDC_HCMAX,10);
  HCSUM = new TEdit(this,IDC_HCSUM,10);
  HCMIN1 = new TEdit(this,IDC_HCMIN1,10);
  HCMINTEXT = new TEdit(this,IDC_HCMINTEXT);
  DCMan->AddButton(IDC_COMPLETEBTN,MID_ENTERTOCOMPLETE);
  ISAPPROVAL = new TStatic(this,IS_APPROVED);
  CHECKBOX[0] = new TCheckBox(this,IDC_CHECK1);
  CHECKBOX[1] = new TCheckBox(this,IDC_CHECK2);
  CHECKBOX[2] = new TCheckBox(this,IDC_CHECK3);
  memset(&origInfo,0,sizeof(origInfo));

  // If Amount/Checknum/Approval Num exist, consider them number 1
  authMsg.CopyFld(FN_HOLDCHECKAMOUNT1,FN_AMOUNT);
  authMsg.CopyFld(FN_HOLDCHECKCHECKNUM1,FN_CHECKNUMBER);
  authMsg.CopyFld(FN_HOLDCHECKAPPROVALNUM1,FN_APPROVALNUM);

  // Save Original Info
  authMsg.GetFld(FN_HOLDCHECKAMOUNT1,sizeof(origInfo[0].Amount),origInfo[0].Amount);
  authMsg.GetFld(FN_HOLDCHECKCHECKNUM1,sizeof(origInfo[0].CheckNumber),origInfo[0].CheckNumber);
  authMsg.GetFld(FN_HOLDCHECKAMOUNT2,sizeof(origInfo[1].Amount),origInfo[1].Amount);
  authMsg.GetFld(FN_HOLDCHECKCHECKNUM2,sizeof(origInfo[1].CheckNumber),origInfo[1].CheckNumber);
  authMsg.GetFld(FN_HOLDCHECKAMOUNT3,sizeof(origInfo[2].Amount),origInfo[2].Amount);
  authMsg.GetFld(FN_HOLDCHECKCHECKNUM3,sizeof(origInfo[2].CheckNumber),origInfo[2].CheckNumber);
  authMsg.GetFld(FN_HOLDCHECKAMOUNT4,sizeof(origInfo[3].Amount),origInfo[3].Amount);
  authMsg.GetFld(FN_HOLDCHECKCHECKNUM4,sizeof(origInfo[3].CheckNumber),origInfo[3].CheckNumber);
}

HoldCheckDialog::~HoldCheckDialog()
{
  Destroy(IDCANCEL);
  delete DCMan;
}

//---------------------------------
// Prohibit ESCAPE from this window
//---------------------------------
TResult HoldCheckDialog::EvCommand(uint id, THandle hWndCtl, uint notifyCode)
{
 TResult result;
  switch( notifyCode )
   {
    case 0:
       if ( id==IDCANCEL )
        {
         // If Message Window has a buddy (special handling message)
         // we want to toggle it's position when Esc is hit
         msgWin->Move();
         return 0;
        }
       // Treat Enter key as Tab unless focus is on Button or there
       // are errors on the screen
       if ( id==IDOK )
        {
         if ( editData() )
           if ( GetFocus() != DCMan->ButtonPtr(IDC_COMPLETEBTN)->GetHandle() )
             SetControlFocus((THow)WS_TABSTOP);
         return 0;
        }
      break;

    case EN_SETFOCUS:
    case CBN_SETFOCUS:
      if ( ! displayMode )
        DCMan->SetFocus((int)id);
      else
        return 0;  
      break;

    default:
      break;
   }

  result = TDialog::EvCommand(id, hWndCtl, notifyCode);
  return result;
}

//---------------------
// Handle Click of Okay
//---------------------
void HoldCheckDialog::BNOkayClicked()
{
 int i;
 char SourceTypes[5];

 if ( ! editData() )
  return;


 DCMan->SetAllReadOnly();
 DCMan->ButtonPtr(IDC_COMPLETEBTN)->ShowWindow(SW_HIDE);
 for (i=0; i<3; ++i)
  CHECKBOX[i]->EnableWindow(false);

 // Select first unchecked item for AMOUNT and CHECK NUMBER.
 if ( CHECKBOX[0]->GetCheck() != BF_CHECKED &&
      authMsg.GetFld(FN_HOLDCHECKAMOUNT1) )
  {
   authMsg.CopyFld(FN_AMOUNT,FN_HOLDCHECKAMOUNT1);
   authMsg.CopyFld(FN_CHECKNUMBER,FN_HOLDCHECKCHECKNUM1);
  }
 else
 if ( CHECKBOX[1]->GetCheck() != BF_CHECKED &&
      authMsg.GetFld(FN_HOLDCHECKAMOUNT2) )

  {
   authMsg.CopyFld(FN_AMOUNT,FN_HOLDCHECKAMOUNT2);
   authMsg.CopyFld(FN_CHECKNUMBER,FN_HOLDCHECKCHECKNUM2);
  }
 else
 if ( CHECKBOX[2]->GetCheck() != BF_CHECKED &&
      authMsg.GetFld(FN_HOLDCHECKAMOUNT3) )
  {
   authMsg.CopyFld(FN_AMOUNT,FN_HOLDCHECKAMOUNT3);
   authMsg.CopyFld(FN_CHECKNUMBER,FN_HOLDCHECKCHECKNUM3);
  }
 else
  {
   authMsg.CopyFld(FN_AMOUNT,FN_HOLDCHECKAMOUNT4);
   authMsg.CopyFld(FN_CHECKNUMBER,FN_HOLDCHECKCHECKNUM4);
  }

 // Delete approval numbers for any items that were unchecked
 if ( origInfo[0].Amount[0] &&
      CHECKBOX[0]->GetCheck() != BF_CHECKED )
   authMsg.DelFld(FN_HOLDCHECKAPPROVALNUM1);
 if ( origInfo[1].Amount[0] &&
      CHECKBOX[1]->GetCheck() != BF_CHECKED )
   authMsg.DelFld(FN_HOLDCHECKAPPROVALNUM2);
 if ( origInfo[2].Amount[0] &&
      CHECKBOX[2]->GetCheck() != BF_CHECKED )
  authMsg.DelFld(FN_HOLDCHECKAPPROVALNUM3);

 // Record which items were voice and which were POS (checked items
 // were POS).
 strcpy(SourceTypes,"VVVV");
 if ( CHECKBOX[0]->GetCheck() == BF_CHECKED )
  SourceTypes[0] = 'P';
 if ( CHECKBOX[1]->GetCheck() == BF_CHECKED )
  SourceTypes[1] = 'P';
 if ( CHECKBOX[2]->GetCheck() == BF_CHECKED )
  SourceTypes[2] = 'P';
 authMsg.PutFld(FN_HOLDCHECKSOURCETYPES,SourceTypes); 

 AppNotifyMsg(Parent,NOTIFY_DIALOGCOMPLETE,0);
 ShowWindow(SW_HIDE);
 displayMode=true;
}

void HoldCheckDialog::SetupWindow()
{
 char Buf[25];
 bool showApproval=false;

 TDialog::SetupWindow();
 computeMaxSum();
 sprintf(Buf,"(%d%%=)",authMsg.merchantRec.Data.HoldCheck1Percent);
 HCMINTEXT->Transfer(Buf,tdSetData);
 DCMan->CopyAllFromMsg();

 if ( authMsg.GetFld(FN_HOLDCHECKAPPROVALNUM1) )
  {
   showApproval=true;
   CHECKBOX[0]->ShowWindow(SW_SHOW);
   CHECKBOX[0]->Check();
   DCMan->EditPtr(FN_HOLDCHECKAMOUNT1)->SetReadOnly(true);
   DCMan->EditPtr(FN_HOLDCHECKCHECKNUM1)->SetReadOnly(true);
  }
 if ( authMsg.GetFld(FN_HOLDCHECKAPPROVALNUM2) )
  {
   showApproval=true;
   CHECKBOX[1]->ShowWindow(SW_SHOW);
   CHECKBOX[1]->Check();
   DCMan->EditPtr(FN_HOLDCHECKAMOUNT2)->SetReadOnly(true);
   DCMan->EditPtr(FN_HOLDCHECKCHECKNUM2)->SetReadOnly(true);
  }
 if ( authMsg.GetFld(FN_HOLDCHECKAPPROVALNUM3) )
  {
   showApproval=true;
   CHECKBOX[2]->ShowWindow(SW_SHOW);
   CHECKBOX[2]->Check();
   DCMan->EditPtr(FN_HOLDCHECKAMOUNT3)->SetReadOnly(true);
   DCMan->EditPtr(FN_HOLDCHECKCHECKNUM3)->SetReadOnly(true);
  }

 if ( showApproval )
  ISAPPROVAL->ShowWindow(SW_SHOW);

 sumAmounts();
 computeMaxSum();
}

//---------------------------------------------
// Adjust Amount to include cents, if necessary
//---------------------------------------------
void HoldCheckDialog::adjustAmount(int Id)
{
 char Buf[25];
 TEdit* Ptr=DCMan->EditPtr(Id);

 if ( Ptr==NULL )
  return;

 Ptr->Transfer(Buf,tdGetData);
 if ( Buf[0] && strchr(Buf,'.') == NULL )
  {
   strcat(Buf,".00");
   Ptr->Transfer(Buf,tdSetData);
  }
}

//---------------------------------------------------------------
// Get Amount from edit control as an integer value representing
// cents.
// Return 0 if no value
//---------------------------------------------------------------
uint HoldCheckDialog::getAmount(TEdit* AmountCtl)
{
 char Buf[10];
 uint Amount;
 char* dpt;

 AmountCtl->Transfer(Buf,tdGetData);
 if ( ! Buf[0] )
  return 0;

 Amount = atoi(Buf) * 100;
 if ( (dpt=strchr(Buf,'.')) != NULL )
   Amount += atoi(&dpt[1]);

 return Amount;
}

//------------------------------------
// Compute and Display the Maximum Sum
//------------------------------------
void HoldCheckDialog::computeMaxSum()
{
 uint VehicleValue;
 char Buf[20];

 if ( (VehicleValue=getAmount(DCMan->EditPtr(IDC_HCVEHICLEVALUE))) != 0 &&
      ! isEmpty(authMsg.merchantRec.Data.HoldCheckValuePercent) )
  {
   MaxSum=VehicleValue * authMsg.merchantRec.Data.HoldCheckValuePercent / 100;
   if ( MaxSum < (uint) authMsg.merchantRec.Data.HoldCheckAmount * 100 )
    {
     sprintf(Buf,"(%d%%=)",authMsg.merchantRec.Data.HoldCheckValuePercent);
     HCMAXTEXT->Transfer(Buf,tdSetData);
     sprintf(Buf,"%d.%02d",MaxSum/100,MaxSum%100);
     HCMAX->Transfer(Buf,tdSetData);
     return;
    }
  }


 MaxSum=authMsg.merchantRec.Data.HoldCheckAmount * 100;
 sprintf(Buf,"%d.00",authMsg.merchantRec.Data.HoldCheckAmount);
 HCMAX->Transfer(Buf,tdSetData);
 HCMAXTEXT->Transfer("(max=)",tdSetData);
}

void HoldCheckDialog::ENKillfocusAmount1()
{
 adjustAmounts();
}

void HoldCheckDialog::ENKillfocusAmount2()
{
 adjustAmounts();
}


void HoldCheckDialog::ENKillfocusAmount3()
{
 adjustAmounts();
}

void HoldCheckDialog::ENKillfocusAmount4()
{
 adjustAmounts();
}

void HoldCheckDialog::ENKillfocusVehicleValue()
{
 adjustAmounts();
}

//---------------------------------
// Sum and Display Amounts
// Return whether or not it's valid
//---------------------------------
bool HoldCheckDialog::sumAmounts()
{
 char Buf[20];

 uint sum=getAmount(DCMan->EditPtr(IDC_HCAMOUNT1))+
          getAmount(DCMan->EditPtr(IDC_HCAMOUNT2))+
          getAmount(DCMan->EditPtr(IDC_HCAMOUNT3))+
          getAmount(DCMan->EditPtr(IDC_HCAMOUNT4));

 if ( sum==0 )
  {
   HCSUM->Transfer("",tdSetData);
   HCMIN1->Transfer("",tdSetData);
   return true;
  }

 sprintf(Buf,"%d.%02d",sum/100,sum%100);
 HCSUM->Transfer(Buf,tdSetData);
 MinAmount1=sum*authMsg.merchantRec.Data.HoldCheck1Percent/100;
 sprintf(Buf,"%d.%02d",MinAmount1/100,MinAmount1%100);
 HCMIN1->Transfer(Buf,tdSetData);
 return (sum <=MaxSum);
}

//------------------
// Edit all the Data
//------------------
bool HoldCheckDialog::editData()
{
 uint  Amount1, Amount2, Amount3, Amount4;
 DWORD Checknum1, Checknum2, Checknum3, Checknum4;

 adjustAmounts();
 DCMan->CopyAllToMsg();

 // Vehicle Value Required
 if ( ! authMsg.GetFld(FN_VEHICLEVALUE) )
  {
   DCMan->SetFocus(IDC_HCVEHICLEVALUE);
   return false;
  }
  
 // At least 1 Amount is required
 if ( ! authMsg.GetFld(FN_HOLDCHECKAMOUNT1) )
  {
   DCMan->SetFocus(IDC_HCAMOUNT1);
   return false;
  }


 // Make sure 1st amount is big enough
 if ( (Amount1=getAmount(DCMan->EditPtr(IDC_HCAMOUNT1))) < MinAmount1 )
  {
   DCMan->SetFocus(IDC_HCAMOUNT1,MID_HCAMOUNT1TOOLOW);
   DCMan->EditPtr(IDC_HCAMOUNT1)->SetSelection(0,-1);
   return false;
  }

 // 05/11/97 -- Amounts don't have to be unique any more
 Amount2=getAmount(DCMan->EditPtr(IDC_HCAMOUNT2));
 Amount3=getAmount(DCMan->EditPtr(IDC_HCAMOUNT3));
 Amount4=getAmount(DCMan->EditPtr(IDC_HCAMOUNT4));

// 05/11/97 Amounts don't need to be unique any more
#ifdef UNIQUE_AMOUNTS
 // Amount 2 must be unique
 if ( Amount2 != 0 &&
      ( Amount2==Amount1 ||
        Amount2==Amount3 ||
        Amount2==Amount4 ) )
  {
   DCMan->SetFocus(IDC_HCAMOUNT2,MID_HCAMOUNTNOTUNIQUE);
   DCMan->EditPtr(IDC_HCAMOUNT2)->SetSelection(0,-1);
   return false;
  }

 // Amount 3 must be unique
 if ( Amount3 != 0 &&
      ( Amount3==Amount1 ||
        Amount3==Amount2 ||
        Amount3==Amount4 ) )
  {
   DCMan->SetFocus(IDC_HCAMOUNT3,MID_HCAMOUNTNOTUNIQUE);
   DCMan->EditPtr(IDC_HCAMOUNT3)->SetSelection(0,-1);
   return false;
  }

 // Amount 4 must be unique
 if ( Amount4 != 0 &&
      ( Amount4==Amount1 ||
        Amount4==Amount2 ||
        Amount4==Amount3 ) )
  {
   DCMan->SetFocus(IDC_HCAMOUNT4,MID_HCAMOUNTNOTUNIQUE);
   DCMan->EditPtr(IDC_HCAMOUNT4)->SetSelection(0,-1);
   return false;
  }
#endif

 // Sum must not exceed max
 if ( Amount1 + Amount2 + Amount3 + Amount4 > MaxSum )
  {
   int id;
   if ( Amount1 > MaxSum )
    id=IDC_HCAMOUNT1;
   else
   if ( Amount1 + Amount2 > MaxSum )
    id=IDC_HCAMOUNT2;
   else
   if ( Amount1 + Amount2 + Amount3 > MaxSum )
    id=IDC_HCAMOUNT3;
   else
    id=IDC_HCAMOUNT4;
   DCMan->SetFocus(id,MID_HCSUMTOOHIGH);
   DCMan->EditPtr(id)->SetSelection(0,-1);
   return false;
  }

 if ( ! authMsg.GetFld(FN_HOLDCHECKCHECKNUM1,Checknum1) )
  Checknum1=0;
 if ( ! authMsg.GetFld(FN_HOLDCHECKCHECKNUM2,Checknum2) )
  Checknum2=0;
 if ( ! authMsg.GetFld(FN_HOLDCHECKCHECKNUM3,Checknum3) )
  Checknum3=0;
 if ( ! authMsg.GetFld(FN_HOLDCHECKCHECKNUM4,Checknum4) )
  Checknum4=0;

 // Each Amount entered must have a Check Number
 if ( ! Checknum1 )
  {
   DCMan->SetFocus(FN_HOLDCHECKCHECKNUM1);
   return false;
  }

 if ( Amount2 )
  {
   if ( ! Checknum2 )
    {
     DCMan->SetFocus(FN_HOLDCHECKCHECKNUM2);
     return false;
    }
   if ( Checknum2 == Checknum1 ||
        Checknum2 == Checknum3 ||
        Checknum2 == Checknum4 )
    {
     DCMan->SetFocus(FN_HOLDCHECKCHECKNUM2,MID_CHECKNUMNOTUNIQUE);
     return false;
    }
  }

 if ( Amount3 )
  {
   if ( ! Checknum3 )
    {
     DCMan->SetFocus(FN_HOLDCHECKCHECKNUM3);
     return false;
    }
   if ( Checknum3 == Checknum1 ||
        Checknum3 == Checknum2 ||
        Checknum3 == Checknum4 )
    {
     DCMan->SetFocus(FN_HOLDCHECKCHECKNUM3,MID_CHECKNUMNOTUNIQUE);
     return false;
    }
  }

 if ( Amount4 )
  {
   if ( ! Checknum4 )
    {
     DCMan->SetFocus(FN_HOLDCHECKCHECKNUM4);
     return false;
    }
   if ( Checknum4 == Checknum1 ||
        Checknum4 == Checknum2 ||
        Checknum4 == Checknum3 )
    {
     DCMan->SetFocus(FN_HOLDCHECKCHECKNUM4,MID_CHECKNUMNOTUNIQUE);
     return false;
    }
  }


 return true;
}

void HoldCheckDialog::adjustAmounts()
{
 adjustAmount(IDC_HCAMOUNT1);
 adjustAmount(IDC_HCAMOUNT2);
 adjustAmount(IDC_HCAMOUNT3);
 adjustAmount(IDC_HCAMOUNT4);
 adjustAmount(IDC_HCVEHICLEVALUE);
 computeMaxSum();
 sumAmounts();
}

void HoldCheckDialog::handleApprovedButton(int n,FLDNUM amountFN,
                                             FLDNUM checknumFN)
{
 char compBuf[15];

 if ( CHECKBOX[n]->GetCheck() == BF_CHECKED )
  {
   DCMan->EditPtr(amountFN)->SetReadOnly(false);
   DCMan->EditPtr(checknumFN)->SetReadOnly(false);
   DCMan->SetFocus(amountFN);
   DCMan->EditPtr(amountFN)->SetSelection(0,-1);
   CHECKBOX[n]->SetCheck(BF_UNCHECKED);
  }
 else
  {
   DCMan->EditPtr(amountFN)->Transfer(compBuf,tdGetData);
   if ( strcmp(origInfo[n].Amount,compBuf) )
     return;
   DCMan->EditPtr(checknumFN)->Transfer(compBuf,tdGetData);
   if ( strcmp(origInfo[n].CheckNumber,compBuf) )
     return;
   CHECKBOX[n]->Check();
   DCMan->EditPtr(amountFN)->SetReadOnly(true);
   DCMan->EditPtr(checknumFN)->SetReadOnly(true);
  }
}

void HoldCheckDialog::BNClicked1()
{
 handleApprovedButton(0,FN_HOLDCHECKAMOUNT1,FN_HOLDCHECKCHECKNUM1);
}

void HoldCheckDialog::BNClicked2()
{
 handleApprovedButton(1,FN_HOLDCHECKAMOUNT2,FN_HOLDCHECKCHECKNUM2);
}


void HoldCheckDialog::BNClicked3()
{
 handleApprovedButton(2,FN_HOLDCHECKAMOUNT3,FN_HOLDCHECKCHECKNUM3);
}




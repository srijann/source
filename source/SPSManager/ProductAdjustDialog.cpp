//----------------------------------------------------------------------------
//  Project spsmanager
//
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    spsmanager.apx Application
//  FILE:         productadjustdialog.cpp
//  AUTHOR:
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of ProductAdjustDialog (TDialog).
//
//----------------------------------------------------------------------------
#include "win4me.h"
#pragma hdrstop
#include <stdio.h>
#include "productadjustdialog.h"
#include "unsnum.h"
#include "ListDefs.h"

#define DEFCONTROLS(name)\
  name=new TEdit(this,IDC_##name,4);\
  name##UPDN=new TUpDown(this,IDC_##name##UPDN);\
  ClassNames[i++]=new TStatic(this,IDC_##name##Name)
//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(ProductAdjustDialog, TDialog)
//{{ProductAdjustDialogRSP_TBL_BEGIN}}
  EV_TCN_SELCHANGE(IDC_PRODUCTGROUPS, TCNSelchange),
  EV_UDN_DELTAPOS(IDC_PRODADJUPDN, UDNDeltapos),
  EV_UDN_DELTAPOS(IDC_PRODADJUPDN+1, UDNDeltapos),
  EV_UDN_DELTAPOS(IDC_PRODADJUPDN+2, UDNDeltapos),
  EV_UDN_DELTAPOS(IDC_PRODADJUPDN+3, UDNDeltapos),
  EV_UDN_DELTAPOS(IDC_PRODADJUPDN+4, UDNDeltapos),
  EV_UDN_DELTAPOS(IDC_PRODADJUPDN+5, UDNDeltapos),
  EV_UDN_DELTAPOS(IDC_PRODADJUPDN+6, UDNDeltapos),
  EV_UDN_DELTAPOS(IDC_PRODADJUPDN+7, UDNDeltapos),
  EV_UDN_DELTAPOS(IDC_PRODADJUPDN+8, UDNDeltapos),
  EV_UDN_DELTAPOS(IDC_PRODADJUPDN+9, UDNDeltapos),
  EV_UDN_DELTAPOS(IDC_PRODADJUPDN+10, UDNDeltapos),
  EV_UDN_DELTAPOS(IDC_PRODADJUPDN+11, UDNDeltapos),
  EV_UDN_DELTAPOS(IDC_PRODADJUPDN+12, UDNDeltapos),
//{{ProductAdjustDialogRSP_TBL_END}}
END_RESPONSE_TABLE;

//{{ProductAdjustDialog Implementation}}

ProductAdjustDialog::ProductAdjustDialog(TWindow* parent,
      AuthRules* TheRules,TResId resId)
:
 FormDialog(parent, resId),
 Rules(TheRules),
 CurrentGroup(1)
{
 int n, cnt;

 for ( n=IDC_PRODADJNAME, cnt=0; cnt < NUMPRODUCTSUBCLASSES; ++n, ++cnt)
  ClassName[cnt] = new TStatic(this,n);
 for ( n=IDC_PRODADJVALUE, cnt=0; cnt < NUMPRODUCTSUBCLASSES; ++n, ++cnt)
  AdjValue[cnt] = new TEdit(this,n,4);
 for ( n=IDC_PRODADJUPDN, cnt=0; cnt < NUMPRODUCTSUBCLASSES; ++n, ++cnt)
  AdjUPDN[cnt] = new TUpDown(this,n);

 ProductGroupTab = new TTabControl(this,IDC_PRODUCTGROUPS);
}


ProductAdjustDialog::~ProductAdjustDialog()
{
  Destroy(IDCANCEL);
}

//-----------------------------------
// Handle the Adjustment field clicks
//-----------------------------------
void ProductAdjustDialog::ClickAdjustment(BYTE& Parm,TEdit* Ctl,bool Up)
{
 if ( isEmpty(Parm) )
  {
   Parm = Up ? (BYTE)11 : (BYTE)9;
   OutPutAdjustment(Parm,Ctl);
   return;
  }

 if ( Up )
  Parm++;
 else
  Parm--;

 // Valid values are .1 to 2.0  ... consider 0, 1.0 and 2.1 reset to empty.
 if ( Parm > 20 || Parm==0 || Parm==10 )
  Empty(Parm);

 OutPutAdjustment(Parm,Ctl);
}

//---------------------------------------
// Output Adjustment factor to the window
//---------------------------------------
void ProductAdjustDialog::OutPutAdjustment(BYTE Value,TEdit* Ctl)
{
 char FmtBuf[10];

 if ( isEmpty(Value) )
  FmtBuf[0]=0;
 else
  sprintf(FmtBuf,"%u.%u",Value/10,Value%10);
 Ctl->Transfer(FmtBuf,tdSetData);
}

//----------------------------------------------------
// Transfer Group name to control and add a tab for it
//----------------------------------------------------
static void TransferGroupName(char* Name,int Num,void* tab)
{
 char fmtBuf[25];
 TTabControl* Tab=(TTabControl*)tab;
 if ( Name==NULL ||
      Name[0] == 0 )
  {
   sprintf(fmtBuf,"Group# %d",Num);
   Tab->Add(fmtBuf);
  }
 else
  Tab->Add(Name);
}

void ProductAdjustDialog::SetupWindow()
{
 TDialog::SetupWindow();

 PCList.ForEachClass(TransferGroupName,ProductGroupTab);
 PCList.InitText(ClassName,CurrentGroup);
 OutputGroupValues();
}

//---------------------------------------------
// Handle the Tab Selection
//---------------------------------------------
void ProductAdjustDialog::TCNSelchange(TNotify&)
{
 CurrentGroup=ProductGroupTab->GetSel() + 1;
 PCList.InitText(ClassName,CurrentGroup);
 OutputGroupValues();
}

//------------------------------------
// Output all Values for current group
//------------------------------------
void ProductAdjustDialog::OutputGroupValues()
{
 int row=CurrentGroup-1;
 int col;

 for (col=0; col < NUMPRODUCTSUBCLASSES; ++col)
  OutPutAdjustment(Rules->Data.ProdAdj[row][col],AdjValue[col]);
}

//---------------------
// Handle Up/Down click
//---------------------
bool ProductAdjustDialog::UDNDeltapos(TNmUpDown& udn)
{
 bool IsUp;
 uint Col;

  switch( udn.iDelta )
   {
    case -1: IsUp=true; break;
    case  1: IsUp=false; break;
    default: return true;
   }


 Col = udn.hdr.idFrom - IDC_PRODADJUPDN;
 ClickAdjustment(Rules->Data.ProdAdj[CurrentGroup-1][Col],
                     AdjValue[Col],IsUp);

 return false;
}


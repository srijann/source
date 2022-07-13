//----------------------------------------------------------------------------
//  Project spsmanager
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    spsmanager.apx Application
//  FILE:         SecRulesdialog.cpp
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of SecRulesDialog (TDialog).
//
//----------------------------------------------------------------------------
#include "win4me.h"
#pragma hdrstop
#include <owl/validate.h>
#include "SecRulesDialog.h"
#include "TranType.h"
#include "CheckAuthFn.h"
#include "DBManMsg.h"
#include "SaveFile.h"

//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(SecRulesDialog, TDialog)
//{{SecRulesDialogRSP_TBL_BEGIN}}
//{{SecRulesDialogRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{SecRulesDialog Implementation}}

#define CHECKBOX(name) name=new TCheckBox(this,IDC_##name)
#define USHORT(name) name=new TEditUns(this,IDC_##name,6);\
  name->SetValidator(new TRangeValidator(0,USHRT_MAX-1))
#define UCHAR(name) name = new TEditUns(this,IDC_##name,3)
#define ULONG(name) name = new TEditUns(this,IDC_##name,7)  
#define CARANGE(name) name = new TEdit(this,IDC_##name,3)

//------------------
// Class Constructor
//------------------
SecRulesDialog::SecRulesDialog(TWindow* parent,SecRules* OrigRec)
:
 FormDialog(parent,IDD_SECRULES,RecTypeSecRules),
 origRec(OrigRec),
 SetupComplete(false)
{
 char Title[100];

 strcpy(Title,(char*)origRec->Data.SecRulesClassName);
 strcat(Title," Security Rules");
 SetCaption(Title);
 memcpy(&newRec.Data,&origRec->Data,sizeof(newRec.Data));

 CHECKBOX(MAXBANKVERIFY);
 CHECKBOX(HIRISKPRODUCT2);
 CHECKBOX(HIRISKPRODUCT1);
 CHECKBOX(HIRISKCHECKNUMBER);
 CHECKBOX(HIRISKAMOUNT);
 CHECKBOX(HIRISKDEMOGRAPHIC);
 CHECKBOX(NOAPARTMENT);
 CHECKBOX(MAXRCODES);
 CHECKBOX(HIRISKCALICENSE);
 CHECKBOX(SSNFRAUD);
 CHECKBOX(LOWCREDITSCORE);
 CHECKBOX(ADDRESSUNVERIFIABLE);
 CHECKBOX(SSNINVALID);
 CHECKBOX(PHONEUNVERIFIABLE);
 CHECKBOX(CHECKMUSTBESIGNED);
 CHECKBOX(NOCARRYOUT);
 CHECKBOX(HIRISKAGE);

 // The product stuff is not being implemented yet
 //TEditUns* HiRiskProduct1ToTime;
 //TEditUns* HiRiskProduct1FromTime;
 //TEditUns* HiRiskProduct2ToTime;
 //TEditUns* HiRiskProduct2FromTime;
 //TEditUns* HiRiskProduct2DayOfWeek;
 //USHORT(HiRiskProduct2Amount);
 //TEditUns* HiRiskProduct2Value;
 //TEditUns* HiRiskProdcut2FromTime;
 //TEditUns* HiRiskProduct1DayOfWeek;
 //USHORT(TEditUns* HiRiskProduct1Amount);
 //TEditUns* HiRiskProduct1Value;

 ULONG(HiRiskCheckNumberValue);
 USHORT(HiRiskAmountValue);
 UCHAR(MaxRcodesNum);
 UCHAR(MaxBankVerifyDays);
 UCHAR(MaxBankVerifyNumber);
 USHORT(LowCreditScoreValue);
 USHORT(HiRiskDemographicClass);
 UCHAR(HiRiskAgeValue);

 CARANGE(HiRiskCALicense10);
 CARANGE(HiRiskCALicense8);
 CARANGE(HiRiskCALicense7);
 CARANGE(HiRiskCALicense6);
 CARANGE(HiRiskCALicense5);
 CARANGE(HiRiskCALicense4);
 CARANGE(HiRiskCALicense3);
 CARANGE(HiRiskCALicense2);
 CARANGE(HiRiskCALicense1);
 CARANGE(HiRiskCALicense9);
}



//-----------------
// Class Destructor
//-----------------
SecRulesDialog::~SecRulesDialog()
{
 Destroy(IDCANCEL);
 delete origRec;
}

//-----------------------------------------------------
// Handle the EV_CHANGE command to handle level changes
//-----------------------------------------------------
TResult SecRulesDialog::EvCommand(uint id, THandle hWndCtl, uint notifyCode)
{
  TResult result;

  result = FormDialog::EvCommand(id, hWndCtl, notifyCode);

  switch ( notifyCode )
   {
    case BN_CLICKED:
    case EN_CHANGE:  if ( SetupComplete )
                       ChangeHandler(id);
                     break;
   }

  return result;
}

//----------------------------------------------------------
// Update the entire window for the current transaction type
//----------------------------------------------------------
#define SETCHECK(name)\
 name->SetCheck( newRec.isEnabled(newRec.name) ? BF_CHECKED: BF_UNCHECKED)

#define SETNUM(name,Enabler)\
 if (newRec.isEnabled(newRec.Enabler))\
  {\
   name->EnableWindow(true);\
   name->Set(newRec.Data.name);\
  }\
 else\
  {\
   name->Transfer("",tdSetData);\
   name->EnableWindow(false);\
  }

#define TRANSFERLIC(num)\
  memcpy(Buf,newRec.Data.HiRiskCALicenses[num-1],2);\
  HiRiskCALicense##num->Transfer(Buf,tdSetData)

void SecRulesDialog::UpdateWin()
{
 char Buf[3];

 SETCHECK(MAXBANKVERIFY);
 SETCHECK(HIRISKCHECKNUMBER);
 SETCHECK(HIRISKAMOUNT);
 SETCHECK(HIRISKDEMOGRAPHIC);
 SETCHECK(NOAPARTMENT);
 SETCHECK(MAXRCODES);
 SETCHECK(HIRISKCALICENSE);
 SETCHECK(SSNFRAUD);
 SETCHECK(LOWCREDITSCORE);
 SETCHECK(ADDRESSUNVERIFIABLE);
 SETCHECK(SSNINVALID);
 SETCHECK(PHONEUNVERIFIABLE);
 SETCHECK(CHECKMUSTBESIGNED);
 SETCHECK(NOCARRYOUT);
 SETCHECK(HIRISKAGE);

 SETNUM(HiRiskCheckNumberValue,HIRISKCHECKNUMBER);
 SETNUM(HiRiskAmountValue,HIRISKAMOUNT);
 SETNUM(MaxRcodesNum,MAXRCODES);
 SETNUM(MaxBankVerifyDays,MAXBANKVERIFY);
 SETNUM(MaxBankVerifyNumber,MAXBANKVERIFY);
 SETNUM(LowCreditScoreValue,LOWCREDITSCORE);
 SETNUM(HiRiskDemographicClass,HIRISKDEMOGRAPHIC);
 SETNUM(HiRiskAgeValue,HIRISKAGE);

 memset(Buf,0,sizeof(Buf));
 if ( newRec.isEnabled(newRec.HIRISKCALICENSE) )
  {
   TRANSFERLIC(1);
   TRANSFERLIC(2);
   TRANSFERLIC(3);
   TRANSFERLIC(4);
   TRANSFERLIC(5);
   TRANSFERLIC(6);
   TRANSFERLIC(7);
   TRANSFERLIC(8);
   TRANSFERLIC(9);
   TRANSFERLIC(10);
  }
 else
  {
   HiRiskCALicense1->EnableWindow(false);
   HiRiskCALicense2->EnableWindow(false);
   HiRiskCALicense3->EnableWindow(false);
   HiRiskCALicense4->EnableWindow(false);
   HiRiskCALicense5->EnableWindow(false);
   HiRiskCALicense6->EnableWindow(false);
   HiRiskCALicense7->EnableWindow(false);
   HiRiskCALicense8->EnableWindow(false);
   HiRiskCALicense9->EnableWindow(false);
   HiRiskCALicense10->EnableWindow(false);
  }
}

//-----------------------------------------------------
// Override virtual function to provide initial display
//-----------------------------------------------------
void SecRulesDialog::SetupWindow()
{
 TDialog::SetupWindow();
 UpdateWin();
 SetupComplete=true;
}

//----------------------
// Query modified status
//----------------------
bool SecRulesDialog::IsModified()
{
 if ( FormDialog::IsModified() )
  return true;
 else
  return (memcmp(&origRec->Data,&newRec.Data,
                  sizeof(origRec->Data)) ==0) ? false : true;
}

//-------------------------
// Save Changes to DataBase
//-------------------------
bool SecRulesDialog::Save()
{
 DBManMessage Msg;

 if ( ! Msg.DBPut(this,T_PUTSECRULES,FN_SECRULESRECORD,&newRec) )
  return false;

 ResetModified();
 return true;
}

//--------------------------
// Save changes to temp file
//--------------------------
bool SecRulesDialog::SaveTemp(TFile* File)
{
 SAVERECORD_SENTINEL Sentinel;

 Sentinel.Type = GetType();
 Sentinel.Version = SaveRecordVersion[Sentinel.Type];
 Sentinel.Len = sizeof(newRec.Data);

 if ( File->Write(&Sentinel,sizeof(Sentinel)) != sizeof(Sentinel) ||
      File->Write(&newRec.Data,sizeof(newRec.Data))
            != sizeof(newRec.Data) )
  return false;

 ResetModified();
 return true;

}

//------------------------
// Reset the modified flag
//------------------------
void SecRulesDialog::ResetModified()
{
 memcpy(&origRec->Data,&newRec.Data,sizeof(origRec->Data));
 FormDialog::ResetModified();
}

#define CLICK(name) case IDC_##name:\
  newRec.setEnabled(newRec.name,name->GetCheck()==BF_CHECKED); break
#define CLICK2(name,name2) case IDC_##name:\
  newRec.setEnabled(newRec.name,name->GetCheck()==BF_CHECKED);\
  if ( name->GetCheck() != BF_CHECKED)\
   {\
    name2->Transfer("",tdSetData);\
    name2->EnableWindow(false);\
   }\
   else\
    {\
     name2->EnableWindow(true);\
     name2->SetFocus();\
    }\
  break;

#define CLICK3(name,name2,name3) case IDC_##name:\
  newRec.setEnabled(newRec.name,name->GetCheck()==BF_CHECKED);\
  if ( name->GetCheck() != BF_CHECKED)\
   {\
    name2->Transfer("",tdSetData);\
    name2->EnableWindow(false);\
    name3->Transfer("",tdSetData);\
    name3->EnableWindow(false);\
   }\
   else\
    {\
     name2->EnableWindow(true);\
     name2->SetFocus();\
     name3->EnableWindow(true);\
    }\
  break;


#define TRANSFERNUM(name) case IDC_##name:\
  name->Get(newRec.Data.name); break

#define TRANSFERCALIC(num) case IDC_##HiRiskCALicense##num:\
  HiRiskCALicense##num->Transfer(Buf,tdGetData);\
  if ( strlen(Buf) != 2 ) strcpy(Buf,"  ");\
  memcpy(newRec.Data.HiRiskCALicenses[num-1],Buf,2); break

void SecRulesDialog::ChangeHandler(uint Id)
{
 char Buf[10];

 switch(Id)
  {
   CLICK3(MAXBANKVERIFY,MaxBankVerifyNumber,MaxBankVerifyDays);
   CLICK2(HIRISKCHECKNUMBER,HiRiskCheckNumberValue);
   CLICK2(HIRISKAMOUNT,HiRiskAmountValue);
   CLICK2(HIRISKAGE,HiRiskAgeValue);
   CLICK2(MAXRCODES,MaxRcodesNum);

   CLICK(HIRISKDEMOGRAPHIC);
   CLICK(NOAPARTMENT);
   CLICK(SSNFRAUD);
   CLICK(LOWCREDITSCORE);
   CLICK(ADDRESSUNVERIFIABLE);
   CLICK(SSNINVALID);
   CLICK(PHONEUNVERIFIABLE);
   CLICK(CHECKMUSTBESIGNED);
   CLICK(NOCARRYOUT);

   case IDC_HIRISKCALICENSE:
    newRec.setEnabled(newRec.HIRISKCALICENSE,
                           HIRISKCALICENSE->GetCheck()==BF_CHECKED);
    SetStateRanges( HIRISKCALICENSE->GetCheck() == BF_CHECKED );
    break;

   TRANSFERCALIC(9);
   TRANSFERCALIC(1);
   TRANSFERCALIC(2);
   TRANSFERCALIC(3);
   TRANSFERCALIC(4);
   TRANSFERCALIC(5);
   TRANSFERCALIC(6);
   TRANSFERCALIC(7);
   TRANSFERCALIC(8);
   TRANSFERCALIC(10);

   TRANSFERNUM(HiRiskCheckNumberValue);
   TRANSFERNUM(HiRiskAmountValue);
   TRANSFERNUM(MaxRcodesNum);
   TRANSFERNUM(MaxBankVerifyDays);
   TRANSFERNUM(MaxBankVerifyNumber);
   TRANSFERNUM(LowCreditScoreValue);
   TRANSFERNUM(HiRiskDemographicClass);
   TRANSFERNUM(HiRiskAgeValue);
  }
}


//------------------------------
// Perform all Field validations
//------------------------------
#define VALID(name) if ( ! name->IsValid() )\
 { name->SetFocus(); name->ValidatorError(); return false; }
bool SecRulesDialog::Validate()
{
 VALID(HiRiskAmountValue);
 VALID(HiRiskCALicense1);
 VALID(HiRiskCALicense2);
 VALID(HiRiskCALicense3);
 VALID(HiRiskCALicense4);
 VALID(HiRiskCALicense5);
 VALID(HiRiskCALicense6);
 VALID(HiRiskCALicense7);
 VALID(HiRiskCALicense8);
 VALID(HiRiskCALicense9);
 VALID(HiRiskCALicense10);
 return true;
}

void SecRulesDialog::SetStateRanges(bool Enable)
{
 HiRiskCALicense1->EnableWindow(Enable);
 HiRiskCALicense2->EnableWindow(Enable);
 HiRiskCALicense3->EnableWindow(Enable);
 HiRiskCALicense4->EnableWindow(Enable);
 HiRiskCALicense5->EnableWindow(Enable);
 HiRiskCALicense6->EnableWindow(Enable);
 HiRiskCALicense7->EnableWindow(Enable);
 HiRiskCALicense8->EnableWindow(Enable);
 HiRiskCALicense9->EnableWindow(Enable);
 HiRiskCALicense10->EnableWindow(Enable);
 if ( ! Enable )
  {
   HiRiskCALicense1->Transfer("",tdSetData);
   HiRiskCALicense2->Transfer("",tdSetData);
   HiRiskCALicense3->Transfer("",tdSetData);
   HiRiskCALicense4->Transfer("",tdSetData);
   HiRiskCALicense5->Transfer("",tdSetData);
   HiRiskCALicense6->Transfer("",tdSetData);
   HiRiskCALicense7->Transfer("",tdSetData);
   HiRiskCALicense8->Transfer("",tdSetData);
   HiRiskCALicense9->Transfer("",tdSetData);
   HiRiskCALicense10->Transfer("",tdSetData);
  }
}


//----------------------------------------------------------------------------
//  Project spsmanager
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    spsmanager.apx Application
//  FILE:         datareqdialog.cpp
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of DataReqDialog (TDialog).
//
//----------------------------------------------------------------------------
#include "win4me.h"
#pragma hdrstop
#include <owl/validate.h>
#include "datareqdialog.h"
#include "TranType.h"
#include "CheckAuthFn.h"
#include "DBManMsg.h"
#include "SaveFile.h"

//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(DataReqDialog, TDialog)
//{{DataReqDialogRSP_TBL_BEGIN}}
  EV_TCN_SELCHANGE(IDC_TRANTYPETAB, TCNSelchange),
  EV_TCN_SELCHANGING(IDC_TRANTYPETAB, TCNSelchanging),
  EV_WM_CHAR,
//{{DataReqDialogRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{DataReqDialog Implementation}}

#define CHECKBOX(name) name=new TCheckBox(this,IDC_##name)
#define AMOUNT(name) name=new TEditUns(this,IDC_##name,6);\
  name->SetValidator(new TRangeValidator(0,USHRT_MAX-1))
#define STATIC(name) name=new TStatic(this,IDS_##name)

//------------------
// Class Constructor
//------------------
DataReqDialog::DataReqDialog(TWindow* parent,DataRequirements* OrigRec)
:
 FormDialog(parent,IDD_DATAREQ,RecTypeDataRequirements),
 origRec(OrigRec),
 CurType(OrigRec->Pos),
 paintingFlag(false)
{
 char Title[100];
 TabCtl = new TTabControl(this,IDC_TRANTYPETAB);
 strcpy(Title,(char*)origRec->Data.DataReqClassName);
 strcat(Title," Data Requirements");
 SetCaption(Title);
 memcpy(&newRec.Data,&origRec->Data,sizeof(newRec.Data));

 CHECKBOX(CHECKTYPE);
 CHECKBOX(LICENSE);
 CHECKBOX(DOB);
 CHECKBOX(CHECKNUMBER);
 CHECKBOX(MICR);
 CHECKBOX(LICENSEORMICR);
 CHECKBOX(NAME);
 CHECKBOX(ADDRESS);
 CHECKBOX(RESIDENCETYPE);
 CHECKBOX(CITY);
 CHECKBOX(CONSUMERSTATE);
 CHECKBOX(ZIPCODE);
 CHECKBOX(PHONE);
 CHECKBOX(SSN);
 CHECKBOX(PRODUCTCLASS);
 CHECKBOX(TRANSPORTMETHOD);
 CHECKBOX(BUSINESSNAME);
 CHECKBOX(PRIVATELABEL);
 CHECKBOX(TITLE);
 CHECKBOX(FINANCEAMOUNT);

 CHECKBOX(SPOUSEEMPLOYERPHONE);
 CHECKBOX(SPOUSESALARY);
 CHECKBOX(SPOUSEYEARSEMPLOYED);
 CHECKBOX(SPOUSEPOSITION);
 CHECKBOX(SPOUSEEMPLOYERADR);
 CHECKBOX(SPOUSEEMPLOYER);
 CHECKBOX(SPOUSESSN);
 CHECKBOX(SPOUSENAME);

 CHECKBOX(POSITION);
 CHECKBOX(YEARSEMPLOYED);
 CHECKBOX(SALARY);
 CHECKBOX(EMPLOYERPHONE);
 CHECKBOX(EMPLOYERADR);
 CHECKBOX(EMPLOYERNAME);

 AMOUNT(CHECKTYPEAMT);
 AMOUNT(LICENSEAMT);
 AMOUNT(DOBAMT);
 AMOUNT(CHECKNUMBERAMT);
 AMOUNT(MICRAMT);
 AMOUNT(LICENSEORMICRAMT);
 AMOUNT(NAMEAMT);
 AMOUNT(ADDRESSAMT);
 AMOUNT(RESIDENCETYPEAMT);
 AMOUNT(CITYAMT);
 AMOUNT(CONSUMERSTATEAMT);
 AMOUNT(ZIPCODEAMT);
 AMOUNT(PHONEAMT);
 AMOUNT(SSNAMT);
 AMOUNT(PRODUCTCLASSAMT);
 AMOUNT(TRANSPORTMETHODAMT);
 AMOUNT(BUSINESSNAMEAMT);
 AMOUNT(PRIVATELABELAMT);
 AMOUNT(TITLEAMT);
 AMOUNT(SPOUSEDATAAMT);
 AMOUNT(EMPLOYMENTDATAAMT);
 AMOUNT(FINANCEAMOUNTAMT);

 CHECKBOX(DOBIFMICRONLY);

 STATIC(STATIC1);
 STATIC(STATIC2);
 STATIC(STATIC3);
 STATIC(STATIC4);
}

//-----------------
// Class Destructor
//-----------------
DataReqDialog::~DataReqDialog()
{
 Destroy(IDCANCEL);
 delete origRec;
}

//---------------------------------------------
// Handle the Tab Selection
//---------------------------------------------
void DataReqDialog::TCNSelchange(TNotify&)
{
 switch( CurType=TabCtl->GetSel() )
  {
   case 0: CurType=newRec.Pos;
           break;
   case 1: CurType=newRec.Voice;
           break;
   case 2: CurType=newRec.Security;
           break;
  }

 UpdateWin(); 
}

//---------------------------------------------------
// Don't allow Tab switch with invalid data in window
//---------------------------------------------------
bool DataReqDialog::TCNSelchanging(TNotify&)
{
 if ( ! Validate() )
   return true;
 else
   return false;
}

//-----------------------------------------------------
// Handle the EV_CHANGE command to handle level changes
//-----------------------------------------------------
TResult DataReqDialog::EvCommand(uint id, THandle hWndCtl, uint notifyCode)
{
  TResult result;

  result = FormDialog::EvCommand(id, hWndCtl, notifyCode);

  switch ( notifyCode )
   {
    case BN_CLICKED:
    case EN_CHANGE:  if ( paintingFlag )
                       ChangeHandler(id);
                     break;
   }

  return result;
}

//----------------------------------------------------------
// Update the entire window for the current transaction type
//----------------------------------------------------------
#define SETCHECK(name)\
 name->SetCheck( newRec.isRequired(CurType,newRec.name) ? BF_CHECKED: BF_UNCHECKED)
 
#define SETAMOUNT(name)\
 name##AMT->Set(newRec.getAmount(CurType,newRec.name));\
 if (newRec.isRequired(CurType,newRec.name))\
   name##AMT->EnableWindow(true);\
 else name##AMT->EnableWindow(false);

void DataReqDialog::UpdateWin()
{
 paintingFlag=false;
 SETCHECK(CHECKTYPE);
 SETCHECK(LICENSE);
 SETCHECK(DOB);
 DOBIFMICRONLY->EnableWindow( newRec.isRequired(CurType,newRec.DOB) );
 SETCHECK(CHECKNUMBER);
 SETCHECK(MICR);
 SETCHECK(LICENSEORMICR);
 SETCHECK(NAME);
 SETCHECK(ADDRESS);
 SETCHECK(RESIDENCETYPE);
 SETCHECK(CITY);
 SETCHECK(CONSUMERSTATE);
 SETCHECK(ZIPCODE);
 SETCHECK(PHONE);
 SETCHECK(SSN);
 SETCHECK(PRODUCTCLASS);
 SETCHECK(TRANSPORTMETHOD);
 SETCHECK(BUSINESSNAME);
 SETCHECK(PRIVATELABEL);
 SETCHECK(TITLE);
 SETCHECK(DOBIFMICRONLY);
 SETCHECK(FINANCEAMOUNT);

 SETCHECK(SPOUSEEMPLOYERPHONE);
 SETCHECK(SPOUSESALARY);
 SETCHECK(SPOUSEYEARSEMPLOYED);
 SETCHECK(SPOUSEPOSITION);
 SETCHECK(SPOUSEEMPLOYERADR);
 SETCHECK(SPOUSEEMPLOYER);
 SETCHECK(SPOUSESSN);
 SETCHECK(SPOUSENAME);

 SETCHECK(POSITION);
 SETCHECK(YEARSEMPLOYED);
 SETCHECK(SALARY);
 SETCHECK(EMPLOYERPHONE);
 SETCHECK(EMPLOYERADR);
 SETCHECK(EMPLOYERNAME);

 SETAMOUNT(CHECKTYPE);
 SETAMOUNT(LICENSE);
 SETAMOUNT(DOB);
 SETAMOUNT(CHECKNUMBER);
 SETAMOUNT(MICR);
 SETAMOUNT(LICENSEORMICR);
 SETAMOUNT(NAME);
 SETAMOUNT(ADDRESS);
 SETAMOUNT(RESIDENCETYPE);
 SETAMOUNT(CITY);
 SETAMOUNT(CONSUMERSTATE);
 SETAMOUNT(ZIPCODE);
 SETAMOUNT(PHONE);
 SETAMOUNT(SSN);
 SETAMOUNT(PRODUCTCLASS);
 SETAMOUNT(TRANSPORTMETHOD);
 SETAMOUNT(BUSINESSNAME);
 SETAMOUNT(PRIVATELABEL);
 SETAMOUNT(TITLE);
 SETAMOUNT(FINANCEAMOUNT);
 enableEmployerGroup();
 enableSpouseGroup();
 SETAMOUNT(SPOUSEDATA);
 SETAMOUNT(EMPLOYMENTDATA);
 paintingFlag=true;
}

//-----------------------------------------------------
// Override virtual function to provide initial display
//-----------------------------------------------------
void DataReqDialog::SetupWindow()
{
 TDialog::SetupWindow();
 TabCtl->Add("Pos");
 TabCtl->Add("Voice");
 TabCtl->Add("Bank Verify");
 UpdateWin();
}

//----------------------
// Query modified status
//----------------------
bool DataReqDialog::IsModified()
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
bool DataReqDialog::Save()
{
 DBManMessage Msg;

 if ( ! Msg.DBPut(this,T_PUTDATAREQ,FN_DATAREQRECORD,&newRec) )
  return false;

 ResetModified();
 return true;
}

//--------------------------
// Save changes to temp file
//--------------------------
bool DataReqDialog::SaveTemp(TFile* File)
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
void DataReqDialog::ResetModified()
{
 memcpy(&origRec->Data,&newRec.Data,sizeof(origRec->Data));
 FormDialog::ResetModified();
}

#define CLICK(name) case IDC_##name: \
  if (name->GetCheck()==BF_CHECKED) {\
    newRec.setRequired(CurType,newRec.name,true);\
    name##AMT->EnableWindow(true);\
    name##AMT->Set(newRec.getAmount(CurType,newRec.name));\
  } else {\
    newRec.setRequired(CurType,newRec.name,false);\
    name##AMT->EnableWindow(false); } break
#define CLICK2(name,name2) case IDC_##name: \
  if (name->GetCheck()==BF_CHECKED) {\
    newRec.setRequired(CurType,newRec.name,true);\
    name##AMT->EnableWindow(true);\
    name2->EnableWindow(true);\
    name##AMT->Set(newRec.getAmount(CurType,newRec.name));\
  } else {\
    newRec.setRequired(CurType,newRec.name,false);\
    name2->SetCheck(BF_UNCHECKED);\
    name2->EnableWindow(false);\
    newRec.setRequired(CurType,newRec.name2,false);\
     name##AMT->EnableWindow(false); } break

#define CLICKX(name) case IDC_##name: \
  newRec.setRequired(CurType,newRec.name,name->GetCheck()==BF_CHECKED); break
#define CLICKEMP(name) case IDC_##name: \
  newRec.setRequired(CurType,newRec.name,name->GetCheck()==BF_CHECKED);\
  setEmployerGroup();\
  enableEmployerGroup();\
  break
#define CLICKSPOUSE(name) case IDC_##name: \
  newRec.setRequired(CurType,newRec.name,name->GetCheck()==BF_CHECKED);\
  setSpouseGroup();\
  enableSpouseGroup();\
  break
#define TRANSFER(name) case IDC_##name##AMT:\
  name##AMT->Get(Amount);\
  newRec.setAmount(CurType,newRec.name,Amount); break
void DataReqDialog::ChangeHandler(uint Id)
{
 unsigned short Amount;
 switch(Id)
  {
  CLICK(CHECKTYPE);
  CLICK(LICENSE);
  CLICK2(DOB,DOBIFMICRONLY);
  CLICK(CHECKNUMBER);
  CLICK(MICR);
  CLICK(LICENSEORMICR);
  CLICK(NAME);
  CLICK(ADDRESS);
  CLICK(RESIDENCETYPE);
  CLICK(CITY);
  CLICK(CONSUMERSTATE);
  CLICK(ZIPCODE);
  CLICK(PHONE);
  CLICK(SSN);
  CLICK(PRODUCTCLASS);
  CLICK(TRANSPORTMETHOD);
  CLICK(BUSINESSNAME);
  CLICK(PRIVATELABEL);
  CLICK(TITLE);
  CLICK(FINANCEAMOUNT);
  CLICKX(DOBIFMICRONLY);

  CLICKSPOUSE(SPOUSEEMPLOYERPHONE);
  CLICKSPOUSE(SPOUSESALARY);
  CLICKSPOUSE(SPOUSEYEARSEMPLOYED);
  CLICKSPOUSE(SPOUSEPOSITION);
  CLICKSPOUSE(SPOUSEEMPLOYERADR);
  CLICKSPOUSE(SPOUSEEMPLOYER);
  CLICKSPOUSE(SPOUSESSN);
  CLICKSPOUSE(SPOUSENAME);

  CLICKEMP(POSITION);
  CLICKEMP(YEARSEMPLOYED);
  CLICKEMP(SALARY);
  CLICKEMP(EMPLOYERPHONE);
  CLICKEMP(EMPLOYERADR);
  CLICKEMP(EMPLOYERNAME);

  TRANSFER(CHECKTYPE);
  TRANSFER(LICENSE);
  TRANSFER(LICENSEORMICR);
  TRANSFER(DOB);
  TRANSFER(CHECKNUMBER);
  TRANSFER(MICR);
  TRANSFER(NAME);
  TRANSFER(ADDRESS);
  TRANSFER(RESIDENCETYPE);
  TRANSFER(CITY);
  TRANSFER(CONSUMERSTATE);
  TRANSFER(ZIPCODE);
  TRANSFER(PHONE);
  TRANSFER(SSN);
  TRANSFER(PRODUCTCLASS);
  TRANSFER(TRANSPORTMETHOD);
  TRANSFER(BUSINESSNAME);
  TRANSFER(PRIVATELABEL);
  TRANSFER(TITLE);
  TRANSFER(FINANCEAMOUNT);
  TRANSFER(SPOUSEDATA);
  TRANSFER(EMPLOYMENTDATA);
 }

}


//------------------------------
// Perform all Field validations
//------------------------------
#define VALID(name) if ( ! name->IsValid() )\
 { name->SetFocus(); name->ValidatorError(); return false; }
bool DataReqDialog::Validate()
{
 VALID(CHECKTYPEAMT);
 VALID(LICENSEAMT);
 VALID(LICENSEORMICRAMT);
 VALID(DOBAMT);
 VALID(CHECKNUMBERAMT);
 VALID(MICRAMT);
 VALID(NAMEAMT);
 VALID(ADDRESSAMT);
 VALID(RESIDENCETYPEAMT);
 VALID(CITYAMT);
 VALID(CONSUMERSTATEAMT);
 VALID(ZIPCODEAMT);
 VALID(PHONEAMT);
 VALID(SSNAMT);
 VALID(PRODUCTCLASSAMT);
 VALID(TRANSPORTMETHODAMT);
 VALID(BUSINESSNAMEAMT);
 VALID(PRIVATELABELAMT);
 VALID(TITLEAMT);
 VALID(FINANCEAMOUNTAMT);
 VALID(SPOUSEDATAAMT);
 VALID(EMPLOYMENTDATAAMT);

 return true;
}

void DataReqDialog::enableEmployerGroup()
{
 if ( POSITION->GetCheck() == BF_CHECKED ||
      YEARSEMPLOYED->GetCheck() == BF_CHECKED ||
      SALARY->GetCheck() == BF_CHECKED ||
      EMPLOYERPHONE->GetCheck() == BF_CHECKED ||
      EMPLOYERADR->GetCheck() == BF_CHECKED ||
      EMPLOYERNAME->GetCheck() == BF_CHECKED )
  {
   EMPLOYMENTDATAAMT->EnableWindow(true);
  }
 else
  {
   EMPLOYMENTDATAAMT->Transfer("",tdSetData);
   EMPLOYMENTDATAAMT->EnableWindow(false);
  }

}

void DataReqDialog::enableSpouseGroup()
{
 if ( SPOUSEEMPLOYERPHONE->GetCheck() == BF_CHECKED ||
      SPOUSESALARY->GetCheck() == BF_CHECKED ||
      SPOUSEYEARSEMPLOYED->GetCheck() == BF_CHECKED ||
      SPOUSEPOSITION->GetCheck() == BF_CHECKED ||
      SPOUSEEMPLOYERADR->GetCheck() == BF_CHECKED ||
      SPOUSEEMPLOYER->GetCheck() == BF_CHECKED ||
      SPOUSESSN->GetCheck() == BF_CHECKED ||
      SPOUSENAME->GetCheck() == BF_CHECKED )
  {
   SPOUSEDATAAMT->EnableWindow(true);
  }
 else
  {
   SPOUSEDATAAMT->Transfer("",tdSetData);
   SPOUSEDATAAMT->EnableWindow(false);
  }

}


void DataReqDialog::setEmployerGroup()
{
 if ( POSITION->GetCheck() == BF_CHECKED ||
      YEARSEMPLOYED->GetCheck() == BF_CHECKED ||
      SALARY->GetCheck() == BF_CHECKED ||
      EMPLOYERPHONE->GetCheck() == BF_CHECKED ||
      EMPLOYERADR->GetCheck() == BF_CHECKED ||
      EMPLOYERNAME->GetCheck() == BF_CHECKED )
  {
   newRec.setRequired(CurType,newRec.EMPLOYMENTDATA,true);
  }
 else
  {
   newRec.setRequired(CurType,newRec.EMPLOYMENTDATA,false);
   newRec.setAmount(CurType,newRec.EMPLOYMENTDATA,-1);
  }

}

void DataReqDialog::setSpouseGroup()
{
 if ( SPOUSEEMPLOYERPHONE->GetCheck() == BF_CHECKED ||
      SPOUSESALARY->GetCheck() == BF_CHECKED ||
      SPOUSEYEARSEMPLOYED->GetCheck() == BF_CHECKED ||
      SPOUSEPOSITION->GetCheck() == BF_CHECKED ||
      SPOUSEEMPLOYERADR->GetCheck() == BF_CHECKED ||
      SPOUSEEMPLOYER->GetCheck() == BF_CHECKED ||
      SPOUSESSN->GetCheck() == BF_CHECKED ||
      SPOUSENAME->GetCheck() == BF_CHECKED )
  {
   newRec.setRequired(CurType,newRec.SPOUSEDATA,true);
  }
 else
  {
   newRec.setRequired(CurType,newRec.SPOUSEDATA,false);
   newRec.setAmount(CurType,newRec.SPOUSEDATA,-1);
  }

}


void DataReqDialog::EvKillFocus(THandle hWndGetFocus )
{
    TDialog::EvKillFocus(hWndGetFocus );

  // INSERT>> Your code here.

}


void DataReqDialog::EvChar(uint key, uint repeatCount, uint flags)
{
    TDialog::EvChar(key, repeatCount, flags);

  // INSERT>> Your code here.

}


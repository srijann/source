//----------------------------------------------------------------------------
//  Project spsmanager
//
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    spsmanager.apx Application
//  FILE:         merchantdialog.cpp
//  AUTHOR:
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of MerchantDialog (TDialog).
//
//----------------------------------------------------------------------------
#include "win4me.h"
#pragma hdrstop
#include <owl/validate.h>
#include "merchantdialog.h"
#include "TranType.h"
#include "CheckAuthFn.h"
#include "DBManMsg.h"
#include "SaveFile.h"
#include "CheckService.h"

//{{MerchantDialog Implementation}}

#define DISPLAYEDIT(ctl,fld)\
 DisplayEdit(ctl,GetRec(MerchantRec::fld)->Data.fld,MerchantRec::fld)
#define DISPLAYTIMEZONE\
 DisplayTimeZone(GetRec(MerchantRec::MerchantTimeZone)->Data.MerchantTimeZone)
#define DISPLAYCLASS(ctl,fld,enabler)\
 DisplayBox(ctl,GetRec(MerchantRec::enabler)->Data.Classes.fld,MerchantRec::enabler)
#define INITSUBCLASS(ctl,fld,enabler)\
 InitSubClassBox(ctl,GetRec(MerchantRec::enabler)->Data.Classes.fld)

#define COMBOBOX(name) name = new TComboBox(this,IDC_##name)
#define CHECKBOX(name) name = new TCheckBox(this,IDC_##name)
#define RADIOBUTTON(name) name = new TRadioButton(this,IDC_##name)
#define EDIT(name,len) name = new TEdit(this,IDC_##name,len)
#define EDITUNS(name,len) name = new TEditUns(this,IDC_##name,len)

//------------
// Constructor
//------------
MerchantDialog::MerchantDialog(TWindow* parent, MerchantRec* AccountRec,
                               MerchantRec* StationRec)
:
 FormDialog(parent,IDD_MERCHANT,RecTypeMerchantRecord),
 stationRec(StationRec),
 accountRec(AccountRec),
 AuthRulesList(0),
 PCList(0),
 SetupComplete(false)
{
 char Title[100];

 if ( stationRec )
  workingRec=stationRec;
 else
  workingRec=accountRec; 

 COMBOBOX(VFYUNVFUNDSRULES);
 COMBOBOX(GUUNVFUNDSRULES);
 COMBOBOX(VFYPOSFILERULES);
 COMBOBOX(VFYRESPMSG);
 COMBOBOX(VFYDATAREQ);
 COMBOBOX(VFYAUTHRULESSUBCLASS);
 COMBOBOX(VFYAUTHRULESCLASS);
 COMBOBOX(GUPOSFILERULES);
 COMBOBOX(GURESPMSG);
 COMBOBOX(GUDATAREQ);
 COMBOBOX(GUAUTHRULESSUBCLASS);
 COMBOBOX(GUAUTHRULESCLASS);
 COMBOBOX(MERCHANTTIMEZONE);
 COMBOBOX(PRODUCTGROUP);

 // ENABLER BUTTONS
 CHECKBOX(VFYDATAREQBTN);
 CHECKBOX(VFYPOSFILERULESBTN);
 CHECKBOX(GUPOSFILERULESBTN);
 CHECKBOX(VFYRESPMSGBTN);
 CHECKBOX(GURESPMSGBTN);
 CHECKBOX(VFYUNVFUNDSRULESBTN);
 CHECKBOX(GUUNVFUNDSRULESBTN);
 CHECKBOX(GUDATAREQBTN);
 CHECKBOX(GUAUTHRULESBTN);
 CHECKBOX(VFYAUTHRULESBTN);
 CHECKBOX(MERCHANTTIMEZONEBTN);
 CHECKBOX(MERCHANTSICBTN);
 CHECKBOX(MERCHANTZIPBTN);
 CHECKBOX(MERCHANTSTATEBTN);
 CHECKBOX(FEDREGBTN);
 CHECKBOX(SERVICESBTN);
 CHECKBOX(PRODUCTGROUPBTN);
 CHECKBOX(MESSAGENUMBERBTN);

  // SERVICES
 RADIOBUTTON(VERIFYSERVICE);
 RADIOBUTTON(HOLDCHECKSERVICE);
 RADIOBUTTON(PREAUTHSERVICE);
 RADIOBUTTON(GUARANTEESERVICE);
 RADIOBUTTON(GUARANTEEVERIFYSERVICE);

 // FED RESERVE REGION CHECKBOXES
 CHECKBOX(FEDREG1);
 CHECKBOX(FEDREG2);
 CHECKBOX(FEDREG3);
 CHECKBOX(FEDREG4);
 CHECKBOX(FEDREG5);
 CHECKBOX(FEDREG6);
 CHECKBOX(FEDREG7);
 CHECKBOX(FEDREG8);
 CHECKBOX(FEDREG9);
 CHECKBOX(FEDREG10);
 CHECKBOX(FEDREG11);
 CHECKBOX(FEDREG12);

 // EDIT FIELDS
 EDIT(MERCHANTZIP,6);
 MERCHANTSTATE=new TEditStateCodes(this,IDC_MERCHANTSTATE,3);
 EDIT(MERCHANTNAME,16);
 EDIT(MERCHANTID,0);
 EDIT(MERCHANTSIC,5);
 EDIT(MESSAGENUMBER,5);

 EDITUNS(VERIFYSERVICEAMOUNT,6);
 VERIFYSERVICEAMOUNT->SetValidator(new TRangeValidator(0,USHRT_MAX-1));
 EDITUNS(PREAUTHDAYS,3);
 EDITUNS(PREAUTHAMOUNT,6);
 PREAUTHAMOUNT->SetValidator(new TRangeValidator(0,USHRT_MAX-1));
 EDITUNS(HOLDCHECKAMOUNT,6);
 HOLDCHECKAMOUNT->SetValidator(new TRangeValidator(0,USHRT_MAX-1));
 EDITUNS(HOLDCHECKVALUEPERCENT,4);
 EDITUNS(HOLDCHECK1PERCENT,4);

 if ( stationRec )
  sprintf(Title,"Station %s",stationRec->Data.MerchantID);
 else
  sprintf(Title,"Account %s",accountRec->Data.MerchantID);
 SetCaption(Title);

 // Save Original record for comparing
 memcpy(&origRec.Data,&workingRec->Data,sizeof(origRec.Data));
}

//-----------
// Destructor
//-----------
MerchantDialog::~MerchantDialog()
{
 Destroy(IDCANCEL);
 if ( stationRec )
  delete stationRec;
 if ( accountRec )
  delete accountRec;
 if ( AuthRulesList )
  delete AuthRulesList;
 if ( PCList )
  delete PCList;
}

//-----------------------------------
// Override Setup for initial display
//-----------------------------------
#define ENABLEBTN(ctl,enabler)\
 ctl->EnableWindow(true); SHOW(ctl);\
 ctl->SetCheck(workingRec->isFieldEnabled(MerchantRec::enabler) ?\
                 BF_CHECKED:BF_UNCHECKED)
#define SHOW(ctl)\
 ctl->ModifyStyle(0,WS_VISIBLE)
void MerchantDialog::SetupWindow()
{
 TDialog::SetupWindow();
 int i, Cnt;

 // If it's a Station Record, Enable all of the push buttons and make
 // them visible
 if ( stationRec )
  {
   ENABLEBTN(VFYDATAREQBTN,VFYDataReq);
   ENABLEBTN(VFYPOSFILERULESBTN,VFYPosFileRules);
   ENABLEBTN(GUPOSFILERULESBTN,GUPosFileRules);
   ENABLEBTN(VFYRESPMSGBTN,VFYRespMsg);
   ENABLEBTN(GURESPMSGBTN,GURespMsg);
   ENABLEBTN(VFYUNVFUNDSRULESBTN,VFYUnvFundsRules);
   ENABLEBTN(GUUNVFUNDSRULESBTN,GUUnvFundsRules);
   ENABLEBTN(GUDATAREQBTN,GUDataReq);
   ENABLEBTN(GUAUTHRULESBTN,GUAuthRules);
   ENABLEBTN(VFYAUTHRULESBTN,VFYAuthRules);
   ENABLEBTN(MERCHANTTIMEZONEBTN,MerchantTimeZone);
   ENABLEBTN(MERCHANTSICBTN,MerchantSic);
   ENABLEBTN(MERCHANTZIPBTN,MerchantZip);
   ENABLEBTN(MERCHANTSTATEBTN,MerchantState);
   ENABLEBTN(FEDREGBTN,FedRegions);
   ENABLEBTN(SERVICESBTN,MerchantService);
   ENABLEBTN(PRODUCTGROUPBTN,ProductGroup);
   ENABLEBTN(MESSAGENUMBERBTN,MessageNumber);
  }

 // Output Edit Fields
 DisplayEdit(MERCHANTID,workingRec->Data.MerchantID,-1);
 DisplayEdit(MERCHANTNAME,workingRec->Data.MerchantName,-1);
 DisplayServices();
 DISPLAYEDIT(MERCHANTSIC,MerchantSic);
 DISPLAYEDIT(MERCHANTSTATE,MerchantState);
 DISPLAYEDIT(MERCHANTZIP,MerchantZip);
 DISPLAYEDIT(MESSAGENUMBER,MessageNumber);

 // Init the TimeZone List
 for (i=0, Cnt=timeZoneInfo.NumTimeZones(); i<Cnt; ++i)
  MERCHANTTIMEZONE->AddString(timeZoneInfo.GetInfo(i));
 DISPLAYTIMEZONE;

 // Display the Fed Reserve Regions
 DisplayFedRegions();
 SetupComplete = true;
}


//------------------------------------------------------------
// Do the window creation, initializing all the lists from the
// database. Abort if fail to get any lists.
//------------------------------------------------------------
bool MerchantDialog::Create()
{
 char *Ptr;

 if ( ! FormDialog::Create() )
  return false;
 StringList* DataReqList;
 StringList* UnvFundsRulesList;
 StringList* PosResponseList;
 StringList* PosFileRulesList;

 // Initialize the Lists and Abort the dialog if we can't get any
 // of them.

 AuthRulesList=new StringTreeList(AUTHRULES_LIST);
 if ( AuthRulesList->GetInitFail() )
   return false;

 UnvFundsRulesList=new StringList(UNVFUNDSRULES_LIST);
 if ( UnvFundsRulesList->GetInitFail() )
   return false;

 DataReqList=new StringList(DATAREQ_LIST);
 if ( DataReqList->GetInitFail() )
   return false;

 PosResponseList=new StringList(POSRESPONSE_LIST);
 if ( PosResponseList->GetInitFail() )
  return false;

 PosFileRulesList=new StringList(POSRULES_LIST);
 if ( PosFileRulesList->GetInitFail() )
  return false;

 // Init Product Groups Combo Box
 PCList = new ProductClassList;
 if ( PCList->GetInitFail() )
   return false;

 PCList->InitComboBoxClasses(PRODUCTGROUP,true);
 DisplayProductGroup();

 // Init Auth Rules Combo Boxes

 AuthRulesList->Next(true);
 // Add all the Class Names
 while( (Ptr=AuthRulesList->Next()) != NULL )
   if ( Ptr[0]!= 'C' )
    {
     VFYAUTHRULESCLASS->AddString(&Ptr[1]);
     GUAUTHRULESCLASS->AddString(&Ptr[1]);
    }
 DISPLAYCLASS(VFYAUTHRULESCLASS,VFYAuthRules,VFYAuthRules);
 DISPLAYCLASS(GUAUTHRULESCLASS,GUAuthRules,GUAuthRules);
 INITSUBCLASS(VFYAUTHRULESSUBCLASS,VFYAuthRules,VFYAuthRules);
 DISPLAYCLASS(VFYAUTHRULESSUBCLASS,VFYAuthRulesSub,VFYAuthRules);
 INITSUBCLASS(GUAUTHRULESSUBCLASS,GUAuthRules,GUAuthRules);
 DISPLAYCLASS(GUAUTHRULESSUBCLASS,GUAuthRulesSub,GUAuthRules);

 // Init Data Requirements Combo Boxes
 DataReqList->Next(true);
 while( (Ptr=DataReqList->Next()) != NULL )
  {
   VFYDATAREQ->AddString(Ptr);
   GUDATAREQ->AddString(Ptr);
  }
 DISPLAYCLASS(VFYDATAREQ,VFYDataReq,VFYDataReq);
 DISPLAYCLASS(GUDATAREQ,GUDataReq,GUDataReq);
 delete DataReqList;

 // Init UnvFundsRules Combo Boxes
 UnvFundsRulesList->Next(true);
 while( (Ptr=UnvFundsRulesList->Next()) != NULL )
  {
   VFYUNVFUNDSRULES->AddString(Ptr);
   GUUNVFUNDSRULES->AddString(Ptr);
  }
 DISPLAYCLASS(VFYUNVFUNDSRULES,VFYUnvFundsRules,VFYUnvFundsRules);
 DISPLAYCLASS(GUUNVFUNDSRULES,GUUnvFundsRules,GUUnvFundsRules);
 delete UnvFundsRulesList;

 // Init Pos Response Message Combo Boxes
 PosResponseList->Next(true);
 while( (Ptr=PosResponseList->Next()) != NULL )
  {
   VFYRESPMSG->AddString(Ptr);
   GURESPMSG->AddString(Ptr);
  }
 DISPLAYCLASS(VFYRESPMSG,VFYRespMsg,VFYRespMsg);
 DISPLAYCLASS(GURESPMSG,GURespMsg,GURespMsg);
 delete PosResponseList;

 // Init Posfile Rules Combo Boxes
 PosFileRulesList->Next(true);
 while( (Ptr=PosFileRulesList->Next()) != NULL )
  {
   VFYPOSFILERULES->AddString(Ptr);
   GUPOSFILERULES->AddString(Ptr);
  }
 DISPLAYCLASS(VFYPOSFILERULES,VFYPosFileRules,VFYPosFileRules);
 DISPLAYCLASS(GUPOSFILERULES,GUPosFileRules,GUPosFileRules);
 delete PosFileRulesList;

 EnableByService();

 return true;
}

//-------------------------------------------------------------
// Get a pointer to the correct MerchantRec object based on the
// Value of a field enabler (ie., either account or station).
//-------------------------------------------------------------
MerchantRec* MerchantDialog::GetRec(int Enabler)
{
 // If there is no station record then it's always the account
 // record 
 if ( ! stationRec )
  return accountRec;
 else
  // Use station record if enabled
  if ( stationRec->isFieldEnabled(Enabler) )
    return stationRec;
  else
    return accountRec;
}

//-----------------------------------------------------------
// Init the SubClass box for all subclasses of a Parent Class
//-----------------------------------------------------------
void MerchantDialog::InitSubClassBox(TComboBox* Box,char *Class)
{
 char *Ptr;
 int Count=0;

 Box->Clear();
 Box->ClearList();
 AuthRulesList->Next(true);
 while( (Ptr=AuthRulesList->Next()) != NULL &&
        ! ( Ptr[0] == 'P' &&
            strcmp(Class,&Ptr[1]) == 0)  );
 if ( Ptr==NULL )
  return;
 while( (Ptr=AuthRulesList->Next()) != NULL &&
       Ptr[0]=='C' )
   {
    Box->AddString(&Ptr[1]);
    ++Count;
   }

 if ( Count )
  Box->AddString("");  
}



//-----------------------------------------------------
// Catch changes to fields and call appropriate handler
//-----------------------------------------------------
TResult MerchantDialog::EvCommand(uint id, THandle hWndCtl, uint notifyCode)
{
  TResult result;

  result = FormDialog::EvCommand(id, hWndCtl, notifyCode);

  switch ( notifyCode )
   {
    case CBN_SELCHANGE:  if ( SetupComplete )
                           BoxChange(id);
                         break;

    case BN_CLICKED:     if ( SetupComplete )
                           BTNChange(id);
                         break;

    case EN_CHANGE:      if ( SetupComplete )
                           EditChange(id);
                         break;
   }

  return result;
}

//------------------------------------
// Handle a Combo Box Selection Change
//------------------------------------
#define TRANSFERCLASS(name,fld,enabler) case IDC_##name:\
 if ( workingRec->isFieldEnabled(MerchantRec::enabler) )\
   name->GetString(workingRec->Data.Classes.fld,name->GetSelIndex())
void MerchantDialog::BoxChange(uint id)
{
 switch(id)
  {
  TRANSFERCLASS(GUPOSFILERULES,GUPosFileRules,GUPosFileRules); break;
  TRANSFERCLASS(VFYPOSFILERULES,VFYPosFileRules,VFYPosFileRules); break;

  TRANSFERCLASS(GURESPMSG,GURespMsg,GURespMsg); break;
  TRANSFERCLASS(VFYRESPMSG,VFYRespMsg,VFYRespMsg); break;

  TRANSFERCLASS(VFYUNVFUNDSRULES,VFYUnvFundsRules,VFYUnvFundsRules); break;
  TRANSFERCLASS(GUUNVFUNDSRULES,GUUnvFundsRules,GUUnvFundsRules); break;
  TRANSFERCLASS(VFYDATAREQ,VFYDataReq,VFYDataReq); break;
  TRANSFERCLASS(VFYAUTHRULESSUBCLASS,VFYAuthRulesSub,VFYAuthRules); break;
  TRANSFERCLASS(GUDATAREQ,GUDataReq,GUDataReq); break;
  TRANSFERCLASS(GUAUTHRULESSUBCLASS,GUAuthRulesSub,GUAuthRules); break;

  TRANSFERCLASS(GUAUTHRULESCLASS,GUAuthRules,GUAuthRules);
    INITSUBCLASS(GUAUTHRULESSUBCLASS,GUAuthRules,GUAuthRules);
    // Change of class means a NULLing of SUBCLASS unless one
    // is selected.
    memset(workingRec->Data.Classes.GUAuthRulesSub,0,
              sizeof(workingRec->Data.Classes.GUAuthRulesSub));
    DISPLAYCLASS(GUAUTHRULESSUBCLASS,GUAuthRulesSub,GUAuthRules);
    break;

  TRANSFERCLASS(VFYAUTHRULESCLASS,VFYAuthRules,VFYAuthRules);
    INITSUBCLASS(VFYAUTHRULESSUBCLASS,VFYAuthRules,VFYAuthRules);
    memset(workingRec->Data.Classes.VFYAuthRulesSub,0,
              sizeof(workingRec->Data.Classes.VFYAuthRulesSub));
    DISPLAYCLASS(VFYAUTHRULESSUBCLASS,VFYAuthRulesSub,VFYAuthRules);
    break;

  case IDC_MERCHANTTIMEZONE:
   if ( workingRec->isFieldEnabled(MerchantRec::MerchantTimeZone) )
    workingRec->Data.MerchantTimeZone=(BYTE)MERCHANTTIMEZONE->GetSelIndex();

  case IDC_PRODUCTGROUP:
   if ( workingRec->isFieldEnabled(MerchantRec::ProductGroup) )
    workingRec->Data.ProductGroup =
      (BYTE) PRODUCTGROUP->GetItemData( PRODUCTGROUP->GetSelIndex() );
   break;
 }
}

//---------------------------------
// Handle change to an Edit Control
//---------------------------------
#define TRANSFER(CtlName,FldName) case IDC_##CtlName:\
  if ( workingRec->isFieldEnabled(MerchantRec::FldName) )\
    CtlName->Transfer(workingRec->Data.FldName,tdGetData); break
void MerchantDialog::EditChange(uint id)
{
 switch(id)
  {
   TRANSFER(MERCHANTSIC,MerchantSic);
   TRANSFER(MERCHANTZIP,MerchantZip);
   TRANSFER(MERCHANTSTATE,MerchantState);
   TRANSFER(MESSAGENUMBER,MessageNumber);

   case IDC_MERCHANTNAME:
        MERCHANTNAME->Transfer(workingRec->Data.MerchantName,tdGetData);
        break;

   case IDC_VERIFYSERVICEAMOUNT:
         if ( workingRec->isFieldEnabled(MerchantRec::MerchantService) )
           VERIFYSERVICEAMOUNT->Get(workingRec->Data.VerifyAmount);
         break;

   case IDC_PREAUTHDAYS:
         if ( workingRec->isFieldEnabled(MerchantRec::MerchantService) )
           PREAUTHDAYS->Get(workingRec->Data.PreauthDays);
         break;

   case IDC_PREAUTHAMOUNT:
         if ( workingRec->isFieldEnabled(MerchantRec::MerchantService) )
           PREAUTHAMOUNT->Get(workingRec->Data.PreauthAmount);
         break;

   case IDC_HOLDCHECKAMOUNT:
         if ( workingRec->isFieldEnabled(MerchantRec::MerchantService) )
           HOLDCHECKAMOUNT->Get(workingRec->Data.HoldCheckAmount);
         break;

   case IDC_HOLDCHECKVALUEPERCENT:
         if ( workingRec->isFieldEnabled(MerchantRec::MerchantService) )
           HOLDCHECKVALUEPERCENT->Get(workingRec->Data.HoldCheckValuePercent);
         break;

   case IDC_HOLDCHECK1PERCENT:
         if ( workingRec->isFieldEnabled(MerchantRec::MerchantService) )
           HOLDCHECK1PERCENT->Get(workingRec->Data.HoldCheck1Percent);
         break;

  }
}

//---------------------------
// Handle click of a checkbox
//---------------------------
#define TRANSFERREG(n) case IDC_FEDREG##n:\
  if ( workingRec->isFieldEnabled(MerchantRec::FedRegions) )\
    workingRec->selectRegion(n,FEDREG##n->GetCheck()==BF_CHECKED); break
#define TRANSFERSVC(name,svc) case IDC_##name:\
  if ( workingRec->isFieldEnabled(MerchantRec::MerchantService) )\
   {\
    workingRec->Data.MerchantService=svc;\
    EnableByService();\
   } break
#define TRANSFERSVCX(name,svc,focusctl) case IDC_##name:\
  if ( workingRec->isFieldEnabled(MerchantRec::MerchantService) )\
   {\
    workingRec->Data.MerchantService=svc;\
    EnableByService();\
    if ( name->GetCheck() == BF_CHECKED )\
     { focusctl->SetFocus(); focusctl->SetSelection(0,10); }\
   } break
#define TRANSFERENABLER(name,fld) case IDC_##name:\
  workingRec->enableField(MerchantRec::fld,name->GetCheck()==BF_CHECKED)
void MerchantDialog::BTNChange(uint id)
{
 bool isChk;

 switch(id)
  {
  TRANSFERENABLER(VFYDATAREQBTN,VFYDataReq);
    if ( (isChk=(VFYDATAREQBTN->GetCheck() == BF_CHECKED)) == true )
     strcpy(stationRec->Data.Classes.VFYDataReq,
            accountRec->Data.Classes.VFYDataReq);
    DISPLAYCLASS(VFYDATAREQ,VFYDataReq,VFYDataReq);
    if ( isChk )
      VFYDATAREQ->SetFocus();
    break;

  TRANSFERENABLER(GUDATAREQBTN,GUDataReq);
    if ( (isChk=(GUDATAREQBTN->GetCheck() == BF_CHECKED)) == true )
     strcpy(stationRec->Data.Classes.GUDataReq,
            accountRec->Data.Classes.GUDataReq);
    DISPLAYCLASS(GUDATAREQ,GUDataReq,GUDataReq);
    if ( isChk )
      GUDATAREQ->SetFocus();
    break;

  TRANSFERENABLER(VFYRESPMSGBTN,VFYRespMsg);
    if ( (isChk=(VFYRESPMSGBTN->GetCheck() == BF_CHECKED)) == true )
     strcpy(stationRec->Data.Classes.VFYRespMsg,
            accountRec->Data.Classes.VFYRespMsg);
    DISPLAYCLASS(VFYRESPMSG,VFYRespMsg,VFYRespMsg);
    if ( isChk )
      VFYRESPMSG->SetFocus();
    break;

  TRANSFERENABLER(GURESPMSGBTN,GURespMsg);
    if ( (isChk=(GURESPMSGBTN->GetCheck() == BF_CHECKED)) == true )
     strcpy(stationRec->Data.Classes.GURespMsg,
            accountRec->Data.Classes.GURespMsg);
    DISPLAYCLASS(GURESPMSG,GURespMsg,GURespMsg);
    if ( isChk )
      GURESPMSG->SetFocus();
    break;

  TRANSFERENABLER(VFYUNVFUNDSRULESBTN,VFYUnvFundsRules);
    if ( (isChk=(VFYUNVFUNDSRULESBTN->GetCheck() == BF_CHECKED)) == true )
     strcpy(stationRec->Data.Classes.VFYUnvFundsRules,
            accountRec->Data.Classes.VFYUnvFundsRules);
    DISPLAYCLASS(VFYUNVFUNDSRULES,VFYUnvFundsRules,VFYUnvFundsRules);
    if ( isChk )
      VFYUNVFUNDSRULES->SetFocus();
    break;

  TRANSFERENABLER(GUUNVFUNDSRULESBTN,GUUnvFundsRules);
    if ( (isChk=(GUUNVFUNDSRULESBTN->GetCheck() == BF_CHECKED)) == true )
     strcpy(stationRec->Data.Classes.GUUnvFundsRules,
            accountRec->Data.Classes.GUUnvFundsRules);
    DISPLAYCLASS(GUUNVFUNDSRULES,GUUnvFundsRules,GUUnvFundsRules);
    if ( isChk )
      GUUNVFUNDSRULES->SetFocus();
    break;

  TRANSFERENABLER(VFYAUTHRULESBTN,VFYAuthRules);
    if ( (isChk=(VFYAUTHRULESBTN->GetCheck() == BF_CHECKED)) == true )
     {
      strcpy(stationRec->Data.Classes.VFYAuthRules,
             accountRec->Data.Classes.VFYAuthRules);
      strcpy(stationRec->Data.Classes.VFYAuthRulesSub,
             accountRec->Data.Classes.VFYAuthRulesSub);
     }
    DISPLAYCLASS(VFYAUTHRULESCLASS,VFYAuthRules,VFYAuthRules);
    DISPLAYCLASS(VFYAUTHRULESSUBCLASS,VFYAuthRulesSub,VFYAuthRules);
    if ( isChk )
      VFYAUTHRULESCLASS->SetFocus();
    break;

  TRANSFERENABLER(GUAUTHRULESBTN,GUAuthRules);
    if ( (isChk=(GUAUTHRULESBTN->GetCheck() == BF_CHECKED)) == true )
     {
      strcpy(stationRec->Data.Classes.GUAuthRules,
             accountRec->Data.Classes.GUAuthRules);
      strcpy(stationRec->Data.Classes.GUAuthRulesSub,
             accountRec->Data.Classes.GUAuthRulesSub);
     }
    DISPLAYCLASS(GUAUTHRULESCLASS,GUAuthRules,GUAuthRules);
    DISPLAYCLASS(GUAUTHRULESSUBCLASS,GUAuthRulesSub,GUAuthRules);
    if ( isChk )
      GUAUTHRULESCLASS->SetFocus();
    break;

  TRANSFERENABLER(MERCHANTTIMEZONEBTN,MerchantTimeZone);
    if ( (isChk=(MERCHANTTIMEZONEBTN->GetCheck() == BF_CHECKED)) == true )
     stationRec->Data.MerchantTimeZone=accountRec->Data.MerchantTimeZone;
    DISPLAYTIMEZONE;
    if ( isChk )
      MERCHANTTIMEZONE->SetFocus();
    break;

  TRANSFERENABLER(PRODUCTGROUPBTN,ProductGroup);
    if ( (isChk=(PRODUCTGROUPBTN->GetCheck() == BF_CHECKED)) == true )
     stationRec->Data.ProductGroup=accountRec->Data.ProductGroup;
    DisplayProductGroup();
    if ( isChk )
      PRODUCTGROUP->SetFocus();
    break;


  TRANSFERENABLER(MERCHANTSICBTN,MerchantSic);
    if ( (isChk=(MERCHANTSICBTN->GetCheck() == BF_CHECKED)) == true )
     strcpy(stationRec->Data.MerchantSic,accountRec->Data.MerchantSic);
    DISPLAYEDIT(MERCHANTSIC,MerchantSic);
    if ( isChk )
     {
      MERCHANTSIC->SetFocus();
      MERCHANTSIC->SetSelection(0,4);
     }
    break;

  TRANSFERENABLER(MESSAGENUMBERBTN,MessageNumber);
    if ( (isChk=(MESSAGENUMBERBTN->GetCheck() == BF_CHECKED)) == true )
     strcpy(stationRec->Data.MessageNumber,accountRec->Data.MessageNumber);
    DISPLAYEDIT(MESSAGENUMBER,MessageNumber);
    if ( isChk )
     {
      MESSAGENUMBER->SetFocus();
      MESSAGENUMBER->SetSelection(0,4);
     }
    break;

  TRANSFERENABLER(MERCHANTZIPBTN,MerchantZip);
    if ( (isChk=(MERCHANTZIPBTN->GetCheck() == BF_CHECKED)) == true )
     strcpy(stationRec->Data.MerchantZip,accountRec->Data.MerchantZip);
    DISPLAYEDIT(MERCHANTZIP,MerchantZip);
    if ( isChk )
     {
      MERCHANTZIP->SetFocus();
      MERCHANTZIP->SetSelection(0,5);
     }
    break;

  TRANSFERENABLER(MERCHANTSTATEBTN,MerchantState);
    if ( (isChk=(MERCHANTSTATEBTN->GetCheck() == BF_CHECKED)) == true )
     strcpy(stationRec->Data.MerchantState,accountRec->Data.MerchantState);
    DISPLAYEDIT(MERCHANTSTATE,MerchantState);
    if ( isChk )
     {
      MERCHANTSTATE->SetFocus();
      MERCHANTSTATE->SetSelection(0,2);
     }
    break;

  TRANSFERENABLER(FEDREGBTN,FedRegions);
    if ( FEDREGBTN->GetCheck() == BF_CHECKED )
     stationRec->Data.FedRegionBits = accountRec->Data.FedRegionBits;
    DisplayFedRegions();
    break;

  TRANSFERENABLER(SERVICESBTN,MerchantService);
    if ( (isChk=(SERVICESBTN->GetCheck() == BF_CHECKED)) == true )
     {
      stationRec->Data.MerchantService=accountRec->Data.MerchantService;
      stationRec->Data.VerifyAmount=accountRec->Data.VerifyAmount;
     }
    DisplayServices();
    if ( isChk && workingRec->Data.MerchantService==CHECKSERVICE_GUARANTEEVERIFY )
     {
      VERIFYSERVICEAMOUNT->SetFocus();
      VERIFYSERVICEAMOUNT->SetSelection(0,6);
     }
    break;

  TRANSFERENABLER(VFYPOSFILERULESBTN,VFYPosFileRules);
    if ( (isChk=(VFYPOSFILERULESBTN->GetCheck() == BF_CHECKED)) == true )
     strcpy(stationRec->Data.Classes.VFYPosFileRules,
            accountRec->Data.Classes.VFYPosFileRules);
    DISPLAYCLASS(VFYPOSFILERULES,VFYPosFileRules,VFYPosFileRules);
    if ( isChk )
      VFYPOSFILERULES->SetFocus();
    break;

  TRANSFERENABLER(GUPOSFILERULESBTN,GUPosFileRules);
    if ( (isChk=(GUPOSFILERULESBTN->GetCheck() == BF_CHECKED)) == true )
     strcpy(stationRec->Data.Classes.GUPosFileRules,
            accountRec->Data.Classes.GUPosFileRules);
    DISPLAYCLASS(GUPOSFILERULES,GUPosFileRules,GUPosFileRules);
    if ( isChk )
      GUPOSFILERULES->SetFocus();
    break;

  TRANSFERSVC(VERIFYSERVICE,CHECKSERVICE_VERIFY);
  TRANSFERSVCX(PREAUTHSERVICE,CHECKSERVICE_PREAUTH,PREAUTHAMOUNT);
  TRANSFERSVCX(HOLDCHECKSERVICE,CHECKSERVICE_HOLDCHECK,HOLDCHECKAMOUNT);
  TRANSFERSVC(GUARANTEESERVICE,CHECKSERVICE_GUARANTEE);
  TRANSFERSVCX(GUARANTEEVERIFYSERVICE,CHECKSERVICE_GUARANTEEVERIFY,
                VERIFYSERVICEAMOUNT);

  // Federal Reserve regions
  TRANSFERREG(1);
  TRANSFERREG(2);
  TRANSFERREG(3);
  TRANSFERREG(4);
  TRANSFERREG(5);
  TRANSFERREG(6);
  TRANSFERREG(7);
  TRANSFERREG(8);
  TRANSFERREG(9);
  TRANSFERREG(10);
  TRANSFERREG(11);
  TRANSFERREG(12);
 }
}

//---------------------------------------------------
// Display an Edit Field and Enable/Disable the field
//---------------------------------------------------
void MerchantDialog::DisplayEdit(TEdit* Ctl,char* Field,int Enabler)
{
 Ctl->Transfer(Field,tdSetData);
 if ( Enabler==-1 )
  return;

 Ctl->EnableWindow( workingRec->isFieldEnabled(Enabler) );
}

//-----------------------------------------------------------
// Display a Combo Box Selection and Enable/Disable the field
//-----------------------------------------------------------
void MerchantDialog::DisplayBox(TComboBox* Ctl,char* Field,int Enabler)
{
 Ctl->SetText(Field);
 Ctl->EnableWindow(workingRec->isFieldEnabled(Enabler));
}

//------------------------------------------------------------------
// Display the Federal Reserve Regions and Enable/Disable the fields
//------------------------------------------------------------------
#define CHECKREG(n) FEDREG##n->SetCheck( Rec->isRegionSelected(n) ? \
                      BF_CHECKED : BF_UNCHECKED );\
                    FEDREG##n->EnableWindow(Enabled)
void MerchantDialog::DisplayFedRegions(void)
{
 bool Enabled=workingRec->isFieldEnabled(MerchantRec::FedRegions);
 MerchantRec* Rec=GetRec(MerchantRec::FedRegions);
 CHECKREG(1);
 CHECKREG(2);
 CHECKREG(3);
 CHECKREG(4);
 CHECKREG(5);
 CHECKREG(6);
 CHECKREG(7);
 CHECKREG(8);
 CHECKREG(9);
 CHECKREG(10);
 CHECKREG(11);
 CHECKREG(12);
}

//--------------------------------------------------------
// Display Merchant Services and Enable/Disable the fields
//--------------------------------------------------------
#define CHECKSVC(CtlName,FldName)\
  CtlName->SetCheck( Rec->isServiceEnabled(MerchantRec::FldName) ? \
                      BF_CHECKED : BF_UNCHECKED );\
  CtlName->EnableWindow(Enabled)
void MerchantDialog::DisplayServices(void)
{
 MerchantRec* Rec=GetRec(MerchantRec::MerchantService);

 // 1st Uncheck them all
 GUARANTEESERVICE->SetCheck(BF_UNCHECKED);
 VERIFYSERVICE->SetCheck(BF_UNCHECKED);
 HOLDCHECKSERVICE->SetCheck(BF_UNCHECKED);
 PREAUTHSERVICE->SetCheck(BF_UNCHECKED);
 GUARANTEEVERIFYSERVICE->SetCheck(BF_UNCHECKED);

 switch( Rec->Data.MerchantService )
  {
   default:
   case CHECKSERVICE_GUARANTEE:  GUARANTEESERVICE->Check(); break;
   case CHECKSERVICE_VERIFY:     VERIFYSERVICE->Check(); break;
   case CHECKSERVICE_HOLDCHECK:
          HOLDCHECKSERVICE->Check();
          HOLDCHECKAMOUNT->Set(Rec->Data.HoldCheckAmount);
          HOLDCHECKVALUEPERCENT->Set(Rec->Data.HoldCheckValuePercent);
          HOLDCHECK1PERCENT->Set(Rec->Data.HoldCheck1Percent);
          break;
   case CHECKSERVICE_PREAUTH:
         PREAUTHSERVICE->Check();
         PREAUTHDAYS->Set(Rec->Data.PreauthDays);
         PREAUTHAMOUNT->Set(Rec->Data.PreauthAmount);
         break; 
   case CHECKSERVICE_GUARANTEEVERIFY:
          GUARANTEEVERIFYSERVICE->Check();
          VERIFYSERVICEAMOUNT->Set(Rec->Data.VerifyAmount);
          break;
  }

 EnableByService();
}

//------------------------------------------------------------
// Display the Merchant Time Zone and Enable/Disable the field
//------------------------------------------------------------
void MerchantDialog::DisplayTimeZone(int Value)
{
 MERCHANTTIMEZONE->SetSelIndex(Value);
 MERCHANTTIMEZONE->EnableWindow(
   workingRec->isFieldEnabled(MerchantRec::MerchantTimeZone));
}

//-------------------------------------------------------
// Display the Product Group and Enable/Disable the field
//-------------------------------------------------------
void MerchantDialog::DisplayProductGroup()
{
 PCList->SetComboBoxClass(PRODUCTGROUP,
   GetRec(MerchantRec::ProductGroup)->Data.ProductGroup);
 PRODUCTGROUP->EnableWindow(
   workingRec->isFieldEnabled(MerchantRec::ProductGroup));
}


//--------------------------------------------------------------
// Enable/Disable Fields based on the currently selected service
//--------------------------------------------------------------
#define ENABLEBT(Ctl,service) Ctl->EnableWindow(service);\
 Ctl->ShowWindow(service ? SW_SHOW : SW_HIDE)

#define ENABLEFLD(Ctl,enabler,service)\
 Ctl->EnableWindow(workingRec->isFieldEnabled(MerchantRec::enabler) && service);

void MerchantDialog::EnableByService(void)
{
 BYTE Service=GetRec(MerchantRec::MerchantService)->Data.MerchantService;
 bool isGuarantee = (Service==CHECKSERVICE_VERIFY) ? false : true;
 bool isVerify = (Service==CHECKSERVICE_VERIFY ||
                  Service==CHECKSERVICE_GUARANTEEVERIFY) ? true : false;
 bool SVCEnabled=workingRec->isFieldEnabled(MerchantRec::MerchantService);

 //Enable/Disable the Service Buttons
 VERIFYSERVICE->EnableWindow(SVCEnabled);
 GUARANTEESERVICE->EnableWindow(SVCEnabled);
 HOLDCHECKSERVICE->EnableWindow(SVCEnabled);
 PREAUTHSERVICE->EnableWindow(SVCEnabled);
 GUARANTEEVERIFYSERVICE->EnableWindow(SVCEnabled);

 ENABLEFLD(VERIFYSERVICEAMOUNT,MerchantService,
           isVerify && Service==CHECKSERVICE_GUARANTEEVERIFY);
 ENABLEFLD(PREAUTHDAYS,MerchantService,
           Service==CHECKSERVICE_PREAUTH);
 ENABLEFLD(PREAUTHAMOUNT,MerchantService,
           Service==CHECKSERVICE_PREAUTH);
 ENABLEFLD(HOLDCHECKAMOUNT,MerchantService,
           Service==CHECKSERVICE_HOLDCHECK);
 ENABLEFLD(HOLDCHECKVALUEPERCENT,MerchantService,
           Service==CHECKSERVICE_HOLDCHECK);
 ENABLEFLD(HOLDCHECK1PERCENT,MerchantService,
           Service==CHECKSERVICE_HOLDCHECK);
 ENABLEFLD(VFYUNVFUNDSRULES,VFYUnvFundsRules,isVerify);
 ENABLEFLD(GUUNVFUNDSRULES,GUUnvFundsRules,isGuarantee);
 ENABLEFLD(VFYRESPMSG,VFYRespMsg,isVerify);
 ENABLEFLD(GURESPMSG,GURespMsg,isGuarantee);
 ENABLEFLD(VFYDATAREQ,VFYDataReq,isVerify);
 ENABLEFLD(VFYAUTHRULESSUBCLASS,VFYAuthRules,isVerify);
 ENABLEFLD(VFYAUTHRULESCLASS,VFYAuthRules,isVerify);
 ENABLEFLD(GUDATAREQ,GUDataReq,isGuarantee);
 ENABLEFLD(GUAUTHRULESSUBCLASS,GUAuthRules,isGuarantee);
 ENABLEFLD(GUAUTHRULESCLASS,GUAuthRules,isGuarantee);
 ENABLEFLD(VFYPOSFILERULES,VFYPosFileRules,isVerify);
 ENABLEFLD(GUPOSFILERULES,GUPosFileRules,isGuarantee);

 if ( stationRec )
  {
   ENABLEBT(VFYDATAREQBTN,isVerify);
   ENABLEBT(VFYUNVFUNDSRULESBTN,isVerify);
   ENABLEBT(VFYRESPMSGBTN,isVerify);
   ENABLEBT(VFYAUTHRULESBTN,isVerify);
   ENABLEBT(VFYPOSFILERULESBTN,isVerify);
   ENABLEBT(GUUNVFUNDSRULESBTN,isGuarantee);
   ENABLEBT(GURESPMSGBTN,isGuarantee);
   ENABLEBT(GUDATAREQBTN,isGuarantee);
   ENABLEBT(GUAUTHRULESBTN,isGuarantee);
   ENABLEBT(GUPOSFILERULESBTN,isGuarantee);
  }
}

//----------------------------------------------------------------
// Return the Modified status by comparing the original rules with
// the working rules.
//----------------------------------------------------------------
bool MerchantDialog::IsModified()
{
 if ( FormDialog::IsModified() )
  return true;
 else
  return (memcmp(&workingRec->Data,&origRec.Data,
                sizeof(workingRec->Data))==0) ? false : true;
}

//-----------------------------
// Handle the Save menu command
//-----------------------------
bool MerchantDialog::Save()
{
 DBManMessage Msg;

 if ( ! Validator() )
  return false;

 if ( ! Msg.DBPut(this,T_PUTMERCHANTREC,FN_MERCHANTRECORD,workingRec) )
  return false;

 ResetModified();
 return true;
}

//----------------------------------
// Handle the Save Temp menu command
//----------------------------------
bool MerchantDialog::SaveTemp(TFile* File)
{
 SAVERECORD_SENTINEL Sentinel;

 Sentinel.Type = GetType();
 Sentinel.Version = SaveRecordVersion[Sentinel.Type];
 Sentinel.Len = sizeof(workingRec->Data);
 if ( stationRec )
  Sentinel.Len += sizeof(stationRec->Data);

 if ( File->Write(&Sentinel,sizeof(Sentinel)) != sizeof(Sentinel) ||
      File->Write(&workingRec->Data,sizeof(workingRec->Data))
            != sizeof(workingRec->Data) )
  return false;

 if ( stationRec &&
      File->Write(&accountRec->Data,sizeof(accountRec->Data)) !=
        sizeof(accountRec->Data) )
  return false;


 ResetModified();
 return true;
}

void MerchantDialog::ResetModified()
{
 memcpy(&origRec.Data,&workingRec->Data,sizeof(origRec.Data));
 FormDialog::ResetModified();
}

bool MerchantDialog::Validator()
{
 if ( strlen(workingRec->Data.MerchantName) <= 5 )
  {
   SetTop();
   MessageBox("NAME MUST BE > 5 CHARACTERS");
   MERCHANTNAME->SetFocus();
   return false;
  }
 if ( workingRec->isFieldEnabled(MerchantRec::MerchantSic) &&
      strlen(workingRec->Data.MerchantSic) != 4 )
  {
   SetTop();
   MessageBox("A 4 DIGIT SIC CODE IS REQUIRED");
   MERCHANTSIC->SetFocus();
   return false;
  }
 if ( workingRec->isFieldEnabled(MerchantRec::MerchantZip) &&
      strlen(workingRec->Data.MerchantZip) == 0 )
  {
   SetTop();
   MessageBox("A ZIP CODE IS REQUIRED");
   MERCHANTZIP->SetFocus();
   return false;
  }
 if ( workingRec->isFieldEnabled(MerchantRec::MerchantService) &&
      workingRec->Data.MerchantService==CHECKSERVICE_GUARANTEEVERIFY &&
      ( isEmpty(workingRec->Data.VerifyAmount) ||
        workingRec->Data.VerifyAmount==0 ) )
  {
   SetTop();
   MessageBox("AN AMOUNT IS REQUIRED FOR GUARANTEE/VERIFY");
   VERIFYSERVICEAMOUNT->SetFocus();
   return false;
  }

 if ( workingRec->isFieldEnabled(MerchantRec::MerchantService) &&
      workingRec->Data.MerchantService==CHECKSERVICE_PREAUTH &&
      ( isEmpty(workingRec->Data.PreauthDays) ||
        workingRec->Data.PreauthDays==0 ) )
  {
   SetTop();
   MessageBox("NUMBER OF DAYS IS REQUIRED FOR PREAUTH");
   PREAUTHDAYS->SetFocus();
   return false;
  }

 if ( workingRec->isFieldEnabled(MerchantRec::MerchantService) &&
      workingRec->Data.MerchantService==CHECKSERVICE_PREAUTH &&
      ( isEmpty(workingRec->Data.PreauthAmount) ||
        workingRec->Data.PreauthAmount==0 ) )
  {
   SetTop();
   MessageBox("AVERAGE TICKET AMOUNT REQUIRED FOR PREAUTH");
   PREAUTHAMOUNT->SetFocus();
   return false;
  }

 if ( workingRec->isFieldEnabled(MerchantRec::MerchantService) &&
      workingRec->Data.MerchantService==CHECKSERVICE_HOLDCHECK &&
      ( isEmpty(workingRec->Data.HoldCheckAmount) ||
        workingRec->Data.HoldCheckAmount==0 ) )
  {
   SetTop();
   MessageBox("MAXIMUM AMOUNT REQUIRED FOR HOLDCHECK");
   HOLDCHECKAMOUNT->SetFocus();
   return false;
  }

 if ( workingRec->isFieldEnabled(MerchantRec::MerchantService) &&
      workingRec->Data.MerchantService==CHECKSERVICE_HOLDCHECK &&
      ( isEmpty(workingRec->Data.HoldCheckValuePercent) ||
        workingRec->Data.HoldCheckValuePercent==0 ) )
  {
   SetTop();
   MessageBox("MAX % OF VALUE REQUIRED FOR HOLDCHECK");
   HOLDCHECKVALUEPERCENT->SetFocus();
   return false;
  }

 if ( workingRec->isFieldEnabled(MerchantRec::MerchantService) &&
      workingRec->Data.MerchantService==CHECKSERVICE_HOLDCHECK &&
      ( isEmpty(workingRec->Data.HoldCheck1Percent) ||
        workingRec->Data.HoldCheck1Percent==0 ) )
  {
   SetTop();
   MessageBox("MIN % OF 1ST CHECK REQUIRED FOR HOLDCHECK");
   HOLDCHECK1PERCENT->SetFocus();
   return false;
  }

 return true;
}

bool MerchantDialog::Validate()
{
 if ( ! VERIFYSERVICEAMOUNT->IsValid() )
  {
   VERIFYSERVICEAMOUNT->ValidatorError();
   VERIFYSERVICEAMOUNT->SetFocus();
   return false;
  }

 if ( ! PREAUTHAMOUNT->IsValid() )
  {
   PREAUTHAMOUNT->ValidatorError();
   PREAUTHAMOUNT->SetFocus();
   return false;
  }

 if ( ! HOLDCHECKAMOUNT->IsValid() )
  {
   HOLDCHECKAMOUNT->ValidatorError();
   HOLDCHECKAMOUNT->SetFocus();
   return false;
  }

  return true;
}

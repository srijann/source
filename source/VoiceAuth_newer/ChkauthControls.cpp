//----------------------------------------------------------------------------
//  Project VoiceAuth
//
//  Copyright © 1996. All Rights Reserved.
//
//  FILE:         CheckAuthControls.cpp
//  AUTHOR:       Jack Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of CheckAuthControls
//
//  This is a member function of TCheckAuthDialog to set up all the
//  controls. It's called from the class constructor.
//----------------------------------------------------------------------------

#include "win4me.h"
#include "chkauth.h"
#include "chkauthdialog.h"
#include "decimalvalidator.h"
#include "EditStateCodes.h"
#include "MsgId.h"

#define NUMCONTROLS 43

//--------------------------
// Setup all of the controls
//--------------------------
void TCheckAuthDlgClient::SetupControls()
{
 // These are the ones handled directly in the dialog code
 TimerBar=new TGauge(this,IDC_TIMERBAR,
    TIMERBAR_LEFT,TIMERBAR_TOP,TIMERBAR_WIDTH,TIMERBAR_HEIGHT);
 TimerBar->SetRange(0,TIMERBAR_SECONDS-1);
 CancelButton=new TButton(this,IDC_CANCELBUTTON);
 StartButton=new TButton(this,IDC_STARTBUTTON);
 ViewButton=new TButton(this,IDC_VIEWBTN);

 // The rest are handled by the Dialog Manager

 DCMan = new DialogControlManager(this,NUMCONTROLS,AuthMsg,Instruct);

 // Add the TEdit's
 DCMan->Add(IDC_CHECKNUMBER,CHECKNUMBER_LEN,FN_CHECKNUMBER,MID_CHECKNUMBER);
 DCMan->Add(IDC_NAME,NAME_LEN,FN_CONSUMERNAME,MID_NAME)->
   SetValidator(new TFilterValidator("A-Z."));
 DCMan->Add(IDC_ABA,ABA_LEN,FN_BANKNUMBER,MID_BANKNUMBER);
 DCMan->Add(IDC_ACCOUNT,ACCOUNT_LEN,FN_BANKACCOUNT,MID_BANKACCOUNT);
 DCMan->Add(IDC_DOB,DOB_LEN,FN_DOB,MID_DOB);
 DCMan->Add(IDC_LICENSE,LICENSE_LEN,FN_LICENSE,MID_LICENSE);
 DCMan->Add(IDC_MERCHNUM,MERCHNUM_LEN,FN_MERCHID,MID_MERCHNUM);
 DCMan->Add(IDC_CITY,CITY_LEN,FN_CITY,MID_CITY);
 DCMan->Add(IDC_ZIPCODE,ZIPCODE_LEN,FN_ZIPCODE,MID_ZIPCODE);
 DCMan->Add(IDC_ADDRESS,ADDRESS_LEN*2,FN_ADDRESS,MID_ADDRESS);
 DCMan->Add(IDC_BUSINESSNAME,BUSINESSNAME_LEN,FN_BUSINESSNAME,MID_BUSINESSNAME);
 DCMan->Add(IDC_PHONE,PHONE_LEN,FN_PHONE,MID_PHONE);
 DCMan->Add(IDC_SSN,SSN_LEN,FN_SSN,MID_SSN);
 DCMan->Add(IDC_AMOUNT,AMOUNT_LEN,FN_AMOUNT,MID_AMOUNT)->
       SetValidator(new TDecimalValidator(5,2));

 // Add the TEditStateCodes
 DCMan->Add(IDC_LICENSESTATE,
             new TEditStateCodes(this,IDC_LICENSESTATE,POS_UNDER),
                  FN_LICENSESTATE,MID_LICENSESTATE);
 DCMan->Add(IDC_CONSUMERSTATE,
             new TEditStateCodes(this,IDC_CONSUMERSTATE,POS_OVER),
                  FN_CONSUMERSTATE,MID_CONSUMERSTATE);

 // Add the TComboBox's
 DCMan->AddComboBox(IDC_CHECKTYPE,FN_CHECKTYPE,MID_CHECKTYPE);
 DCMan->AddComboBox(IDC_TRANSPORTMETHOD,FN_TRANSPORTMETHOD,
                    MID_TRANSPORTMETHOD);
 DCMan->AddComboBox(IDC_PRODUCTCLASS,FN_PRODUCTCLASS,MID_PRODUCTCLASS,
                    FLDTYPE_DWORD);
 DCMan->AddComboBox(IDC_PRIVATELABEL,FN_PRIVATELABEL,MID_PRIVATELABEL);
 DCMan->AddComboBox(IDC_TITLE,FN_TITLE,MID_TITLE);
 DCMan->AddComboBox(IDC_APARTMENT,FN_APARTMENT,MID_APARTMENT);

 // Add the TStatic's
 DCMan->AddStatic(IDCS_TRANSPORT,IDC_TRANSPORTMETHOD);
 DCMan->AddStatic(IDCS_CONSUMERSTATE,IDC_CONSUMERSTATE);
 DCMan->AddStatic(IDCS_ADDRESS,IDC_ADDRESS);
 DCMan->AddStatic(IDCS_CHECKTYPE,IDC_CHECKTYPE);
 DCMan->AddStatic(IDCS_CHECKNUMBER,IDC_CHECKNUMBER);
 DCMan->AddStatic(IDCS_CITY,IDC_CITY);
 DCMan->AddStatic(IDCS_APARTMENT,IDC_APARTMENT);
 DCMan->AddStatic(IDCS_ZIP,IDC_ZIPCODE);
 DCMan->AddStatic(IDCS_PRODUCTCLASS,IDC_PRODUCTCLASS);
 DCMan->AddStatic(IDCS_BUSINESSNAME,IDC_BUSINESSNAME);
 DCMan->AddStatic(IDCS_PRIVATELABEL,IDC_PRIVATELABEL);
 DCMan->AddStatic(IDCS_PHONE,IDC_PHONE);
 DCMan->AddStatic(IDCS_SSN,IDC_SSN);
 DCMan->AddStatic(IDCS_NAME,IDC_NAME);
 DCMan->AddStatic(IDCS_MICR,IDC_ABA);
 DCMan->AddStatic(IDCS_DOB,IDC_DOB);
 DCMan->AddStatic(IDCS_LICENSE,IDC_LICENSE);
 DCMan->AddStatic(IDCS_LICENSESTATE,IDC_LICENSESTATE);
 DCMan->AddStatic(IDCS_MERCHNUM,IDC_MERCHNUM);
 DCMan->AddStatic(IDCS_AMOUNT,IDC_AMOUNT);
 DCMan->AddStatic(IDCS_TITLE,IDC_TITLE);
}

//------------------------------------------------
// Setup all the lists we need for drop down boxes
//------------------------------------------------
void TCheckAuthDlgClient::SetupLists()
{
 TComboBoxEx* Box;

 // We get the product list but we can't init the box.
 // We have to do it for each transaction
 ProductList = new ProductClassList;

 // The rest of the lists are static
 Box=DCMan->ComboBoxPtr(IDC_CHECKTYPE);
 Box->AddItem("Personal",'P');
 Box->AddItem("Business",'B');
 Box=DCMan->ComboBoxPtr(IDC_TRANSPORTMETHOD);
 Box->AddItem("Carryout",'C');
 Box->AddItem("Delivery",'D');
 Box=DCMan->ComboBoxPtr(IDC_PRIVATELABEL);
 Box->AddItem("Yes",'Y');
 Box->AddItem("No",'N');
 Box=DCMan->ComboBoxPtr(IDC_TITLE);
 Box->AddItem("Mr",'R');
 Box->AddItem("Mrs",'S');
 Box->AddItem("Ms",'M');
 Box=DCMan->ComboBoxPtr(IDC_APARTMENT);
 Box->AddItem("Yes",'Y');
 Box->AddItem("No",'N');
}



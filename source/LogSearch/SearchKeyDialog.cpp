//----------------------------------------------------------------------------
//  Project LogSearch
//  Secure Payment Systems
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    LogSearch.apx Application
//  FILE:         searchkeydialog.cpp
//  AUTHOR:       Jack Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of SearchKeyDialog (TDialog).
//
//----------------------------------------------------------------------------
#include "win4me.h"

#include <stdio.h>
#include <time.h>
#include "searchkeydialog.h"
#include "checkauthfn.h"
#include "DateStuff.h"

//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(SearchKeyDialog, TDialog)
//{{SearchKeyDialogRSP_TBL_BEGIN}}
  EV_BN_CLICKED(IDC_KEYALLBTN, BNKeyAllClicked),
  EV_BN_CLICKED(IDC_SEARCHENTIREBTN, BNSearcnEntireClicked),
  EV_BN_CLICKED(IDC_FINDBTN, BNFindClicked),
  EV_BN_CLICKED(IDC_FULLKEYBTN, BNFullKeyClicked),
  EV_BN_CLICKED(IDC_PARTIALKEYBTN, BNPartialKeyClicked),
  EV_BN_CLICKED(IDC_CURRENTDAYBTN, BNCurrentDayClicked),
  EV_BN_CLICKED(IDC_LASTNDAYSBTN, BNLastNDaysClicked),
  EV_BN_CLICKED(IDC_CURRENTYEARBTN, BNCurrentYearClicked),
  EV_BN_CLICKED(IDC_RANGEBTN, BNRangeClicked),
  EV_BN_CLICKED(IDC_DATEBTN, BNDateClicked),
//{{SearchKeyDialogRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{SearchKeyDialog Implementation}}

#define KEYEDITCTL(n,len) n=new TEdit(this,IDC_##n,len+1)
#define EDITCTL(n,len) n=new TEdit(this,IDC_##n,len+1)


SearchKeyDialog::SearchKeyDialog(TWindow* parent,SEARCHINFO& Info)
:
 TDialog(parent, IDD_SEARCHKEYDIALOG),
 oldInfo(Info)
{
 KEYEDITCTL(KEYMERCHID,5);
 KEYEDITCTL(KEYLICENSESTATE,2);
 KEYEDITCTL(KEYLICENSE,15);
 KEYEDITCTL(KEYBANKNUMBER,9);
 KEYEDITCTL(KEYBANKACCOUNT,16);
 KEYEDITCTL(KEYSSN,9);
 KEYEDITCTL(KEYPHONE,10);
 KEYEDITCTL(KEYAPPROVALNUM,4);
 KEYEDITCTL(KEYOPERATORID,3);
 EDITCTL(DATE,6);
 EDITCTL(FROMDATE,6);
 EDITCTL(TODATE,6);
 EDITCTL(LASTNDAYS,2);
 FULLKEYBTN = new TRadioButton(this,IDC_FULLKEYBTN);
 PARTIALKEYBTN = new TRadioButton(this,IDC_PARTIALKEYBTN);
 SEARCHENTIREBTN = new TRadioButton(this,IDC_SEARCHENTIREBTN);
 CURRENTDAYBTN = new TRadioButton(this,IDC_CURRENTDAYBTN);
 LASTNDAYSBTN = new  TRadioButton(this,IDC_LASTNDAYSBTN);
 CURRENTYEARBTN = new TRadioButton(this,IDC_CURRENTYEARBTN);
 DATEBTN = new TRadioButton(this,IDC_DATEBTN);
 RANGEBTN = new TRadioButton(this,IDC_RANGEBTN);
 KEYALLBTN = new TCheckBox(this,IDC_KEYALLBTN);
}


SearchKeyDialog::~SearchKeyDialog()
{
  Destroy(IDCANCEL);
}


void SearchKeyDialog::SetupWindow()
{
 TEdit* focus=0;
 char Date[7];

 TDialog::SetupWindow();

 keyFieldsLocked=true;

 if ( oldInfo.PartialKeyMatch )
  PARTIALKEYBTN->Check();
 else
  FULLKEYBTN->Check();

 if ( oldInfo.noKeyFlds )
  {
   KEYALLBTN->Check();
   BNKeyAllClicked();
  }
 else
 switch(oldInfo.FldNum1 )
  {
     case FN_MERCHID:
       KEYMERCHID->Transfer(oldInfo.KeyFld1,tdSetData);
       focus=KEYMERCHID;
       break;
     case FN_LICENSESTATE:
       KEYLICENSESTATE->Transfer(oldInfo.KeyFld1,tdSetData);
       KEYLICENSE->Transfer(oldInfo.KeyFld2,tdSetData);
       focus=KEYLICENSESTATE;
       break;
     case FN_BANKNUMBER:
       KEYBANKNUMBER->Transfer(oldInfo.KeyFld1,tdSetData);
       KEYBANKACCOUNT->Transfer(oldInfo.KeyFld2,tdSetData);
       focus=KEYBANKACCOUNT;
       break;
     case FN_SSN:
       KEYSSN->Transfer(oldInfo.KeyFld1,tdSetData);
       focus=KEYSSN;
       break;
     case FN_PHONE:
       KEYPHONE->Transfer(oldInfo.KeyFld1,tdSetData);
       focus=KEYPHONE;
       break;
     case FN_APPROVALNUM:
       KEYAPPROVALNUM->Transfer(oldInfo.KeyFld1,tdSetData);
       focus=KEYAPPROVALNUM;
       break;
     case FN_OPERATORID:
       KEYOPERATORID->Transfer(oldInfo.KeyFld1,tdSetData);
       focus=KEYOPERATORID;
       break;
    }

 clearDateGroup();
 if ( oldInfo.CurrentDay )
  BNCurrentDayClicked();
 else
 if ( oldInfo.CurrentYear )
  BNCurrentYearClicked();
 else
  if ( oldInfo.LastNDays[0] )
  {
   BNLastNDaysClicked();
   LASTNDAYS->Transfer(oldInfo.LastNDays,tdSetData);
   if ( ! focus )
    focus=LASTNDAYS;
  }
 else
 if ( oldInfo.FromDate[0] &&
      oldInfo.ToDate[0] &&
      strcmp(oldInfo.FromDate,oldInfo.ToDate) )
  {
   BNRangeClicked();
   yyyymmddTOmmddyy(Date,oldInfo.FromDate);
   FROMDATE->Transfer(Date,tdSetData);
   yyyymmddTOmmddyy(Date,oldInfo.ToDate);
   TODATE->Transfer(Date,tdSetData);
   if ( ! focus )
     focus=FROMDATE;
  }
 else
 if ( oldInfo.FromDate[0] )
  {
   BNDateClicked();
   yyyymmddTOmmddyy(Date,oldInfo.FromDate);
   DATE->Transfer(Date,tdSetData);
   if ( ! focus )
     focus=DATE;
  }
 else
  BNSearcnEntireClicked();

 if ( focus )
  {
   focus->SetFocus();
   focus->SetSelection(0,-1);
  }

 keyFieldsLocked=false;
}


void SearchKeyDialog::BNKeyAllClicked()
{
 bool enable;
 clearAllKeys(0);
 enable = ( KEYALLBTN->GetCheck() == BF_CHECKED ) ? false : true;
 KEYMERCHID->EnableWindow(enable);
 KEYLICENSESTATE->EnableWindow(enable);
 KEYLICENSE->EnableWindow(enable);
 KEYBANKNUMBER->EnableWindow(enable);
 KEYBANKACCOUNT->EnableWindow(enable);
 KEYSSN->EnableWindow(enable);
 KEYPHONE->EnableWindow(enable);
 KEYAPPROVALNUM->EnableWindow(enable);
 KEYOPERATORID->EnableWindow(enable);
 FULLKEYBTN->EnableWindow(enable);
 PARTIALKEYBTN->EnableWindow(enable);
 if ( enable )
  KEYMERCHID->SetFocus();
}

void SearchKeyDialog::clearDateGroup()
{
 SEARCHENTIREBTN->SetCheck(BF_UNCHECKED);
 CURRENTDAYBTN->SetCheck(BF_UNCHECKED);
 LASTNDAYSBTN->SetCheck(BF_UNCHECKED);
 CURRENTYEARBTN->SetCheck(BF_UNCHECKED);
 DATEBTN->SetCheck(BF_UNCHECKED);
 RANGEBTN->SetCheck(BF_UNCHECKED);
 DATE->Clear();
 FROMDATE->Clear();
 TODATE->Clear();
 LASTNDAYS->Clear();
 DATE->EnableWindow(false);
 FROMDATE->EnableWindow(false);
 TODATE->EnableWindow(false);
 LASTNDAYS->EnableWindow(false);
}

TResult SearchKeyDialog::EvCommand(uint id, THandle hWndCtl, uint notifyCode)
{
  TResult result;

  switch ( notifyCode )
   {
    case EN_CHANGE:  ChangeHandler(id);
                     break;
   }

  result = TDialog::EvCommand(id, hWndCtl, notifyCode);
  return result;
}

//-----------------------------------------------------
// CLEAR ALL KEY FIELDS EXCEPT ONES INDICATED BY CALLER
//-----------------------------------------------------
#define CLEAR(name) if ( not1 != IDC_##name && not2 != IDC_##name )\
                     name->Clear();
void SearchKeyDialog::clearAllKeys(TEdit* ctl,uint not1,uint not2)
{
 if ( keyFieldsLocked )
  return;

 if ( ctl )
  {
   char buf[100];
   ctl->Transfer(buf,tdGetData);
   if ( strlen(buf) != 1 )
     return;
  }

 keyFieldsLocked=true;
 CLEAR(KEYMERCHID);
 CLEAR(KEYLICENSESTATE);
 CLEAR(KEYLICENSE);
 CLEAR(KEYBANKNUMBER);
 CLEAR(KEYBANKACCOUNT);
 CLEAR(KEYSSN);
 CLEAR(KEYPHONE);
 CLEAR(KEYAPPROVALNUM);
 CLEAR(KEYOPERATORID);
 keyFieldsLocked=false;
}

//-----------------------------
// HANDLE CHANGES TO KEY FIELDS
//-----------------------------
void SearchKeyDialog::ChangeHandler(uint Id)
{
 switch(Id)
  {
   case IDC_KEYMERCHID: clearAllKeys(KEYMERCHID,Id); break;
   case IDC_KEYLICENSESTATE: clearAllKeys(KEYLICENSESTATE,Id,IDC_KEYLICENSE); break;
   case IDC_KEYLICENSE: clearAllKeys(KEYLICENSE,Id,IDC_KEYLICENSESTATE); break;
   case IDC_KEYBANKNUMBER: clearAllKeys(KEYBANKNUMBER,Id,IDC_KEYBANKACCOUNT); break;
   case IDC_KEYBANKACCOUNT: clearAllKeys(KEYBANKACCOUNT,Id,IDC_KEYBANKNUMBER); break;
   case IDC_KEYSSN: clearAllKeys(KEYSSN,Id); break;
   case IDC_KEYPHONE: clearAllKeys(KEYPHONE,Id); break;
   case IDC_KEYAPPROVALNUM: clearAllKeys(KEYAPPROVALNUM,Id); break;
   case IDC_KEYOPERATORID: clearAllKeys(KEYOPERATORID,Id); break;
  }
}

//-----------------------------------------------------
// TEST A KEY FIELD TO SEE IF IT'S PRESENT AND/OR VALID
//-----------------------------------------------------
bool SearchKeyDialog::getKey(TEdit* ctl,char* buf,bool mustFill)
{
 ctl->Transfer(buf,tdGetData);
 char messageBuf[100];

 if ( ! buf[0] )
  return false;

 if ( mustFill==false ||
      strlen(buf) == ctl->TextLimit -1 )
  return true;

 sprintf(messageBuf,"KEY MUST BE %d CHARACTERS",ctl->TextLimit-1);
 MessageBox(messageBuf);
 ctl->SetFocus();
 ctl->SetSelection(0,-1);
 return false;
}

//--------------------
// EDIT THE KEY FIELDS
//--------------------
bool SearchKeyDialog::editKeys()
{
 bool fullKeys = PARTIALKEYBTN->GetCheck() != BF_CHECKED;

 if ( KEYALLBTN->GetCheck() == BF_CHECKED )
  {
   newInfo.noKeyFlds=true;
   return true;
  }

 // Merchant Key? 
 if ( getKey(KEYMERCHID,newInfo.KeyFld1,fullKeys) )
  {
   newInfo.FldNum1=FN_MERCHID;
   return true;
  }
 if ( newInfo.KeyFld1[0] )
  return false;

 // Driver's License Key?
 if ( getKey(KEYLICENSESTATE,newInfo.KeyFld1,true) )
  {
   newInfo.FldNum1=FN_LICENSESTATE;
   getKey(KEYLICENSE,newInfo.KeyFld2,false);
   newInfo.FldNum2=FN_LICENSE;
   return true;
  }
 if ( newInfo.KeyFld1[0] )
  return false;

 // Bank Number Key?
 if ( getKey(KEYBANKNUMBER,newInfo.KeyFld1,fullKeys) )
  {
   newInfo.FldNum1=FN_BANKNUMBER;
   getKey(KEYBANKACCOUNT,newInfo.KeyFld2,false);
   newInfo.FldNum2=FN_BANKACCOUNT;
   // If Account Number is Entered then the Bank number must be filled
   if ( newInfo.KeyFld2[0] && strlen(newInfo.KeyFld1) != 9 )
    {
     MessageBox("BANK NUMBER MUST BE 9 DIGITS");
     KEYBANKNUMBER->SetFocus();
     KEYBANKNUMBER->SetSelection(0,-1);
     return false;
    }
   return true;
  }
 if ( newInfo.KeyFld1[0] )
  return false;

 // SSN Key
 if ( getKey(KEYSSN,newInfo.KeyFld1,fullKeys) )
  {
   newInfo.FldNum1=FN_SSN;
   return true;
  }
 if ( newInfo.KeyFld1[0] )
  return false;

 // PHONE Key
 if ( getKey(KEYPHONE,newInfo.KeyFld1,fullKeys) )
  {
   newInfo.FldNum1=FN_PHONE;
   return true;
  }
 if ( newInfo.KeyFld1[0] )
  return false;

 // APPROVAL NUMBER Key
 if ( getKey(KEYAPPROVALNUM,newInfo.KeyFld1,true) )
  {
   newInfo.FldNum1=FN_APPROVALNUM;
   return true;
  }
 if ( newInfo.KeyFld1[0] )
  return false;

 // OPERATOR ID Key
 if ( getKey(KEYOPERATORID,newInfo.KeyFld1,false) )
  {
   newInfo.FldNum1=FN_OPERATORID;
   return true;
  }

 MessageBox("A KEY IS REQUIRED");
 KEYMERCHID->SetFocus();
 return false;
}

//---------------------
// EDIT THE DATE FIELDS
//---------------------
bool SearchKeyDialog::editDates()
{
 time_t now=time(NULL);
 struct tm *t=localtime(&now);

 if ( CURRENTDAYBTN->GetCheck() == BF_CHECKED )
  {
   sprintf(newInfo.FromDate,"%04d%02d%02d",t->tm_year+1900,
              t->tm_mon+1,t->tm_mday);
   strcpy(newInfo.ToDate,newInfo.FromDate);
   return newInfo.CurrentDay=true;
  }

 if ( LASTNDAYSBTN->GetCheck() == BF_CHECKED )
  {
   int NDays;
   LASTNDAYS->Transfer(newInfo.LastNDays,tdGetData);
   if ( strlen(newInfo.LastNDays) == 0 )
    {
     MessageBox("NUMBER OF DAYS IS REQUIRED");
     LASTNDAYS->SetFocus();
     return false;
    }
   NDays=atoi(newInfo.LastNDays);
   sprintf(newInfo.ToDate,"%04d%02d%02d",t->tm_year+1900,
              t->tm_mon+1,t->tm_mday);
   // Convert YearDay to month/Day
   ++t->tm_yday; // Base 0 to Base 1
   if ( NDays < t->tm_yday )
    YdayToMMDD(t->tm_year,t->tm_yday-NDays,t->tm_mon,t->tm_mday);
   else
    {
     int yearDay;
     --t->tm_year;
     yearDay = (t->tm_year%4) ? 365 : 366;
     yearDay -= NDays - t->tm_yday;
     YdayToMMDD(t->tm_year,yearDay,t->tm_mon,t->tm_mday);
    }

   sprintf(newInfo.FromDate,"%04d%02d%02d",t->tm_year+1900,
            t->tm_mon,t->tm_mday);
   return true;
  }

 if ( CURRENTYEARBTN->GetCheck() == BF_CHECKED )
  {
   sprintf(newInfo.ToDate,"%04d1231",t->tm_year+1900);
   sprintf(newInfo.FromDate,"%04d0101",t->tm_year+1900);
   return newInfo.CurrentYear=true;
  }

 if ( DATEBTN->GetCheck() == BF_CHECKED )
  {
   char Date[7];
   DATE->Transfer(Date,tdGetData);
   if ( ! isValidDate(Date) )
    {
     MessageBox("A VALID DATE IN FORM MMDDYY IS REQUIRED");
     DATE->SetFocus();
     DATE->SetSelection(0,-1);
     return false;
    }
   mmddyyTOyyyymmdd(Date,newInfo.FromDate);
   strcpy(newInfo.ToDate,newInfo.FromDate);
   return true;
  }

 if ( RANGEBTN->GetCheck() == BF_CHECKED )
  {
   char fromDate[7], toDate[7];
   FROMDATE->Transfer(fromDate,tdGetData);
   TODATE->Transfer(toDate,tdGetData);
   if ( ! isValidDate(fromDate) )
    {
     MessageBox("A VALID DATE IN FORM MMDDYY IS REQUIRED");
     FROMDATE->SetFocus();
     FROMDATE->SetSelection(0,-1);
     return false;
    }
   if ( ! isValidDate(toDate) )
    {
     MessageBox("A VALID DATE IN FORM MMDDYY IS REQUIRED");
     TODATE->SetFocus();
     TODATE->SetSelection(0,-1);
     return false;
    }
   if ( memcmp(&toDate[4],&fromDate[4],2) < 0 ||
        ( memcmp(&toDate[4],&fromDate[4],2) == 0 &&
          memcmp(&toDate[0],&fromDate[0],4) < 0) )
    {
     MessageBox("'TO' DATE IS EARLIER THAN 'FROM' DATE");
     TODATE->SetFocus();
     TODATE->SetSelection(0,-1);
     return false;
    }

   mmddyyTOyyyymmdd(toDate,newInfo.ToDate);
   mmddyyTOyyyymmdd(fromDate,newInfo.FromDate); 
   return true;
  }

 return newInfo.AllDates=true;
}

//----------------------------
// HANDLE CLICK OF FIND BUTTON
//----------------------------
void SearchKeyDialog::BNFindClicked()
{
 CmOk();
}

//---------------------------------------------
// EDIT KEY/DATE Fields Prior to Closing Window
//---------------------------------------------
bool SearchKeyDialog::CanClose()
{
 memset(&newInfo,0,sizeof(newInfo));

 newInfo.PartialKeyMatch = ( PARTIALKEYBTN->GetCheck() == BF_CHECKED );

 if ( ! editKeys() ||
      ! editDates() )
  return false;

 memcpy(&oldInfo,&newInfo,sizeof(oldInfo));
 return TDialog::CanClose();
}


void SearchKeyDialog::BNFullKeyClicked()
{
 FULLKEYBTN->Check();
 PARTIALKEYBTN->SetCheck(BF_UNCHECKED);
}


void SearchKeyDialog::BNPartialKeyClicked()
{
 PARTIALKEYBTN->Check();
 FULLKEYBTN->SetCheck(BF_UNCHECKED);
}

void SearchKeyDialog::BNSearcnEntireClicked()
{
 clearDateGroup();
 SEARCHENTIREBTN->Check();
}


void SearchKeyDialog::BNCurrentDayClicked()
{
 clearDateGroup();
 CURRENTDAYBTN->Check();
}


void SearchKeyDialog::BNLastNDaysClicked()
{
 clearDateGroup();
 LASTNDAYSBTN->Check();
 LASTNDAYS->EnableWindow(true);
 LASTNDAYS->SetFocus();
}


void SearchKeyDialog::BNCurrentYearClicked()
{
 clearDateGroup();
 CURRENTYEARBTN->Check();
}


void SearchKeyDialog::BNRangeClicked()
{
 clearDateGroup();
 RANGEBTN->Check();
 FROMDATE->EnableWindow(true);
 TODATE->EnableWindow(true);
 FROMDATE->SetFocus();
}


void SearchKeyDialog::BNDateClicked()
{
 clearDateGroup();
 DATEBTN->Check();
 DATE->EnableWindow(true);
 DATE->SetFocus();
}

//--------------
// Validate Date
//--------------
bool SearchKeyDialog::isValidDate(char* Date)
{
 if ( strlen(Date) != 6 )
    return false;

 if ( memcmp(Date,"01",2) < 0 ||
      memcmp(Date,"12",2) > 0 )
  return false;

 if ( memcmp(&Date[2],"01",2) < 0 ||
      memcmp(&Date[2],"31",2) > 0 )
  return false;

 return true;
}

//-----------------------------------------------------
// Convert date in form mmddyy to date in form yyyymmdd
//-----------------------------------------------------
void SearchKeyDialog::mmddyyTOyyyymmdd(char* mmddyy,char *yyyymmdd)
{
 if ( memcmp(&mmddyy[4],"90",2) < 0 )
   strcpy(yyyymmdd,"20");
 else
   strcpy(yyyymmdd,"19");

 memcpy(&yyyymmdd[2],&mmddyy[4],2);
 memcpy(&yyyymmdd[4],mmddyy,4);
 yyyymmdd[8]=0;
}

//-----------------------------------------------------
// Convert date in form mmddyy to date in form yyyymmdd
//-----------------------------------------------------
void SearchKeyDialog::yyyymmddTOmmddyy(char* mmddyy,char *yyyymmdd)
{
 memcpy(mmddyy,&yyyymmdd[4],4);
 memcpy(&mmddyy[4],&yyyymmdd[2],2);
 mmddyy[6]=0;
}


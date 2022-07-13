//----------------------------------------------------------------------------
//  Project LogSearch
//  Secure Payment Systems
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    LogSearch.apx Application
//  FILE:         chkauthdialog.cpp
//  AUTHOR:       Jack Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of ChkauthDialog (TDialog).
//
//----------------------------------------------------------------------------
#include "win4me.h"

#include <time.h>
#include <stdio.h>
#include "chkauthdialog.h"
#include "CheckAuthFn.h"
#include "TranType.h"
#include "SPSManDB.h"
#include "Rcodes.h"


//{{ChkauthDialog Implementation}}

#define STATIC(name) name=new TStatic(this,IDC_##name)

ChkauthDialog::ChkauthDialog(TWindow* parent,ProductClassList* PCList)
:
 TDialog(parent, IDD_CHKAUTH),
 productClassList(PCList)
{
   STATIC(OPERATORID);
   STATIC(CALLTIME);
   STATIC(CALLDATE);
   STATIC(CALLMINUTES);
   STATIC(CALLSECONDS);
   STATIC(CONSUMERNAME);
   STATIC(BANKACCOUNT);
   STATIC(BANKNUMBER);
   STATIC(MERCHID);
   STATIC(PRIVATELABEL);
   STATIC(BUSINESSNAME);
   STATIC(TRANSPORTMETHOD);
   STATIC(PRODUCTCLASS);
   STATIC(SSN);
   STATIC(PHONE);
   STATIC(APARTMENT);
   STATIC(ZIPCODE);
   STATIC(CONSUMERSTATE);
   STATIC(CITY);
   STATIC(ADDRESS);
   STATIC(TITLE);
   STATIC(CHECKNUMBER);
   STATIC(DOB);
   STATIC(LICENSE);
   STATIC(LICENSESTATE);
   STATIC(CHECKTYPE);
   STATIC(AMOUNT);
   STATIC(APPROVALNUM);
}


ChkauthDialog::~ChkauthDialog()
{
  Destroy(IDCANCEL);
}

#define OUTPUT(name) Transfer.ToCtl(FN_##name,name)
#define YN(name) if ( Msg->GetFld(FN_##name,byteVal) )\
                   name->Transfer( (byteVal=='Y') ? "YES" : "NO", tdSetData);\
                 else\
                   name->Clear()
void ChkauthDialog::Display(IPCMessage* Msg)
{
 time_t startTime, stopTime;
 struct tm* t;
 char fmtBuf[100];
 BYTE byteVal;
 DWORD iVal;
 char *Ptr;
 struct MerchantRecord* Rec;

 Transfer.SetIPCMsg(Msg);
 OUTPUT(OPERATORID);
 CALLTIME->Clear();
 CALLDATE->Clear();
 CALLMINUTES->Clear();
 CALLSECONDS->Clear();

 if ( Msg->GetFld(FN_TIMESTAMP,(DWORD)startTime) )
  {
   t=localtime(&startTime);
   sprintf(fmtBuf,"%02d/%02d/%02d",t->tm_mon+1,t->tm_mday,t->tm_year%100);
   CALLDATE->Transfer(fmtBuf,tdSetData);
   sprintf(fmtBuf,"%02d:%02d",t->tm_hour,t->tm_min);
   CALLTIME->Transfer(fmtBuf,tdSetData);
   if ( Msg->GetFld(FN_TIMESTAMP2,(DWORD)stopTime) &&
        stopTime >= startTime )
    {
     sprintf(fmtBuf,"%d",(stopTime-startTime)/60);
     CALLMINUTES->Transfer(fmtBuf,tdSetData);
     sprintf(fmtBuf,"%d",(stopTime-startTime)%60);
     CALLSECONDS->Transfer(fmtBuf,tdSetData);
    }
  }

 if ( Msg->GetFld(FN_APPROVALNUM,iVal) )
  {
   sprintf(fmtBuf,"%04d",iVal);
   APPROVALNUM->Transfer(fmtBuf,tdSetData);
  }
 else
  {
   BYTE Rcode;
   Msg->GetFld(FN_RCODE,Rcode);
   if ( Rcode==RCODE_CALLCANCELLED )
    APPROVALNUM->Transfer("CAN",tdSetData);
   else
    APPROVALNUM->Transfer("DEC",tdSetData);
  }

 OUTPUT(CONSUMERNAME);
 OUTPUT(BANKACCOUNT);
 OUTPUT(BANKNUMBER);
 OUTPUT(MERCHID);

 YN(PRIVATELABEL);
 OUTPUT(BUSINESSNAME);

 if ( Msg->GetFld(FN_TRANSPORTMETHOD,byteVal) )
  TRANSPORTMETHOD->Transfer( (byteVal=='C') ? "CARRYOUT" : "DELIVERY", tdSetData);
 else
  TRANSPORTMETHOD->Clear();

 if ( Msg->GetFld(FN_PRODUCTCLASS,iVal) &&
      (Rec=(struct MerchantRecord*)Msg->GetFldPtr(FN_MERCHANTRECORD)) != NULL &&
      (Ptr=productClassList->FindName(Rec->ProductGroup,iVal)) != NULL )
  PRODUCTCLASS->Transfer(Ptr,tdSetData);
 else
  PRODUCTCLASS->Clear();

 OUTPUT(SSN);
 OUTPUT(PHONE);
 YN(APARTMENT);
 OUTPUT(ZIPCODE);
 OUTPUT(CONSUMERSTATE);
 OUTPUT(CITY);
 OUTPUT(ADDRESS);

 if ( Msg->GetFld(FN_TITLE,byteVal) )
  switch(byteVal)
   {
    case 'R': TITLE->Transfer("MR",tdSetData); break;
    case 'S': TITLE->Transfer("MRS",tdSetData); break;
    case 'M': TITLE->Transfer("MS",tdSetData); break;
    default: TITLE->Clear();
   }
 else
  TITLE->Clear();


 OUTPUT(CHECKNUMBER);
 OUTPUT(DOB);
 OUTPUT(LICENSE);
 OUTPUT(LICENSESTATE);
 if ( Msg->GetFld(FN_CHECKTYPE,byteVal) )
  CHECKTYPE->Transfer( (byteVal=='P') ? "Personal" : "Business", tdSetData);
 else
  CHECKTYPE->Clear();
 OUTPUT(AMOUNT);
}


void ChkauthDialog::SetupWindow()
{
 TDialog::SetupWindow();
}


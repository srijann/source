//----------------------------------------------------------------------------
//  Project VoiceAuth
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    VoiceAuth.apx Application
//  FILE:         decisiondialog.cpp
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of DecisionDialog (TDialog).
//
//----------------------------------------------------------------------------
#include "win4me.h"

#include <stdio.h>
#include "decisiondialog.h"
#include "rcodes.h"
#include "BankVerify.h"
#include "unsnum.h"

//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(DecisionDialog, TDialog)
//{{DecisionDialogRSP_TBL_BEGIN}}
  EV_UDN_DELTAPOS(IDC_ACT_UPDOWN, UDNACTDeltapos),
  EV_UDN_DELTAPOS(IDC_CHK_UPDOWN, UDNCHKDeltapos),
//{{DecisionDialogRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{DecisionDialog Implementation}}


#define ACTROW(n)\
 ActRows[n-1].DATE=new TStatic(this,IDC_ACT##n##DATE);\
 ActRows[n-1].MERCH=new TStatic(this,IDC_ACT##n##MERCH);\
 ActRows[n-1].AMT=new TStatic(this,IDC_ACT##n##AMT);\
 ActRows[n-1].CKNUM=new TStatic(this,IDC_ACT##n##CKNUM);\
 ActRows[n-1].BV=new TStatic(this,IDC_ACT##n##BV);\
 ActRows[n-1].RC=new TStatic(this,IDC_ACT##n##RC);\
 ActRows[n-1].IDS=new TStatic(this,IDC_ACT##n##IDS);
#define CHECKSROW(n)\
 CheckRows[n-1].DATE=new TStatic(this,IDC_CK##n##DATE);\
 CheckRows[n-1].MERCH=new TStatic(this,IDC_CK##n##MERCH);\
 CheckRows[n-1].AMT=new TStatic(this,IDC_CK##n##AMT);\
 CheckRows[n-1].CKNUM=new TStatic(this,IDC_CK##n##CKNUM);\
 CheckRows[n-1].TYPE=new TStatic(this,IDC_CK##n##TYPE);\
 CheckRows[n-1].IDS=new TStatic(this,IDC_CK##n##IDS);

DecisionDialog::DecisionDialog(TWindow* parent)
:
 TDialog(parent,IDD_DECISION)
{
  // INSERT>> Your constructor code here.
 RcodeCheck[0] = new TCheckBox(this,IDC_R1CHK);
 RcodeCheck[1] = new TCheckBox(this,IDC_R2CHK);
 RcodeCheck[2] = new TCheckBox(this,IDC_R3CHK);
 RcodeCheck[3] = new TCheckBox(this,IDC_R4CHK);
 RcodeCheck[4] = new TCheckBox(this,IDC_R5CHK);
 RcodeCheck[5] = new TCheckBox(this,IDC_R6CHK);
 RcodeCheck[6] = new TCheckBox(this,IDC_R7CHK);
 RcodeCheck[7] = new TCheckBox(this,IDC_R8CHK);
 ActivityUpDn = new TUpDown(this,IDC_ACT_UPDOWN);
 ChecksUpDn = new TUpDown(this,IDC_CHK_UPDOWN);
 ACTROW(1);
 ACTROW(2);
 ACTROW(3);
 ACTROW(4);
 ACTROW(5);
 CHECKSROW(1);
 CHECKSROW(2);
 CHECKSROW(3);
 BVABA=new TStatic(this,IDC_BVABA,10);
 BVACCOUNT=new TStatic(this,IDC_BVACCOUNT,17);
 BVACCTOPENDATE=new TStatic(this,IDC_BVACCTOPENDATE,7);
 BVAVGBAL = new TStatic(this,IDC_BVAVGBAL,6);
 BVACCTCLOSEDDATE = new TStatic(this,IDC_BVACCTCLOSEDDATE,7);
 BVPER1NUM=new TStatic(this,IDC_BVPER1NUM);
 BVPER2NUM=new TStatic(this,IDC_BVPER2NUM);
 BVPER3NUM=new TStatic(this,IDC_BVPER3NUM);
 BVPER1AMT=new TStatic(this,IDC_BVPER1AMT);
 BVPER2AMT=new TStatic(this,IDC_BVPER2AMT);
 BVPER3AMT=new TStatic(this,IDC_BVPER3AMT);
 BVAGE=new TStatic(this,IDC_BVAGE);
 BVCHKNUM=new TStatic(this,IDC_BVCHKNUM);
 BVRESPTEXT=new TStatic(this,IDC_BVRESPTEXT);
 BVAMOUNT=new TStatic(this,IDC_BVAMOUNT);
 TRANSCORE=new TStatic(this,IDC_TRANSCORE);
 SSNSCORE=new TStatic(this,IDC_SSNSCORE);
 CONSPHOK=new TStatic(this,IDC_CONSPHOK);
 EMPPHOK=new TStatic(this,IDC_EMPPHOK);

 BVCLOSEDBTN=new TRadioButton(this,IDC_BVCLOSEDBTN);
 BVIFBTN=new TRadioButton(this,IDC_BVIFBTN);
 BVGOODBTN=new TRadioButton(this,IDC_BVGOODBTN);
 BVUNVERIFYBTN=new TRadioButton(this,IDC_BVUNVERIFYBTN);
 COMMENTBOX=new TStatic(this,IDC_COMMENTBOX);
 MOTHERSNAME=new TStatic(this,IDC_MOTHERSNAME);

 strcpy(BVStatus,BVSTATUS_NOTVERIFIED);
}

DecisionDialog::~DecisionDialog()
{
  Destroy(IDCANCEL);
}

static int _USERENTRY activityCompare(const void *a1, const void *a2)
{
 ACTIVITY* Act1=(ACTIVITY*)a1;
 ACTIVITY* Act2=(ACTIVITY*)a2;
 int cc = Act1->Age - Act2->Age;

 if ( cc )
  return cc;

 return Act2->Time - Act1->Time;
}

static int _USERENTRY checkCompare(const void *c1, const void *c2)
{
 BADCHECK* Ck1=(BADCHECK*)c1;
 BADCHECK* Ck2=(BADCHECK*)c2;

 return memcmp(Ck2->CheckDate,Ck1->CheckDate,sizeof(Ck2->CheckDate));
}


void DecisionDialog::Display(IPCMessage* Msg)
{
 char Rcodes[40];
 int  Len=sizeof(Rcodes);
 int  i;
 char Buf[250];
 int  Age;
 int  CheckNum;
 int  Amount;
 uint totalAmount;
 uint numTrans;
 BYTE Rcode;
 VOICEOUTPUT OutPut;
 int Score;
 BYTE YesNo;
 DWORD SSNScore;

 clearAll();
 AuthMsg.CopyMsgIn(Msg);
 AuthMsg.initClassMembers();

 // Set up the Rcode Check Boxes
 if ( AuthMsg.GetFld(FN_RCODES,Rcodes,Len) )
  for (i=0; i<Len; ++i)
   switch(Rcodes[i])
    {
     case RCODE_R1DECLINE: RcodeCheck[0]->Check(); break;
     case RCODE_R2DECLINE: RcodeCheck[1]->Check(); break;
     case RCODE_R3DECLINE: RcodeCheck[2]->Check(); break;
     case RCODE_R4DECLINE: RcodeCheck[3]->Check(); break;
     case RCODE_R5DECLINE: RcodeCheck[4]->Check(); break;
     case RCODE_R6DECLINE: RcodeCheck[5]->Check(); break;
     case RCODE_R7DECLINE: RcodeCheck[6]->Check(); break;
     case RCODE_R8DECLINE: RcodeCheck[7]->Check(); break;
    }

 // Set up Activity
 ActivityIndex=numActivity=0;
 if ( (Act=(ACTIVITY*)AuthMsg.GetFldPtr(FN_ACTIVITY)) != NULL )
  {
   numActivity=AuthMsg.GetFldLen(FN_ACTIVITY) / sizeof(ACTIVITY);
   // Sort by date and time
   qsort(Act,numActivity,sizeof(ACTIVITY),activityCompare);
   // Set the Up Down control
   displayActivity();
   if ( numActivity > MAXACTIVITY )
    {
     ActivityUpDn->EnableWindow(true);
     ActivityUpDn->Show(SW_NORMAL);
    }
  }

 // Set up Checks
 ChecksIndex=numChecks=0;
 if ( (BadChecks=(BADCHECK*)AuthMsg.GetFldPtr(FN_BADCHECKS)) != NULL )
  {
   numChecks=AuthMsg.GetFldLen(FN_BADCHECKS) / sizeof(BADCHECK);
   qsort(BadChecks,numChecks,sizeof(BADCHECK),checkCompare);
   displayChecks();
   if ( numChecks > MAXCHECKS )
    {
     ChecksUpDn->EnableWindow(true);
     ChecksUpDn->Show(SW_NORMAL);
    }
  }

 // Display Micr Number Fields
 Len=sizeof(Buf);
 if ( AuthMsg.GetFld(FN_BANKNUMBER,Len,Buf) )
  BVABA->Transfer(Buf,tdSetData);
 Len=sizeof(Buf);
 if ( AuthMsg.GetFld(FN_BANKACCOUNT,Len,Buf) )
  BVACCOUNT->Transfer(Buf,tdSetData);

 // Display Consumer Age
 if ( (Age=AuthMsg.ConsumerAge()) == 0 )
  BVAGE->Transfer("?",tdSetData);
 else
  {
   sprintf(Buf,"%d",Age);
   BVAGE->Transfer(Buf,tdSetData);
  }
 if ( (CheckNum=AuthMsg.CheckNumber()) == 0 )
  BVCHKNUM->Transfer("?",tdSetData);
 else
  {
   sprintf(Buf,"%d",CheckNum);
   BVCHKNUM->Transfer(Buf,tdSetData);
  }

 // Display Check AMount
 Amount=AuthMsg.CheckAmount();
 sprintf(Buf,"%d",Amount);
 BVAMOUNT->Transfer(Buf,tdSetData);

 // Display the Response Code
 AuthMsg.GetFld(FN_RCODE,Rcode);
 if ( ISKENWOODRCODE(Rcode) )
  {
   Len=sizeof(Buf);
   if ( ! AuthMsg.GetFld(FN_TEXTRESPONSE,Len,Buf) )
    sprintf(Buf,"KENWOOD RCODE %d",Rcode);
  }
 else
  {
   if ( Rcode > 99 )
    sprintf(Buf,"UNRECOGIZED %d",Rcode);
   else
    strcpy(Buf,rcodeInfo.rcodeText(Rcode));
  }
 BVRESPTEXT->Transfer(Buf,tdSetData);

 // Set up the period amounts
 AuthMsg.CountActivity(PERIOD1DAYS,0,0,numTrans,totalAmount);
 numTrans += 1;
 totalAmount += Amount;
 sprintf(Buf,"%d",numTrans);
 BVPER1NUM->Transfer(Buf,tdSetData);
 sprintf(Buf,"%d",totalAmount);
 BVPER1AMT->Transfer(Buf,tdSetData);

 AuthMsg.CountActivity(PERIOD2DAYS,0,0,numTrans,totalAmount);
 numTrans += 1;
 totalAmount += Amount;
 sprintf(Buf,"%d",numTrans);
 BVPER2NUM->Transfer(Buf,tdSetData);
 sprintf(Buf,"%d",totalAmount);
 BVPER2AMT->Transfer(Buf,tdSetData);

 AuthMsg.CountActivity(PERIOD3DAYS,0,0,numTrans,totalAmount);
 numTrans += 1;
 totalAmount += Amount;
 sprintf(Buf,"%d",numTrans);
 BVPER3NUM->Transfer(Buf,tdSetData);
 sprintf(Buf,"%d",totalAmount);
 BVPER3AMT->Transfer(Buf,tdSetData);

 // Set up Bank Verify Status Info (Buttons and Fields)
 Len=sizeof(OutPut);
/*
 typedef struct {
  char  Unused[37];
  char  OperatorId[4];
  char  BVAverageBalance[5];   // Average balance
  char  BVAccountDate[6];      // Account Open or Close date depending
                               // on BV status
  char  BVStatus[2];           //   IF  = Insufficient funds
                               //   AC  = Account Closed
                               //   VG  = Verified Good
                               //   UV  = Unverifiable
                               //  '  ' = Not Bank Verified
 } VOICEOUTPUT;
*/
 if ( Msg->GetFld(FN_TEXTRESPONSE,&OutPut,Len) )
  {
   char fmtBuf[20];

   memcpy(fmtBuf,OutPut.BVAverageBalance,sizeof(OutPut.BVAverageBalance));
   fmtBuf[sizeof(OutPut.BVAverageBalance)]=0;
   BVAVGBAL->Transfer(fmtBuf,tdSetData);
   memcpy(fmtBuf,OutPut.BVAccountDate,sizeof(OutPut.BVAccountDate));
   fmtBuf[sizeof(OutPut.BVAccountDate)]=0;

   if ( memcmp(OutPut.BVStatus,"IF",2) == 0 )
    {
     BVIFBTN->Check();
     BVACCTOPENDATE->Transfer(fmtBuf,tdSetData);
    }
   else
   if ( memcmp(OutPut.BVStatus,"AC",2) == 0 )
    {
     BVCLOSEDBTN->Check();
     BVACCTCLOSEDDATE->Transfer(fmtBuf,tdSetData);
    }
   else
   if ( memcmp(OutPut.BVStatus,"VG",2) == 0 )
     BVGOODBTN->Check();
   if ( memcmp(OutPut.BVStatus,"UV",2) == 0 )
    BVUNVERIFYBTN->Check();
  }

 if ( Msg->GetFld(FN_SSNSCORE,SSNScore) )
  {
   sprintf(Buf,"%d",SSNScore);
   SSNSCORE->Transfer(Buf,tdSetData);
  }

 if ( Msg->GetFld(FN_CONSUMERPHONEGOOD,YesNo) )
  {
   Buf[0]=YesNo;
   Buf[1]=0;
   CONSPHOK->Transfer(Buf,tdSetData);
  }

 if ( Msg->GetFld(FN_EMPLOYERPHONEGOOD,YesNo) )
  {
   Buf[0]=YesNo;
   Buf[1]=0;
   EMPPHOK->Transfer(Buf,tdSetData);
  }

 if ( Msg->GetFld(FN_TRANSACTIONSCORE,(DWORD)Score) )
  {
   sprintf(Buf,"%d",Score);
   TRANSCORE->Transfer(Buf,tdSetData);
  }

 Len=sizeof(Buf);
 if ( Msg->GetFld(FN_COMMENTS,Len,Buf) )
  COMMENTBOX->Transfer(Buf,tdSetData);

 Len=sizeof(Buf);
 if ( Msg->GetFld(FN_MOTHERSNAME,Len,Buf) )
   MOTHERSNAME->Transfer(Buf,tdSetData); 
}

void DecisionDialog::formatActivityRow(int Row,int Index)
{
 char Buf[50];

 sprintf(Buf,"%2.2s/%2.2s/%2.2s",&Act[Index].Date[0],
         &Act[Index].Date[2],&Act[Index].Date[4]);
 ActRows[Row].DATE->Transfer(Buf,tdSetData);
 memcpy(Buf,Act[Index].MerchID,sizeof(Act[Index].MerchID));
 Buf[sizeof(Act[Index].MerchID)]=0;
 ActRows[Row].MERCH->Transfer(Buf,tdSetData);
 sprintf(Buf,"%d",Act[Index].Amount);
 ActRows[Row].AMT->Transfer(Buf,tdSetData);
 sprintf(Buf,"%d",Act[Index].CheckNumber);
 ActRows[Row].CKNUM->Transfer(Buf,tdSetData);
 memcpy(Buf,Act[Index].Output.BVStatus,sizeof(Act[Index].Output.BVStatus));
 Buf[sizeof(Act[Index].Output.BVStatus)]=0;
 ActRows[Row].BV->Transfer(Buf,tdSetData);

 // If we have a BV status of IF or AC then display the Bank verify
 // info in the response field instead of a response message.
 if ( memcmp(Act[Index].Output.BVStatus,BVSTATUS_NOTENOUGHMONEY,2)==0 )
  {
   memset(Buf,0,sizeof(Buf));
   sprintf(Buf,"%2.2s/%2.2s/%2.2s",&Act[Index].Output.BVAccountDate[0],
            &Act[Index].Output.BVAccountDate[2],&Act[Index].Output.BVAccountDate[4]);
   strcat(Buf," $");
   memcpy(&Buf[strlen(Buf)],Act[Index].Output.BVAverageBalance,
            sizeof(Act[Index].Output.BVAverageBalance));
  }
 else
 if ( memcmp(Act[Index].Output.BVStatus,BVSTATUS_ACCOUNTCLOSED,2)==0 )
   sprintf(Buf,"%2.2s/%2.2s/%2.2s",&Act[Index].Output.BVAccountDate[0],
            &Act[Index].Output.BVAccountDate[2],&Act[Index].Output.BVAccountDate[4]);
 else
  if ( Act[Index].Rcode > 99 )
    sprintf(Buf,"UNDEFINED %d",Act[Index].Rcode);
  else
   strcpy(Buf,rcodeInfo.rcodeText(Act[Index].Rcode));

 ActRows[Row].RC->Transfer(Buf,tdSetData);

 memcpy(Buf,Act[Index].Ids,sizeof(Act[Index].Ids));
 Buf[sizeof(Act[Index].Ids)]=0;

  // Don't display Id's for activity which didn't trigger the
 // referral. We know this because the unused ones are set to
 // lowercase in the CheckAuth module.
 int i;
 for (i=0; i<sizeof(Act[Index].Ids); ++i)
  if ( islower(Buf[i]) )
   Buf[i] = ' ';
   
 ActRows[Row].IDS->Transfer(Buf,tdSetData);
 //ActRows[Row].RC->EnableWindow( ISAPPROVALRCODE(Act[Index].Rcode) );
}

#define MOVE(fld,ctl)\
  memcpy(Buf,BadChecks[Index].fld,sizeof(BadChecks[Index].fld));\
  Buf[sizeof(BadChecks[Index].fld)]=0;\
  CheckRows[Row].ctl->Transfer(Buf,tdSetData)
void DecisionDialog::formatChecksRow(int Row,int Index)
{
 char Buf[50];

 sprintf(Buf,"%2.2s/%2.2s/%2.2s",&BadChecks[Index].CheckDate[2],
         &BadChecks[Index].CheckDate[4],&BadChecks[Index].CheckDate[0]);
 CheckRows[Row].DATE->Transfer(Buf,tdSetData);
 MOVE(MerchId,MERCH);
 MOVE(Amount,AMT);
 MOVE(CheckNumber,CKNUM);
 MOVE(TypeReturnedCheck,TYPE);
 MOVE(Ids,IDS);
}

//---------------------
// Display the Activity
//---------------------
void DecisionDialog::displayActivity()
{
 int Row, Index;

 for (Row=0, Index=ActivityIndex;
        Row < MAXACTIVITY && Index < numActivity; ++Row, ++Index )
  formatActivityRow(Row,Index);
}

//-----------------------
// Display the Check Data
//-----------------------
void DecisionDialog::displayChecks()
{
 int Row, Index;

 for (Row=0, Index=ChecksIndex;
        Row < MAXCHECKS && Index < numChecks; ++Row, ++Index )
  formatChecksRow(Row,Index);
}

bool DecisionDialog::UDNACTDeltapos(TNmUpDown& udn)
{
 switch( udn.iDelta )
  {
   case -1: if ( ActivityIndex > 0 )
             --ActivityIndex;
            break;
   case  1: if ( ActivityIndex < numActivity - MAXACTIVITY )
             ++ActivityIndex;
            break;
   default: return true;
  }

 displayActivity();
 return true;
}


bool DecisionDialog::UDNCHKDeltapos(TNmUpDown& udn)
{
 switch( udn.iDelta )
  {
   case -1: if ( ChecksIndex > 0 )
             --ChecksIndex;
            break;
   case  1: if ( ChecksIndex < numChecks - MAXCHECKS )
             ++ChecksIndex;
            break;
   default: return true;
  }

 displayChecks();
 return true;
}

//-----------------------------------
// Clear all the fields in the window
//-----------------------------------
void DecisionDialog::clearAll()
{
 int i;
 for(i=0;i<8;++i)
  RcodeCheck[i]->SetCheck(BF_UNCHECKED);

 BVCLOSEDBTN->SetCheck(BF_UNCHECKED);
 BVIFBTN->SetCheck(BF_UNCHECKED);
 BVGOODBTN->SetCheck(BF_UNCHECKED);
 BVUNVERIFYBTN->SetCheck(BF_UNCHECKED);

 ActivityUpDn->Show(SW_HIDE);
 ChecksUpDn->Show(SW_HIDE);

 BVABA->Clear();
 BVACCOUNT->Clear();
 BVACCTOPENDATE->Clear();
 BVAVGBAL->Clear();
 BVACCTCLOSEDDATE->Clear();
 BVPER1NUM->Clear();
 BVPER2NUM->Clear();
 BVPER3NUM->Clear();
 BVPER1AMT->Clear();
 BVPER2AMT->Clear();
 BVPER3AMT->Clear();
 BVAGE->Clear();
 BVCHKNUM->Clear();
 BVRESPTEXT->Clear();
 BVAMOUNT->Clear();

 for (i=0; i<MAXACTIVITY; ++i)
  {
   ActRows[i].DATE->Clear();
   ActRows[i].MERCH->Clear();
   ActRows[i].AMT->Clear();
   ActRows[i].CKNUM->Clear();
   ActRows[i].BV->Clear();
   ActRows[i].RC->Clear();
   ActRows[i].IDS->Clear();
  }

 for (i=0; i<MAXCHECKS; ++i)
   {
    CheckRows[i].DATE->Clear();
    CheckRows[i].MERCH->Clear();
    CheckRows[i].AMT->Clear();
    CheckRows[i].CKNUM->Clear();
    CheckRows[i].TYPE->Clear();
    CheckRows[i].IDS->Clear();
   };

 TRANSCORE->Clear();
 SSNSCORE->Clear();
 CONSPHOK->Clear();
 EMPPHOK->Clear();
 COMMENTBOX->Clear();
 MOTHERSNAME->Clear();
 
}


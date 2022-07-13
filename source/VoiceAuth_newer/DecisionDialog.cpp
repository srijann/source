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
//  JHE011698 - Modified to blank out ID Type in activity display when
//              the character is lower case. The lower case letter
//              indicates that the transaction was not involved in the
//              referral.
//
//  JHE061698 - Changed mind on above. Shift lowercase back to uppercase
//              and display always
//----------------------------------------------------------------------------
#include "win4me.h"

#include <stdio.h>
#include "decisiondialog.h"
#include "chkauth.h"
#include "Appnotify.h"
#include "rcodes.h"
#include "msgid.h"
#include "destname.h"
#include "BankVerify.h"
#include "unsnum.h"
#include "trantype.h"
#include "YesNoDialog.h"
#include "SSNScoreDialog.h"
#include "OperatorId.h"
#include "MsgWin.rh"

//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(DecisionDialog, TDialog)
//{{DecisionDialogRSP_TBL_BEGIN}}
  EV_BN_CLICKED(IDC_APPROVEBTN, BNOkayClicked),
  EV_BN_CLICKED(IDC_BVDECLINE, BNDeclineClicked),
  EV_UDN_DELTAPOS(IDC_ACT_UPDOWN, UDNACTDeltapos),
  EV_UDN_DELTAPOS(IDC_CHK_UPDOWN, UDNCHKDeltapos),
  EV_BN_CLICKED(IDC_BVIFBTN, BNIFClicked),
  EV_BN_CLICKED(IDC_BVGOODBTN, BNGoodClicked),
  EV_BN_CLICKED(IDC_BVCLOSEDBTN, BNClosedClicked),
  EV_BN_CLICKED(IDC_BVUNVERIFYBTN, BNUnverifableClicked),
  EV_BN_CLICKED(IDC_BVUNVBTN, BNUnvClicked),
  EV_BN_CLICKED(IDC_CHANGEBUTTON, BNChangeClicked),
//{{DecisionDialogRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{DecisionDialog Implementation}}


#define ACTROW(n)\
 ActRows[n-1].DATE=new TEdit(this,IDC_ACT##n##DATE);\
 ActRows[n-1].MERCH=new TEdit(this,IDC_ACT##n##MERCH);\
 ActRows[n-1].AMT=new TEdit(this,IDC_ACT##n##AMT);\
 ActRows[n-1].CKNUM=new TEdit(this,IDC_ACT##n##CKNUM);\
 ActRows[n-1].BV=new TEdit(this,IDC_ACT##n##BV);\
 ActRows[n-1].RC=new TEdit(this,IDC_ACT##n##RC);\
 ActRows[n-1].IDS=new TEdit(this,IDC_ACT##n##IDS);
#define CHECKSROW(n)\
 CheckRows[n-1].DATE=new TEdit(this,IDC_CK##n##DATE);\
 CheckRows[n-1].MERCH=new TEdit(this,IDC_CK##n##MERCH);\
 CheckRows[n-1].AMT=new TEdit(this,IDC_CK##n##AMT);\
 CheckRows[n-1].CKNUM=new TEdit(this,IDC_CK##n##CKNUM);\
 CheckRows[n-1].TYPE=new TEdit(this,IDC_CK##n##TYPE);\
 CheckRows[n-1].IDS=new TEdit(this,IDC_CK##n##IDS);

DecisionDialog::DecisionDialog(TWindow* parent, CheckAuth& Msg,
                     IPCMessage& ReplyMsg,TMsgWin* MsgWin,DWORD MsgNum)
:
 TDialog(parent,IDD_DECISION),
 AuthMsg(Msg),
 replyMsg(ReplyMsg),
 msgWin(MsgWin),
 msgWin2(0),
 msgNum(MsgNum),
 verifyButtonClicked(false)
{
 // Create the Check Boxes to show the Controls which were violated
 RcodeCheck[0] = new TCheckBox(this,IDC_R1CHK);
 RcodeCheck[1] = new TCheckBox(this,IDC_R2CHK);
 RcodeCheck[2] = new TCheckBox(this,IDC_R3CHK);
 RcodeCheck[3] = new TCheckBox(this,IDC_R4CHK);
 RcodeCheck[4] = new TCheckBox(this,IDC_R5CHK);
 RcodeCheck[5] = new TCheckBox(this,IDC_R6CHK);
 RcodeCheck[6] = new TCheckBox(this,IDC_R7CHK);
 RcodeCheck[7] = new TCheckBox(this,IDC_R8CHK);

 // Create the Up/Down controls for the Actvity and Bad Check displays
 ActivityUpDn = new TUpDown(this,IDC_ACT_UPDOWN);
 ChecksUpDn = new TUpDown(this,IDC_CHK_UPDOWN);

 // Create the controls for the Activity rows
 ACTROW(1);
 ACTROW(2);
 ACTROW(3);
 ACTROW(4);
 ACTROW(5);
 //ACTROW(6);
 //ACTROW(7);

 // Create the controls for the Bad Check Rows
 CHECKSROW(1);
 CHECKSROW(2);
 CHECKSROW(3);

 // Create the controls for the Bank Verification fields
 BVABA=new TEdit(this,IDC_BVABA,10);
 BVACCOUNT=new TEdit(this,IDC_BVACCOUNT,17);
 BVACCTOPENDATE=new TEdit(this,IDC_BVACCTOPENDATE,7);
 BVAVGBAL = new TEdit(this,IDC_BVAVGBAL,6);
 BVACCTCLOSEDDATE = new TEdit(this,IDC_BVACCTCLOSEDDATE,7);
 BVPER1NUM=new TEdit(this,IDC_BVPER1NUM);
 BVPER2NUM=new TEdit(this,IDC_BVPER2NUM);
 BVPER3NUM=new TEdit(this,IDC_BVPER3NUM);
 BVPER1AMT=new TEdit(this,IDC_BVPER1AMT);
 BVPER2AMT=new TEdit(this,IDC_BVPER2AMT);
 BVPER3AMT=new TEdit(this,IDC_BVPER3AMT);
 BVAGE=new TEdit(this,IDC_BVAGE);
 BVCHKNUM=new TEdit(this,IDC_BVCHKNUM);
 BVRESPTEXT=new TEdit(this,IDC_BVRESPTEXT);
 BVAMOUNT=new TEdit(this,IDC_BVAMOUNT);
 BVBANKNAME=new TEdit(this,IDC_BANKNAME,sizeof(BankPhoneRec.BankName));
 BVBANKPHONE=new TEdit(this,IDC_BANKPHONE,sizeof(BankPhoneRec.PhoneNumber));
 BVBANKCOMMENT=new TEdit(this,IDC_BANKCOMMENT,sizeof(BankPhoneRec.Comment));
 CHANGEBUTTON = new TButton(this,IDC_CHANGEBUTTON);
 COMMENTBOX = new TEdit(this,IDC_COMMENTBOX,sizeof(CommentBuf));
 MOTHERSNAME = new TEdit(this,IDC_MOTHERSNAME,26);
 strcpy(BVStatus,BVSTATUS_NOTVERIFIED);
}

DecisionDialog::~DecisionDialog()
{
 if ( msgWin2 )
  delete msgWin2;
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

//------------------------------------------
// Paint the Window with initial information
//------------------------------------------
void DecisionDialog::SetupWindow()
{
 char Rcodes[40];
 int  Len=sizeof(Rcodes);
 int  i;
 char Buf[100];
 int  Age;
 int  CheckNum;
 int  Amount;
 uint totalAmount;
 uint numTrans;
 BYTE Rcode;

 TDialog::SetupWindow();

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

 // Set up Bank Verify Fields
 BVACCTOPENDATE->EnableWindow(false);
 BVAVGBAL->EnableWindow(false);
 BVACCTCLOSEDDATE->EnableWindow(false);

 Len=sizeof(Buf);
 if ( AuthMsg.GetFld(FN_BANKNUMBER,Len,Buf) )
  BVABA->Transfer(Buf,tdSetData);
 Len=sizeof(Buf);
 if ( AuthMsg.GetFld(FN_BANKACCOUNT,Len,Buf) )
  BVACCOUNT->Transfer(Buf,tdSetData);

 // Display the Consumer Age
 if ( (Age=AuthMsg.ConsumerAge()) == 0 )
  BVAGE->Transfer("?",tdSetData);
 else
  {
   sprintf(Buf,"%d",Age);
   BVAGE->Transfer(Buf,tdSetData);
  }

 // Display the Check Number
 if ( (CheckNum=AuthMsg.CheckNumber()) == 0 )
   BVCHKNUM->Transfer(Buf,tdSetData);
 else
  {
   sprintf(Buf,"%d",CheckNum);
   BVCHKNUM->Transfer(Buf,tdSetData);
  }

 // Display the Check Amount
 Amount=AuthMsg.CheckAmount();
 sprintf(Buf,"%d",Amount);
 BVAMOUNT->Transfer(Buf,tdSetData);

 // Display the Response Code
 AuthMsg.GetFld(FN_RCODE,Rcode);
 if ( rcodeInfo.IsKenwoodRcode(Rcode) )
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

 // Display Mother's Maiden Name
 Len=sizeof(Buf);
 if ( AuthMsg.GetFld(FN_MOTHERSNAME,Len,Buf) )
  MOTHERSNAME->Transfer(Buf,tdSetData);

 // Get Bank Phone Info
 BankPhoneMsg.CopyFld(FN_BANKNUMBER,&AuthMsg);
 BankPhoneMsg.PutFld(FN_TRANTYPE,T_GETRECORD);
 BankPhoneMsg.SendRecv(BANKPHONE_DEST,(WORD)10000,true);
 Len=sizeof(BankPhoneRec);
 if ( BankPhoneMsg.GetFld(FN_BANKPHONERECORD,&BankPhoneRec,Len) )
  {
   BVBANKNAME->Transfer(BankPhoneRec.BankName,tdSetData);
   BVBANKNAME->SetReadOnly(true);
   BVBANKCOMMENT->Transfer(BankPhoneRec.Comment,tdSetData);
   BVBANKCOMMENT->SetReadOnly(true);
   BVBANKPHONE->Transfer(BankPhoneRec.PhoneNumber,tdSetData);
   BVBANKPHONE->SetReadOnly(true);
   BVBANKNAME->SetFocus();
   CHANGEBUTTON->ShowWindow(SW_SHOW);
  }
 else
  memset(&BankPhoneRec,0,sizeof(BankPhoneRec));

 displayDecisionMsg();
 if ( msgNum )
  {
   msgWin2 = new TMsgWin(msgWin,IDD_MSGWIN2,94);
   msgWin2->Create();
   msgWin2->Say(msgNum);
  }
}

//-------------------------------
// Format an Activity Row Display
//-------------------------------
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
 memcpy(Buf,Act[Index].Ids,sizeof(Act[Index].Ids));
 Buf[sizeof(Act[Index].Ids)]=0;
 
 // Don't display Id's for activity which didn't trigger the
 // referral. We know this because the unused ones are set to
 // lowercase in the CheckAuth module.
 int i;
 for (i=0; i<sizeof(Act[Index].Ids); ++i)
// JHE061698 
  Buf[i] = toupper(Buf[i]);

// JHE011698
//  if ( islower(Buf[i]) )
//   Buf[i] = ' ';

 ActRows[Row].IDS->Transfer(Buf,tdSetData);

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
 //ActRows[Row].RC->EnableWindow( ISAPPROVALRCODE(Act[Index].Rcode) );
}

//--------------------------------
// Format a Bad Checks Row Display
//--------------------------------

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

//----------------------------
// Display the Bad Check Data
//----------------------------
void DecisionDialog::displayChecks()
{
 int Row, Index;

 for (Row=0, Index=ChecksIndex;
        Row < MAXCHECKS && Index < numChecks; ++Row, ++Index )
  formatChecksRow(Row,Index);
}

//---------------------------------
// Prohibit ESCAPE from this window
//---------------------------------
TResult DecisionDialog::EvCommand(uint id, THandle hWndCtl, uint notifyCode)
{
 TResult result;
  switch( notifyCode )
   {
    case 0:
       if ( id==IDCANCEL || id==IDOK )
        {
         if ( id==IDCANCEL && msgWin2 )
           msgWin2->Move();
         return 0;
        }
      break;

    default:
      break;
   }

  result = TDialog::EvCommand(id, hWndCtl, notifyCode);
  return result;
}

//-----------------------------------------
// Handle Up/Down Clicks for Activity Rows
//------------------------------------------
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


//-------------------------------------------
// Handle Up/Down Clicks for Bad Checks Rows
//-------------------------------------------
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

//------------------------------------------
// Handle click of Insufficient Funds button
//-------------------------------------------
void DecisionDialog::BNIFClicked()
{
 verifyButtonClicked=true;
 BVACCTOPENDATE->Transfer("",tdSetData);
 BVAVGBAL->Transfer("",tdSetData);
 BVACCTCLOSEDDATE->Transfer("",tdSetData);
 BVACCTOPENDATE->EnableWindow(true);
 BVACCTOPENDATE->SetReadOnly(false);
 BVAVGBAL->EnableWindow(true);
 BVAVGBAL->SetReadOnly(false);
 BVACCTCLOSEDDATE->EnableWindow(false);
 BVACCTCLOSEDDATE->SetReadOnly(true);
 strcpy(BVStatus,BVSTATUS_NOTENOUGHMONEY);
 BVACCTOPENDATE->SetFocus();
}

//-------------------------------------
// Handle Click of Verified Good Button
//-------------------------------------
void DecisionDialog::BNGoodClicked()
{
 verifyButtonClicked=true;
 BVACCTOPENDATE->Transfer("",tdSetData);
 BVAVGBAL->Transfer("",tdSetData);
 BVACCTCLOSEDDATE->Transfer("",tdSetData);
 BVACCTOPENDATE->EnableWindow(false);
 BVACCTOPENDATE->SetReadOnly(true);
 BVAVGBAL->EnableWindow(false);
 BVAVGBAL->SetReadOnly(true);
 BVACCTCLOSEDDATE->EnableWindow(false);
 BVACCTCLOSEDDATE->SetReadOnly(true);
 strcpy(BVStatus,BVSTATUS_VERIFIEDGOOD);
}

//--------------------------------------
// Handle click of Account Closed Button
//--------------------------------------
void DecisionDialog::BNClosedClicked()
{
 verifyButtonClicked=true;
 BVACCTOPENDATE->Transfer("",tdSetData);
 BVAVGBAL->Transfer("",tdSetData);
 BVACCTCLOSEDDATE->Transfer("",tdSetData);
 BVACCTOPENDATE->EnableWindow(false);
 BVACCTOPENDATE->SetReadOnly(true);
 BVAVGBAL->EnableWindow(false);
 BVAVGBAL->SetReadOnly(true);
 BVACCTCLOSEDDATE->EnableWindow(true);
 BVACCTCLOSEDDATE->SetReadOnly(false);
 strcpy(BVStatus,BVSTATUS_ACCOUNTCLOSED);
 BVACCTCLOSEDDATE->SetFocus();
}

//------------------------------------------
// Handle Click of Unverifiable Funds Button
//------------------------------------------
void DecisionDialog::BNUnverifableClicked()
{
 verifyButtonClicked=true;
 BVACCTOPENDATE->Transfer("",tdSetData);
 BVAVGBAL->Transfer("",tdSetData);
 BVACCTCLOSEDDATE->Transfer("",tdSetData);
 BVACCTOPENDATE->EnableWindow(false);
 BVACCTOPENDATE->SetReadOnly(true);
 BVAVGBAL->EnableWindow(false);
 BVAVGBAL->SetReadOnly(true);
 BVACCTCLOSEDDATE->EnableWindow(false);
 BVACCTCLOSEDDATE->SetReadOnly(true);
 strcpy(BVStatus,BVSTATUS_UNVERIFIABLE);
}

//-------------------------------
// Handle Click of Approve Button
//-------------------------------
void DecisionDialog::BNOkayClicked()
{
 if ( ! isVerifyButtonChecked() )
   return;
 AuthMsg.PutFld(FN_RCODE,(BYTE)RCODE_FORCEDAPPROVAL);
 AuthMsg.GenerateApprovalNumber();
 sendReply();
 AppNotifyMsg(Parent,NOTIFY_DIALOGCOMPLETE,NOTIFY_APPROVED);
 CmOk();
}

//-------------------------------
// Handle Click of Decline Button
//-------------------------------
void DecisionDialog::BNDeclineClicked()
{
 if ( ! isVerifyButtonChecked() )
   return;
 sendReply();
 AppNotifyMsg(Parent,NOTIFY_DIALOGCOMPLETE,NOTIFY_DECLINED);
 CmOk();
}

//----------------------------------------------
// Handle Click of ? Button (funds unverifiable)
// Invokes Automatic Decision
//----------------------------------------------
void DecisionDialog::BNUnvClicked()
{
 if ( ! isVerifyButtonChecked() )
   return;
 strcpy(BVStatus,BVSTATUS_UNVERIFIABLE);
 if ( doUnverifiedFundsRules() )
  BNOkayClicked();
 else
  BNDeclineClicked();
}

//--------------------------------------------
// Handle click of Change button for Bank Info
//--------------------------------------------
void DecisionDialog::BNChangeClicked()
{
 // Enable fields so they can be changed
 BVBANKNAME->SetReadOnly(false);
 BVBANKCOMMENT->SetReadOnly(false);
 BVBANKPHONE->SetReadOnly(false);
 BVBANKNAME->SetFocus();
}

//----------------------
// Send reply to Kenwood
//----------------------
bool DecisionDialog::sendReply()
{
 VOICEOUTPUT OutPut;
 char Buf[20];
 bool ret;
 HWND captureWin;

 replyMsg.ClearMsg();

 // Build Output Field and store
 memset(&OutPut,' ',sizeof(OutPut));
 COperatorId OperatorId;
 OperatorId.Get(OutPut.OperatorId,sizeof(OutPut.OperatorId));
 memcpy(OutPut.BVStatus,BVStatus,sizeof(OutPut.BVStatus));
 // Insufficient funds? .. we store Account Open Date and Average Balance
 if ( strcmp(BVStatus,BVSTATUS_NOTENOUGHMONEY) == 0 )
  {
   BVACCTOPENDATE->Transfer(Buf,tdGetData);
   memcpy(OutPut.BVAccountDate,Buf,strlen(Buf));
   BVAVGBAL->Transfer(Buf,tdGetData);
   memcpy(OutPut.BVAverageBalance,Buf,strlen(Buf));
  }
 else
 if ( strcmp(BVStatus,BVSTATUS_ACCOUNTCLOSED) == 0 )
  {
   BVACCTCLOSEDDATE->Transfer(Buf,tdGetData);
   memcpy(OutPut.BVAccountDate,Buf,strlen(Buf));
  }
  // Get Comments, If Any
 COMMENTBOX->Transfer(CommentBuf,tdGetData);
 AuthMsg.PutFld(FN_COMMENTS,CommentBuf); 
 replyMsg.PutFld(FN_TEXTRESPONSE,&OutPut,sizeof(OutPut));
 AuthMsg.PutFld(FN_TEXTRESPONSE,&OutPut,sizeof(OutPut));
 replyMsg.CopyFld(FN_TERMINFO,&AuthMsg);
 replyMsg.CopyFld(FN_RCODE,&AuthMsg);
 replyMsg.CopyFld(FN_APPROVALNUM,&AuthMsg);
 replyMsg.PutFld(FN_TRANTYPE,T_AUTHREPLY);
 msgWin->Say(MID_SENDINGAUTHREPLY);
 if ( msgWin2 )
  msgWin2->ShowWindow(SW_HIDE);
 captureWin=GetApplication()->GetMainWindow()->SetCapture();
 GetApplication()->GetMainWindow()->SetCursor(0,IDC_WAIT);
 ret = replyMsg.SendRecv(VOICEAUTH_DEST,(WORD)10000,true);

 // If this is a Preauth, have to send a forced preauth to the
 // database
 if ( AuthMsg.isPreauth() )
  {
   BYTE Rcode;
   AuthMsg.GetFld(FN_RCODE,Rcode);
   if ( rcodeInfo.IsApprovalRcode(Rcode) )
    {
     msgWin->Say(MID_SENDINGPREAUTH);
     AuthMsg.forcePreAuth();
    }
  }

 // If the Bank Phone Info is new or changed, send a record
 BVABA->Transfer(BankPhoneRec.BankNumber,tdGetData);
 BVBANKNAME->Transfer(BankPhoneRec.BankName,tdGetData);
 BVBANKPHONE->Transfer(BankPhoneRec.PhoneNumber,tdGetData);
 BVBANKCOMMENT->Transfer(BankPhoneRec.Comment,tdGetData);
 if ( BankPhoneRec.PhoneNumber[0] &&
      ( ! BankPhoneMsg.GetFld(FN_BANKPHONERECORD) ||
         memcmp(&BankPhoneRec,BankPhoneMsg.GetFldPtr(FN_BANKPHONERECORD),
                sizeof(BankPhoneRec))) )
  {
   BankPhoneMsg.PutFld(FN_BANKPHONERECORD,&BankPhoneRec,
                          sizeof(BankPhoneRec));
   BankPhoneMsg.PutFld(FN_TRANTYPE,T_ADDRECORD);
   if ( msgWin2 )
    msgWin2->ShowWindow(SW_HIDE);
   BankPhoneMsg.SendRecv(BANKPHONE_DEST,(WORD)10000,true);
  }

 GetApplication()->GetMainWindow()->SetCursor(0,IDC_ARROW);
 ::SetCapture(captureWin);

 // Do I need to send a Forced PREAUTH?

 // Figure out what to do here if it fails

 return ret;
}

//-------------------------------------------------------------
// Figure out what situation we have and display an appropriate
// message.
//--------------------------------------------------------------
void DecisionDialog::displayDecisionMsg()
{
 BYTE rCode;
 int  sayID;
 int  numRcodes=AuthMsg.numRcodesBreached();

 AuthMsg.GetFld(FN_RCODE,rCode);

 switch( rCode )
  {
   case RCODE_KENWOOD1: sayID= numRcodes ? MID_DECIDEKW1RCODES:
                                              MID_DECIDEKW1;
                        break;
   case RCODE_KENWOOD2: sayID= numRcodes ? MID_DECIDEKW2RCODES:
                                              MID_DECIDEKW2;
                        break;
   case RCODE_KENWOOD3: sayID= numRcodes ? MID_DECIDEKW3RCODES:
                                              MID_DECIDEKW3;
                        break;
   case RCODE_KENWOOD4: sayID= numRcodes ? MID_DECIDEKW4RCODES:
                                              MID_DECIDEKW4;
                        break;
   case RCODE_KENWOOD5: sayID= numRcodes ? MID_DECIDEKW5RCODES:
                                              MID_DECIDEKW5;
                        break;

   case RCODE_R1DECLINE: sayID = (numRcodes>1) ? MID_DECIDEMULTIRCODES:
                                                  MID_DECIDER1;
                         break;

   case RCODE_R2DECLINE: sayID = (numRcodes>1) ? MID_DECIDEMULTIRCODES:
                                                  MID_DECIDER2;
                         break;

   case RCODE_R3DECLINE: sayID = (numRcodes>1) ? MID_DECIDEMULTIRCODES:
                                                  MID_DECIDER3;
                         break;

   case RCODE_R4DECLINE: sayID = (numRcodes>1) ? MID_DECIDEMULTIRCODES:
                                                  MID_DECIDER4;
                         break;

   case RCODE_R5DECLINE: sayID = (numRcodes>1) ? MID_DECIDEMULTIRCODES:
                                                  MID_DECIDER5;
                         break;

   case RCODE_R6DECLINE: sayID = (numRcodes>1) ? MID_DECIDEMULTIRCODES:
                                                  MID_DECIDER6;
                         break;

   case RCODE_R7DECLINE: sayID = (numRcodes>1) ? MID_DECIDEMULTIRCODES:
                                                  MID_DECIDER7;
                         break;

   case RCODE_R8DECLINE: sayID = (numRcodes>1) ? MID_DECIDEMULTIRCODES:
                                                  MID_DECIDER8;
                         break;

   case RCODE_MULTIPLEPREAUTHS:
                        sayID = MID_DECIDEMULTIPREAUTHS; break;

   case RCODE_BADCHECKS:
                        sayID = numRcodes ? MID_DECIDELOSTSTOLENRCODES :
                                  MID_DECIDELOSTSTOLEN;
                        break;

   case RCODE_PREAUTHEXCEEDED:
                        sayID = MID_DECIDEPREAUTHEXCEEDED; break;

   default:  sayID=MID_DECIDEGENERIC; break;
  }

 msgWin->Say(sayID);
}

//------------------------------
// Do the Unverified Funds rules
//------------------------------
bool DecisionDialog::doUnverifiedFundsRules()
{
 int Score;
 char Phone[25];
 char SSN[10];
 int  YesNo;

 // Have to Have rules to do this
 if ( ! AuthMsg.hasScoringRules )
  return false;

 // If I have Points for SSN Scoring, then I need to get the SSN Score
 if ( ! isEmpty(AuthMsg.scoringRules.Data.SSNScoreValue[NUMSSNSCOREROWS-1]) &&
      AuthMsg.GetFld(FN_SSN,sizeof(SSN),SSN) )
  {
   msgWin->Say(MID_SSNSCORE);
   SSNScoreDialog(this,SSN,Score).Execute();
   if ( Score )
    AuthMsg.PutFld(FN_SSNSCORE,(DWORD)Score);
  }

 // If I have Points for Consumer Phone Number scoring and I have a
 // Consumer Phone Number, then I need to find out if the phone number
 // is good.
 if ( ( AuthMsg.scoringRules.Data.ConsumerPhoneGoodPoints ||
        AuthMsg.scoringRules.Data.ConsumerPhoneNoGoodPoints ) &&
      AuthMsg.GetFld(FN_PHONE,sizeof(Phone),Phone) )
  {
   msgWin->Say(MID_CONSUMERPHONEVERIFY);
   YesNoDialog(this,YesNo,Phone).Execute();
   AuthMsg.PutFld(FN_CONSUMERPHONEGOOD, (BYTE) (YesNo ? 'Y' : 'N' ));
  }

 // If I have Points for the Employer Phone Number and I have an
 // Employer Phone Number, then I need to find out if the phone number
 // is good.
 if ( ( AuthMsg.scoringRules.Data.EmployerPhoneGoodPoints ||
        AuthMsg.scoringRules.Data.EmployerPhoneNoGoodPoints ) &&
      AuthMsg.GetFld(FN_EMPLOYERPHONE,sizeof(Phone),Phone) )
  {
   msgWin->Say(MID_EMPLOYERPHONEVERIFY);
   YesNoDialog(this,YesNo,Phone).Execute();
   AuthMsg.PutFld(FN_EMPLOYERPHONEGOOD, (BYTE) (YesNo ? 'Y' : 'N' ));
  }

 return AuthMsg.ScoreTransaction(Score);
}

//------------------------------------------------------------------
// Checks to see if one of the verification radio buttons is checked.
// Displays error message and returns false if none checked.
//-------------------------------------------------------------------
bool DecisionDialog::isVerifyButtonChecked()
{
 if ( ! verifyButtonClicked )
  msgWin->Say(MID_NOVERIFYCHECK);
 return verifyButtonClicked; 
}



//----------------------------------------------------------------------------
//  Project ServerApp
//  JHE Software, Inc.
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    ServerApp Application
//  FILE:         ServerApp.cpp
//  AUTHOR:       Jack H Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  This is the Framework for your Server Application. Grow this
//  object to build your Server Application. It is instantiated when
//  the main window is created.
//
//  You need to build two objects, both contained herein, ie.,
//  the ServerAppInfo and the ServerApp objects. See below for details.
//----------------------------------------------------------------------------

#include "win4me.h"  
#include "ServerApp.h"
#include "messages.h"
#include "trantype.h"
#include "checkauthfn.h"
#include <time.h>
#include "DestName.h"
#include "SpsManDb.h"
#include "CheckService.h"
#include "rcodes.h"

// MAB 01/19/99
#include "stdio.h"
#include <time.h>


#define TIMEOUT (10*60)

#define MAXQ 25
static IPCMessage Queue[MAXQ];
static BYTE nextReferralNumber=1;

// MAB    01/19/99
/*
void llogg(int line, char *buf1, char *buf2, char *buf3, char *buf4, char *buf5, long l, int i );
void logg(int line, char *buf1, char *buf2, long l, int i );

void logg(int line, char *buf1, char *buf2, long l, int i )
{
   time_t tnow;
   FILE  *fout;
   time(&tnow);
   fout = fopen(".\\aaa.log", "a");
   fprintf(fout,"@%d. %s -%s-at %s. %ld-%d.\n", line, buf1, buf2, ctime(&tnow), l ,i);
   fclose(fout);
}
void llogg(int line, char *buf1, char *buf2, char *buf3, char *buf4, char *buf5, long l, int i )
{
//   char szbuf[128];
   time_t tnow;
   FILE  *fout;

   time(&tnow);
//   sprintf(szbuf,"%s",ctime(&tnow);

   fout = fopen(".\\aaa.log", "a");
   fprintf(fout,"@%d. %s src=%s-%s, dest=%s-%s-at %s. %ld-%d.\n", line, buf1, buf2, buf3, buf4, buf5, ctime(&tnow), l ,i);
   fclose(fout);
}
*/

//-----------------------------------------------------------------
// The ServerAppInfo object. Provides Information about the Server.
//-----------------------------------------------------------------
ServerAppInfo::ServerAppInfo()
{
 // You Must supply a Process Name for the Server
 strcpy(ServerProcessName,PROCESSNAME_REFERRALQUEUE);

 // Supply A Version Number for the Dialog Window and ADMIN commands.
 strcpy(ProgramVersionInfo,"1.2");
}

//---------------------------------------------------------------------
// ServerApp Constructor .. Do your Server Initialization Here, opening
// files, etc.
//---------------------------------------------------------------------
ServerApp::ServerApp()
{
 // Set this flag to true if your initialization succeeds. If it fails
 // set it to false. The Main function will abort the process.
 ServerInit=true;
 IPCLogEvent(ServerStarted);
}

//----------------------------------------------------------------------
// ServerApp Destructor .. Shut down your ServerApp here, closing files,
// etc.
//----------------------------------------------------------------------
ServerApp::~ServerApp()
{
 if ( ServerInit )
  {
   // Do the Shutdown stuff here. You don't want to do it if your
   // initialization failed.
   ServerInit=false;
   IPCLogEvent(ServerStopped);
  }
}


//----------------------------------------------------------------------
// Handle Your Application Messages Here. All your member functions have
// access to the IPCMessage. Reply or Send the Message after you have
// processed it. The calling function does not do this for you.
//-----------------------------------------------------------------------
void ServerApp::DoApplicationMsg()
{
 char TranType[10];
 int  Len=sizeof(TranType);

 //logg(__LINE__, "RefQue DoAppMsg()", "",0,0 );

 if ( ! Msg.GetFld(FN_TRANTYPE,Len,TranType) )
  return;

 //logg(__LINE__, "RefQue DoAppMsg() TranType is",TranType,0,0 );

 if ( strcmp(TranType,T_GETREFERRAL) == 0 )
 {
    //logg(__LINE__, "RefQue DoAppMsg() TranType is GetReferral",TranType,0,0 );
    getQueue();
 }
 else
 if ( strcmp(TranType,T_PUTREFERRAL) == 0 )
 {
    //logg(__LINE__, "RefQue DoAppMsg() TranType is PutReferral",TranType,0,0 );
    putQueue();
 }
//logg(__LINE__, "RefQue DoAppMsg() end",TranType,0,0 );
}

//---------------------------------------------------------------------
// Do your ADMIN Message Support here. ADMIN messages are text messages
// found in the IPCFN_ADMINMESSAGE field.
//
// The MAIN Window also has an ADMIN Message Handler to take default
// actions for common commands. Before taking the default action,
// the admin message is sent here for processing.
//
// If the ADMIN Message is one of your own commands or you want to
// override and prevent the default action from taking place, return
// true.
//
// If you don't recognize the command, return false to allow the
// default action to take place.
//---------------------------------------------------------------------
bool ServerApp::DoAdminMsg()
{
 char MsgText[255];
 int  len=sizeof(MsgText);

 // The field will always be there
 Msg.GetFld(IPCFN_ADMINMESSAGE,len,MsgText);

 // Catch the HELP command and insert your own Command list. The
 // Default Handler will append the default commands to your list.
 if ( memcmp(MsgText,"HELP",4) == 0 )
  {
   strcat(MsgText,", NOHELP");
   Msg.PutFld(IPCFN_ADMINMESSAGE,MsgText);
   return false;
  }

 return false;
}

//--------------------------------------
// Get a message from the referral queue
//--------------------------------------
void ServerApp::getQueue()
{
 BYTE ReferralNumRequested;
 BYTE ReferralNumQueued;
 time_t TimeQueued;
 int  i;
 IPCROUTEINFO routeInfo;

// MAB 01/19/99
   // char szbuf[512];
   char MsgText[128];
   float fTime;
   //time_t start, finish;
   //logg(__LINE__, "RefQue getQueue() ", "",0,0 );

 if ( ! Msg.GetFld(FN_REFERRALNUM,ReferralNumRequested) ||
      ReferralNumRequested == 0 )
 {
   //logg(__LINE__, "RefQue getQueue() ", "RequestNum",ReferralNumRequested,0 );
   return;
 }
 //logg(__LINE__, "RefQue getQueue() ", "RequestNum",ReferralNumRequested,0 );

 // Save the Routing Info from the request
 memcpy(&routeInfo,Msg.MsgRouteInfo(),sizeof(routeInfo));

 //llogg(__LINE__, "RefQue getQueue() ", routeInfo.SourceProcess,routeInfo.SourceComputer,
 //         routeInfo.DestProcess,  routeInfo.DestComputer,0,0);

 // Search the queue for a matching referral number.
 for (i=0; i<MAXQ; ++i)
  if ( Queue[i].GetFld(FN_REFERRALNUM,ReferralNumQueued) &&
       Queue[i].GetFld(FN_TIMESTAMP,(DWORD)TimeQueued) &&
       ReferralNumQueued==ReferralNumRequested )
  {
   //logg(__LINE__, "RefQue getQueue() ", "find RequestNum",ReferralNumRequested,0 );
   // Check if it's stale. Don't return it if it is
   if ( time(NULL) - TimeQueued < TIMEOUT )
    {
     //logg(__LINE__, "RefQue getQueue() ", "RequestNum",ReferralNumRequested,0 );
     Msg.CopyMsgIn(&Queue[i]);
     memcpy(Msg.MsgRouteInfo(),&routeInfo,sizeof(routeInfo));
    }
   //logg(__LINE__, "RefQue getQueue() ", "RequestNum",ReferralNumRequested,0 );
   // Delete this item from the queue
   Queue[i].DelFld(FN_REFERRALNUM);
   break;
  }
 //logg(__LINE__, "RefQue getQueue() ", "RequestNum End",ReferralNumRequested,0 );
 Msg.SendMsg(REFQUEUERESPONSE_DEST);
}

//------------------------------------
// Put a message to the referral queue
//------------------------------------
void ServerApp::putQueue()
{
 int i;
 time_t lowTimeStamp=time(NULL);
 int    lowTimeStampIndex=0;
 time_t TimeQueued;
 //logg(__LINE__, "RefQue putQueue() ", "",0,0 );

 // If this is a Hold Check Decline, we want to look for a hold check
 // approval for the same Merchant/Id. If present, we merge the data
 if ( mergeHoldCheck() )
  {
   Msg.SendMsg(REFQUEUERESPONSE_DEST);
   return;
  }
 else
  issueReferralNumber();
 //logg(__LINE__, "RefQue putQueue() ", "",0,0 );

 // Make sure transaction has a TimeStamp
 if ( ! Msg.GetFld(FN_TIMESTAMP) )
  Msg.PutFld(FN_TIMESTAMP,(DWORD)time(NULL));

 // Loop through the queue for an empty slot. Record the oldest message
 // in case we don't find an empty slot.
 for (i=0; i<MAXQ; ++i)
  {
   //logg(__LINE__, "RefQue putQueue() ", "",0,0 );
   if ( ! Queue[i].GetFld(FN_REFERRALNUM) ||
        ! Queue[i].GetFld(FN_TIMESTAMP,(DWORD)TimeQueued) )
    {
     //logg(__LINE__, "RefQue putQueue() ", "Find slot",0,0 );
     Queue[i].CopyMsgIn(&Msg);
     Msg.SendMsg(REFQUEUERESPONSE_DEST);
     return;
    }
   if ( TimeQueued < lowTimeStamp )
    {
     lowTimeStamp=TimeQueued;
     lowTimeStampIndex=i;
    }
  }

 // Queue is full, replace the oldest message
 Queue[ lowTimeStampIndex ].CopyMsgIn(&Msg);
 //logg(__LINE__, "RefQue putQueue() SendMsg End", "",0,0 );
 Msg.SendMsg(REFQUEUERESPONSE_DEST);
}

//------------------------------------
// Put a message to the referral queue
//------------------------------------
bool ServerApp::mergeHoldCheck()
{
 MerchantRecord* Merchant=(MerchantRecord*)Msg.GetFldPtr(FN_MERCHANTRECORD);
 MerchantRecord* QMerchant;
 BYTE Rcode;
 BYTE QRcode;
 int i;
 FLDNUM ChecknumFN, AmountFN, ApprovalNumFN;

 if ( Merchant == NULL ||
      Merchant->MerchantService != CHECKSERVICE_HOLDCHECK )
  return false;

 for (i=0; i<MAXQ; ++i)
  {
   // Look for same Hold Check Merchant
   if ( (QMerchant=(MerchantRecord*)Queue[i].GetFldPtr(FN_MERCHANTRECORD)) == NULL ||
        QMerchant->MerchantService != CHECKSERVICE_HOLDCHECK ||
        strcmp(QMerchant->MerchantID,Merchant->MerchantID) )
     continue;

   // If Driver's License is present, it must match
   if ( ! compareFields(Msg,Queue[i],FN_LICENSESTATE) ||
        ! compareFields(Msg,Queue[i],FN_LICENSE) )
     continue;

   // If Driver's License is not present, Account number must match
   if ( ! Msg.GetFld(FN_LICENSESTATE) &&
        ( ! compareFields(Msg,Queue[i],FN_BANKNUMBER) ||
          ! compareFields(Msg,Queue[i],FN_BANKACCOUNT) ) )
    continue;

   if ( ! Msg.GetFld(FN_RCODE,Rcode) ||
        ! Queue[i].GetFld(FN_RCODE,QRcode) )
    return false;

   // If Amount and Rcode is the same as the previous one,
   // consider it a duplicate.
   if ( Rcode == QRcode &&
        compareFields(Msg,Queue[i],FN_AMOUNT) )
     {
      // Delete the old item from the queue and requeue the new item
      Queue[i].DelFld(FN_REFERRALNUM);
      return false;
     }

   if ( ! Queue[i].GetFld(FN_HOLDCHECKAMOUNT2) )
    {
     ChecknumFN=FN_HOLDCHECKCHECKNUM2;
     AmountFN=FN_HOLDCHECKAMOUNT2;
     ApprovalNumFN=FN_HOLDCHECKAPPROVALNUM2;
    }
   else
   if ( ! Queue[i].GetFld(FN_HOLDCHECKAMOUNT3) )
    {
     ChecknumFN=FN_HOLDCHECKCHECKNUM3;
     AmountFN=FN_HOLDCHECKAMOUNT3;
     ApprovalNumFN=FN_HOLDCHECKAPPROVALNUM3;
    }
   else
   if ( ! Queue[i].GetFld(FN_HOLDCHECKAMOUNT4) )
    {
     ChecknumFN=FN_HOLDCHECKCHECKNUM4;
     AmountFN=FN_HOLDCHECKAMOUNT4;
     ApprovalNumFN=FN_HOLDCHECKAPPROVALNUM4;
    }
   else
    return false;

   // Copy Amount, Check Number and Approval Number (if any) from the
   // new message.
   Queue[i].CopyFld(AmountFN,FN_AMOUNT,&Msg);
   Queue[i].CopyFld(ChecknumFN,FN_CHECKNUMBER,&Msg);
   Queue[i].CopyFld(ApprovalNumFN,FN_APPROVALNUM,&Msg);

   // If it's not approved, issue a referral number
   if ( ! ISAPPROVALRCODE(Rcode) )
    {
     issueReferralNumber();
     Queue[i].CopyFld(FN_REFERRALNUM,&Msg);
    }

   return true;
  }


 // Queue it as the first hold check transaction with a Referral number
 // of Zero.
 Msg.CopyFld(FN_HOLDCHECKAMOUNT1,FN_AMOUNT);
 Msg.CopyFld(FN_HOLDCHECKCHECKNUM1,FN_CHECKNUMBER);
 Msg.CopyFld(FN_HOLDCHECKAPPROVALNUM1,FN_APPROVALNUM);
 Msg.PutFld(FN_REFERRALNUM,(BYTE)0);
 return false;
}

// Compare two fields in two messages to see if they're the same
bool ServerApp::compareFields(IPCMessage& Msg1,IPCMessage& Msg2,FLDNUM Fn)
{
 char Buf1[50];
 char Buf2[50];


 Buf1[0] = Buf2[0] = 0;
 Msg1.GetFld(Fn,sizeof(Buf1),Buf1);
 Msg2.GetFld(Fn,sizeof(Buf2),Buf2);

 return ( strcmp(Buf1,Buf2) ? false : true );
}


// Put the next referral number into the requestor's message. It goes
// in our queue and back to him.
void ServerApp::issueReferralNumber()
{
 Msg.PutFld(FN_REFERRALNUM,nextReferralNumber);
 if ( ++nextReferralNumber == 100 )
  nextReferralNumber=1;
}

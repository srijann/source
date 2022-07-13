//----------------------------------------------------------------------------
//  Project ServerApp
//  JHE Software, Inc.
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    ServerApp Application
//  FILE:         ServerApp.cpp
//  AUTHOR:       Jack H Ely
//
//  History
//
//  Ver   When  Who  What
//  ---   ----  ---  ------------------------------------------------
//   1.1  1/98  JHE  Changed open mode from "xq" to "xn" and removed
//                   calls to d_dbflush. Attempting to prevent database
//                   corruption.
//----------------------------------------------------------------------------

#include "ServerApp.h"
#include <time.h>
#include "TranType.h"
#include "DestName.h"
#include "CheckAuthFn.h"
#include "rcodes.h"
#include "Spsmandb.h"

// Max # of Preauths we will return on a fetch request
#define MAX_PREAUTHS 10

static PreauthRecord Record[MAX_PREAUTHS];
static MerchantRecord merchantRecord;

//-----------------------------------------------------------------
// The ServerAppInfo object. Provides Information about the Server.
//-----------------------------------------------------------------
ServerAppInfo::ServerAppInfo()
{
 // You Must supply a Process Name for the Server
 strcpy(ServerProcessName,PROCESSNAME_PREAUTHDB);

 // Supply A Version Number for the Dialog Window and ADMIN commands.
 strcpy(ProgramVersionInfo,"1.1");
}

//---------------------------------------------------------------------
// ServerApp Constructor .. Do your Server Initialization Here, opening
// files, etc.
//---------------------------------------------------------------------
ServerApp::ServerApp() :
 ServerInit(false)
{
 int Status;
 int retryCount;

 // Open the VELOCIS SPSManager Database
 for (retryCount=0; retryCount<10; ++retryCount)
   if ( s_login("rds","admin","secret",&SessionHandle) == S_OKAY )
     break;
   else
     Sleep(5000);

 if ( retryCount==10 )
  {
   IPCLogEvent(VelocisSecurityError);
   return;
  }

 // Open mode changed from "xq" to "xn" V 2.1
 if ( (Status=d_open("PreauthDb","xn",SessionHandle,&DBHandle)) != S_OKAY )
  {
   DBError(__FILE__,__LINE__,Status,"d_open");
   return;
  }

 // Set this flag to true if your initialization succeeds. If it fails
 // set it to false. The Main function will abort the process.
 ServerInit=true;
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
   d_close(SessionHandle);
   s_logout(SessionHandle);
   MessageBeep(0xffffffff);
  }
}

static char* TranTypeTable[] =
 {
  T_GETPREAUTHS,
  T_DOPREAUTH,
  T_DOPOSTAUTH,
  T_FORCEPREAUTH,
  NULL
 };

enum
 {
  GETPREAUTHS=0,
  DOPREAUTH,
  DOPOSTAUTH,
  FORCEPREAUTH,
  NOTINTABLE
 };

//----------------------------------------------------------------------
// Handle Your Application Messages Here. All your member functions have
// access to the IPCMessage. Reply or Send the Message after you have
// processed it. The calling function does not do this for you.
//-----------------------------------------------------------------------
void ServerApp::DoApplicationMsg()
{
 char TranType[20];
 int  I;

 Msg.GetFld(FN_TRANTYPE,sizeof(TranType),TranType);
 for (I=0; TranTypeTable[I] != NULL &&
           stricmp(TranTypeTable[I],TranType); ++I);
 switch( I )
  {
   case GETPREAUTHS: getPreauths(); break;

   case DOPREAUTH:   doPreauth(); break;

   case DOPOSTAUTH:  doPostAuth(); break;

   case FORCEPREAUTH: forcePreauth(); break;
   
   default:
        BADDATA("FN_TRANTYPE");
        break;
  }

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

 if ( memcmp(MsgText,"PURGE",5) == 0 )
  {
   doPurge();
   strcpy(MsgText,"OKAY");
   Msg.PutFld(IPCFN_ADMINMESSAGE,MsgText);
   return true;
  }

 return false;
}

//---------------------------------
// Handle Pre Authorization Request
//---------------------------------
void ServerApp::doPreauth()
{
 int numPreauths;
 int Len=sizeof(merchantRecord);
 short status;

 // Merchant Record is required
 if ( ! Msg.GetFld(FN_MERCHANTRECORD,&merchantRecord,Len) )
  {
   Msg.PutFld(FN_RCODE,(BYTE)RCODE_SYSTEMERROR);
   Msg.SendMsg(PREAUTH_REPLY);
   return;
  }

 // If there are already Preauths Outstanding, return the
 // Pre auth Outstanding Rcode.
 if ( (numPreauths=fetchALLPreauths()) > 0 )
  {
   // If only 1 PreAuth by same merchant within 15 minutes,
   // assume it's a duplicate and allow it
   if ( numPreauths==1 &&
        strcmp(merchantRecord.MerchantID,Record[0].PreauthMerchantID) == 0 &&
        time(NULL) - Record[0].TimeofPreauth <= 15 * 60 )
    {
     Msg.PutFld(FN_RCODE,(BYTE)RCODE_PREAUTHAPPROVAL);
     Msg.SendMsg(PREAUTH_REPLY);
     return;
    }

   // Otherwise, issue the multiple PreAuths rcode
   Msg.PutFld(FN_RCODE,(BYTE)RCODE_MULTIPLEPREAUTHS);
   Msg.PutFld(FN_PREAUTHRECORDS,&Record,
               numPreauths * sizeof(struct PreauthRecord));
   Msg.SendMsg(PREAUTH_REPLY);
   return;
  }

 // No Preauth outstanding, write one out
 Record[0].TimeofPreauth=time(NULL);
 Record[0].TimeofPostauth=0;
 strcpy(Record[0].PreauthMerchantID,merchantRecord.MerchantID);
 getID(Record[0].PreauthConsumerID);
 Record[0].PreauthRetention = merchantRecord.PreauthDays;
 if ( (status = d_fillnew(PREAUTHRECORD,&Record,DBHandle)) != S_OKAY )
  {
   DBError(__FILE__,__LINE__,status,"d_fillnew",PREAUTHRECORD);
   Msg.PutFld(FN_RCODE,(BYTE)RCODE_SYSTEMERROR);
  }
 // Call to Flush removed V 1.1
 // else
 // if ( (status=d_dbflush(DBHandle)) != S_OKAY )
 //  {
 //   DBError(__FILE__,__LINE__,status,"d_dbflush",PREAUTHRECORD);
 //   Msg.PutFld(FN_RCODE,(BYTE)RCODE_SYSTEMERROR);
 // }

 Msg.PutFld(FN_RCODE,(BYTE)RCODE_PREAUTHAPPROVAL);
 Msg.SendMsg(PREAUTH_REPLY);
}

//----------------------------------
// Handle Post authorization Request
//----------------------------------
void ServerApp::doPostAuth()
{
 int  Len=sizeof(merchantRecord);
 char ID[sizeof(Record[0].PreauthConsumerID)];
 short status;
 DWORD Amount;

 // Merchant Record is required
 if ( ! Msg.GetFld(FN_MERCHANTRECORD,&merchantRecord,Len) ||
      ! getID(ID) ||
      ! Msg.GetFld(FN_AMOUNTROUNDED,Amount) )
  {
   Msg.PutFld(FN_RCODE,(BYTE)RCODE_SYSTEMERROR);
   Msg.SendMsg(PREAUTH_REPLY);
   return;
  }

 // Look for a matching Preauth on file
 if ( d_keyfind(PREAUTHCONSUMERID,ID,DBHandle) == S_OKAY )
  for (;;)
   {
    if ( d_recread(PREAUTHRECORD,&Record,DBHandle) != S_OKAY ||
         strcmp(Record[0].PreauthConsumerID,ID) )
     break;
    if ( strcmp(Record[0].PreauthMerchantID,merchantRecord.MerchantID) == 0 &&
         ! isExpired(&Record[0]) &&
         ( Record[0].TimeofPostauth == 0 ||
           ( time(NULL) - Record[0].TimeofPostauth <= 15 * 60 &&
             Amount == Record[0].PostAuthAmount ) ) ) 
     {
      Msg.PutFld(FN_RCODE,(BYTE)RCODE_PREAUTHAPPROVAL);
      Msg.SendMsg(PREAUTH_REPLY);
      if ( Record[0].TimeofPostauth == 0 )
       {
        Record[0].TimeofPostauth=time(NULL);
        Record[0].PostAuthAmount=Amount;
        if ( (status=d_recwrite(PREAUTHRECORD,&Record,DBHandle)) != S_OKAY )
          DBError(__FILE__,__LINE__,status,"d_recwrite",PREAUTHRECORD);
        // Call to flush removed V 1.1
        // else
        //if ( (status=d_dbflush(DBHandle)) != S_OKAY )
        //  DBError(__FILE__,__LINE__,status,"d_dbflush",PREAUTHRECORD);
       }
      return;
     }

    if ( d_recnext(DBHandle) != S_OKAY )
     break;
   }



 // Rcode of zero means no PreAuth on file
 Msg.PutFld(FN_RCODE,(BYTE)0);
 Msg.SendMsg(PREAUTH_REPLY);
}

//-----------------------------------------------------------------
// Force a Preauth to be written to the file, regardless of whether
// one already exists
//-----------------------------------------------------------------
void ServerApp::forcePreauth()
{
 int  Len=sizeof(merchantRecord);
 short status;

 // Merchant Record is required
 if ( ! Msg.GetFld(FN_MERCHANTRECORD,&merchantRecord,Len) ||
      ! getID(Record[0].PreauthConsumerID) )
  {
   Msg.PutFld(FN_RCODE,(BYTE)RCODE_SYSTEMERROR);
   Msg.SendMsg(PREAUTH_REPLY);
   return;
  }

 Record[0].TimeofPreauth = time(NULL);
 Record[0].TimeofPostauth = 0;
 strcpy(Record[0].PreauthMerchantID,merchantRecord.MerchantID);
 if ( (status=d_fillnew(PREAUTHRECORD,&Record,DBHandle)) != S_OKAY )
  {
   DBError(__FILE__,__LINE__,status,"d_fillnew",PREAUTHRECORD);
   Msg.PutFld(FN_RCODE,(BYTE)RCODE_SYSTEMERROR);
  }
 // Call to flush removed V 1.1
 // else
 //if ( (status=d_dbflush(DBHandle)) != S_OKAY )
 // {
 //  DBError(__FILE__,__LINE__,status,"d_dbflush",PREAUTHRECORD);
 //  Msg.PutFld(FN_RCODE,(BYTE)RCODE_SYSTEMERROR);
 // }
 //else
 // Msg.PutFld(FN_RCODE,(BYTE)RCODE_PREAUTHAPPROVAL);

 Msg.SendMsg(PREAUTH_REPLY);
}

//----------------------------------------------
// Get all of the outstanding Preauths for an ID
//----------------------------------------------
void ServerApp::getPreauths()
{
 int numPreauths=fetchALLPreauths();

 if ( numPreauths > 0 )
   Msg.PutFld(FN_PREAUTHRECORDS,&Record,
                numPreauths * sizeof(struct PreauthRecord));

 Msg.SendMsg(PREAUTH_REPLY);
}

//-----------------------------------------------
// Fetch all of the unexpired PreAuths for the ID
//-----------------------------------------------
int ServerApp::fetchALLPreauths()
{
 char Key[sizeof(Record[0].PreauthConsumerID)];
 short status;
 int   i;

 if ( ! getID(Key) )
  return -1;

 if ( (status=d_keyfind(PREAUTHCONSUMERID,Key,DBHandle)) == S_NOTFOUND )
  return 0;

 if ( status != S_OKAY )
  {
   DBError(__FILE__,__LINE__,status,"d_keyfind",PREAUTHRECORD,PREAUTHCONSUMERID);
   return -1;
  }

 for (i=0; i<MAX_PREAUTHS; )
  {
   status=d_recread(PREAUTHRECORD,&Record[i],DBHandle);
   if ( status != S_OKAY ||
        strcmp(Key,Record[i].PreauthConsumerID) )
    return i;

   if ( ! isExpired(&Record[i]) ||
        Record[i].TimeofPostauth == 0 )
    ++i;

   if ( d_recnext(DBHandle)!=S_OKAY )
    break;
  }

 return i;
}

//-------------------
// Get ID for request
//-------------------
bool ServerApp::getID(char* Buf)
{
 int StateLen=3;
 int LicenseLen=sizeof(Record[0].PreauthConsumerID)-2;
 int ABALen;
 int AccountLen;

 memset(Record[0].PreauthConsumerID,0,sizeof(Record[0].PreauthConsumerID));
 if ( Msg.GetFld(FN_LICENSESTATE,StateLen,Buf) &&
      Msg.GetFld(FN_LICENSE,LicenseLen,&Buf[2]) )
  return true;

 // Contantinate MICR number as ID
 memset(Record[0].PreauthConsumerID,0,sizeof(Record[0].PreauthConsumerID));
 strcpy(Buf,"MR");
 ABALen=10;
 if ( Msg.GetFld(FN_BANKNUMBER,ABALen,&Buf[2]) )
  {
   ABALen=strlen(Buf);
   AccountLen=sizeof(Record[0].PreauthConsumerID)-ABALen;
   if ( Msg.GetFld(FN_BANKACCOUNT,AccountLen,&Buf[ABALen]) )
     return true;
  }

 return false;
}

//------------------------------------------
// Determine if a preauth record has expired
//------------------------------------------
bool ServerApp::isExpired(PreauthRecord* Rec)
{
 time_t now=time(NULL);
 return now - Rec->TimeofPreauth >= (uint) Rec->PreauthRetention * 24 * 60 * 60;
}

//------------------------------------------------------
// Purge Expired and Used up preauths from the data base
//------------------------------------------------------
bool ServerApp::doPurge()
{
 short stat;
 int   PurgeCnt;
 int   RecCnt;
 char sPurgeCnt[10], sRecCnt[10];

 for (RecCnt=PurgeCnt=0, stat=d_recfrst(PREAUTHRECORD,DBHandle);
      stat==S_OKAY;
      stat=d_recnext(DBHandle))
  {
   ++RecCnt;

   if ( (stat=d_recread(PREAUTHRECORD,&Record,DBHandle)) != S_OKAY )
    {
     DBError(__FILE__,__LINE__,stat,"d_recread",PREAUTHRECORD);
     return false;
    }

   if ( Record[0].TimeofPostauth != 0 ||
        isExpired(&Record[0]) )
    {
     d_delete(DBHandle);
     ++PurgeCnt;
    }
  }

 if ( stat != S_NOTFOUND )
  {
   DBError(__FILE__,__LINE__,stat,"d_recnext",PREAUTHRECORD);
   return false;
  }

 // Call to flush removed V 1.1
 // if ( (stat=d_dbflush(DBHandle)) != S_OKAY )
 // {
 //  DBError(__FILE__,__LINE__,stat,"d_dbflush",PREAUTHRECORD);
 //  return false;
 // }

 sprintf(sPurgeCnt,"%d",PurgeCnt);
 sprintf(sRecCnt,"%d",RecCnt);
 IPCLogEvent(PurgeComplete,0,sRecCnt,sPurgeCnt);
 return true;
}

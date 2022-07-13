//----------------------------------------------------------------------------
//  Project BankPhone
//  Secure Payment Systems
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    BankPhone Database Application
//  FILE:         ServerApp.cpp
//  AUTHOR:       Jack H Ely
//
//  OVERVIEW
//  ~~~~~~~~
//
//  This is the VELOCIS application server program for the Bank Phone
//  file. The Bank phone file is used to find the phone numbers and
//  names of banks, using the Bank number as a key.
//
//  History
//
//  Ver   When  Who  What
//  ---   ----  ---  ------------------------------------------------
//   1.1  1/98  JHE  Changed open mode from "xq" to "xn" and removed
//                   calls to d_dbflush. Attempting to prevent database
//                   corruption.
//
//----------------------------------------------------------------------------

#include "ServerApp.h"
#include <time.h>
#include "TranType.h"
#include "DestName.h"
#include "CheckAuthFn.h"
#include "messages.h"

static struct BankPhoneRecord recBuf;

//-----------------------------------------------------------------
// The ServerAppInfo object. Provides Information about the Server.
//-----------------------------------------------------------------
ServerAppInfo::ServerAppInfo()
{
 // You Must supply a Process Name for the Server
 strcpy(ServerProcessName,PROCESSNAME_BANKPHONE);

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

 // Open the VELOCIS SPSManager Database. Assume that the Velocis
 // service might be initializing as we're trying to open the database.
 // Therefore, the retry loop...
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

 // Open the Bank Phone Database
 if ( (Status=d_open("BankPhoneDB","xn",SessionHandle,&DBHandle)) != S_OKAY )
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
  }
}

// These are the Transaction Types we handle
static char* TranTypeTable[] =
 {
  T_GETRECORD,
  T_ADDRECORD,
  T_CHANGERECORD,
  T_DELETERECORD,
  NULL
 };

// Assign Number to the Transaction types for the switch table
enum
 {
  GETRECORD,
  ADDRECORD,
  CHANGERECORD,
  DELETERECORD,
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
   case GETRECORD: getRecord(); break;

   case CHANGERECORD:
   case ADDRECORD: addRecord(); break;

   case DELETERECORD: deleteRecord(); break;

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
 return false;
}

//-----------------------------------------------------
// Insert Reply structure and reply to a search request
//-----------------------------------------------------
void ServerApp::errorReply(unsigned short Response,char* Text)
{
 DBRESULT Result;

 Result.ResponseCode=Response;
 time((time_t*)&Result.TimeStamp);
 if ( strlen(Text) >= sizeof(Result.Text) )
  Text[sizeof(Result.Text)-1]=0;
 strcpy(Result.Text,Text);
 Msg.PutFld(FN_DBRESULT,&Result,sizeof(Result)-sizeof(Result.Text)+
    strlen(Result.Text)+1);
 Msg.SendMsg("REPLY");
}

//----------------------------------------
// Add a Bank Phone Record to the Database
//----------------------------------------
void ServerApp::addRecord()
{
 int Len=sizeof(recBuf);
 short status;

 // Get the Phone record
 if ( ! Msg.GetFld(FN_BANKPHONERECORD,&recBuf,Len) )
  return;

 // Attempt to Add it. Reply okay if successful
 if ( (status=d_fillnew(BANKPHONERECORD,&recBuf,DBHandle)) == S_OKAY )
  {
   Msg.SendMsg("REPLY");
   //if ( (status=d_dbflush(DBHandle)) != S_OKAY )
   //  DBError(__FILE__,__LINE__,status,"d_dbflush",BANKPHONERECORD);
   return;
  }

 // If it's not a duplicate error then send an error reply
 if ( status != S_DUPLICATE )
  {
   errorReply(DB_ERROR,"A Database error has occurred");
   DBError(__FILE__,__LINE__,status,"fillnew",BANKPHONERECORD);
   return;
  }

 // If it's a duplicate error, assume change is requested
 d_keyfind(BANKNUMBER,recBuf.BankNumber,DBHandle);
 if ( (status=d_recwrite(BANKPHONERECORD,&recBuf,DBHandle)) != S_OKAY )
  {
   errorReply(DB_ERROR,"A Database error has occurred");
   DBError(__FILE__,__LINE__,status,"fillnew",BANKPHONERECORD);
   return;
  }

 // Just return the message. No DBINFO indicates success
 Msg.SendMsg("REPLY");
 //if ( (status=d_dbflush(DBHandle)) != S_OKAY )
 //  DBError(__FILE__,__LINE__,status,"d_dbflush",BANKPHONERECORD);
}

//---------------------------------------------
// Get a positive file record from the database
//---------------------------------------------
void ServerApp::getRecord()
{
 short status=S_NOTFOUND;
 char  BankNumber[sizeof(recBuf.BankNumber)];
 int   Len;

 // Get Bank Number and Locate Record
 if ( Msg.GetFld(FN_BANKNUMBER,sizeof(BankNumber),BankNumber) )
  status=d_keyfind(BANKNUMBER,BankNumber,DBHandle);

 if ( status != S_OKAY )
  {
   if ( status==S_NOTFOUND )
    errorReply(DB_NOTFOUND,"RECORD NOT ON FILE");
   else
    {
     errorReply(DB_ERROR,"A DATABASE ERROR HAS OCCURRED");
     DBError(__FILE__,__LINE__,status,"d_keyfind",BANKPHONERECORD,BANKNUMBER);
    }
   return;
  }

 // read the record
 Len=sizeof(recBuf);
 memset(&recBuf,0,sizeof(recBuf));

 // Read record
 if ( (status=d_recread(BANKPHONERECORD,&recBuf,DBHandle)) != S_OKAY )
  {
   errorReply(DB_ERROR,"A Database error has occurred");
   DBError(__FILE__,__LINE__,status,"d_recread",BANKPHONERECORD);
   return;
  }

 Msg.PutFld(FN_BANKPHONERECORD,&recBuf,Len);
 Msg.SendMsg("REPLY");
}

//---------------------------------------------
// Get a positive file record from the database
//---------------------------------------------
void ServerApp::deleteRecord()
{
 short status=S_NOTFOUND;
 char  BankNumber[sizeof(recBuf.BankNumber)];

 // Get Bank Number and Locate Record
 if ( Msg.GetFld(FN_BANKNUMBER,sizeof(BankNumber),BankNumber) )
  status=d_keyfind(BANKNUMBER,BankNumber,DBHandle);

 if ( status != S_OKAY )
  {
   if ( status==S_NOTFOUND )
    errorReply(DB_NOTFOUND,"RECORD NOT ON FILE");
   else
    {
     errorReply(DB_ERROR,"A DATABASE ERROR HAS OCCURRED");
     DBError(__FILE__,__LINE__,status,"d_keyfind",BANKPHONERECORD,BANKNUMBER);
    }
   return;
  }

 // delete the record
 if ( (status=d_delete(DBHandle)) != S_OKAY )
  {
   errorReply(DB_ERROR,"A DATABASE ERROR HAS OCCURRED");
   DBError(__FILE__,__LINE__,status,"d_keyfind",BANKPHONERECORD,BANKNUMBER);
  }

 // Okay Reply
 Msg.SendMsg("REPLY");
}


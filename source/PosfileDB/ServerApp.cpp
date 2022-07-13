//----------------------------------------------------------------------------
//  Project ServerApp
//  JHE Software, Inc.
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    ServerApp Application
//  FILE:         ServerApp.cpp
//  AUTHOR:       Jack H Ely
//
//
//  History
//
//  Ver   When  Who  What
//  ---   ----  ---  ------------------------------------------------
//  1.1   1/98  JHE  Changed open mode from "xq" to "xn" and removed
//                   calls to d_dbflush. Attempting to prevent database
//                   corruption.
//  1.2   6/98  JHE  Changed to add Phone as key to file
//----------------------------------------------------------------------------

#include "ServerApp.h"
#include <time.h>
//#include <limits.h>
#include "TranType.h"
#include "DestName.h"
#include "CheckAuthFn.h"
#include "messages.h"

static struct PosfileRecord recBuf;
static struct PosfileRecord recBuf2;
static struct MicrKey micrKey;
static struct LicenseKey licenseKey;

//-----------------------------------------------------------------
// The ServerAppInfo object. Provides Information about the Server.
//-----------------------------------------------------------------
ServerAppInfo::ServerAppInfo()
{
 // You Must supply a Process Name for the Server
 strcpy(ServerProcessName,PROCESSNAME_POSFILE);

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

 // Open mode changed from "xq" to "xn" V 1.1
 
 if ( (Status=d_open("posfile","xn",SessionHandle,&DBHandle)) != S_OKAY )
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

static char* TranTypeTable[] =
 {
  T_GETRECORD,
  T_ADDRECORD,
  T_CHANGERECORD,
  T_DELETERECORD,
  T_POSFILEUPDATE,
  NULL
 };

enum
 {
  GETRECORD,
  ADDRECORD,
  CHANGERECORD,
  DELETERECORD,
  POSFILEUPDATE,
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

   case ADDRECORD: addRecord(); break;

   case CHANGERECORD: changeRecord(); break;

   case DELETERECORD: deleteRecord(); break;

   case POSFILEUPDATE: update(); break;

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

//-------------------------------------------
// Add a positive file record to the database
//-------------------------------------------
void ServerApp::addRecord()
{
 int Len=sizeof(recBuf);
 short status;

 if ( ! Msg.GetFld(FN_POSITIVEFILERECORD,&recBuf,Len) )
  return;

 // Insure MICR keys is unique
 if ( recBuf.BankNumber[0] &&
      d_keyfind(MICRKEY,recBuf.BankNumber,DBHandle) == S_OKAY )
  {
   errorReply(DB_ERROR,"Account Number already on file");
   return;
  }

 // Insure LICENSE key is unique
 if ( recBuf.LicenseState[0] &&
      d_keyfind(LICENSEKEY,recBuf.LicenseState,DBHandle) == S_OKAY )
  {
   errorReply(DB_ERROR,"License already on file");
   return;
  }

 // Insure Phone key is unique
 if ( recBuf.PhoneKey[0] &&
      d_keyfind(PHONEKEY,recBuf.PhoneKey,DBHandle) == S_OKAY )
  {
   errorReply(DB_ERROR,"PhoneKey Number already on file");
   return;
  }


 recBuf.FirstApproval = (unsigned long) time(NULL);
 recBuf.LastApproval = 0;
 recBuf.NumApprovals = 0;
 recBuf.AmountApprovals = 0;

 // Add the record
 if ( (status=d_fillnew(POSFILERECORD,&recBuf,DBHandle)) != S_OKAY )
  {
   errorReply(DB_ERROR,"A Database error has occurred");
   DBError(__FILE__,__LINE__,status,"fillnew",POSFILERECORD);
   return;
  }

 // Add License Key
 if ( recBuf.LicenseState[0] &&
      (status=d_keystore(LICENSEKEY,DBHandle)) != S_OKAY )
  {
   errorReply(DB_ERROR,"A Database error has occurred");
   DBError(__FILE__,__LINE__,status,"keystore",POSFILERECORD,LICENSEKEY);
   return;
  }

 // Add Micr Key
 if ( recBuf.BankNumber[0] &&
      (status=d_keystore(MICRKEY,DBHandle)) != S_OKAY )
  {
   errorReply(DB_ERROR,"A Database error has occurred");
   DBError(__FILE__,__LINE__,status,"keystore",POSFILERECORD,MICRKEY);
   return;
  }

  // Add PhoneKey Key
 if ( recBuf.PhoneKey[0] &&
      (status=d_keystore(PHONEKEY,DBHandle)) != S_OKAY )
  {
   errorReply(DB_ERROR,"A Database error has occurred");
   DBError(__FILE__,__LINE__,status,"keystore",POSFILERECORD,PHONEKEY);
   return;
  }

 // Just return the message. No DBINFO indicates success
 Msg.PutFld(FN_POSITIVEFILERECORD,&recBuf,sizeof(recBuf));
 Msg.SendMsg("REPLY");
 // Call to flush removed V 1.1
 //if ( (status=d_dbflush(DBHandle)) != S_OKAY )
 //  DBError(__FILE__,__LINE__,status,"d_dbflush",POSFILERECORD);
}

//---------------------------------------------
// Get a positive file record from the database
//---------------------------------------------
void ServerApp::getRecord()
{
 short status=S_NOTFOUND;
 long  key;

 // This indicates that the transaction has been to the positive file
 Msg.PutFld(FN_POSITIVEFILESTATUS,(BYTE)1);
 char PhoneKey[sizeof(recBuf.PhoneKey)];
 memset(PhoneKey,0,sizeof(PhoneKey));
 memset(&micrKey,0,sizeof(micrKey));
 memset(&licenseKey,0,sizeof(licenseKey));
 Msg.GetFld(FN_BANKNUMBER,sizeof(micrKey.BankNumber),
                      micrKey.BankNumber);
 Msg.GetFld(FN_BANKACCOUNT,sizeof(micrKey.BankAccount),
                      micrKey.BankAccount);
 Msg.GetFld(FN_LICENSESTATE,sizeof(licenseKey.LicenseState),
                        licenseKey.LicenseState);
 Msg.GetFld(FN_LICENSE,sizeof(licenseKey.License),licenseKey.License);
 Msg.GetFld(FN_PHONE,sizeof(PhoneKey),PhoneKey);

 // Try Bank Account
 if ( micrKey.BankNumber[0] )
  status=d_keyfind(key=MICRKEY,&micrKey,DBHandle);

 // Try License
 if ( status != S_OKAY )
   if ( licenseKey.LicenseState[0] )
    status=d_keyfind(key=LICENSEKEY,&licenseKey,DBHandle);

 // Try PhoneKey
 if ( status != S_OKAY )
   if ( PhoneKey[0] )
    status=d_keyfind(key=PHONEKEY,PhoneKey,DBHandle);

 // If got a key hit, read the record
 if ( status==S_OKAY )
  {
   int Len=sizeof(recBuf);
   memset(&recBuf,0,sizeof(recBuf));
   // Read primary record
   if ( (status=d_recread(POSFILERECORD,&recBuf,DBHandle)) != S_OKAY )
    {
     errorReply(DB_ERROR,"A Database error has occurred");
     DBError(__FILE__,__LINE__,status,"d_recread",POSFILERECORD);
     return;
    }

   // Turn off the HITKEY FLAGS if on
   recBuf.Flags &= ~POSFILEFLAGS_HITKEYMICR;
   recBuf.Flags &= ~POSFILEFLAGS_HITKEYLICENSE;
   recBuf.Flags &= ~POSFILEFLAGS_HITKEYPHONE;
   // Turn on Hit Key flags
   // This is needed by the change function so we can know which
   // key we hit on, in case the other's get changed
   if ( licenseKey.LicenseState[0] &&
        strcmp(recBuf.LicenseState,licenseKey.LicenseState) == 0 &&
        strcmp(recBuf.License,licenseKey.License) == 0 )
    recBuf.Flags |= POSFILEFLAGS_HITKEYLICENSE;

   if ( micrKey.BankNumber[0] &&
        strcmp(recBuf.BankNumber,micrKey.BankNumber) == 0 &&
        strcmp(recBuf.BankAccount,micrKey.BankAccount) == 0 )
    recBuf.Flags |= POSFILEFLAGS_HITKEYMICR;

   if ( PhoneKey[0] &&
        strcmp(recBuf.PhoneKey,PhoneKey) == 0 )
    recBuf.Flags |= POSFILEFLAGS_HITKEYPHONE;

   Msg.PutFld(FN_POSITIVEFILERECORD,&recBuf,Len);
   Msg.SendMsg("REPLY");
   return;
  }

 // We have either a NOTFOUND or an error condition
 if ( status==S_NOTFOUND )
  errorReply(DB_NOTFOUND,"RECORD NOT ON FILE");
 else
  {
   errorReply(DB_ERROR,"A DATABASE ERROR HAS OCCURRED");
//   DBError(__FILE__,__LINE__,status,"d_keyfind",POSFILERECORD,key);
  }
}

//------------------------------------------------
// Delete a positive file record from the database
//------------------------------------------------
void ServerApp::deleteRecord()
{
 short status=S_NOTFOUND;
 long  key;

 // Attempt to Find first by MICR key, then by License Key and PhoneKey Key
 if ( recBuf.BankNumber[0] )
   status=d_keyfind(key=MICRKEY,recBuf.BankNumber,DBHandle);

 if ( status==S_NOTFOUND &&
      recBuf.LicenseState[0] )
   status=d_keyfind(key=LICENSEKEY,recBuf.LicenseState,DBHandle);

 if ( status==S_NOTFOUND &&
      recBuf.PhoneKey[0] )
   status=d_keyfind(key=PHONEKEY,recBuf.PhoneKey,DBHandle);

 if ( status==S_OKAY )
  {
   if ( (status=d_disdel(DBHandle)) != DB_OKAY )
    {
     errorReply(DB_ERROR,"A DATABASE ERROR HAS OCCURRED");
     DBError(__FILE__,__LINE__,status,"d_disdel",POSFILERECORD);
    }
   Msg.SendMsg("REPLY");
   return;
  }

 // We have either a NOTFOUND or an error condition
 if ( status==S_NOTFOUND )
  errorReply(DB_NOTFOUND,"RECORD NOT ON FILE");
 else
  {
   errorReply(DB_ERROR,"A DATABASE ERROR HAS OCCURRED");
   DBError(__FILE__,__LINE__,status,"d_keyfind",POSFILERECORD,key);
  }

}

//----------------------------------------
// Change an existing positive file record
//----------------------------------------
void ServerApp::changeRecord()
{
 short status=S_NOTFOUND;
 long  key;
 int   Len=sizeof(recBuf);
 DB_ADDR Addr;

 if ( ! Msg.GetFld(FN_POSITIVEFILERECORD,&recBuf,Len) )
  return;

 // Attempt to Find first by MICR key and then by License Key and then by
 // PhoneKey. One of the keys has to be flagged as the current key.
 if ( (recBuf.Flags & POSFILEFLAGS_HITKEYMICR) &&
      recBuf.BankNumber[0] )
   status=d_keyfind(key=MICRKEY,recBuf.BankNumber,DBHandle);

 if ( status==S_NOTFOUND &&
      (recBuf.Flags & POSFILEFLAGS_HITKEYLICENSE) &&
      recBuf.LicenseState[0] )
   status=d_keyfind(key=LICENSEKEY,recBuf.LicenseState,DBHandle);

  if ( status==S_NOTFOUND &&
       (recBuf.Flags & POSFILEFLAGS_HITKEYPHONE) &&
      recBuf.PhoneKey[0] )
   status=d_keyfind(key=PHONEKEY,recBuf.PhoneKey,DBHandle);

 if ( status != S_OKAY )
  {
   // We have either a NOTFOUND or an error condition
   if ( status==S_NOTFOUND )
    errorReply(DB_NOTFOUND,"RECORD NOT ON FILE");
   else
    {
     errorReply(DB_ERROR,"A DATABASE ERROR HAS OCCURRED");
     DBError(__FILE__,__LINE__,status,"d_keyfind",POSFILERECORD,key);
    }
   return;
  }

 // Read the current record 
 if ( (status=d_recread(POSFILERECORD,&recBuf2,DBHandle)) != S_OKAY )
  {
   errorReply(DB_ERROR,"A Database error has occurred");
   DBError(__FILE__,__LINE__,status,"d_recread",POSFILERECORD);
   return;
  }
 d_crget(&Addr,DBHandle);

 // If any key is being changed, make sure it's not a duplicate
 // before attempting to add it.
 // Insure MICR keys is unique
 if ( recBuf.BankNumber[0] &&
      memcmp(recBuf.BankNumber,recBuf2.BankNumber,
              sizeof(struct MicrKey)) &&
      d_keyfind(MICRKEY,recBuf.BankNumber,DBHandle) == S_OKAY )
  {
   errorReply(DB_ERROR,"Account Number already on file");
   return;
  }

 // Insure LICENSE key is unique
 if ( recBuf.LicenseState[0] &&
      memcmp(recBuf.LicenseState,recBuf2.LicenseState,
              sizeof(struct LicenseKey)) &&
      d_keyfind(LICENSEKEY,recBuf.LicenseState,DBHandle) == S_OKAY )
  {
   errorReply(DB_ERROR,"License already on file");
   return;
  }

 // Insure PhoneKey key is unique
 if ( recBuf.PhoneKey[0] &&
      memcmp(recBuf.PhoneKey,recBuf2.PhoneKey,
              sizeof(recBuf.PhoneKey)) &&
      d_keyfind(PHONEKEY,recBuf.PhoneKey,DBHandle) == S_OKAY )
  {
   errorReply(DB_ERROR,"PhoneKey Number already on file");
   return;
  }

 d_crset(&Addr,DBHandle);

 // Write new record
 if ( (status=d_recwrite(POSFILERECORD,&recBuf,DBHandle)) != S_OKAY )
  {
   errorReply(DB_ERROR,"A Database error has occurred");
   DBError(__FILE__,__LINE__,status,"d_recwrite",POSFILERECORD);
   return;
  }

 // Store optional keys that didn't exist
 // Add License Key
 if ( recBuf.LicenseState[0] &&
      d_keyexist(LICENSEKEY,DBHandle) != S_OKAY &&
      (status=d_keystore(LICENSEKEY,DBHandle)) != S_OKAY )
  {
   errorReply(DB_ERROR,"A Database error has occurred");
   DBError(__FILE__,__LINE__,status,"keystore",POSFILERECORD,LICENSEKEY);
   return;
  }

 // Add Micr Key
 if ( recBuf.BankNumber[0] &&
      d_keyexist(MICRKEY,DBHandle) != S_OKAY &&
      (status=d_keystore(MICRKEY,DBHandle)) != S_OKAY )
  {
   errorReply(DB_ERROR,"A Database error has occurred");
   DBError(__FILE__,__LINE__,status,"keystore",POSFILERECORD,MICRKEY);
   return;
  }

  // Add Micr Key
 if ( recBuf.PhoneKey[0] &&
      d_keyexist(PHONEKEY,DBHandle) != S_OKAY &&
      (status=d_keystore(PHONEKEY,DBHandle)) != S_OKAY )
  {
   errorReply(DB_ERROR,"A Database error has occurred");
   DBError(__FILE__,__LINE__,status,"keystore",POSFILERECORD,PHONEKEY);
   return;
  }


 Msg.SendMsg("REPLY");
}

//-----------------------------------------------------------
// Update the positive file with transaction information. All
// Approvals come here.
//-----------------------------------------------------------
void ServerApp::update()
{
 short status=S_NOTFOUND;
 DWORD Amount;
 DWORD DOB;

 // This is a transaction in progress so we reply and then process
 // it
 Msg.SendMsg("REPLY");

 // Try Bank Account
 memset(&micrKey,0,sizeof(micrKey));
 if ( Msg.GetFld(FN_BANKNUMBER,sizeof(micrKey.BankNumber),
                      micrKey.BankNumber) &&
      Msg.GetFld(FN_BANKACCOUNT,sizeof(micrKey.BankAccount),
                      micrKey.BankAccount) )
  status=d_keyfind(MICRKEY,&micrKey,DBHandle);

 // Try License
 memset(&licenseKey,0,sizeof(licenseKey));
 if ( Msg.GetFld(FN_LICENSESTATE,sizeof(licenseKey.LicenseState),
                       licenseKey.LicenseState) &&
      Msg.GetFld(FN_LICENSE,sizeof(licenseKey.License),
                       licenseKey.License) &&
      status != S_OKAY )
    status=d_keyfind(LICENSEKEY,&licenseKey,DBHandle);

 // Try PhoneKey
 memset(recBuf.PhoneKey,0,sizeof(recBuf.PhoneKey));
 if ( Msg.GetFld(FN_PHONE,sizeof(recBuf.PhoneKey),recBuf.PhoneKey) &&
      status != S_OKAY )
    status=d_keyfind(PHONEKEY,recBuf.PhoneKey,DBHandle);

 Msg.GetFld(FN_AMOUNTROUNDED,Amount);
 if ( ! Msg.GetFld(FN_DOB,DOB) )
  DOB=0;

 // If got a key hit, read the record, update, and rewrite
 if ( status==S_OKAY )
  {
   DB_ADDR Addr;

   if ( (status=d_recread(POSFILERECORD,&recBuf,DBHandle)) != S_OKAY )
    {
     DBError(__FILE__,__LINE__,status,"d_recread",POSFILERECORD);
     return;
    }
   ++recBuf.NumApprovals;
   recBuf.AmountApprovals += Amount;
   recBuf.LastApproval = (unsigned long)time(NULL);
   if ( ! recBuf.DateOfBirth )
     recBuf.DateOfBirth = DOB;
   if ( ! recBuf.PhoneKey[0] )
    Msg.GetFld(FN_PHONE,sizeof(recBuf2.PhoneKey),
                 recBuf2.PhoneKey);
   if ( ! recBuf.ConsumerInfo.ConsumerName[0] )
    Msg.GetFld(FN_CONSUMERNAME,sizeof(recBuf.ConsumerInfo.ConsumerName),
                 recBuf.ConsumerInfo.ConsumerName);
   if ( ! recBuf.ConsumerInfo.BusinessName[0] )
    Msg.GetFld(FN_BUSINESSNAME,sizeof(recBuf.ConsumerInfo.BusinessName),
                 recBuf.ConsumerInfo.BusinessName);

   // If either of the keys have changed, make sure it's unique
   // before we try to store it
   if ( licenseKey.LicenseState[0] &&
        memcmp(licenseKey.LicenseState,recBuf.LicenseState,
                sizeof(licenseKey)) )
    {
     d_crget(&Addr,DBHandle);
     if ( d_keyfind(LICENSEKEY,&licenseKey,DBHandle) == S_NOTFOUND )
      memcpy(recBuf.LicenseState,&licenseKey,sizeof(licenseKey));
     d_crset(&Addr,DBHandle);
    }

   if ( micrKey.BankNumber[0] &&
        memcmp(micrKey.BankNumber,recBuf.BankNumber,
                sizeof(micrKey)) )
   {
    d_crget(&Addr,DBHandle);
    if ( d_keyfind(MICRKEY,&micrKey,DBHandle) == S_NOTFOUND )
      memcpy(recBuf.BankNumber,&micrKey,sizeof(micrKey));
    d_crset(&Addr,DBHandle);
   }

   if ( recBuf2.PhoneKey[0] &&
        memcmp(recBuf2.PhoneKey,recBuf.PhoneKey,sizeof(recBuf.PhoneKey)) )
   {
    d_crget(&Addr,DBHandle);
    if ( d_keyfind(PHONEKEY,recBuf2.PhoneKey,DBHandle) == S_NOTFOUND )
      memcpy(recBuf.PhoneKey,recBuf2.PhoneKey,sizeof(recBuf.PhoneKey));
    d_crset(&Addr,DBHandle);
   }


   if ( (status=d_recwrite(POSFILERECORD,&recBuf,DBHandle)) != S_OKAY )
     DBError(__FILE__,__LINE__,status,"d_recwrite",POSFILERECORD);

   // Store any keys that did not previously exist
   if ( recBuf.BankNumber[0] &&
        d_keyexist(MICRKEY,DBHandle) != S_OKAY )
     d_keystore(MICRKEY,DBHandle);

   if ( recBuf.LicenseState[0] &&
        d_keyexist(LICENSEKEY,DBHandle) != S_OKAY )
     d_keystore(LICENSEKEY,DBHandle);

   if ( recBuf.PhoneKey[0] &&
        d_keyexist(PHONEKEY,DBHandle) != S_OKAY )
     d_keystore(PHONEKEY,DBHandle);

   return;
  }

 // Add a new record
 memset(&recBuf,0,sizeof(recBuf));
 memcpy(recBuf.BankNumber,&micrKey,sizeof(micrKey));
 memcpy(recBuf.LicenseState,&licenseKey,sizeof(licenseKey));
 Msg.GetFld(FN_PHONE,sizeof(recBuf.PhoneKey),
              recBuf.PhoneKey);
 Msg.GetFld(FN_CONSUMERNAME,sizeof(recBuf.ConsumerInfo.ConsumerName),
              recBuf.ConsumerInfo.ConsumerName);
 Msg.GetFld(FN_BUSINESSNAME,sizeof(recBuf.ConsumerInfo.BusinessName),
              recBuf.ConsumerInfo.BusinessName);

 recBuf.LastApproval = recBuf.FirstApproval = time(NULL);
 recBuf.DateOfBirth = DOB;
 recBuf.NumApprovals = 1;
 recBuf.AmountApprovals = Amount;

  // Add the record
 if ( (status=d_fillnew(POSFILERECORD,&recBuf,DBHandle)) != S_OKAY )
  {
   DBError(__FILE__,__LINE__,status,"fillnew",POSFILERECORD);
   return;
  }

 // Add License Key
 if ( recBuf.LicenseState[0] &&
      (status=d_keystore(LICENSEKEY,DBHandle)) != S_OKAY )
  {
   DBError(__FILE__,__LINE__,status,"keystore",POSFILERECORD,LICENSEKEY);
   return;
  }

 // Add Micr Key
 if ( recBuf.BankNumber[0] &&
      (status=d_keystore(MICRKEY,DBHandle)) != S_OKAY )
  {
   DBError(__FILE__,__LINE__,status,"keystore",POSFILERECORD,MICRKEY);
   return;
  }

 // Add PhoneKey Key
  if ( recBuf.PhoneKey[0] &&
      (status=d_keystore(PHONEKEY,DBHandle)) != S_OKAY )
  {
   DBError(__FILE__,__LINE__,status,"keystore",POSFILERECORD,PHONEKEY);
   return;
  }

}

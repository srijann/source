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
//  2.1  1/98  JHE  Changed open mode from "xq" to "xn" and removed
//                  calls to d_dbflush. Attempting to prevent database
//                  corruption.
//
//                  Include code to convert activity records to version
//                  1.
//
//                  Provide Purge function by ADMIN Command
//
// 2.2  1/98  JHE   Fixed bug introduced in last modification wherein
//                  the starting key value was not correctly to
//                  correspond to the new sort order
//----------------------------------------------------------------------------

#include "ServerApp.h"
#include <time.h>
#include <limits.h>
#include "TranType.h"
#include "DestName.h"
#include "CheckAuthFn.h"
#include "messages.h"

static struct LogRecord logRec;
static int maxDBKeyInfo;
static DBKEYINFO* DBKeyInfo;

static union
 {
  struct MerchantKey Merchant;
  struct OperatorKey Operator;
  struct MicrKey Micr;
  struct LicenseKey License;
  struct SSNKey SSn;
  struct PhoneKey Phone;
  struct ApprovalNumberKey ApprovalNumber;
  struct DateKey Date;
 } SearchKey;

//-----------------------------------------------------------------
// The ServerAppInfo object. Provides Information about the Server.
//-----------------------------------------------------------------
ServerAppInfo::ServerAppInfo()
{
 // You Must supply a Process Name for the Server
 strcpy(ServerProcessName,PROCESSNAME_LOGGER);

 // Supply A Version Number for the Dialog Window and ADMIN commands.
 strcpy(ProgramVersionInfo,"2.2");
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
 unsigned short maxSize, size;

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
 
 if ( (Status=d_open("SPSLogDb","xn",SessionHandle,&DBHandle)) != S_OKAY )
  {
   DBError(__FILE__,__LINE__,Status,"d_open");
   return;
  }

 // Figure the Maximum size buffer we need to store Key State
 // Information.

 d_keyszstate(MERCHANTKEY,&maxSize,DBHandle);
 d_keyszstate(MICRKEY,&size,DBHandle);  maxSize = max(maxSize,size);
 d_keyszstate(LICENSEKEY,&size,DBHandle);  maxSize = max(maxSize,size);
 d_keyszstate(SSNKEY,&size,DBHandle);  maxSize = max(maxSize,size);
 d_keyszstate(PHONEKEY,&size,DBHandle);  maxSize = max(maxSize,size);
 d_keyszstate(APPROVALNUMBERKEY,&size,DBHandle);  maxSize = max(maxSize,size);
 d_keyszstate(OPERATORKEY,&size,DBHandle);  maxSize = max(maxSize,size);
 d_keyszstate(DATEKEY,&size,DBHandle); maxSize=max(maxSize,size);
 DBKeyInfo = (DBKEYINFO*) new
     char[ maxDBKeyInfo=maxSize + sizeof(DBKEYINFO) ];

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
   delete[] DBKeyInfo;
  }
}

static char* TranTypeTable[] =
 {
  T_VOICEAUTH,
  T_DBSEARCH,
  T_DBNEXT,
  "DOPURGE",
  NULL
 };

enum
 {
  VOICEAUTH,
  DBSEARCH,
  DBNEXT,
  DOPURGE,
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
   case VOICEAUTH:
        if ( logTransaction() )
         {
          Msg.SendMsg("REPLY");
          // Removed V 2.1
          //if ( (status=d_dbflush(DBHandle)) != S_OKAY )
          //  DBError(__FILE__,__LINE__,status,"d_dbflush",LOGRECORD);
         }
        break;

   case DBSEARCH:
        dbSearch();
        break;

   case DBNEXT:
        dbNext();
        break;

   case DOPURGE:
        Purge();
        break;     

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
   strcat(MsgText,", LOAD <filename>,PURGE <days>");
   Msg.PutFld(IPCFN_ADMINMESSAGE,MsgText);
   return false;
  }

 // Do Purge
 if ( memcmp(MsgText,"PURGE",5) == 0 )
  {
   char* sp;
   strtok(MsgText," ");

   if ( (sp=strtok(NULL," ")) == NULL )
    {
     Msg.PutFld(IPCFN_ADMINMESSAGE,"DAYS REQUIRED");
     return true;
    }

   Msg.PutFld(IPCFN_ADMINMESSAGE,"PURGE IN PROGRESS");
   // Send a Message to Myself telling me to start the purge
   PurgeAfterDays=atoi(sp);
   IPCMessage NewMsg;
   NewMsg.PutFld(FN_TRANTYPE,"DOPURGE");
   NewMsg.SendMsg(PROCESSNAME_LOGGER);
   return true;
  }

 // Handle the LOAD command
 if ( memcmp(MsgText,"LOAD",4) == 0 )
  {
   char* sp;
   FILE* f;

   strtok(MsgText," ");

   if ( (sp=strtok(NULL," ")) == NULL )
    {
     Msg.PutFld(IPCFN_ADMINMESSAGE,"FILENAME REQUIRED");
     return true;
    }
   if ( (f=fopen(sp,"rb")) == NULL )
    {
     Msg.PutFld(IPCFN_ADMINMESSAGE,"ERROR OPENING FILE");
     return true;
    }
   if ( loadFile(f) )
    Msg.PutFld(IPCFN_ADMINMESSAGE,"FILE LOADED");
   else
    Msg.PutFld(IPCFN_ADMINMESSAGE,"FILE LOAD FAILED!");

   return true;
  }

 return false;
}

//----------------------
// Log Voice Transaction
//----------------------
bool ServerApp::logTransaction()
{
 time_t TimeStamp;
 struct tm* t;
 char  timeBuf[9];
 char  ssn[10];
 short status;
 char* RecPtr=(char*)Msg.GetMsgPointer() + sizeof(IPCROUTEINFO);

 // Invalidate Current Key Position
 memset(&logRec,0,sizeof(logRec));

 // Fill in the key fields
 if ( ! Msg.GetFld(FN_TIMESTAMP,(DWORD)TimeStamp) )
  {
   TimeStamp=time(NULL);
   Msg.PutFld(FN_TIMESTAMP,(DWORD)TimeStamp);
  }

 t=localtime(&TimeStamp);
 sprintf(timeBuf,"%04d%02d%02d",t->tm_year+1900,t->tm_mon+1,t->tm_mday);
 logRec.Date = atoi(timeBuf);
 sprintf(timeBuf,"%02d%02d%02d",t->tm_hour,t->tm_min,t->tm_sec);
 logRec.Time = atoi(timeBuf);

 // Don't log transaction without merchant number
 if ( ! Msg.GetFld(FN_MERCHID,sizeof(logRec.MerchantID),
                     logRec.MerchantID) )
   return false;

 if ( ! Msg.GetFld(FN_OPERATORID,logRec.Operator) )
  logRec.Operator=1;
 Msg.GetFld(FN_PHONE,sizeof(logRec.PhoneNumber),logRec.PhoneNumber);
 Msg.GetFld(FN_APPROVALNUM,logRec.ApprovalNumber);
 Msg.GetFld(FN_BANKNUMBER,sizeof(logRec.BankNumber),logRec.BankNumber);
 Msg.GetFld(FN_BANKACCOUNT,sizeof(logRec.BankAccount),logRec.BankAccount);
 if ( Msg.GetFld(FN_LICENSESTATE,3,logRec.DriversLicense) )
  if ( ! Msg.GetFld(FN_LICENSE,sizeof(logRec.DriversLicense)-2,
               &logRec.DriversLicense[2]) )
    memset(logRec.DriversLicense,0,sizeof(logRec.DriversLicense));
 if ( Msg.GetFld(FN_SSN,sizeof(ssn),ssn) )
  logRec.SSn = atoi(ssn);

 // Delete the keys out of the message. Don't need to store them twice
 Msg.DelFld(FN_MERCHID);
 Msg.DelFld(FN_PHONE);
 Msg.DelFld(FN_BANKNUMBER);
 Msg.DelFld(FN_BANKACCOUNT);
 Msg.DelFld(FN_LICENSESTATE);
 Msg.DelFld(FN_LICENSE);
 Msg.DelFld(FN_SSN);
 Msg.DelFld(FN_OPERATORID);
 logRec.MsgLength = (WORD)( Msg.GetMsgLength() - sizeof(IPCROUTEINFO));
 memcpy(logRec.Message,RecPtr,
     min((int)sizeof(logRec.Message),
         (int)logRec.MsgLength));

 // Write Primary record
 if ( (status = d_fillnew(LOGRECORD,&logRec,DBHandle)) != S_OKAY )
  {
   DBError(__FILE__,__LINE__,status,"d_fillnew",LOGRECORD);
   return false;
  }

 // Store the optional keys
 if ( logRec.PhoneNumber[0] &&
      (status=d_keystore(PHONEKEY,DBHandle)) != S_OKAY )
   DBError(__FILE__,__LINE__,status,"d_keystore",PHONEKEY);

 if ( logRec.BankNumber[0] &&
     (status=d_keystore(MICRKEY,DBHandle)) != S_OKAY )
   DBError(__FILE__,__LINE__,status,"d_keystore",MICRKEY);

 if ( logRec.DriversLicense[0] &&
     (status=d_keystore(LICENSEKEY,DBHandle)) != S_OKAY )
   DBError(__FILE__,__LINE__,status,"d_keystore",LICENSEKEY);

 if ( logRec.SSn &&
     (status=d_keystore(SSNKEY,DBHandle)) != S_OKAY )
   DBError(__FILE__,__LINE__,status,"d_keystore",SSNKEY);

 if ( logRec.ApprovalNumber &&
     (status=d_keystore(APPROVALNUMBERKEY,DBHandle)) != S_OKAY )
   DBError(__FILE__,__LINE__,status,"d_keystore",APPROVALNUMBERKEY);

 return true;
}

//--------------
// Get Date/Time
//
//   Date must be in form YYYYMMDD
//   Time must be in form HHMMSS
//--------------
void ServerApp::getDateTime(unsigned long& Date,unsigned long& Time)
{
 if ( ! Msg.GetFld(FN_DATE,Date) )
  Date=ULONG_MAX;
 if ( !  Msg.GetFld(FN_TIME,Time) )
  Time=ULONG_MAX;
}

//-------------------------
// Get key from Transaction
//-------------------------
bool ServerApp::getKey(long& KeyNum)
{
 memset(&SearchKey,0,sizeof(SearchKey));

 if ( Msg.GetFld(FN_MERCHID,sizeof(SearchKey.Merchant.MerchantID),
                    SearchKey.Merchant.MerchantID) )
  {
   getDateTime(SearchKey.Merchant.Date,SearchKey.Merchant.Time);
   KeyNum=MERCHANTKEY;
   return true;
  }
 if ( Msg.GetFld(FN_OPERATORID,SearchKey.Operator.Operator) )
  {
   getDateTime(SearchKey.Operator.Date,SearchKey.Operator.Time);
   KeyNum=OPERATORKEY;
   return true;
  }
 if ( Msg.GetFld(FN_BANKNUMBER,sizeof(SearchKey.Micr.BankNumber),
                    SearchKey.Micr.BankNumber) )
  {
   Msg.GetFld(FN_BANKACCOUNT,sizeof(SearchKey.Micr.BankAccount),
                    SearchKey.Micr.BankAccount);
   getDateTime(SearchKey.Micr.Date,SearchKey.Micr.Time);
   KeyNum=MICRKEY;
   return true;
  }
 if ( Msg.GetFld(FN_LICENSESTATE,3,SearchKey.License.DriversLicense) )
  {
   Msg.GetFld(FN_LICENSE,sizeof(SearchKey.License.DriversLicense)-2,
               &SearchKey.License.DriversLicense[2]);
   getDateTime(SearchKey.License.Date,SearchKey.License.Time);
   KeyNum=LICENSEKEY;
   return true;
  }
 if ( Msg.GetFld(FN_SSN,SearchKey.SSn.SSn) )
  {
   getDateTime(SearchKey.SSn.Date,SearchKey.SSn.Time);
   KeyNum=SSNKEY;
   return true;
  }
 if ( Msg.GetFld(FN_PHONE,sizeof(SearchKey.Phone.PhoneNumber),
                  SearchKey.Phone.PhoneNumber) )
  {
   getDateTime(SearchKey.Phone.Date,SearchKey.Phone.Time);
   KeyNum=PHONEKEY;
   return true;
  }
 if ( Msg.GetFld(FN_APPROVALNUM,SearchKey.ApprovalNumber.ApprovalNumber) )
  {
   getDateTime(SearchKey.ApprovalNumber.Date,SearchKey.ApprovalNumber.Time);
   KeyNum=APPROVALNUMBERKEY;
   return true;
  }

 KeyNum=DATEKEY;
 getDateTime(SearchKey.Date.Date,SearchKey.Date.Time);
 return true;
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

//-------------------
// Start a new search
//-------------------
void ServerApp::dbSearch()
{
 long KeyVal;
 short status;

 getKey(KeyVal);

 if ( (status=d_keyfind(KeyVal,&SearchKey,DBHandle)) == S_NOTFOUND )
   status=d_keynext(KeyVal,DBHandle);

 if ( status==S_OKAY )
  {
   DBKeyInfo->KeyField = KeyVal;
   d_keyrdstate(KeyVal,DBKeyInfo->KeyState,DBHandle);

   if ( ! dbRead() || ! copyLogRecordToMsg() )
     errorReply(DB_ERROR,"A Database Error Has Occurred!");
   else
    {
     unsigned short size;
     d_keyszstate(KeyVal,&size,DBHandle);
     Msg.PutFld(FN_DBKEY,DBKeyInfo,size+sizeof(DBKEYINFO));
     Msg.SendMsg("REPLY");
    }

   return;
  }

 // Log Error, if any
 if ( status != S_NOTFOUND )
  {
   errorReply(DB_ERROR,"A Database Error Has Occurred!");
   DBError(__FILE__,__LINE__,status,"d_keyfind",LOGRECORD,KeyVal);
   return;
  }

 errorReply(DB_NOTFOUND,"EOF");
}

//----------------------------------------------------------
// Read Log record and all sets into global logRec buffer
//----------------------------------------------------------
bool ServerApp::dbRead()
{
 short status=d_recread(LOGRECORD,&logRec,DBHandle);
 
 if ( status != S_OKAY )
  {
   DBError(__FILE__,__LINE__,status,"d_recread",LOGRECORD);
   return false;
  }

 return true;
}

//---------------------
// Retrieve Next Record
//---------------------
void ServerApp::dbNext()
{
 int Len=maxDBKeyInfo;
 short status;

 // Get the Key Info structure and restore the Key state
 if ( ! Msg.GetFld(FN_DBKEY,DBKeyInfo,Len) )
  {
   errorReply(DB_ERROR,"KEY STATE NOT PRESENT");
   return;
  }

 if ( (status=d_keywrstate(DBKeyInfo->KeyField,DBKeyInfo->KeyState,
                              DBHandle)) != S_OKAY )
  {
   errorReply(DB_ERROR,"A DATABASE ERROR HAS OCCURRED");
   DBError(__FILE__,__LINE__,status,"d_keywrstate",LOGRECORD,
                 DBKeyInfo->KeyField);
   return;
  }

 if ( (status=d_keynext(DBKeyInfo->KeyField,DBHandle))== S_OKAY )
  {
   d_keyrdstate(DBKeyInfo->KeyField,DBKeyInfo->KeyState,DBHandle);

   if ( ! dbRead() || ! copyLogRecordToMsg() )
     errorReply(DB_ERROR,"A Database Error Has Occurred!");
   else
    {
     unsigned short size;
     d_keyszstate(DBKeyInfo->KeyField,&size,DBHandle);
     Msg.PutFld(FN_DBKEY,DBKeyInfo,size+sizeof(DBKEYINFO));
     Msg.SendMsg("REPLY");
    }

   return;
  }

 // Log Error, if any
 if ( status != S_NOTFOUND )
  {
   errorReply(DB_ERROR,"A Database Error Has Occurred!");
   DBError(__FILE__,__LINE__,status,"d_keyfind",LOGRECORD,
                DBKeyInfo->KeyField);
   return;
  }

 errorReply(DB_NOTFOUND,"EOF");
}

//----------------------------------------------------------------
// Copy the record in the global logRec buffer into the current
// Msg
//----------------------------------------------------------------
bool ServerApp::copyLogRecordToMsg()
{
 int Offset=sizeof(IPCROUTEINFO);
 char* RecPtr=(char*)Msg.GetMsgPointer() + Offset;
 memcpy(RecPtr,logRec.Message,logRec.MsgLength);
 Msg.SetMsgLength((WORD)(logRec.MsgLength + sizeof(IPCROUTEINFO)));
 if ( ! Msg.ParseMsg() )
  {
   IPCLogEvent(TrashedBuffer,0,"copyLogRecordToMsg");
   return false;
  }

 // Re-Insert Key Fields
 Msg.PutFld(FN_MERCHID,logRec.MerchantID);
 Msg.PutFld(FN_OPERATORID,logRec.Operator);
 if ( logRec.SSn )
  Msg.PutFld(FN_SSN,logRec.SSn);
 if ( logRec.PhoneNumber[0] )
  Msg.PutFld(FN_PHONE,logRec.PhoneNumber);
 if ( logRec.ApprovalNumber )
  Msg.PutFld(FN_APPROVALNUM,logRec.ApprovalNumber);
 if ( logRec.BankNumber[0] )
  Msg.PutFld(FN_BANKNUMBER,logRec.BankNumber);
 if ( logRec.BankAccount[0] )
  Msg.PutFld(FN_BANKACCOUNT,logRec.BankAccount);
 if ( logRec.DriversLicense[0] )
  {
   char State[3];
   memcpy(State,logRec.DriversLicense,2);
   State[2]=0;
   Msg.PutFld(FN_LICENSESTATE,State);
   Msg.PutFld(FN_LICENSE,&logRec.DriversLicense[2]);
  }

 // See if Activity record conversion is required
 ConvertActivity();
 return true;
}

//------------------------------
// Load records from a flat file
//------------------------------
bool ServerApp::loadFile(FILE* f)
{
 uint Len;

 while( fread(&Len,sizeof(Len),1,f) )
  {
   if ( fread(Msg.GetMsgPointer(),1,Len,f) != Len )
    return false;
   Msg.SetMsgLength((unsigned short)Len);
   if (! Msg.ParseMsg() )
    return false;
   logTransaction();
  }

 fclose(f);
 // Removed V 2.1
 //if ( (status=d_dbflush(DBHandle)) != S_OKAY )
 //    DBError(__FILE__,__LINE__,status,"d_dbflush",LOGRECORD);
 return true;
}

//-------------------------
// Purge All Records > Days
//-------------------------
void ServerApp::Purge()
{
 time_t PurgeTime=time(NULL) - (PurgeAfterDays) * 24 * 60 * 60;
 struct tm* t=localtime(&PurgeTime);
 char   Date[9];
 short  status;
 struct DateKey Key;
 int    NumPurged=0;
 int    NumRetained=0;

 sprintf(Date,"%04d%02d%02d",t->tm_year+1900,t->tm_mon+1,t->tm_mday);
 Key.Date = atoi(Date);
 Key.Time = 0;

 if ( (status=d_recfrst(LOGRECORD,DBHandle)) != S_OKAY &&
      status != S_NOTFOUND )
  {
   DBError(__FILE__,__LINE__,status,"d_recfrst",LOGRECORD);
   return;
  }

 do
  {
   if ( (status=d_recread(LOGRECORD,&logRec,DBHandle)) != S_OKAY )
    {
     DBError(__FILE__,__LINE__,status,"d_recread",LOGRECORD);
     return;
   }
   if ( logRec.Date < Key.Date )
    {
     if ( (status=d_delete(DBHandle)) != S_OKAY )
      {
       DBError(__FILE__,__LINE__,status,"d_delete",LOGRECORD);
       return;
      }
     ++NumPurged; 
    }
   else
    ++NumRetained;
  }
 while( (status=d_recnext(DBHandle)) == S_OKAY );

 if ( status != S_NOTFOUND )
  DBError(__FILE__,__LINE__,status,"d_recnext",LOGRECORD);

 char Purged[20];
 char Retained[20];
 sprintf(Purged,"%d",NumPurged);
 sprintf(Retained,"%d",NumRetained);
 IPCLogEvent(PurgeComplete,0,Retained,Purged);
}

//--------------------------------------------
// Convert Activity records to current version
//--------------------------------------------
#undef SSN
#include "kwrec.h"
void ServerApp::ConvertActivity()
{
 int Len;
 ACTIVITY* ARec=(ACTIVITY*)Msg.GetFldPtr(FN_ACTIVITY,&Len);

 // The current version is 1
 if ( ARec==NULL || ARec->Version == 1 )
  return;

 // Convert Version 0 to Version 1
 ACTIVITY_VERSION_0* ARec0=(ACTIVITY_VERSION_0*)ARec;
 int NumRecords=Len / sizeof(ACTIVITY_VERSION_0);
 ACTIVITY* ARec1=new ACTIVITY[NumRecords];
 int i;

 // Version 0 has no Version number or ID field.
 for (i=0; i<NumRecords; ++i)
  {
   memcpy(ARec1[i].MerchID,ARec0[i].MerchID,sizeof(ACTIVITY_VERSION_0));
   ARec1[i].Version=1;
   memset(ARec1[i].Ids,' ',sizeof(ARec1[i].Ids));
  }

 Msg.PutFld(FN_ACTIVITY,ARec1,NumRecords*sizeof(ACTIVITY));
 delete[] ARec1;
}



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
//
//  Risk Management Database
//  Data Components
//
//      Rule Classes / SubClasses
//      PosFile Rule Classes
//      Data Requirement Definition Classes
//      Security Rule Classes
//      Account Records
//      Merchant Records
//
//  History
//
//  Ver   When  Who  What
//  ---   ----  ---  ------------------------------------------------
//  1.08  1/98  JHE  Changed open mode from "xq" to "xn" and removed
//                   calls to d_dbflush. Attempting to prevent database
//                   corruption.
//----------------------------------------------------------------------------

#include "ServerApp.h"
#include <classlib/file.h>
#include "StringList.h"
#include "TranType.h"
#include "OpenInfo.h"
#include "DataReq.h"
#include "UnvFundsRules.h"
#include "MerchantRec.h"
#include "PosResponse.h"
#include "PositiveRules.h"
#define MAXRECSIZE 8192
#include "DestName.h"
#include "kwrec.h"
#include "ListDefs.h"

static PositiveRules globalPositiveRules;
static PositiveRules workingPositiveRules;
static DataRequirements globalDataReq;
static DataRequirements workingDataReq;
static UnvFundsRules globalUnvFundsRules;
static UnvFundsRules workingUnvFundsRules;
static MerchantRec stationRec;
static MerchantRec accountRec;
static PosResponse globalPosResponse;
static PosResponse workingPosResponse;
static MERCHANT    kwMerchant;

static char IOBuf[MAXRECSIZE];

static StringList* PositiveRulesList;
static StringList* UnvFundsRulesList;
static StringList* DataReqList;
static StringList* PosResponseList;
static OpenClassInfo ClassInfo;
static struct ListRecord listRecord;

//-----------------------------------------------------------------
// The ServerAppInfo object. Provides Information about the Server.
//-----------------------------------------------------------------
ServerAppInfo::ServerAppInfo()
{
 // You Must supply a Process Name for the Server
 strcpy(ServerProcessName,PROCESSNAME_MANAGERDB);

 // Supply A Version Number for the Dialog Window and ADMIN commands.
 strcpy(ProgramVersionInfo,"1.08");
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

 if ( (Status=d_open("SPSManV23","xn",SessionHandle,&DBHandle)) != S_OKAY )
  {
   DBError(__FILE__,__LINE__,Status,"d_open");
   return;
  }

 if ( ! InitLists() )
  return;

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
   if  (PositiveRulesList)
    delete PositiveRulesList;
   if ( UnvFundsRulesList )
    delete UnvFundsRulesList;
   if ( DataReqList )
    delete DataReqList;
   if ( PosResponseList )
    delete PosResponseList;
  }
}

static char* TranTypeTable[] =
 {
  T_GETLIST,
  T_PUTLIST,
  T_GETAUTHRULES,
  T_PUTAUTHRULES,
  T_GETDATAREQ,
  T_PUTDATAREQ,
  T_GETUNVFUNDSRULES,
  T_PUTUNVFUNDSRULES,
  T_GETPOSFILERULES,
  T_PUTPOSFILERULES,
  T_GETPOSRESPONSEREC,
  T_PUTPOSRESPONSEREC,
  T_GETPOSRESPONSETEXT,
  T_GETMERCHANTREC,
  T_PUTMERCHANTREC,
  T_VOICEAUTH,
  T_POSAUTH,
  NULL
 };

enum
 {
  GETLIST=0,
  PUTLIST,
  GETAUTHRULES,
  PUTAUTHRULES,
  GETDATAREQ,
  PUTDATAREQ,
  GETUNVFUNDSRULES,
  PUTUNVFUNDSRULES,
  GETPOSFILERULES,
  PUTPOSFILERULES,
  GETPOSRESPONSEREC,
  PUTPOSRESPONSEREC,
  GETPOSRESPONSETEXT,
  GETMERCHANTREC,
  PUTMERCHANTREC,
  VOICEAUTH,
  POSAUTH,
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

   case GETLIST:           GetList(); break;

   case PUTLIST:           PutList(); break; 

   case GETAUTHRULES:      GetAuthRules(); break;

   case PUTAUTHRULES:      PutAuthRules(); break;

   case GETDATAREQ:        GetDataReq(); break;

   case PUTDATAREQ:        PutDataReq(); break;

   case GETUNVFUNDSRULES:  GetUnvFundsRules(); break;

   case PUTUNVFUNDSRULES:  PutUnvFundsRules(); break;

   case GETPOSFILERULES:    GetPositiveRules(); break;

   case PUTPOSFILERULES:    PutPositiveRules(); break;

   case GETPOSRESPONSEREC:  GetResponseRec(); break;

   case PUTPOSRESPONSEREC:  PutResponseRec(); break;

   case GETPOSRESPONSETEXT: GetResponseText(); break;

   case GETMERCHANTREC:     GetMerchantRec(); break;

   case PUTMERCHANTREC:     PutMerchantRec(); break;

   case VOICEAUTH:
   case POSAUTH:            CheckAuthorization(); break;

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

 return false;
}

//-----------------------------------------
// Log a file Error (not for VELOCIS files)
//-----------------------------------------
bool ServerApp::LogError(char *IOType,char *FileName)
{
 IPCLogEvent(FileError,0,IOType,FileName,strerror(errno));
 return false;
}


//----------------------------------------------------------
// Initialize all the Lists we have to service for the GUI's
//----------------------------------------------------------
bool ServerApp::InitLists()
{
 // Call the List Init functions
 if ( ! InitAuthRules() ||
      ! InitDataReq() ||
      ! InitUnvFundsRules() ||
      ! InitPositiveRules() ||
      ! InitPosResponse() )
  return false;

 return true;
}

//---------------------------------------
// Initialize the Data Requirements stuff
//---------------------------------------
bool ServerApp::InitDataReq()
{
 char Key[sizeof(workingDataReq.Data.DataReqClassName)];
 int dbStatus;

 DataReqList = new StringList(1024,1024,DATAREQ_LIST);

 // Init the Data Requirements Class List
 while( (dbStatus=d_keynext(DATAREQCLASSNAME,DBHandle)) == S_OKAY )
  {
   if ( (dbStatus=d_keyread(DATAREQCLASSNAME,Key,DBHandle)) != S_OKAY )
    {
     DBError(__FILE__,__LINE__,dbStatus,"d_keyread",-1,DATAREQCLASSNAME);
     return false;
    }
   DataReqList->Add(Key);
  }

 // Get the Global RULES record, make an empty one if not
 // present. Store it in global memory as we expect it to be
 // accessed frequently
 if ( d_keyfind(DATAREQCLASSNAME,"DEFAULT",DBHandle) != S_OKAY ||
      d_recread(DATAREQRECORD,&globalDataReq.Data,DBHandle) != S_OKAY )
  {
   globalDataReq.FormatNew("DEFAULT");
   DataReqList->Add("DEFAULT");
  }

 return true;
}

//-------------------------------
// Get a Data Requirements Record
//-------------------------------
void ServerApp::GetDataReq()
{
 int Len=sizeof(ClassInfo);

 // Get the ClassInfo structure from the record and use it as the key.
 // It must be present.
 if ( ! Msg.GetFld(FN_DBKEY,&ClassInfo,Len) )
  {
   DBReply(DB_ERROR);
   BADDATA("KEY DATA");
   return;
  }

 // If Class is Global return the GLOBAL record
 if ( strcmp(ClassInfo.ClassName,"DEFAULT") == 0 )
  {
   Msg.PutFld(FN_DATAREQRECORD,&globalDataReq.Data,
                sizeof(globalDataReq.Data));
   DBReply(DB_OKAY);
   return;
  }

 // Get the Record
 if ( ! ClassInfo.IsNew )
  {
   if ( ! GetRecord(FN_DATAREQRECORD,ClassInfo.ClassName,workingDataReq) )
     return;

   DBReply(DB_OKAY);
   return;
  }

 // If there's a template record, use it, otherwise, use the
 // global record
 if ( ClassInfo.TemplateClassName[0] )
  {
   // Get the template record
   if ( ! GetRecord(0,ClassInfo.TemplateClassName,workingDataReq) )
     return;

   memset(workingDataReq.Data.DataReqClassName,0,
            sizeof(workingDataReq.Data.DataReqClassName));
  }
 else
   memcpy(&workingDataReq.Data,&globalDataReq.Data,
           sizeof(workingDataReq.Data));


 strcpy(workingDataReq.Data.DataReqClassName,ClassInfo.ClassName);

 Msg.PutFld(FN_DATAREQRECORD,&workingDataReq.Data,
              sizeof(workingDataReq.Data));
 DBReply(DB_OKAY);
}

//-------------------------------
// Put a Data Requirements Record
//-------------------------------
void ServerApp::PutDataReq()
{
 bool IsNew;

 if ( ! PutRecord(FN_DATAREQRECORD,workingDataReq,IsNew) )
  return;

 // If it's a change to the GLOBAL record, save it in memory
 if ( strcmp(workingDataReq.Data.DataReqClassName,"DEFAULT") == 0 )
  globalDataReq.TransferDataIn(&workingDataReq.Data);
 else
 // If new record, insert keys into tree
 if ( IsNew )
    DataReqList->Add(workingDataReq.Data.DataReqClassName);
}

//----------------------------
// Get a Security Rules Record
//----------------------------
void ServerApp::GetUnvFundsRules()
{
 int Len=sizeof(ClassInfo);

 // Get the ClassInfo structure from the record and use it as the key.
 // It must be present.
 if ( ! Msg.GetFld(FN_DBKEY,&ClassInfo,Len) )
  {
   DBReply(DB_ERROR);
   BADDATA("KEY DATA");
   return;
  }

 // If Class is Global return the GLOBAL record
 if ( strcmp(ClassInfo.ClassName,"DEFAULT") == 0 )
  {
   Msg.PutFld(FN_UNVFUNDSRULESRECORD,&globalUnvFundsRules.Data,
                sizeof(globalUnvFundsRules.Data));
   DBReply(DB_OKAY);
   return;
  }

 // Get the Record
 if ( ! ClassInfo.IsNew )
  {
   if ( ! GetRecord(FN_UNVFUNDSRULESRECORD,ClassInfo.ClassName,workingUnvFundsRules) )
     return;

   DBReply(DB_OKAY);
   return;
  }

 // If there's a template record, use it, otherwise provide an
 // Empty record
 if ( ClassInfo.TemplateClassName[0] )
  {
   // Get the template record
   if ( ! GetRecord(0,ClassInfo.TemplateClassName,workingUnvFundsRules) )
     return;

   memset(workingUnvFundsRules.Data.UnvFundsRulesClassName,0,
            sizeof(workingUnvFundsRules.Data.UnvFundsRulesClassName));
   strcpy(workingUnvFundsRules.Data.UnvFundsRulesClassName,ClassInfo.ClassName);
  }
 else
   workingUnvFundsRules.FormatNew(ClassInfo.ClassName);

 Msg.PutFld(FN_UNVFUNDSRULESRECORD,&workingUnvFundsRules.Data,
              sizeof(workingUnvFundsRules.Data));
 DBReply(DB_OKAY);
}

//----------------------------
// Put a Security Rules Record
//----------------------------
void ServerApp::PutUnvFundsRules()
{
 bool IsNew;

 if ( ! PutRecord(FN_UNVFUNDSRULESRECORD,workingUnvFundsRules,IsNew) )
  return;

 // If it's a change to the GLOBAL record, save it in memory
 if ( strcmp(workingUnvFundsRules.Data.UnvFundsRulesClassName,"DEFAULT") == 0 )
  globalUnvFundsRules.TransferDataIn(&workingUnvFundsRules.Data);
 else
 // If new record, insert keys into tree
 if ( IsNew )
    UnvFundsRulesList->Add(workingUnvFundsRules.Data.UnvFundsRulesClassName);
}

//---------------------------------------
// Initialize the Data Requirements stuff
//---------------------------------------
bool ServerApp::InitUnvFundsRules()
{
 char Key[sizeof(workingUnvFundsRules.Data.UnvFundsRulesClassName)];
 int dbStatus;

 UnvFundsRulesList = new StringList(1024,1024,UNVFUNDSRULES_LIST);

 // Init the Data Requirements Class List
 while( (dbStatus=d_keynext(UNVFUNDSRULESCLASSNAME,DBHandle)) == S_OKAY )
  {
   if ( (dbStatus=d_keyread(UNVFUNDSRULESCLASSNAME,Key,DBHandle)) != S_OKAY )
    {
     DBError(__FILE__,__LINE__,dbStatus,"d_keyread",-1,UNVFUNDSRULESCLASSNAME);
     return false;
    }
   UnvFundsRulesList->Add(Key);
  }

 // Get the Global RULES record, make an empty one if not
 // present. Store it in global memory as we expect it to be
 // accessed frequently
 if ( d_keyfind(UNVFUNDSRULESCLASSNAME,"DEFAULT",DBHandle) != S_OKAY ||
      d_recread(UNVFUNDSRULESRECORD,&globalUnvFundsRules.Data,DBHandle) != S_OKAY )
  {
   globalUnvFundsRules.FormatNew("DEFAULT");
   UnvFundsRulesList->Add("DEFAULT");
  }

 return true;
}

//-------------------------------------------------------------
// Fill in DBRESULT structure, Put into the current message and
// send REPLY to client requestor.
//-------------------------------------------------------------
void ServerApp::DBReply(WORD ResponseCode,DBTIMESTAMP TimeStamp,
                        const char* Text)
{
 DBRESULT Result;
 Result.ResponseCode=ResponseCode;
 Result.TimeStamp=TimeStamp;
 if ( Text )
  {
   if ( strlen(Text) > sizeof(Result.Text) -1 )
    {
     memcpy(Result.Text,Text,sizeof(Result.Text)-1);
     Result.Text[sizeof(Result.Text)-1]=0;
    }
   strcpy(Result.Text,Text);
  }
 else
   Result.Text[0] = 0;

 Msg.PutFld(FN_DBRESULT,&Result,
            sizeof(Result)-sizeof(Result.Text)+strlen(Result.Text)+1);
 Msg.SendMsg("REPLY");
}

//-------------------------------------------------------------------
// Find and read a record
//
// If an I/O Error occurs, an error message is logged, an error
// reply is sent, and false is returned.
//
// If a Not Found condition occurs, action depends on presence of
// NotFound param:
//
//  If present, NotFound is set to true and true is returned.
//  If not present a NotFound reply is issued and false is returned.
//
//  If Fn==0, the message is not replied to and the record is not
//  Put into the message.
//-------------------------------------------------------------------
bool ServerApp::GetRecord(FLDNUM Fn,void* Key,DBManRecord& Rec,
                          bool* NotFound,bool useAltKey)
{
 if ( NotFound )
  *NotFound=false;

 // Keyposition
 int KeyType = useAltKey ? Rec.AltKeyType() : Rec.KeyType();
 short dbStatus=d_keyfind( KeyType,Key,DBHandle);

 // If Keyposition okay, read the record
 if ( dbStatus==S_OKAY )
  dbStatus=d_recread((short) Rec.RecType(),Rec.DataPtr(),DBHandle);

 // Handle the returned status
 switch( dbStatus )
  {
   // Okay
   case S_OKAY:      if ( NotFound )
                      *NotFound=false;
                     if ( Fn )
                       Msg.PutFld(Fn,Rec.DataPtr(),Rec.DataLen());
                     return true;

   // Not Found
   case S_NOTFOUND:  if ( NotFound )
                      {
                       *NotFound=true;
                       return true;
                      }
                     else
                      {
                       if ( Fn )
                         DBReply(DB_NOTFOUND);
                       return false;
                      }

   // Anything else is an error
   default:          if ( Fn )
                       DBReply(DB_ERROR);
                     DBError(__FILE__,__LINE__,dbStatus,"d_recread",
                             Rec.RecType(),KeyType);
                     return false;
  }
}

//-----------------------------------------------------------------------
// Put a record to the database and reply to client requestor.
//
// Returns true if successful, otherwise false.
//
// If record already exists, action is as follows:
// If Timestamp of record is zero, or the Timestamp matches the record
// on file, a new timestamp is generated and the record is updated.
//
// If the Timestamp is not zero and does not match the record on file,
// a DB_MODIFIED error is returned to the client requestor and false
// is returned to the caller.
//---------------------------------------------------------------------
bool ServerApp::PutRecord(FLDNUM Fn,DBManRecord& Rec,bool& IsNew,
                          char* AltKeyMsg)
{
 DBTIMESTAMP OriginalTimeStamp;
 int RecLen;
 int dbStatus;

 // Get the new record from the message
 RecLen=Rec.DataLen();
 if ( ! Msg.GetFld(Fn,Rec.DataPtr(),RecLen) )
  {
   DBReply(DB_ERROR);
   BADDATA("PUT RECORD");
   return false;
  }

 // Save old Timestamp and Generate a new timestamp
 OriginalTimeStamp=Rec.GetTimeStamp();
 Rec.PutTimeStamp();

 // Attempt to add as new record
 switch ((dbStatus=d_fillnew((short)Rec.RecType(),Rec.DataPtr(),DBHandle)) )
  {
   case S_DUPLICATE:

    // If the original timestamp was not zero, we read the original
    // record and make sure that the timestamp matches.
    IsNew=false;
    if ( (dbStatus=d_keyfind(Rec.KeyType(),Rec.KeyPtr(),DBHandle)) != S_OKAY )
     {
      // If I got a duplicate error but the primary key was not present
      // then the alternate key must be a duplicate.
      if ( dbStatus==S_NOTFOUND &&
           Rec.AltKeyPtr() != NULL )
       {
        DBReply(DB_DUPLICATE,0,AltKeyMsg);
        return false;
       }

      DBError(__FILE__,__LINE__,dbStatus,"d_keyfind",Rec.RecType(),
              Rec.KeyType());
      DBReply(DB_ERROR);
      return false;
     }

     if ( OriginalTimeStamp )
      {
       if ( (dbStatus=d_recread((short)Rec.RecType(),IOBuf,DBHandle)) != S_OKAY )
         {
          DBError(__FILE__,__LINE__,dbStatus,"d_recread",
                  Rec.RecType(),Rec.KeyType());
          DBReply(DB_ERROR);
          return false;
         }

        // Compare timestamps (If records are identical don't
        // worry about it)
        if ( memcmp(&OriginalTimeStamp,IOBuf,sizeof(DBTIMESTAMP)) )
         {
          char* RecPtr=(char*)Rec.DataPtr();
          if ( memcmp(&IOBuf[sizeof(DBTIMESTAMP)],
                      &RecPtr[sizeof(DBTIMESTAMP)],
                      Rec.DataLen()-sizeof(DBTIMESTAMP)) )
           {
            DBReply(DB_MODIFIED,*(DBTIMESTAMP*)IOBuf);
            return false;
           }
         }
       }

     // Write out the changed record
     if ( (dbStatus=d_recwrite((short)Rec.RecType(),
                               Rec.DataPtr(),DBHandle)) != S_OKAY )
      {
       if ( dbStatus==S_DUPLICATE &&
            Rec.AltKeyPtr() != NULL )
        {
         DBReply(DB_DUPLICATE,0,AltKeyMsg);
         return false;
        }
       DBError(__FILE__,__LINE__,dbStatus,"d_recwrite",Rec.RecType());
       DBReply(DB_ERROR);
       return false;
      }

    break;

   // Fillnew worked.. it's a new record
   case S_OKAY:
    IsNew=true;
    break;

   // Velocis Error must have occurred
   default:
     DBReply(DB_ERROR);
     DBError(__FILE__,__LINE__,dbStatus,"d_fillnew",Rec.RecType());
     return false;
  }

 // Calls to dbflush removed V 2.1 
 // if ( (dbStatus=d_dbflush(DBHandle)) != S_OKAY )
 //  {
 //   DBReply(DB_ERROR);
 //   DBError(__FILE__,__LINE__,dbStatus,"d_dbflush");
 //   return false;
 //  }

 // Delete the record out of the message. The client requestor shouldn't
 // expect for it to be echoed back.
 Msg.DelFld(Fn);
 DBReply(DB_OKAY,Rec.GetTimeStamp());
 return true;
}

//----------------------------
// Get a Merchant Rules Record
//----------------------------
void ServerApp::GetMerchantRec()
{
 char Key[sizeof(stationRec.Data.MerchantName)];
 char AccountKey[4];
 bool NotFound;

 // Get the Key
 if ( ! Msg.GetFld(FN_DBKEY,sizeof(Key),Key) )
  {
   DBReply(DB_ERROR);
   BADDATA("KEY DATA");
   return;
  }

 if ( strlen(Key) != 3 &&
      strlen(Key) < 5 )
  {
   BADDATA("INVALID KEY LENGTH");
   return;
  }

 // If it's a name, look it up by altkey
 if ( strlen(Key) > 5 )
  {
   if ( ! GetRecord(0,Key,stationRec,0,true) )
    return;
   // If it's an account record, return it.
   if ( strlen(stationRec.Data.MerchantID) == 3 )
    {
     Msg.PutFld(FN_ACCOUNTRECORD,&stationRec.Data,sizeof(stationRec.Data));
     DBReply(DB_OKAY);
     return;
    }
   // It it's a station record, get the account record
   Msg.PutFld(FN_MERCHANTRECORD,&stationRec.Data,sizeof(stationRec.Data));
   memcpy(Key,stationRec.Data.MerchantID,3);
   Key[3]=0;
   if ( GetRecord(FN_ACCOUNTRECORD,Key,accountRec) )
    DBReply(DB_OKAY);
   return;
  }

 // Get the Account Record, if Any
 memcpy(AccountKey,Key,3);
 AccountKey[3]=0;
 if ( ! GetRecord(FN_ACCOUNTRECORD,AccountKey,accountRec,&NotFound) )
   return;

 // If Account Not Found, put the 3 character account number back as the
 // key to indicate that the account was not found
 if ( NotFound )
  {
   Msg.PutFld(FN_DBKEY,AccountKey);
   DBReply(DB_NOTFOUND);
   return;
  }

 // If Account only, reply okay
 if ( strlen(Key) == 3 )
  {
   DBReply(DB_OKAY);
   return;
  }

 // Get the station record
 if ( ! GetRecord(FN_MERCHANTRECORD,Key,stationRec,&NotFound) )
   return;

 // If not found, return notfound, else okay
 if ( NotFound )
   DBReply(DB_NOTFOUND);
 else
   DBReply(DB_OKAY);
}

//----------------------------
// Put a Merchant Record
//----------------------------
void ServerApp::PutMerchantRec()
{
 bool iDontCare;
 char *MerchNameMsg="MERCHANT NAME ALREADY EXISTS";

 if ( Msg.GetFld(FN_ACCOUNTRECORD) )
  PutRecord(FN_ACCOUNTRECORD,accountRec,iDontCare,MerchNameMsg);
 else
  PutRecord(FN_MERCHANTRECORD,stationRec,iDontCare,MerchNameMsg);
}

//----------------------------------
// Initialize the POS RESPONSE stuff
//----------------------------------
bool ServerApp::InitPosResponse()
{
 char Key[sizeof(globalPosResponse.Data.ResponseClassName)];
 int dbStatus;

 PosResponseList = new StringList(1024,1024,POSRESPONSE_LIST);

 // Init the Data Requirements Class List
 while( (dbStatus=d_keynext(RESPONSECLASSNAME,DBHandle)) == S_OKAY )
  {
   if ( (dbStatus=d_keyread(RESPONSECLASSNAME,Key,DBHandle)) != S_OKAY )
    {
     DBError(__FILE__,__LINE__,dbStatus,"d_keyread",-1,RESPONSECLASSNAME);
     return false;
    }
   PosResponseList->Add(Key);
  }

 // Get the Global RULES record, make an empty one if not
 // present. Store it in global memory as we expect it to be
 // accessed frequently
 if ( d_keyfind(RESPONSECLASSNAME,"DEFAULT",DBHandle) != S_OKAY ||
      d_recread(RESPONSERECORD,&globalPosResponse.Data,DBHandle) != S_OKAY )
  {
   globalPosResponse.FormatNew("DEFAULT");
   PosResponseList->Add("DEFAULT");
  }

 return true;
}

//--------------------------
// Get a POS Response Record
//--------------------------
void ServerApp::GetResponseRec()
{
 int Len=sizeof(ClassInfo);

 // Get the ClassInfo structure from the record and use it as the key.
 // It must be present.
 if ( ! Msg.GetFld(FN_DBKEY,&ClassInfo,Len) )
  {
   DBReply(DB_ERROR);
   BADDATA("KEY DATA");
   return;
  }

 // If Class is Global return the GLOBAL record
 if ( strcmp(ClassInfo.ClassName,"DEFAULT") == 0 )
  {
   Msg.PutFld(FN_POSRESPONSERECORD,&globalPosResponse.Data,
                sizeof(globalPosResponse.Data));
   DBReply(DB_OKAY);
   return;
  }

 // Get the Record
 if ( ! ClassInfo.IsNew )
  {
   if ( ! GetRecord(FN_POSRESPONSERECORD,ClassInfo.ClassName,
                        workingPosResponse) )
     return;

   DBReply(DB_OKAY);
   return;
  }

 // If there's a template record, use it, otherwise, use the
 // global record
 if ( ClassInfo.TemplateClassName[0] )
  {
   // Get the template record
   if ( ! GetRecord(0,ClassInfo.TemplateClassName,workingPosResponse) )
     return;

   memset(workingPosResponse.Data.ResponseClassName,0,
            sizeof(workingPosResponse.Data.ResponseClassName));
  }
 else
   memcpy(&workingPosResponse.Data,&globalPosResponse.Data,
           sizeof(workingPosResponse.Data));


 strcpy(workingPosResponse.Data.ResponseClassName,ClassInfo.ClassName);

 Msg.PutFld(FN_POSRESPONSERECORD,&workingPosResponse.Data,
              sizeof(workingPosResponse.Data));
 DBReply(DB_OKAY);
}

//--------------------------
// Put a POS RESPONSE Record
//--------------------------
void ServerApp::PutResponseRec()
{
 bool IsNew;

 if ( ! PutRecord(FN_POSRESPONSERECORD,workingPosResponse,IsNew) )
  return;

 // If it's a change to the GLOBAL record, save it in memory
 if ( strcmp(workingPosResponse.Data.ResponseClassName,"DEFAULT") == 0 )
  globalPosResponse.TransferDataIn(&workingPosResponse.Data);
 else
 // If new record, insert keys into tree
 if ( IsNew )
    PosResponseList->Add(workingPosResponse.Data.ResponseClassName);
}

//----------------------------------------------
// Fetch POS Response Text from and store in Msg
//----------------------------------------------
void ServerApp::GetResponseText()
{
 BYTE Rcode;
 char Class[RECNAME_LEN];

 // Have to have response code. If not present, just send the message
 // back without the text
 if ( ! Msg.GetFld(FN_RCODE,Rcode) ||
      Rcode < 20 ||
      Rcode > 99 )
  {
   Msg.SendMsg("REPLY");
   return;
  }

 // 20 is the lowest RCODE.
 Rcode -= 20;
 
 // If there is no DBKEY present, use the Default class, otherwise
 // use the DBKEY as the class name
 if ( ! Msg.GetFld(FN_DBKEY,sizeof(Class),Class) ||
      strcmp(Class,"DEFAULT") == 0 )
  {
   Msg.PutFld(FN_TEXTRESPONSE,globalPosResponse.Data.ResponseText[Rcode]);
   Msg.SendMsg("REPLY");
   return;
  }

 // Read the class record  
 if ( ! GetRecord(0,Class,workingPosResponse) )
   LogMissingClass("?????","POSRESPONSE",Class);
 else
   Msg.PutFld(FN_TEXTRESPONSE,workingPosResponse.Data.ResponseText[Rcode]);

 Msg.SendMsg("REPLY");
 return;
}

/**/

//---------------------------------
// Get a Positive File Rules Record
//---------------------------------
void ServerApp::GetPositiveRules()
{
 int Len=sizeof(ClassInfo);

 // Get the ClassInfo structure from the record and use it as the key.
 // It must be present.
 if ( ! Msg.GetFld(FN_DBKEY,&ClassInfo,Len) )
  {
   DBReply(DB_ERROR);
   BADDATA("KEY DATA");
   return;
  }

 // If Class is Global return the GLOBAL record
 if ( strcmp(ClassInfo.ClassName,"DEFAULT") == 0 )
  {
   Msg.PutFld(FN_POSITIVERULESRECORD,&globalPositiveRules.Data,
                sizeof(globalPositiveRules.Data));
   DBReply(DB_OKAY);
   return;
  }

 // Get the Record
 if ( ! ClassInfo.IsNew )
  {
   if ( ! GetRecord(FN_POSITIVERULESRECORD,ClassInfo.ClassName,
               workingPositiveRules) )
     return;

   DBReply(DB_OKAY);
   return;
  }

 // If there's a template record, use it, otherwise, use the
 // global record
 if ( ClassInfo.TemplateClassName[0] )
  {
   // Get the template record
   if ( ! GetRecord(0,ClassInfo.TemplateClassName,workingPositiveRules) )
     return;

   memset(workingPositiveRules.Data.PositiveRulesClassName,0,
            sizeof(workingPositiveRules.Data.PositiveRulesClassName));
  }
 else
   memcpy(&workingPositiveRules.Data,&globalPositiveRules.Data,
           sizeof(workingPositiveRules.Data));


 strcpy(workingPositiveRules.Data.PositiveRulesClassName,ClassInfo.ClassName);

 Msg.PutFld(FN_POSITIVERULESRECORD,&workingPositiveRules.Data,
              sizeof(workingPositiveRules.Data));
 DBReply(DB_OKAY);
}

//---------------------------------
// Put a Positive File Rules Record
//---------------------------------
void ServerApp::PutPositiveRules()
{
 bool IsNew;

 if ( ! PutRecord(FN_POSITIVERULESRECORD,workingPositiveRules,IsNew) )
  return;

 // If it's a change to the GLOBAL record, save it in memory
 if ( strcmp(workingPositiveRules.Data.PositiveRulesClassName,"DEFAULT") == 0 )
  globalPositiveRules.TransferDataIn(&workingPositiveRules.Data);
 else
 // If new record, insert keys into tree
 if ( IsNew )
    PositiveRulesList->Add(workingPositiveRules.Data.PositiveRulesClassName);
}

//---------------------------------------
// Initialize the Positive Rules stuff
//---------------------------------------
bool ServerApp::InitPositiveRules()
{
 char Key[sizeof(workingPositiveRules.Data.PositiveRulesClassName)];
 int dbStatus;

 PositiveRulesList = new StringList(1024,1024,POSRULES_LIST);

 // Init the Data Requirements Class List
 while( (dbStatus=d_keynext(POSITIVERULESCLASSNAME,DBHandle)) == S_OKAY )
  {
   if ( (dbStatus=d_keyread(POSITIVERULESCLASSNAME,Key,DBHandle)) != S_OKAY )
    {
     DBError(__FILE__,__LINE__,dbStatus,"d_keyread",-1,POSITIVERULESCLASSNAME);
     return false;
    }
   PositiveRulesList->Add(Key);
  }

 // Get the Global RULES record, make an empty one if not
 // present. Store it in global memory as we expect it to be
 // accessed frequently
 if ( d_keyfind(POSITIVERULESCLASSNAME,"DEFAULT",DBHandle) != S_OKAY ||
      d_recread(POSITIVERULESRECORD,&globalPositiveRules.Data,DBHandle) != S_OKAY )
  {
   globalPositiveRules.FormatNew("DEFAULT");
   PositiveRulesList->Add("DEFAULT");
  }

 return true;
}

//---------------------------------------------------
// Get all the stuff needed for a check authorization
//---------------------------------------------------
void ServerApp::CheckAuthorization()
{
 char MerchId[sizeof(stationRec.Data.MerchantID)];
 int  Len=sizeof(MerchId);
 char Account[4];
 bool isGuarantee;
 char* Class;

 // Init the Check Auth Message object
 checkMsg.CopyMsg(&Msg);

 // If no Merchant Id Log an error message and send it on.
 if ( ! Msg.GetFld(FN_MERCHID,MerchId,Len) ||
      Len != 5 )
  {
   checkMsg.SendMsg( checkMsg.isVoice() ? MANDBVOICEAUTH_DEST :
                                          MANDBPOSAUTH_DEST );
   BADDATA("MERCHANT ID");
   return;
  }

 MerchId[5]=0;

 // First try  the account record. If not present, can't do the
 // transaction.
 memcpy(Account,MerchId,3);
 Account[3]=0;
 if ( ! GetRecord(0,Account,accountRec) )
  {
   checkMsg.SendMsg( checkMsg.isVoice() ? MANDBVOICEAUTH_DEST :
                                          MANDBPOSAUTH_DEST );
   IPCLogEvent(AccountMissing,0,Account);
   return;
  }
 else
  if ( GetRecord(0,MerchId,stationRec) )
    accountRec.Merge(&stationRec);
  // V1.02 Overlay Account Number with Merchant ID
  else
    memcpy(accountRec.Data.MerchantID,MerchId,5);

 // If there is Kenwood Merchant Info, overlay our info and delete
 // the Kenwood Info
 Len=sizeof(kwMerchant);
 if ( Msg.GetFld(FN_KWMERCHANTRECORD,&kwMerchant,Len) )
  {
   memcpy(accountRec.Data.MerchantSic,kwMerchant.Sic,
            sizeof(accountRec.Data.MerchantSic)-1);
   accountRec.Data.MerchantSic[sizeof(accountRec.Data.MerchantSic)-1]=0;
   memcpy(accountRec.Data.MerchantZip,kwMerchant.Zip,
            sizeof(accountRec.Data.MerchantZip)-1);
   accountRec.Data.MerchantZip[sizeof(accountRec.Data.MerchantZip)-1]=0;
   memcpy(accountRec.Data.MerchantName,kwMerchant.Name,
              sizeof(accountRec.Data.MerchantName)-1);
   accountRec.Data.MerchantName[sizeof(accountRec.Data.MerchantName)-1]=0;
  }

 // Put the station record to the message
 checkMsg.PutFld(FN_MERCHANTRECORD,&accountRec.Data,sizeof(accountRec.Data));
 checkMsg.initClassMembers();
 FetchAuthRules(accountRec);

 // Determine if guarantee or verify to get the reset of the class
 // records.
 isGuarantee = checkMsg.isGuarantee();

 // Get Data Requirements
 Class = isGuarantee ?
      checkMsg.merchantRec.Data.Classes.GUDataReq :
      checkMsg.merchantRec.Data.Classes.VFYDataReq;
 if ( strcmp(Class,"DEFAULT")==0 )
  checkMsg.PutFld(FN_DATAREQRECORD,&globalDataReq.Data,
                    sizeof(globalDataReq.Data));
 else
  if ( ! GetRecord(0,Class,workingDataReq) )
    LogMissingClass(checkMsg.MerchantID(),"DATAREQ",Class);
  else
    checkMsg.PutFld(FN_DATAREQRECORD,&workingDataReq.Data,
                     sizeof(workingDataReq.Data));

 // Get Security Rules
 Class = isGuarantee ?
      checkMsg.merchantRec.Data.Classes.GUUnvFundsRules :
      checkMsg.merchantRec.Data.Classes.VFYUnvFundsRules;
 if ( strcmp(Class,"DEFAULT")==0 )
  checkMsg.PutFld(FN_UNVFUNDSRULESRECORD,&globalUnvFundsRules.Data,
                    sizeof(globalUnvFundsRules.Data));
 else
  if ( ! GetRecord(0,Class,workingUnvFundsRules) )
    LogMissingClass(checkMsg.MerchantID(),"UNVFUNDSRULES",Class);
  else
    checkMsg.PutFld(FN_UNVFUNDSRULESRECORD,&workingUnvFundsRules.Data,
                  sizeof(workingUnvFundsRules.Data));

 // Get Positive File Rules
 Class = isGuarantee ?
      checkMsg.merchantRec.Data.Classes.GUPosFileRules :
      checkMsg.merchantRec.Data.Classes.VFYPosFileRules;
 if ( strcmp(Class,"DEFAULT")==0 )
  checkMsg.PutFld(FN_POSITIVERULESRECORD,&globalPositiveRules.Data,
                    sizeof(globalPositiveRules.Data));
 else
  if ( ! GetRecord(0,Class,workingPositiveRules) )
    LogMissingClass(checkMsg.MerchantID(),"POSITIVERULES",Class);
  else
    checkMsg.PutFld(FN_POSITIVERULESRECORD,&workingPositiveRules.Data,
                  sizeof(workingPositiveRules.Data));

 // Put in the Merchant Record (truncate the CLASS info.. we shouldn't
 // need it anymore).
 Msg.PutFld(FN_MERCHANTRECORD,&accountRec.Data,
              sizeof(accountRec.Data) - sizeof(accountRec.Data.Classes));
 checkMsg.SendMsg( checkMsg.isVoice() ? MANDBVOICEAUTH_DEST :
                                        MANDBPOSAUTH_DEST );
}

static char* ManagedListNames[] = { AUTHRULES_LIST,
                                    UNVFUNDSRULES_LIST,
                                    POSRULES_LIST,
                                    DATAREQ_LIST,
                                    POSRESPONSE_LIST,
                                     0 };

void ServerApp::GetList()
{
 DBLISTINFO Info;
 LISTHDR hdr;
 int Len=sizeof(Info);
 int i;

 if ( ! Msg.GetFld(FN_DBLISTINFO,&Info,Len) )
  return;

 // Search the names of Managed lists to see if it's one of them
 for ( i=0; ManagedListNames[i] != 0 &&
              stricmp(Info.ListName,ManagedListNames[i]); ++i);

 switch(i)
  {
   case 0: GetAuthRulesList(); return;

   case 1: UnvFundsRulesList->DbAnswer(&Msg); return;

   case 2: PositiveRulesList->DbAnswer(&Msg); return;

   case 3: DataReqList->DbAnswer(&Msg); return;

   case 4: PosResponseList->DbAnswer(&Msg); return;

   default: break;
  }

 Msg.ClearMsg();
 Info.SegmentOffset=0;

 // Attempt to read the list from the database
 if ( d_keyfind(LISTNAME,Info.ListName,DBHandle) == S_OKAY &&
      d_recread(LISTRECORD,&listRecord,DBHandle) == S_OKAY )
  {
   Info.ListLen=Info.SegmentLen=listRecord.ListLen;
   Info.TimeStamp=listRecord.ListTStamp;
   Msg.PutFld(FN_DBLIST,&listRecord.ListTStamp,listRecord.ListLen);
  }
 else
  {
   // Return an Empty List
   Info.ListLen=Info.SegmentLen=sizeof(LISTHDR);
   memset(&hdr,0,sizeof(hdr));
   strcpy(hdr.ListName,Info.ListName);
   Msg.PutFld(FN_DBLIST,&hdr,sizeof(hdr));
  }

 Msg.PutFld(FN_DBLISTINFO,&Info,sizeof(Info));
 Msg.SendMsg("REPLY");
}

void ServerApp::PutList()
{
 int Len=sizeof(IOBuf);
 unsigned short status;
 int i;
 LISTHDR* Hdr;

 if ( ! Msg.GetFld(FN_DBLIST,IOBuf,Len) )
  return;
 Hdr = (LISTHDR*)IOBuf;

 // Search Managed lists  .. Do not allow Put for one of these. These
 // must be managed by this program
 for ( i=0; ManagedListNames[i] != 0; ++i)
  if ( stricmp(ManagedListNames[i],Hdr->ListName) == 0 )
   {
    DBReply(DB_ERROR,0,"ILLEGAL LIST NAME");
    return;
   }

 // Check for Existing List to compare TimeStamps
 if ( d_keyfind(LISTNAME,Hdr->ListName,DBHandle) == S_OKAY &&
      d_recread(LISTRECORD,&listRecord,DBHandle) == S_OKAY )
  {
   if ( Hdr->TimeStamp != 0 &&
        Hdr->TimeStamp != listRecord.ListTStamp )
    {
     DBReply(DB_MODIFIED,listRecord.ListTStamp);
     return;
    }

   listRecord.ListLen=(unsigned short)Len;
   memcpy(&listRecord.ListTStamp,Hdr,Len);
   time((time_t*)&listRecord.ListTStamp);
   if ( (status=d_recwrite(LISTRECORD,&listRecord,DBHandle)) != S_OKAY )
    {
     DBReply(DB_ERROR);
     DBError(__FILE__,__LINE__,status,"d_recwrite",LISTRECORD);
     return;
    }
  }
 else
  {
   listRecord.ListLen=(unsigned short)Len;
   memcpy(&listRecord.ListTStamp,Hdr,Len);
   time((time_t*)&listRecord.ListTStamp);
   if ( (status=d_fillnew(LISTRECORD,&listRecord,DBHandle)) != S_OKAY )
    {
     DBReply(DB_ERROR);
     DBError(__FILE__,__LINE__,status,"d_fillnew",LISTRECORD);
     return;
    }
  }

 Msg.DelFld(FN_DBLIST);
 DBReply(DB_OKAY,listRecord.ListTStamp);
}


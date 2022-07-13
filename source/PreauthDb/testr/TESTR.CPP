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
//  MAB062899 - Replaced Velocis DB PreauthDB table with flat file
//              util Velocis DB could be debugged.
//
//  ---   ----  ---  ------------------------------------------------
//   1.1  1/98  JHE  Changed open mode from "xq" to "xn" and removed
//                   calls to d_dbflush. Attempting to prevent database
//                   corruption.
//----------------------------------------------------------------------------

//#include "ServerApp.h"
#include "testr.h"
#include <time.h>
#include "TranType.h"
#include "DestName.h"
#include "CheckAuthFn.h"
#include "rcodes.h"
#include "Spsmandb.h"

#define DO_IT_THE_HARD_WAY  1
// Max # of Preauths we will return on a fetch request
#define MAX_PREAUTHS 10

#if DO_IT_THE_HARD_WAY
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mem.h>
#define FILE_OPEN_ERROR      1001
#define FILE_WRITE_ERROR     1002
#define LENGTH_ERROR         1003
#define RECORD_NOT_FOUND     1004
#define RECORD_FOUND         1005
#endif

static PreauthRecord Record[MAX_PREAUTHS];
static MerchantRecord merchantRecord;

void loggy(char *buf,int iLine, char *buf1, char *buf2, unsigned long l,int i);
void logg2(char *buf,int iLine, char *buf1, char *buf2, char *buf3,unsigned long l,int i);

void loggy(char *buf,int iLine, char *buf1, char *buf2,unsigned long l,int i)
{
   FILE  *fout;
   return;
   fout = fopen("err.log", "a");
   fprintf(fout,"%s @%d %s - %s. .%ld. .%d.\n",buf, iLine, buf1, buf2, i, l);
   fclose(fout);
}
// loggy(__FILE__,__LINE__, "1. DoApplicationMsg() TranType is",TranType,0,0);

void logg2(char *buf,int iLine, char *buf1, char *buf2, char *buf3,unsigned long l,int i)
{
   FILE  *fout;
   return;
   fout = fopen("err.log", "a");
   fprintf(fout,"\n%s @%d %s\n  %s\n  \n%s\n.%ld. .%d.\n",buf, iLine, buf1, buf2, buf3, i, l);
   fclose(fout);
}

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
//        time(NULL) - Record[0].TimeofPreauth <= 3 * 60 )
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

#if DO_IT_THE_HARD_WAY
  status = m_fillnew(&Record[0]);
  if(status)
#else
// if ( (status = d_fillnew(PREAUTHRECORD,&Record,DBHandle)) != S_OKAY )
 status = d_fillnew(PREAUTHRECORD,&Record,DBHandle);
 if(status != S_OKAY)
#endif
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
 //loggy(__FILE__,__LINE__, "doPostAuth()","",0,0);

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
#if DO_IT_THE_HARD_WAY
  status = m_recread1(&Record[0], Amount, merchantRecord.MerchantID,ID);
  if(status != RECORD_FOUND)
  {
     Msg.PutFld(FN_RCODE,(BYTE)0);
     Msg.SendMsg(PREAUTH_REPLY);
     return;
  }
 //loggy(__FILE__,__LINE__, "doPostAuth()","found Record",0,0);
#else
 if ( d_keyfind(PREAUTHCONSUMERID,ID,DBHandle) == S_OKAY )
  for (;;)
   {
    if ( d_recread(PREAUTHRECORD,&Record,DBHandle) != S_OKAY ||
         strcmp(Record[0].PreauthConsumerID,ID) )
     break;
#endif
    if ( strcmp(Record[0].PreauthMerchantID,merchantRecord.MerchantID) == 0 &&
         ! isExpired(&Record[0]) &&
         ( Record[0].TimeofPostauth == 0 ||
         ( time(NULL) - Record[0].TimeofPostauth <= 15 * 60
//         ( time(NULL) - Record[0].TimeofPostauth <= 3 * 60
         // MAB062899 this doen't seem reasonable, maybee... Preauth.Amount
         // && Amount == Record[0].PostAuthAmount  //??????
        ) ) )
     {
      Msg.PutFld(FN_RCODE,(BYTE)RCODE_PREAUTHAPPROVAL);
      Msg.SendMsg(PREAUTH_REPLY);
      if ( Record[0].TimeofPostauth == 0 )
       {
        Record[0].TimeofPostauth=time(NULL);
        Record[0].PostAuthAmount=Amount;

#if DO_IT_THE_HARD_WAY
        status = m_fillnew(&Record[0]);
        if(status)
#else
        if ( (status=d_recwrite(PREAUTHRECORD,&Record,DBHandle)) != S_OKAY )
#endif
          DBError(__FILE__,__LINE__,status,"d_recwrite",PREAUTHRECORD);
        // Call to flush removed V 1.1
        // else
        //if ( (status=d_dbflush(DBHandle)) != S_OKAY )
        //  DBError(__FILE__,__LINE__,status,"d_dbflush",PREAUTHRECORD);
       }
      return;
     }

#if DO_IT_THE_HARD_WAY
;
#else
    if ( d_recnext(DBHandle) != S_OKAY )
     break;
   }
#endif

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
#if DO_IT_THE_HARD_WAY
  status = m_fillnew(&Record[0]);
  if(status)
#else
// if ( (status = d_fillnew(PREAUTHRECORD,&Record,DBHandle)) != S_OKAY )
 status = d_fillnew(PREAUTHRECORD,&Record,DBHandle));
 if(status != S_OKAY)
#endif
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

#if DO_IT_THE_HARD_WAY
   int iReturn;
   FILE  *fin;
   PreauthRecord RecIn;
   char buff[2];
   //loggy(__FILE__,__LINE__, "FetchAllPreauths()","",0,0);
#endif

 if ( ! getID(Key) )
  return -1;

#if DO_IT_THE_HARD_WAY
   if((fin = fopen("pre.dat", "r")) == NULL)  // couldn't open file
   {
      //loggy(__FILE__,__LINE__, "File open fail","",0,0);
      return -1;
   }
   //loggy(__FILE__,__LINE__, "FetchAllPreauths()","",0,0);

   for (i=0; i<MAX_PREAUTHS; )
   {
   //loggy(__FILE__,__LINE__, "FetchAllPreauths()","",0,0);
      iReturn = fscanf(fin,"%ld %ld %c %ld %s %s",
                  &RecIn.TimeofPreauth,
                  &RecIn.TimeofPostauth,
                  &RecIn.PreauthRetention,
                  &RecIn.PostAuthAmount,
                  RecIn.PreauthMerchantID,
                  RecIn.PreauthConsumerID);

   //loggy(__FILE__,__LINE__, "FetchAllPreauths()",RecIn.PreauthConsumerID,0,0);
   buff[0] = RecIn.PreauthRetention;
   buff[1] = '\0';
   //loggy(__FILE__,__LINE__, "FetchAllPreauths() Retention",buff,0,0);

      if(feof(fin))
         break;
   //loggy(__FILE__,__LINE__, "FetchAllPreauths()","",0,i);

      if(strcmp(RecIn.PreauthConsumerID, Key) == 0)
      {
         //loggy(__FILE__,__LINE__, "FetchAllPreauths()","ID Match",0,0);
         if ( ! isExpired(&RecIn) ||
              RecIn.TimeofPostauth == 0 )
         {
            ++i;
            //loggy(__FILE__,__LINE__, "FetchAllPreauths()","Good Record",0,0);
         }
      }
   }
   //loggy(__FILE__,__LINE__, "Count","",0,i);

   fclose(fin);

#else
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
#endif
 //loggy(__FILE__,__LINE__, "Count","",0,i);
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

#if 0
;
#else
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
#endif
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

//$*************************************************************************
// not presently a binary write, because could not check data with editor
short ServerApp::m_fillnew(PreauthRecord *Rec)
{
   int ii;
   FILE  *fout;
   //loggy(__FILE__,__LINE__, "1. m_fillnew()","",0,0);
   if((fout = fopen("pre.dat", "a")) == NULL)  // couldn't open file
   {
      //loggy(__FILE__,__LINE__, "File open fail 1","",0,0);
      return FILE_OPEN_ERROR;
   }

   ii = fprintf(fout,"%ld %ld %c %ld %s %s\n",
                Rec->TimeofPreauth,
                Rec->TimeofPostauth,
                Rec->PreauthRetention,
                Rec->PostAuthAmount,
                Rec->PreauthMerchantID,
                Rec->PreauthConsumerID);

   fclose(fout);
   if(ii < 5)  //arbitrary
      return FILE_WRITE_ERROR;

   return 0;
}


//$*********************************************************************
short ServerApp::m_recread1(PreauthRecord *Rec, DWORD Amount, char *MerchantID, char *ID)
{
   int ii;
   FILE  *fin;
   PreauthRecord RecIn;

   //loggy(__FILE__,__LINE__, "2. m_recreadAuthID()","Looking for",0,0);
   //loggy(__FILE__,__LINE__, RecIn.PreauthConsumerID,
   //                         RecIn.PreauthMerchantID,Amount,0);
   if((fin = fopen("pre.dat", "r")) == NULL)  // couldn't open file
   {
      //loggy(__FILE__,__LINE__, "File open fail","",0,0);
      return FILE_OPEN_ERROR;
   }

   while(1)
   {
      ii = fscanf(fin,"%ld %ld %c %ld %s %s",
                  &RecIn.TimeofPreauth,
                  &RecIn.TimeofPostauth,
                  &RecIn.PreauthRetention,
                  &RecIn.PostAuthAmount,
                  RecIn.PreauthMerchantID,
                  RecIn.PreauthConsumerID);

      if(feof(fin))
         break;
     //loggy(__FILE__,__LINE__, RecIn.PreauthConsumerID,
     //                       RecIn.PreauthMerchantID,RecIn.PostAuthAmount,0);

      if( (strcmp(RecIn.PreauthConsumerID, ID) == 0)  &&
         // MAB062899 this doen't seem reasonable, maybee... Preauth.Amount
         // (RecIn.PostAuthAmount == Amount)  &&
          (strcmp(RecIn.PreauthMerchantID, MerchantID) == 0)
        )
      {
         Rec->TimeofPreauth    =  RecIn.TimeofPreauth;
         Rec->TimeofPostauth   =  RecIn.TimeofPostauth;
         Rec->PreauthRetention =  RecIn.PreauthRetention;
         Rec->PostAuthAmount   =  RecIn.PostAuthAmount;
         strcpy(Rec->PreauthMerchantID, RecIn.PreauthMerchantID);
         strcpy(Rec->PreauthConsumerID, RecIn.PreauthConsumerID);
         fclose(fin);
         //loggy(__FILE__,__LINE__, "2. m_recreadAuthID() Record Found",RecIn.PreauthConsumerID,0,0);
         return RECORD_FOUND;
      }
   }

   fclose(fin);
   return RECORD_NOT_FOUND;
}

//$*********************************************************************
int ServerApp::m_UpdateTimeOfPostauth(PreauthRecord *Rec, long lTimeOfPostauth)
{
   int   ii;
   int   iReturnCode = RECORD_NOT_FOUND;
   int   iCount = 0;
   FILE  *fin;
   FILE  *fout;

   PreauthRecord RecIn;
   //loggy(__FILE__,__LINE__, "3. m_UpdateTimeOfPostauth()","",0,0);

   if((fin = fopen("pre.dat", "r")) == NULL)  // couldn't open file
   {
      //loggy(__FILE__,__LINE__, "3 File in open fail","",0,0);
      return FILE_OPEN_ERROR;
   }

   remove("pre.tmp");
   if((fout = fopen("pre.tmp", "w")) == NULL)  // couldn't open file
   {
      //loggy(__FILE__,__LINE__, "3 File out open fail","",0,0);
      return FILE_OPEN_ERROR;
   }

   while(1)
   {
      ii = fscanf(fin,"%ld %ld %c %ld %s %s",
                  &RecIn.TimeofPreauth,
                  &RecIn.TimeofPostauth,
                  &RecIn.PreauthRetention,
                  &RecIn.PostAuthAmount,
                  RecIn.PreauthMerchantID,
                  RecIn.PreauthConsumerID);

      //loggy(__FILE__,__LINE__, "3 rec ",RecIn.PreauthConsumerID,0,iCount);
      iCount++;

      if(feof(fin))
      {
         //loggy(__FILE__,__LINE__, "3 feof","",0,0);
         break;
      }

      // Is this the record to update. Must compare
      if(strcmp(RecIn.PreauthConsumerID,Rec[0].PreauthConsumerID)==0)
         if(
            (RecIn.TimeofPreauth  == Rec->TimeofPreauth) &&
            (RecIn.PostAuthAmount == Rec->PostAuthAmount) &&
            (strcmp(RecIn.PreauthMerchantID,Rec->PreauthMerchantID) == 0)
           )
           {
            //loggy(__FILE__,__LINE__, "3 rec found","",0,iCount);
            RecIn.TimeofPostauth = lTimeOfPostauth;
            iReturnCode = RECORD_FOUND;
           }

      ii = fprintf(fout,"%ld %ld %c %ld %s %s\n",
                   RecIn.TimeofPreauth,
                   RecIn.TimeofPostauth,
                   RecIn.PreauthRetention,
                   RecIn.PostAuthAmount,
                   RecIn.PreauthMerchantID,
                   RecIn.PreauthConsumerID);

       //loggy(__FILE__,__LINE__, "3 rec written","",0,iCount);

   }

   fclose(fout);
   fclose(fin);
   remove("pre.dat");
   rename("pre.tmp","pre.dat");
   return iReturnCode;
}

//$*********************************************************************
int ServerApp::m_delete(PreauthRecord *Rec)
{
   int   ii;
   int   iCount = 0;
   int   iReturnCode = RECORD_NOT_FOUND;
   FILE  *fin;
   FILE  *fout;

   PreauthRecord RecIn;
   //loggy(__FILE__,__LINE__, "4. m_delete()","",0,0);

   if((fin = fopen("pre.dat", "r")) == NULL)  // couldn't open file
   {
      //loggy(__FILE__,__LINE__, "File in open fail","",0,0);
      return FILE_OPEN_ERROR;
   }

   remove("pre.tmp");
   if((fout = fopen("pre.tmp", "w")) == NULL)  // couldn't open file
   {
      //loggy(__FILE__,__LINE__, "File out open fail","",0,0);
      return FILE_OPEN_ERROR;
   }

   while(1)
   {
      //loggy(__FILE__,__LINE__, "4. m_delete()","",0,iCount);
      ii = fscanf(fin,"%ld %ld %c %ld %s %s",
                  &RecIn.TimeofPreauth,
                  &RecIn.TimeofPostauth,
                  &RecIn.PreauthRetention,
                  &RecIn.PostAuthAmount,
                  RecIn.PreauthMerchantID,
                  RecIn.PreauthConsumerID);

      iCount++;
      if(feof(fin))
      {
         //loggy(__FILE__,__LINE__, "4. m_delete()","end of file",0,iCount);
         break;
      }

      // Is this the record to update. Must compare
      if(strcmp(RecIn.PreauthConsumerID,Rec[0].PreauthConsumerID)==0)
         if(
            (RecIn.TimeofPreauth  == Rec->TimeofPreauth) &&
            (RecIn.PostAuthAmount == Rec->PostAuthAmount) &&
            (strcmp(RecIn.PreauthMerchantID,Rec->PreauthMerchantID) == 0)
           )
      {
         iReturnCode = RECORD_FOUND;
         continue;
      }

      // else write the record out to file again
      ii = fprintf(fout,"%ld %ld %c %ld %s %s\n",
                   RecIn.TimeofPreauth,
                   RecIn.TimeofPostauth,
                   RecIn.PreauthRetention,
                   RecIn.PostAuthAmount,
                   RecIn.PreauthMerchantID,
                   RecIn.PreauthConsumerID);
   }

   fclose(fout);
   fclose(fin);
   remove("pre.dat");
   rename("pre.tmp","pre.dat");
   return iReturnCode;
}


#if 0
//$********************************************************************
bool ServerApp::doWrite()
{
 short stat;
 int   PurgeCnt;
 int   RecCnt;
 char sPurgeCnt[10], sRecCnt[10];

 //loggy(__FILE__,__LINE__, "1. DoWrite()","",0,0);
for (RecCnt=PurgeCnt=0, stat=d_recfrst(PREAUTHRECORD,DBHandle);
      stat==S_OKAY;
      stat=d_recnext(DBHandle))
  {
   ++RecCnt;

   if ( (stat=d_recread(PREAUTHRECORD,&Record,DBHandle)) != S_OKAY )
    {
     DBError(__FILE__,__LINE__,stat,"d_recread",PREAUTHRECORD);
 //loggy(__FILE__,__LINE__, "2. In DoWrite()","",0,0);
     return false;
    }

//logg2(__FILE__,__LINE__,"Data in DoWrite()",Record[0].PreauthConsumerID,
       Record[0].PreauthMerchantID,Record[0].TimeofPreauth,RecCnt);

 if ( stat != S_NOTFOUND )
  {
   //loggy(__FILE__,__LINE__, "3. In DoWrite()","",0,0);
   DBError(__FILE__,__LINE__,stat,"d_recnext",PREAUTHRECORD);
   return false;
  }

 }
 //loggy(__FILE__,__LINE__, "4. In DoWrite()","",0,0);

 return true;
}

//$*********************************************************************
int ServerApp::m_recreadAuthID(char *AuthID, PreauthRecord *Rec)
{
   int ii;
   FILE  *fin;
   PreauthRecord RecIn;

   //loggy(__FILE__,__LINE__, "2. m_recreadAuthID()","",0,0);
   if((fin = fopen("pre.dat", "r")) == NULL)  // couldn't open file
   {
      //loggy(__FILE__,__LINE__, "File open fail","",0,0);
      return FILE_OPEN_ERROR;
   }

   while(1)
   {
      ii = fscanf(fin,"%ld %ld %c %ld %s %s",
                  &RecIn.TimeofPreauth,
                  &RecIn.TimeofPostauth,
                  &RecIn.PreauthRetention,
                  &RecIn.PostAuthAmount,
                  RecIn.PreauthMerchantID,
                  RecIn.PreauthConsumerID);

      if(feof(fin))
         break;

      if( (strcmp(RecIn.PreauthConsumerID, Rec->PreauthConsumerID) == 0)  &&
          (RecIn.TimeofPreauth  == Rec->TimeofPreauth)   &&
          (RecIn.PostAuthAmount == Rec->PostAuthAmount)  &&
          (strcmp(RecIn.PreauthMerchantID,Rec->PreauthMerchantID) == 0)
        )
      {
         Rec->TimeofPreauth    =  RecIn.TimeofPreauth;
         Rec->TimeofPostauth   =  RecIn.TimeofPostauth;
         Rec->PreauthRetention =  RecIn.PreauthRetention;
         Rec->PostAuthAmount   =  RecIn.PostAuthAmount;
         strcpy(Rec->PreauthMerchantID, RecIn.PreauthMerchantID);
         strcpy(Rec->PreauthConsumerID, RecIn.PreauthConsumerID);
         fclose(fin);
         return RECORD_FOUND;
      }
   }

   fclose(fin);
   return RECORD_NOT_FOUND;
}


#endif

//-----------
// SPSVoice.C
//-----------
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/msg.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "ipcmsg.h"
#include "trantype.h"
#include "authfn.h"
#include "spslog.h"
#include "inetman.h"
#include "ipckeys.h"
#include "pipes.h"
#include "kwrec.h"
#include "recfiltr.h"

// Max time to wait for Kenwood response (ms)
#define RECVWAIT 6000
#define MAXSPSFIELD 200

// Global C++ objects
SPSLogger Logger("SPSVoice");
INETManager Inet;
IPCMessage Msg;
AuditFilter FilterAudit;
CheckFilter FilterChecks;

// Message Queue Id
int MsgQId;

// static function prototypes
static void init(int argc,char* argv[]);
static void DoInquiry(void);
static void DoReply(void);
static int  ClusterNumber;
static char DataFileName[50];
static int  IOBufSize=8192;
static int  filelength(int fd);
static char* IOBuf;
static int  GetData(void);
static void FormatSPStoKW(void);
static int  FormatKWtoSPS(void);
static void buildInq(void);
static key_t IPCKey;

Pipe* SendPipe;
Pipe* RecvPipe;
Pipe* ReplyPipe;

int IdleState=true;
WORD CurrentSyncId=0;
WORD LastReplySyncId;

//--------------
// Main function
//--------------
void main(int argc,char* argv[])
{
 char TranType[40];

 init(argc,argv);

 for(;;)
   if ( Msg.Recv(MsgQId,1) )
    {
     if ( ! Msg.GetFld(FN_TRANTYPE,sizeof(TranType),TranType) )
      {
       Logger.Log("MESSAGE WITH NO TRANTYPE DROPPED");
       continue;
      }
     if ( strcmp(TranType,T_VOICEAUTH) == 0 )
      DoInquiry();
     else
     if ( strcmp(TranType,T_AUTHREPLY) == 0 )
      DoReply();
     else
      Logger.Log("MESSAGE WITH INVALID TRANTYPE (%s) DROPPED.",TranType);
    }
}

//--------------
// Init function
//--------------
static void init(int argc,char* argv[])
{
 char PipeName[60];

 // Make sure the INTERNET came up okay
 if ( ! Inet.IsOkay() )
   Logger.Abort();

 // Validate command line args
 if ( argc < 2 )
  {
   Logger.Log("USAGE IS SPSVoice <ClusterNumber>");
   Logger.Abort();
  }

 if ( (ClusterNumber = atoi(argv[1])) < 100 )
  {
   Logger.Log("VOICE CLUSTER NUMBERS MUST BE >= 100");
   Logger.Abort();
  }

 // Get the Message Queue Id
 IPCKey = IPCKEY_VOICEMSGQUEUE + ClusterNumber - 100;
 if ( (MsgQId=msgget(IPCKey,IPC_CREAT)) == -1 )
  {
   Logger.IOError("msgget","IPCKEY_VOICEMSGQUEUE");
   Logger.Abort();
  }

 // Create the pipe objects
 sprintf(PipeName,"/usr/spool/uucp/PST..%d",ClusterNumber);
 SendPipe=new Pipe(PipeName);
 sprintf(PipeName,"/usr/spool/uucp/RST..%d",ClusterNumber);
 RecvPipe=new Pipe(PipeName);
 sprintf(PipeName,"/usr/spool/uucp/FIN..%d",ClusterNumber);
 ReplyPipe=new Pipe(PipeName);
 ReplyPipe->Abort();
 sleep(3);
 ReplyPipe->Flush();

 // Set the data file name for the cluster
 sprintf(DataFileName,"/usr/spool/uucp/DTA..%d",ClusterNumber);

 // Allocate memory for I/O buffer
 if ( (IOBuf=(char*)malloc(IOBufSize)) == NULL )
  {
   Logger.Log("OUT OF MEMORY");
   Logger.Abort();
  }
}

//---------------------
// Do the Voice Request
//---------------------
static void DoInquiry(void)
{
 int Ret;
 char *Ptr;
 KENWOOD_TERMINFO TermInfo;

 // If I wasn't idle then I need to abort Kenwood's wait on the
 // reply pipe.
 if ( ! IdleState )
  {
   ReplyPipe->Abort();
   sleep(1);
   IdleState=true;
  }

 // Flush all the Pipes before sending the new message
 SendPipe->Flush();
 RecvPipe->Flush();
 ReplyPipe->Flush();

 FormatSPStoKW();

 if ( ! SendPipe->Write(IOBuf,strlen(IOBuf)) )
   return;

 memset(IOBuf,0,IOBufSize);  
 Ret = RecvPipe->Read(IOBuf,IOBufSize,RECVWAIT);
 if ( Ret==-1 )
  return;
 if ( Ret==0 )
  {
   Logger.Log("NO RESPONSE FROM %s",RecvPipe->GetName());
   return;
  }

 buildInq(); 
 Msg.ClearMsg();
 if ( (Ptr=strchr(IOBuf,GROUP_SEPARATOR)) != NULL )
  {
   Ptr=strtok(&Ptr[1]," ");
   if ( memcmp(Ptr,"$ABT$",5) == 0 )
    {
     Logger.Log("KENWOOD REJECTED AUTH INQUIRY ON %s",RecvPipe->GetName());
     return;
    }
   else
     Msg.PutFld(FN_TEXTRESPONSE,Ptr);
  }
 else
  {
   Logger.Log("KENWOOD RESPONSE REJECTED. NO RESPONSE TEXT");
   ReplyPipe->Abort();
   return;
  }

 // Get the Data
 if ( ! GetData() )
  {
   ReplyPipe->Abort();
   return;
  }

 TermInfo.IPCKey = (int) IPCKey;
 TermInfo.SyncId=++CurrentSyncId;
 sprintf(TermInfo.TTYName,"VC%d",ClusterNumber);
 Msg.PutFld(FN_TERMINFO,&TermInfo,sizeof(TermInfo));

 if ( FormatKWtoSPS() )
  {
   Msg.Reply();
   IdleState=false;
  }
 else
  {
   ReplyPipe->Abort();
   Logger.Log("%s WAS NOT VALID. MESSAGE DROPPED",DataFileName);
  }
}

//---------------
// Do Voice Reply
//---------------
static void DoReply(void)
{
 char ReplyBuf[100];
 KENWOOD_TERMINFO TermInfo;
 int TermInfoLen;
 DWORD ApprovalNum;
 BYTE  ResponseCode;

 // Get Terminal Info
 TermInfoLen=sizeof(TermInfo);
 if ( ! Msg.GetFld(FN_TERMINFO,&TermInfo,TermInfoLen) )
  {
   Logger.Log("REPLY FROM HOST %s DROPPED: MISSING TERMINFO",
                Msg.MsgRouteInfo()->SourceComputer);
   if ( ! IdleState )
     ReplyPipe->Abort();
   return;
  }

 // If I was in an Idle state then I wasn't expecting a reply.
 if ( IdleState )
  {
   // If SyncId matches our last reply then we assume it's a retry and
   // we acknowledge it, otherwise we log an error message and drop it.
   if ( TermInfo.SyncId == LastReplySyncId )
     Msg.Reply();
   else
     Logger.Log("REPLY FROM HOST %s DROPPED: UNEXPECTED REPLY",
                   Msg.MsgRouteInfo()->SourceComputer);
   return;
  }

 // Enter the idle state
 IdleState=true;

 // We use a SyncID to insure that the reply matches the request.
 if ( TermInfo.SyncId != CurrentSyncId )
  {
   Logger.Log("REPLY FROM HOST %s DROPPED: SYNCID MISMATCH",
       Msg.MsgRouteInfo()->SourceComputer);
   ReplyPipe->Abort();
   return;
  }

 memset(ReplyBuf,' ',sizeof(ReplyBuf));

 // Get the 2 digit response code
 if ( Msg.GetFld(FN_RCODE,ResponseCode) )
  {
   if ( ResponseCode==0 )
    strcpy(ReplyBuf,"NO");
   else
    sprintf(ReplyBuf,"%02d",ResponseCode);
  }
 else
  {
   Logger.Log("REPLY FROM HOST %s DROPPED: MISSING RESPONSE CODE",
              Msg.MsgRouteInfo()->SourceComputer);
   ReplyPipe->Abort();
   return;
  }

 // Insert the Approval Number
 if ( Msg.GetFld(FN_APPROVALNUM,ApprovalNum) )
  sprintf(&ReplyBuf[2],"%06d",ApprovalNum);
 else
  strcat(ReplyBuf,"      ");

 // Get the Response text
 if ( ! Msg.GetFld(FN_TEXTRESPONSE,60,&ReplyBuf[8]) )
  {
   Logger.Log("REPLY FROM HOST DROPPED: MISSING RESPONSE TEXT");
   ReplyPipe->Abort();
   return;
  }

 // Send it
 if ( ReplyPipe->Write(ReplyBuf,strlen(ReplyBuf)) )
  {
   // Send an acknowledgment back to the host
   Msg.Reply();
   LastReplySyncId = TermInfo.SyncId;
  }
}

//----------------------------------------------------
// Simulate filelength function normally found in io.h
//----------------------------------------------------
static int filelength(int fd)
{
 struct stat buf;

 fstat(fd,&buf);
 return buf.st_size;
}


//----------------------------------------
// Format KW Inquiry from SPS Auth request
//----------------------------------------
#define MSGCAT(fn) Msg.GetFld(fn,MAXSPSFIELD,&IOBuf[strlen(IOBuf)])
#define FS IOBuf[strlen(IOBuf)] = FIELD_SEPARATOR
#define GS IOBuf[strlen(IOBuf)] = GROUP_SEPARATOR
static void FormatSPStoKW()
{
 memset(IOBuf,0,IOBufSize);
 strcpy(IOBuf,Msg.MsgRouteInfo()->SourceComputer);
 GS;
 MSGCAT(FN_MERCHID); FS;
 MSGCAT(FN_AMOUNT); FS;
 MSGCAT(FN_LICENSESTATE); FS;
 MSGCAT(FN_LICENSE); FS;
 MSGCAT(FN_DOB); FS;
 MSGCAT(FN_CHECKNUMBER); FS;
 MSGCAT(FN_PHONE); FS;
 MSGCAT(FN_BANKNUMBER); FS;
 MSGCAT(FN_BANKACCOUNT); FS;
 MSGCAT(FN_SSN); FS;
 MSGCAT(FN_KWFLD1); FS;
 MSGCAT(FN_KWFLD2); FS;
 MSGCAT(FN_KWFLD3); FS;
 MSGCAT(FN_KWFLD4); FS;
 MSGCAT(FN_KWFLD5);
}

//----------------------------
// Open and read the Data file
//----------------------------
static int GetData()
{
 int fd;
 int Len;

 // Open the data file and get data
 if ( (fd=open(DataFileName,O_RDONLY)) == -1 )
  {
   Logger.IOError("open",DataFileName);
   close(fd);
   return false;
  }

 // Make sure the IO Buffer is big enough to hold the file
 if ( filelength(fd) >= IOBufSize )
  {
   int NewBufSize=filelength(fd) + 1024;
   char* NewBuf;
   if ( (NewBuf=(char*)realloc(IOBuf,NewBufSize)) == NULL )
    {
     Logger.Log("CAN'T INCREASE BUFSIZE TO %d. OUT OF MEMORY",NewBufSize);
     close(fd);
     return false;
    }
   IOBuf=NewBuf;
   IOBufSize=NewBufSize;
  }

 // Read the file
 if ( (Len=read(fd,IOBuf,IOBufSize-1)) == -1 )
  {
   Logger.IOError("read",DataFileName);
   close(fd);
   return false;
  }

 close(fd);
 IOBuf[Len] = 0;
 return true;
}

MERCHANT Merch;
TINQ Inq;

//-------------------------------------------------------------
// Build an INQ record for the Checks Filter Insertion function
//-------------------------------------------------------------
static void buildInq(void)
{
 int Len;
 memset(&Inq,' ',sizeof(Inq));
 Len=sizeof(Inq.StateCode);
 Msg.GetFld(FN_LICENSESTATE,Inq.StateCode,Len);
 Len=sizeof(Inq.License);
 Msg.GetFld(FN_LICENSE,Inq.License,Len);
 Len=sizeof(Inq.BankNumber);
 Msg.GetFld(FN_BANKNUMBER,Inq.BankNumber,Len);
 Len=sizeof(Inq.BankAccount);
 Msg.GetFld(FN_BANKACCOUNT,Inq.BankAccount,Len);
 Len=sizeof(Inq.PhoneNumber);
 Msg.GetFld(FN_PHONE,Inq.PhoneNumber,Len);
 Len=sizeof(Inq.SSN);
 Msg.GetFld(FN_SSN,Inq.SSN,Len);
}

//-------------------------
// Format KW to SPS message
//-------------------------
static int FormatKWtoSPS(void)
{
 int NumMerchRecs=0;
 char *Ptr;

 FilterAudit.Reset();
 FilterChecks.Reset();

 if ( (Ptr=strtok(IOBuf,"\n")) == NULL )
   return false;

 while (Ptr!=NULL)
  {
   switch(*Ptr)
    {
     case 'A': if ( strlen(Ptr) != sizeof(AUDIT) )
                 return false;
               FilterAudit.Insert((AUDIT*)Ptr,&Inq);
               break;

     case 'M':  ++NumMerchRecs;
                if ( strlen(Ptr) != sizeof(MERCHANT) )
                  return false;
                memcpy(&Merch,Ptr,sizeof(MERCHANT));
                break;

     case 'I':  break;

     case 'C':  if ( strlen(Ptr) != sizeof(CHECK) )
                  return false;
                FilterChecks.Insert((CHECK*)Ptr,&Inq);
                break;

     default:  return false;
    }
   Ptr=strtok(NULL,"\n");
  }

 if ( NumMerchRecs==0 )
   return false;

 // Put the records into the message
 Msg.PutFld(FN_KWMERCHANTRECORD,&Merch,sizeof(Merch));
 FilterAudit.PutMsg(Msg);
 FilterChecks.PutMsg(Msg);

 return true;
}


//-----------
// KWPosOut.C
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
#include "kwrec.h"
#include "pipes.h"

// Global C++ objects
SPSLogger Logger("KWPosOut");
INETManager Inet;
IPCMessage Msg;

#define MAX_PIPEBUF 100

// static function prototypes
static void init(int argc,char* argv[]);
static void DoPosMsg(void);
static int  ClusterNumber=1;
static char DataFileName[50];
static int  BufSize=8192;
static int  KWtoSPS(void);
static int  filelength(int fd);
static char* IOBuf;
static char ComputerName[MAX_IPCNAME+1];
static char ProcessName[MAX_IPCNAME+1];

Pipe* RecvPipe;

//--------------
// Main function
//--------------
void main(int argc,char* argv[])
{
 int len;

 init(argc,argv);

 for(;;)
  {
   // Get a pipe message
   if ( (len=RecvPipe->Read(IOBuf,BufSize)) == -1 )
    continue;
   // Insure a null terminator
   IOBuf[len]=0;
   // Do the transaction
   DoPosMsg();
  }
}

//--------------
// Init function
//--------------
static void init(int argc,char* argv[])
{
 char RecvPipeName[60];

 if ( ! Inet.IsOkay() )
   Logger.Abort();

 if ( argc != 4 )
   {
    Logger.Log("USAGE IS KWPosOut Cluster  DestHostName DestProcessName");
    Logger.Abort();
   }

 ClusterNumber = atoi(argv[1]);
 strcpy(ComputerName,argv[2]);
 strcpy(ProcessName,argv[3]);

 // Set up the pipe/file names
 sprintf(RecvPipeName,"/usr/spool/uucp/TRN..%d",ClusterNumber);
 sprintf(DataFileName,"/usr/spool/uucp/DTA..%d",ClusterNumber);

 // Allocate memory for I/O buffer
 if ( (IOBuf=(char*)malloc(BufSize)) == NULL )
  {
   Logger.Log("OUT OF MEMORY");
   Logger.Abort();
  }
 // Instantiate the Pipe class
 RecvPipe = new Pipe(RecvPipeName);
}


//-------------------
// Do the POS Request
//-------------------
static void DoPosMsg(void)
{
 int fd;
 int Len;
 KENWOOD_TERMINFO TermInfo;
 char *Ptr;

 // Reset the IPC Message buffer for a new message
 Msg.ClearMsg();

 // Insert the TERMINFO structure
 memset(&TermInfo,0,sizeof(TermInfo));
 TermInfo.Cluster = ClusterNumber;
 if ((Ptr=strchr(IOBuf,GROUP_SEPARATOR)) != NULL)
  {
   *Ptr=0;
   if ( strlen(IOBuf) > sizeof(TermInfo.TTYName)-1 )
     IOBuf[sizeof(TermInfo.TTYName)-1] = 0;
   memcpy(&TermInfo.TTYName,IOBuf,strlen(IOBuf));
   Msg.PutFld(FN_TERMINFO,&TermInfo,sizeof(TermInfo));
   Msg.PutFld(FN_TEXTRESPONSE,&Ptr[1]);
  }

 // Open the data file and get data 
 if ( (fd=open(DataFileName,O_RDONLY)) == -1 )
  {
   Logger.IOError("open",DataFileName);
   close(fd);
   return;
  }

 // Make sure the IO Buffer is big enough to hold the file
 if ( filelength(fd) >= BufSize ) 
  {
   int NewBufSize=filelength(fd) + 1024;
   char* NewBuf;
   if ( (NewBuf=(char*)realloc(IOBuf,NewBufSize)) == NULL )
    {
     Logger.Log("CAN'T INCREASE BUFSIZE TO %d. OUT OF MEMORY",NewBufSize);
     close(fd);
     return; 
    }
   IOBuf=NewBuf;
   BufSize=NewBufSize;
  }

 // Read the file
 if ( (Len=read(fd,IOBuf,BufSize-1)) == -1 )
  {
   Logger.IOError("read",DataFileName);
   close(fd);
   return; 
  } 

 close(fd);
 IOBuf[Len] = 0;

 // Format Kenwood to SPS
 if ( KWtoSPS() )
   Msg.Send(ComputerName,ProcessName,ORIGINATE);
 else
   Logger.Log("INVALID FORMAT KENWOOD DTA MESSAGE DROPPED");
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

static void Put(char* FldAdr,int FldLen,FLDNUM fn)
{
 char TempBuf[100];
 
 // Don't put it if it's blank
 if (FldAdr[0] == ' ')
   return;
 
 memcpy(TempBuf,FldAdr,FldLen);
 TempBuf[FldLen]=0;
 Msg.PutFld(fn,TempBuf);
}

#define MAXAUDITRECS 20
#define MAXCHECKRECS 20
AUDIT Audit[MAXAUDITRECS];
CHECK Check[MAXCHECKRECS];
MERCHANT Merch;
TINQ Inq;

//-------------------------
// Format KW to SPS message
//-------------------------
#define PUT(f,n) Put(f,sizeof(f),n)
static int KWtoSPS(void)
{
 int NumAuditRecs=0;
 int NumCheckRecs=0;
 int NumInqRecs=0;
 int NumMerchRecs=0;
 int i, Count;
 char *Ptr;

 Msg.PutFld(FN_TRANTYPE,T_POSAUTH);
 if ( (Ptr=strtok(IOBuf,"\n")) == NULL )
   return false;

 while (Ptr!=NULL)
  {
   switch(*Ptr)
    {
     case 'A': if ( strlen(Ptr) != sizeof(AUDIT) )
                 return false;
                if ( NumAuditRecs <  MAXAUDITRECS )
                 {
                  memcpy(&Audit[NumAuditRecs],Ptr,sizeof(AUDIT));
                  ++NumAuditRecs;
                 }
                break;

     case 'M':  ++NumMerchRecs; 
                if ( strlen(Ptr) != sizeof(MERCHANT) )
                  return false;
                memcpy(&Merch,Ptr,sizeof(MERCHANT));
                break;   

     case 'I':  ++NumInqRecs;
                if ( strlen(Ptr) != sizeof(TINQ) )
                  return false;
                memcpy(&Inq,Ptr,sizeof(TINQ));
                break;

     case 'C':  if ( strlen(Ptr) != sizeof(CHECK) )
                  return false;
                if ( NumCheckRecs < MAXCHECKRECS )
                 {
                  memcpy(&Check[NumCheckRecs],Ptr,sizeof(CHECK));
                  ++NumCheckRecs;
                 }
                break;

     default:  return false; 
    }
   Ptr=strtok(NULL,"\n");
  }
 
 if ( NumInqRecs==0 || NumMerchRecs==0 )
   return false;

 PUT(Inq.Amount,FN_AMOUNT);
 PUT(Inq.StateCode,FN_LICENSESTATE);
 PUT(Inq.License,FN_LICENSE);
 PUT(Inq.DateOfBirth,FN_DOB);
 PUT(Inq.CheckNumber,FN_CHECKNUMBER);
 PUT(Inq.PhoneNumber,FN_PHONE);
 PUT(Inq.BankNumber,FN_BANKNUMBER);
 PUT(Inq.BankAccount,FN_BANKACCOUNT);
 PUT(Inq.SSN,FN_SSN);
 PUT(Inq.Misc1,FN_PRODUCTCLASS);
 PUT(Inq.Misc2,FN_ZIPCODE);
 PUT(Inq.Misc3,FN_ADDRESS);
 PUT(Inq.Misc4,FN_LASTNAME);
 PUT(Inq.Misc5,FN_CHECKTYPE);
 PUT(Merch.MerchId,FN_MERCHNUM);
 for (i=Count=0; i<NumAuditRecs; ++i)
  {
   memcpy(&IOBuf[Count],&Audit[i],sizeof(AUDIT));
   IOBuf[ Count +=sizeof(AUDIT) ] = 0xd;
   IOBuf[++Count] = 0xa;
  }
 for (i=0; i<NumCheckRecs; ++i)
  {
   memcpy(&IOBuf[Count],&Check[i],sizeof(CHECK));
   IOBuf[ Count += sizeof(CHECK) ] = 0xd;
   IOBuf[++Count] = 0xa;
  }
 IOBuf[Count]=0;
 Msg.PutFld(200,IOBuf);
 return true;
}


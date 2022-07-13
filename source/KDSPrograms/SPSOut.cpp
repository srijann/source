//---------
// SPSOut.C
//---------
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/msg.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include "ipcmsg.h"
#include "trantype.h"
#include "authfn.h"
#include "spslog.h"
#include "inetman.h"
#include "ipckeys.h"
#include "kwrec.h"
#include "pipes.h"
#include "recfiltr.h"

// Global C++ objects
SPSLogger Logger("SPSOut");
INETManager Inet;
IPCMessage Msg;
AuditFilter FilterAudit;
CheckFilter FilterChecks;

// static function prototypes
static void  init(int argc,char* argv[]);
static void  DoPosMsg(void);
static int   ClusterNumber;
static char  DataFileName[50];
static int   BufSize=8192;
static int   KWtoSPS(void);
static int   filelength(int fd);
static char* IOBuf;
static char  ComputerName[MAX_IPCNAME+1];
static char  ProcessName[MAX_IPCNAME+1];
static key_t IPCKey;
static int   DisplayMode=0;
static void Dump(const char* Text,const char* Buf,int Len);

Pipe* RecvPipe;
Pipe* SendPipe;

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
 char SendPipeName[60];

 if ( ! Inet.IsOkay() )
   Logger.Abort();

 if ( argc < 4 )
   {
    Logger.Log("USAGE IS SPSOut Cluster DestHostName DestProcessName");
    Logger.Abort();
   }
 if ( argc==5 )
  DisplayMode=1;  

 ClusterNumber = atoi(argv[1]);
 strcpy(ComputerName,argv[2]);
 strcpy(ProcessName,argv[3]);

 // Set up the pipe/file names
 sprintf(RecvPipeName,"/usr/spool/uucp/TRN..%d",ClusterNumber);
 sprintf(DataFileName,"/usr/spool/uucp/DTA..%d",ClusterNumber);
 sprintf(SendPipeName,"/usr/spool/uucp/RSP..%d",ClusterNumber);

 // Allocate memory for I/O buffer
 if ( (IOBuf=(char*)malloc(BufSize)) == NULL )
  {
   Logger.Log("OUT OF MEMORY");
   Logger.Abort();
  }

 // Create the Pipe objects
 RecvPipe = new Pipe(RecvPipeName);
 SendPipe = new Pipe(SendPipeName);

 // Create my MSG Queue
 IPCKey = IPCKEY_POSMSGQUEUE+ClusterNumber;
 if ( msgget(IPCKey,IPC_CREAT) == -1 )
  {
   Logger.IOError("msgget","IPCKEY_POSMSGQUEUE");
   Logger.Abort();
  }
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
 TermInfo.IPCKey = (int)IPCKey;
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
  {
   Msg.Send(ComputerName,ProcessName,ORIGINATE);
   // printf(">> Sent to %s\n",ComputerName);
  }
 else
  {
   Logger.Log("INVALID FORMAT KENWOOD DTA MESSAGE DROPPED");
   // Notify Kenwood that there will be no reply
   SendPipe->Write("$ABT$",5);
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

static void Put(char* FldAdr,int FldLen,FLDNUM fn,char* Text)
{
 char TempBuf[100];
 int  start, stop;

 // Skip past leading blanks to find start of data
 for (start=0; start<FldLen && FldAdr[start]==' '; ++start);
 if ( start==FldLen )
  return;

 // Skip past trailing blanks to find end of data
 for (stop=FldLen-1; FldLen>=0 && FldAdr[stop]==' '; --stop);

 memset(TempBuf,0,sizeof(TempBuf));
 memcpy(TempBuf,&FldAdr[start],stop-start+1);
 Msg.PutFld(fn,TempBuf);
 if ( DisplayMode && Text != NULL )
  printf("%s=%s ",Text,TempBuf);
}

MERCHANT Merch;
TINQ Inq;

//-------------------------
// Format KW to SPS message
//-------------------------
#define PUT(f,n,t) Put(f,sizeof(f),n,t)

static int KWtoSPS(void)
{
 int NumInqRecs=0;
 int NumMerchRecs=0;
 int i;
 char *Ptr;
 int BufLen=strlen(IOBuf);

 FilterAudit.Reset();
 FilterChecks.Reset();

 memset(&Inq,' ',sizeof(Inq));
 Msg.PutFld(FN_TRANTYPE,T_POSAUTH);
 if ( (Ptr=strtok(IOBuf,"\n")) == NULL )
  {
   Dump("No Carriage Returns",IOBuf,BufLen);
   return false;
  }

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

     case 'I':  ++NumInqRecs;
                if ( strlen(Ptr) != sizeof(TINQ) )
                  return false;
                memcpy(&Inq,Ptr,sizeof(TINQ));
                break;

     case 'C':  if ( strlen(Ptr) != sizeof(CHECK) )
                  return false;
                FilterChecks.Insert((CHECK*)Ptr,&Inq);
                break;

     default:  Dump("Unrecognized Record Type",IOBuf,BufLen);
               return false;
    }
   Ptr=strtok(NULL,"\n");
  }

 if ( NumInqRecs==0 || NumMerchRecs==0 )
  {
   if ( NumInqRecs==0 )
    Dump("No Inquiry Record",IOBuf,BufLen);
   else
    Dump("No Merchant Record",IOBuf,BufLen);
   return false;
  }

 PUT(Inq.Amount,FN_AMOUNT,"AMT");
 PUT(Inq.StateCode,FN_LICENSESTATE,"ST");
 PUT(Inq.License,FN_LICENSE,"LIC");
 PUT(Inq.DateOfBirth,FN_DOB,"DOB");
 PUT(Inq.CheckNumber,FN_CHECKNUMBER,"CHKNUM");
 PUT(Inq.PhoneNumber,FN_PHONE,"PH");
 PUT(Inq.BankNumber,FN_BANKNUMBER,"BKNO");
 PUT(Inq.BankAccount,FN_BANKACCOUNT,"BKAC");
 //Put(Inq.BankNumber,sizeof(Inq.BankNumber)+sizeof(Inq.BankAccount),
 //      FN_MICR,"MICR");
 PUT(Inq.SSN,FN_SSN,"SSN");
 PUT(Inq.ProductClass,FN_PRODUCTCLASS,"PROD");
 PUT(Inq.ZipCode,FN_ZIPCODE,"ZIP",);
 PUT(Merch.MerchId,FN_MERCHID,"TID");
 PUT(Inq.CheckSwipe,FN_CHECKSWIPE,"CSW");
 PUT(Inq.LicenseSwipe,FN_LICENSESWIPE,"LSW");
 Msg.PutFld(FN_KWMERCHANTRECORD,&Merch,sizeof(Merch));
 FilterAudit.PutMsg(Msg);
 FilterChecks.PutMsg(Msg);
 return true;
}
//--------------------------------
// Print a dump to file SPSOut.DMP
//--------------------------------
void Dump(const char* Text,const char* Buf,int Len)
{
 unsigned char* sBuf=(unsigned char*)Buf;
 register i, j, k;
 FILE* f=fopen("SPSOUT.DMP","a");
 time_t now;
 struct tm* tblock;

 now = time(NULL);
 tblock = localtime( &now );

 if ( f==NULL )
  return;

 fprintf(f,"%s",asctime(tblock));
 fprintf(f,"%s\n",Text);
 fprintf(f,"Message Length=%d\n",Len);

 #define CHARS_PER_LINE 10
 char  hexBuf[CHARS_PER_LINE*3+1];
 char  charBuf[CHARS_PER_LINE+1];

 for (i=0; i<Len; i += CHARS_PER_LINE)
  {
   memset(hexBuf,0,sizeof(hexBuf));
   memset(charBuf,0,sizeof(charBuf));
   for ( j=0, k=i+j; k<Len && j < CHARS_PER_LINE; ++j, ++k)
    {
     sprintf(&hexBuf[j*3],"%02X ",sBuf[k]);
     if ( isprint(sBuf[k]) )
      charBuf[j] = sBuf[k];
     else
      charBuf[j] = '.';
    }
   fprintf(f,"%05d %-*s %-*s\n",i,CHARS_PER_LINE*3,hexBuf,
           CHARS_PER_LINE,charBuf);
  }

 fclose(f); 
}




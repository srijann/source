//---------
// PosIn.C
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
#include "ipcmsg.h"
#include "trantype.h"
#include "authfn.h"
#include "spslog.h"
#include "inetman.h"
#include "ipckeys.h"
#include "kwrec.h"
#include "pipes.h"

// Global C++ objects
SPSLogger Logger("PosIn");
IPCMessage Msg;
INETManager Inet;

// Message Queue Id
int MsgQId;

// static function prototypes
static void init(int argc,char* argv[]);
static void DoPosMsg(void);
static int  ClusterNumber;
Pipe* RSPPipe;

//--------------
// Main function
//--------------
void main(int argc,char* argv[])
{
 init(argc,argv);

 for(;;)
   if ( Msg.Recv(MsgQId,1) )
     DoPosMsg();
}

//--------------
// Init function
//--------------
static void init(int argc,char* argv[])
{
 int i;
 char RSPPipeName[50];

 if ( argc !=  2 )
  {
   Logger.Log("USAGE IS PosIn <cluster number>");
   Logger.Abort();
  }

 ClusterNumber = atoi(argv[1]);

 // Get the Message Queue Id
 for (i=0; i<10; ++i)
   if ( (MsgQId=msgget(IPCKEY_POSMSGQUEUE+ClusterNumber,0)) == -1 )
     sleep(2);

 if ( MsgQId==-1 )
  {
   Logger.IOError("msgget","IPCKEY_POSMSGQUEUE");
   Logger.Abort(); 
  }

 // Set up the pipe/file names
 sprintf(RSPPipeName,"/usr/spool/uucp/RSP..%d",ClusterNumber);
 RSPPipe=new Pipe(RSPPipeName);
}


//--------------------
// Do the POS Response
//--------------------
static void DoPosMsg(void)
{
 int Len;
 KENWOOD_TERMINFO TermInfo;
 int TermInfoLen=sizeof(TermInfo);
 char PipeBuf[100];
 char ResponseBuf[40];
 char OutputCode[3];
 char ApprovalNum[7];
 DWORD AppNum;
 BYTE  Rcode;

 if ( ! Msg.GetFld(FN_TERMINFO,&TermInfo,TermInfoLen) )
  {
   Logger.Log("SPS RESPONSE MESSAGE DROPPED. NO TERMINFO AVAILABLE");
   return;
  }
 if ( ! Msg.GetFld(FN_TEXTRESPONSE,sizeof(ResponseBuf),ResponseBuf) )
  {
   Logger.Log("SPS RESPONSE MESSAGE DROPPED. NO REPLY TEXT");
   return;
  }
 if ( ! Msg.GetFld(FN_RCODE,Rcode) )
  {
   Logger.Log("SPS RESPONSE MESSAGE DROPPED. NO RESPONSE CODE");
   return;
  }

 if ( Rcode==0 )
  strcpy(OutputCode,"NO");
 else
  sprintf(OutputCode,"%02d",Rcode); 

 if ( Msg.GetFld(FN_APPROVALNUM,AppNum) )
  sprintf(ApprovalNum,"%06d",AppNum);
 else
  strcpy(ApprovalNum,"      ");

 memset(PipeBuf,0,sizeof(PipeBuf));
 sprintf(PipeBuf,"%s%c%s%s%s",TermInfo.TTYName,GROUP_SEPARATOR,
   OutputCode,ApprovalNum,ResponseBuf);
 Len=strlen(PipeBuf);

 // Write to the Pipe
 RSPPipe->Write(PipeBuf,Len);
 puts(PipeBuf);
}



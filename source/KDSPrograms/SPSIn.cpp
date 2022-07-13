//--------
// SPSRoute.C
//--------
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <string.h>
#include <time.h>
#include "ipcmsg.h"
#include "trantype.h"
#include "authfn.h"
#include "spslog.h"
#include "inetman.h"
#include "ipckeys.h"
#include "kwrec.h"

// Global C++ objects
SPSLogger Logger("SPSRoute");
INETManager Inet(BINDSOCK);
IPCMessage Msg;

static int NumVoiceClusters;
static int AdminMsgQueue;

// Voice cluster table.
static struct VoiceTableDef
 {
  char HostName[MAX_IPCNAME+1];
  key_t IPCKey;
 } *VoiceTable;

// static function prototypes
static int createqueue(key_t key);
static void init(int argc,char* argv[]);
static void Route(void);

//--------------
// Main function
//--------------
void main(int argc,char *argv[])
{
 init(argc,argv);

 for(;;)
   if ( Msg.Recv() )
     Route();
}

//--------------
// Init function
//--------------
static void init(int argc,char* argv[])
{
 int i;

 if ( ! Inet.IsOkay() )
   Logger.Abort();

 if ( argc < 5 )
  {
   Logger.Log("USAGE IS SPSRoute <NumVoiceClusters>");
   Logger.Abort();
  }

 NumVoiceClusters=atoi(argv[2]);

 VoiceTable=(struct VoiceTableDef*)
   malloc( NumVoiceClusters * sizeof(*VoiceTable) );
 if ( VoiceTable==NULL )
  {
   Logger.Log("OUT OF MEMORY");
   Logger.Abort();
  }

 // Set up Voice Table
 for (i=0; i<NumVoiceClusters; ++i)
  {
   VoiceTable[i].HostName[0]=0;
   VoiceTable[i].IPCKey = IPCKEY_VOICEMSGQUEUE + i;
  }
 AdminMsgQueue=createqueue(IPCKEY_ADMINMSGQUEUE); 
}

//---------------------
// createqueue function
//---------------------
static int createqueue(key_t key)
{
 int id;
 if ( (id=msgget(key,IPC_CREAT)) == -1 )
  {
   Logger.IOError("msgget");
   Logger.Abort();
  }
 return id;
}

//-----------------------------------------------
// Route the message to appropriate message queue
//-----------------------------------------------
static void Route(void)
{
 char TranType[20];
 KENWOOD_TERMINFO TermInfo;
 int TermInfoLen;
 int i;

 if ( ! Msg.GetFld(FN_TRANTYPE,sizeof(TranType),TranType) )
  {
   Logger.Log("MESSAGE WITH NO TRANTYPE DROPPED");
   return;
  }

 // Authorization Reply? The IPCKey is stored in the Terminfo field.
 // Send the reply there.
 if ( strcmp(TranType,T_AUTHREPLY) == 0 )
   {
    TermInfoLen=sizeof(TermInfo);
    if ( ! Msg.GetFld(FN_TERMINFO,&TermInfo,TermInfoLen) )
     {
      Logger.Log("RESPONSE MESSAGE MISSING TERMINFO. MESSAGE DROPPED");
      return;
     }
    Msg.Send( msgget(TermInfo.IPCKey,0) ,1);
    return;
   }

 // Admin message?
 if ( strcmp(TranType,T_ADMINMSG) == 0 )
  {
   Msg.Send(AdminMsgQueue,1);
   return;
  }

 // Incoming Voice authorization request?
 if ( strcmp(TranType,T_VOICEAUTH) == 0 )
  {
   // Find the host name in the Voice table
   for (i=0; i<NumVoiceClusters; ++i)
    if (strcmp(Msg.MsgRouteInfo()->SourceComputer,VoiceTable[i].HostName)==0)
     {
      Msg.Send( msgget(VoiceTable[i].IPCKey,0), 1 );
      return;
     }
   return;
  }

 // None of the above is an error
 Logger.Log("MESSAGE WITH UNRECOGNIZED TRANTYPE (%s) DROPPED",TranType);
}

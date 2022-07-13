//-----------
// SPSRoute.C
//-----------
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
  key_t  IPCKey;
  char   HostName[MAX_IPCNAME+1];
  time_t TranTime;
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

 if ( argc < 2 )
  {
   Logger.Log("USAGE IS SPSRoute <NumVoiceClusters>");
   Logger.Abort();
  }

 NumVoiceClusters=atoi(argv[1]);

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
   VoiceTable[i].TranTime=0;
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

//--------------------------------
// Select Voice cluster for a host
//--------------------------------
static int GetVoiceCluster(void)
{
 int i, n;

 // First look for a matching host name
 for (i=0; i<NumVoiceClusters; ++i)
  if ( strcmp(Msg.MsgRouteInfo()->SourceComputer,VoiceTable[i].HostName) == 0 )
   return i;

 // No match, look for an empty slot
 for (i=n=0; i<NumVoiceClusters; ++i)
  if ( VoiceTable[i].HostName[0] == 0 )
   {
    strcpy(VoiceTable[i].HostName,Msg.MsgRouteInfo()->SourceComputer);
    return i;
   }
  else
   if ( VoiceTable[i].TranTime < VoiceTable[n].TranTime )
    n=i;

 // No empty slot, reuse the oldest one
 strcpy(VoiceTable[n].HostName,Msg.MsgRouteInfo()->SourceComputer);
 return n;
}

//-----------------------------------------------
// Route the message to appropriate message queue
//-----------------------------------------------
static void Route(void)
{
 char TranType[20];
 KENWOOD_TERMINFO TermInfo;
 int TermInfoLen;
 
 if ( ! Msg.GetFld(FN_TRANTYPE,sizeof(TranType),TranType) )
  {
   Logger.Log("MESSAGE WITH NO TRANTYPE DROPPED");
   return;
  }

 // Authorization Reply? The IPCKey is stored in the Terminfo field.
 // Send the reply there.
 if ( strcmp(TranType,T_AUTHREPLY) == 0 ||
      strcmp(TranType,T_POSAUTH) == 0 )
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
   int i=GetVoiceCluster();
   Msg.Send( msgget(VoiceTable[i].IPCKey,0), 1);
   VoiceTable[i].TranTime=time(NULL);
   return;
  }

 // None of the above is an error
 Logger.Log("MESSAGE WITH UNRECOGNIZED TRANTYPE (%s) DROPPED",TranType);
}

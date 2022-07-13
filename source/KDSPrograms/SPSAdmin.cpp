//-----------
// SPSAdmin.C
//-----------
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <string.h>
#include <unistd.h>
#include "ipcmsg.h"
#include "trantype.h"
#include "authfn.h"
#include "spslog.h"
#include "inetman.h"
#include "ipckeys.h"

// Global C++ objects
SPSLogger Logger("SPSAdmin");
INETManager Inet;
IPCMessage Msg;

// Message Queue Id
int MsgQId;

// static function prototypes
static void init(void);
static void DoAdminMsg(void);

//--------------
// Main function
//--------------
void main(void)
{
 init();

 for(;;)
   if ( Msg.Recv(MsgQId,1) )
     DoAdminMsg();
}

//--------------
// Init function
//--------------
static void init(void)
{
 int i;

 if ( ! Inet.IsOkay() )
   Logger.Abort();

 // Get the Message Queue Id
 for (i=0; i<10; ++i)
   if ( (MsgQId=msgget(IPCKEY_ADMINMSGQUEUE,0)) == -1 )
     sleep(2);

 if ( MsgQId==-1 )
  {
   Logger.IOError("msgget","IPCKEY_ADMINMSGQUEUE");
   Logger.Abort();
  }
}


//-----------------------------------------------
// Route the message to appropriate message queue
//-----------------------------------------------
static void DoAdminMsg(void)
{
 char AdminMsg[200];
 char *KeyWord;

 if ( ! Msg.GetFld(FN_ADMINMSG,sizeof(AdminMsg),AdminMsg) ||
      (KeyWord=strtok(AdminMsg," ")) == NULL )
  {
   Logger.Log("NO ADMIN MESSAGE FIELD PRESENT. MESSAGE DROPPED");
   return;
  }
 

 if ( strcmp(KeyWord,"HELLO")==0 )
   sprintf(AdminMsg,"%s ALIVE",Inet.GetMyHostName());
 else

 if ( strcmp(KeyWord,"REROUTE") == 0 )
  {
   char *OldName;
   char *NewName;

   if ( (OldName=strtok(NULL," ")) == NULL ||
        (NewName=strtok(NULL," ")) == NULL ||
        strlen(OldName) > MAX_IPCNAME ||
        strlen(NewName) > MAX_IPCNAME )
    return;

   if ( ! Inet.InsReRoute(OldName,NewName) )
     strcpy(AdminMsg,"FAILED");
   else
     strcpy(AdminMsg,"OKAY");
  }
 else

 if ( strcmp(KeyWord,"CANCELALLREROUTES") == 0 )
   {
    Inet.ClearAllReRoutes();
    strcpy(AdminMsg,"OKAY");
   } 
 else

 if ( strcmp(KeyWord,"HELP") == 0 )
   strcpy(AdminMsg,"HELP,HELLO,REROUTE,CANCELALLREROUTES");
 else

 strcpy(AdminMsg,"-- Say What?");  

 Msg.PutFld(FN_ADMINMSG,AdminMsg);
 Msg.Reply();
}

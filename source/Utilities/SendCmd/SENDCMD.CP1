#include "win4me.h"
#include <iostream.h>
#include "CheckAuthFn.h"

static IPCMessage Msg;
// Default Values may be changed by getArgs
static short Wait=10000;
static int Tries=1;
static char* ReplyString=0;
bool NoReply=false;
bool Print=false;

bool getArgs(int argc,char* argv[])
{
 int i;

 for (i=3; i<argc; ++i)
  if ( memicmp(argv[i],"/wait=",6) == 0 )
   Wait=(short)atoi(&argv[i][6]) * 1000;
  else
  if ( memicmp(argv[i],"/reply=",7) == 0 )
   ReplyString=&argv[i][7];
  else
  if ( memicmp(argv[i],"/try=",5) == 0 )
   Tries=atoi(&argv[i][5]);
  else
   if ( stricmp(argv[i],"/noreply") == 0 )
    NoReply=true;
   else
   if ( stricmp(argv[i],"/print") == 0 )
    Print=true;
   else
    return false;

 return true;
}

void main(int argc,char* argv[])
{
 char* AdminMsg;

 if ( argc < 3 || ! getArgs(argc,argv) )
  {
   cout << "Usage is SENDCMD ProcessName ""Command String"" [options]\n"
        << "  Options: /wait=nn  (wait nn seconds for reply)\n"
        << "           /reply=""string"" (expect reply of string)\n"
        << "           /try=nn  (try nn times if no response)\n"
        << "           /print (print response/error messages)\n"
        << "           /noreply (expect no reply)\n\n";
   exit(EXIT_FAILURE);
  }

 IPCSyncIOInit("SENDCMD");
 strupr(argv[2]);
 Msg.PutFld(FN_ADMINMSG,argv[2]);

 if ( NoReply )
  {
   if ( ! Msg.SendMsg(argv[1]) )
    {
     if ( Print )
       cout << "Error Sending Message\n";
     exit(0);
    }
   exit(1);
  }

 while( Tries )
  {
   if ( ! Msg.SendRecv(argv[1],Wait) )
    {
     --Tries;
     continue;
    }

   if ( (AdminMsg=(char*)Msg.GetFldPtr(FN_ADMINMSG)) == NULL )
    AdminMsg="?noresponse";
   else
    if ( Print )
      cout << AdminMsg << "\n";

   if ( ReplyString && stricmp(AdminMsg,ReplyString) )
    {
     if ( Print )
       cout << "Invalid Response\n";
     exit(0);
    }

   exit(1);
  }

 if ( Print )
   cout << "No Response\n";
 exit(0);
}

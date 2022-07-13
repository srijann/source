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
//----------------------------------------------------------------------------

#include <windows.h>
#include "ServerApp.h"
#include "splib.h"
#include "stdio.h"
#include <time.h>

void DoTraceMsg(IPCMessage *Msg);

#if 0
void llogg(int line, char *buf1, char *buf2, char *buf3, char *buf4, char *buf5, long l, int i );
void logg(int line, char *buf1, char *buf2, long l, int i );

void logg(int line, char *buf1, char *buf2, long l, int i )
{
   time_t tnow;
   FILE  *fout;
   time(&tnow);
   fout = fopen(".\\aaa.log", "a");
   fprintf(fout,"@%d. %s -%s-at %s. %ld-%d.\n", line, buf1, buf2, ctime(&tnow), l ,i);
   fclose(fout);
}
void llogg(int line, char *buf1, char *buf2, char *buf3, char *buf4, char *buf5, long l, int i )
{
   char szbuf[128];
   time_t tnow;
   FILE  *fout;

   time(&tnow);
   //sprintf(szbuf,"%s",ctime(&tnow));

   fout = fopen(".\\aaa.log", "a");
   fprintf(fout,"@%d. %s -src=%s-%s, dest=%s-%s-at %s. %ld-%d.\n", line, buf1, buf2, buf3, buf4, buf5, ctime(&tnow), l ,i);
   fclose(fout);
}
#endif
//---------------------------------------------
// The RECEIVE Task. Inbound Messages come here
//---------------------------------------------
DWORD RecvTask(ServerApp* App)
{
   SOCKET RecvSocket;
   struct sockaddr_in srv_addr;
   IPCMessage Msg;
   IPCROUTEINFO* rtinfo=Msg.MsgRouteInfo();
   int    fromlen;
   struct sockaddr_in fromaddr;
   int    bread;

   // char szbuf[512];
   char MsgText[128];
   float fTime;
   time_t start, finish;
   int  len=sizeof(MsgText);
   //logg(__LINE__, "NtRte ", "RcvTask start",0,0 );

   // Create the RECV Socket
   if ( (RecvSocket=socket(AF_INET,SOCK_DGRAM,0)) == INVALID_SOCKET )
      {
      App->LogNetworkError("socket","RecvTask",WSAGetLastError());
      return 0;
      }

   // Bind the socket to OUR port
   memset(&srv_addr,0,sizeof(srv_addr));
   srv_addr.sin_family = AF_INET;
   srv_addr.sin_addr.s_addr = htonl (INADDR_ANY) ;
   srv_addr.sin_port = App->IPCPort;

   if ( bind(RecvSocket,(struct sockaddr *)&srv_addr,
             sizeof(srv_addr)) == SOCKET_ERROR )
   {
      App->LogNetworkError("bind","RecvTask",WSAGetLastError());
      return 0;
   }

   // Read and Process ALL messages
   for(;;)
   {
      fromlen=sizeof(fromaddr);
      //logg(__LINE__, "NtRte start loop", "RcvTask processing message",0,0 );

      if ( (bread=recvfrom(RecvSocket,(char *)rtinfo,
            MAX_MSG,0,(struct sockaddr *)&fromaddr,&fromlen)) ==
          SOCKET_ERROR )
      {
         //logg(__LINE__, "NtRte ", "SOCKET_ERROR",0,0 );
         App->LogNetworkError("recvfrom","RecvTask",
                              WSAGetLastError());
         continue;
      }
      //llogg(__LINE__, "NtRte Recv ", rtinfo->SourceProcess,rtinfo->SourceComputer,
      //    rtinfo->DestProcess,  rtinfo->DestComputer,0,0);

      Msg.SetMsgLength((WORD)bread);
      if ( ! Msg.ParseMsg() )
         {
         //logg(__LINE__, "NtRte ", "ParseMsg() failed",0,0 );
         IPCLogEvent(BadMessageFromHost,rtinfo,Msg.GetMsgLength(),
                     0,inet_ntoa(fromaddr.sin_addr));
         continue;
         }

      // See if there is a TRACE message and Handle it

      // ### Use fromaddr Struct Get the COMMAND
      int ii =   Msg.GetFld(IPCFN_DEBUG,len,MsgText);
      //logg(__LINE__, "NtRte RecvTask MsgText and ii",MsgText,0,ii );

      if (Msg.GetFld(IPCFN_DEBUG))
         DoTraceMsg(&Msg);

      if ( rtinfo->ReturnInfo.Flags & RETINFO_REPLY )
         {
         //logg(__LINE__, "NtRte RecvTask","ReplyMsg()",0,0 );
         Msg.ReplyMsg();
         }
      else
        {
        //logg(__LINE__, "NtRte RecvTask","SendMsg() to Destination",0,0 );
        Msg.SendMsg(rtinfo->DestProcess);
        }
      //logg(__LINE__, "NtRte end loop", "RcvTask",0,0 );
   }
}

//------------------------
// Handle a DEBUG COMMAND
//
// A DEBUG COMMAND is a Message with a text Command in the
// IPCFN_DEBUG field. After the DEBUG action is taken, it
// is HANDLED as an APPLICATION Message.
//------------------------
void DoTraceMsg(IPCMessage* Msg)
{
 char MsgText[128];
 int  len=sizeof(MsgText);
 char *command;
 char *scategory;
 WORD  category;

 // Get the COMMAND
 if ( ! Msg->GetFld(IPCFN_DEBUG,len,MsgText) )
  return;

 if ( (command=strtok(MsgText," ")) != NULL )
  {
   if ( (scategory=strtok(NULL," ")) != NULL )
     category=(WORD)atoi(scategory);
   else
     category=0;

   if ( stricmp(command,"TRACE") == 0 )
     IPCLogEvent(MessageTrace,category,MsgText);
   else

   if ( stricmp(command,"TRACEDUMP") == 0 )
     IPCLogEvent(MessageTraceDump,Msg->GetMsgPointer(),
               (int)Msg->GetMsgLength(),category,MsgText);
  }

}


//-----------------------------------------------------------------
// The ServerAppInfo object. Provides Information about the Server.
//-----------------------------------------------------------------
ServerAppInfo::ServerAppInfo()
{
 // You Must supply a Process Name for the Server
 strcpy(ServerProcessName,"NETROUTE");

 // Supply A Version Number for the Dialog Window and ADMIN commands.
 strcpy(ProgramVersionInfo,"1.1");
}

//---------------------------------------------------------------------
// ServerApp Constructor .. The main thread will receive local messages
// and send on the network. Here, we create a thread to receive from
// the network and route locally.
//---------------------------------------------------------------------
ServerApp::ServerApp()
{
 // Set this flag to true if your initialization succeeds. If it fails
 // set it to false. The Main function will abort the process.
 WORD wVersionRequested;
 WSADATA wsaData;
 int err;
 // struct servent *srv;
 wVersionRequested = MAKEWORD(2,0);
 DWORD ThreadId;
 DWORD ServerPortNum;
 int retries;

 ServerInit=false;

 // Do WINSOCK initialization. Give WINSOCK a chance to initialize
 for (err=1, retries=0; err != 0 && retries < 20; ++retries)
  if ( (err = WSAStartup(wVersionRequested, &wsaData)) != 0 )
    Sleep(3000);
 if (err != 0)
  {
   LogNetworkError("WSAStartup","ServerApp",err);
   return;
  }

 // Get the PORT I am supposed to Service
  // Get Filenames from the REGISTRY
 if ( ! IPCEnumRegistry(HKEY_LOCAL_MACHINE,"SOFTWARE\\NetRoute",true,
                        "ServerPort",sizeof(ServerPortNum),&ServerPortNum) )
  {
   MessageBox(0,"ERROR READING REGISTRY","INITIALIZATION ERROR",
              MB_ICONERROR|MB_OK);
   return;
  }

/* If using SERVICES file instead of REGISTRY
 srv=getservbyname("ipcrouter","udp");
 if ( srv==NULL )
  {
   LogNetworkError("getservbyname","ServerApp Init",
                    WSAGetLastError());
   return;
  }
 IPCPort = srv->s_port;
*/
 IPCPort = ntohs( (WORD) ServerPortNum);

 // Create my SEND Socket
 if ( (SendSocket=socket(AF_INET,SOCK_DGRAM,0)) == INVALID_SOCKET )
  {
	   LogNetworkError("socket","ServerApp Init",WSAGetLastError());
   return;
  }

 // The SEND socket uses the unique system generated port number.
 // We don't need to bind to a port.
 // We're Now Ready to SEND so we can start the RECV thread.
 if ( (ThreadHandle=CreateThread(0,0,
    (LPTHREAD_START_ROUTINE)RecvTask,this,0,&ThreadId)) == NULL )
  {
   LogNetworkError("CreateThread","ServerApp Init",
                      GetLastError());
   return;
  }

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
   CloseHandle(ThreadHandle);
   // Do the Shutdown stuff here. You don't want to do it if your
   // initialization failed.
   ServerInit=false;
  }
 WSACleanup();
}

//--------------------------------------------------------------------
// OutBound Messages Come Here.
//--------------------------------------------------------------------
void ServerApp::DoApplicationMsg()
{
 IPCROUTEINFO* rtinfo=Msg.MsgRouteInfo();
 struct sockaddr_in srv_addr;
 int    Length;
 //logg(__LINE__, "NtRte ", "DoApplicationMsg",0,0 );

 // Check for a possible Reroute unless the NOReroute flag is
 // set or it's a message being returned to the originator.
 if ( ! (rtinfo->ReturnInfo.Flags & RETINFO_NOREROUTE) &&
      ! (rtinfo->ReturnInfo.Flags & RETINFO_REPLY) )
  SearchReroute(rtinfo->DestComputer);

 /* Set up the server address packet */
 memset(&srv_addr,0,sizeof(srv_addr));
 srv_addr.sin_family = AF_INET;
 srv_addr.sin_port = IPCPort;
 if ( ! FetchAddress(rtinfo->DestComputer,
                     srv_addr.sin_addr.s_addr) )
  return;

 // Send the Message
 Length = Msg.GetMsgLength();
// MAB
 char MsgText[128];
 int  len=sizeof(MsgText);
 Msg.PutFld(IPCFN_DEBUG, "TRACE");
// ### Use fromaddr Struct
// Get the COMMAND
   int ii = Msg.GetFld(IPCFN_DEBUG,len,MsgText);

 //llogg(__LINE__, "NtRte Send ", rtinfo->SourceProcess,rtinfo->SourceComputer,
 //    rtinfo->DestProcess,  rtinfo->DestComputer,Length,0);

 if ( sendto(SendSocket,(char*)rtinfo,Length,
        0,(struct sockaddr*)&srv_addr,sizeof(srv_addr)) !=
          Length )
  {
  LogNetworkError("sendto","DoApplicationMsg",WSAGetLastError());
  //logg(__LINE__, "NtRte ", "SendTo Error",0,0 );
  }
  //logg(__LINE__, "NtRte ", "SendTo End",0,0 );
}

//---------------------------------------------------------------------
// ADMIN Messsage Support
//---------------------------------------------------------------------
bool ServerApp::DoAdminMsg()
{
 char MsgText[255];
 int  len=sizeof(MsgText);
 char *Command;

 // The field will always be there
 Msg.GetFld(IPCFN_ADMINMESSAGE,len,MsgText);

 // Catch the HELP command and insert your own Command list. The
 // Default Handler will append the default commands to your list.
 if ( memcmp(MsgText,"HELP",4) == 0 )
  {
   strcat(MsgText,", REROUTE");
   Msg.PutFld(IPCFN_ADMINMESSAGE,MsgText);
   return false;
  }

 // Handle the REROUTE command
 if ( (Command=strtok(MsgText," ")) != NULL &&
      stricmp(Command,"REROUTE") == 0 )
  {
   char *OldName;
   char *NewName;

   if ( (OldName=strtok(NULL," ")) == NULL ||
        (NewName=strtok(NULL," ")) == NULL ||
        strlen(OldName) > MAX_IPCNAME ||
        strlen(NewName) > MAX_IPCNAME )
    return false;

   InsertReroute(OldName,NewName);
   strcpy(MsgText,"OKAY");
   Msg.PutFld(IPCFN_ADMINMESSAGE,MsgText);
   return true;
  }

 return false;
}


//-------------------------------------
// Log a Network Error to the Event Log
//-------------------------------------
void ServerApp::LogNetworkError(char* CalledFunc,
                                char* CallingFunc,
                                int ErrCode)
{
 char ErrCodeBuf[10];

 itoa(ErrCode,ErrCodeBuf,10);
 IPCLogEvent(SysCallError,0,CalledFunc,CallingFunc,
             ErrCodeBuf,SocStrerror(ErrCode));
}

IPAddress::IPAddress() :
  NumReroutes(0),
  NumCachedNames(0)
{
}  

//-------------------------------
// Fetch an IP Address for a HOST
//-------------------------------
bool IPAddress::FetchAddress(char* Name,DWORD& Address)
{
 int i;
 struct hostent* host;

 // First check the Cache
 for (i=0; i<NumCachedNames; ++i)
  if ( stricmp(Name,NameCache[i].Name) == 0 )
   {
    Address=NameCache[i].Address;
    return true;
   }

 // Look up Name and Insert into Cache
 host=gethostbyname(Name);
 if ( host==NULL )
  {
   char ErrorCodeBuf[10];
   int  ErrorCode=WSAGetLastError();

   itoa(ErrorCode,ErrorCodeBuf,10);
   IPCLogEvent(HostNotFound,0,Name,SocStrerror(ErrorCode),
               ErrorCodeBuf);
   return false;
  }

 // If the NameCache is full, delete the first entry and place
 // at bottom.
 if ( NumCachedNames == MAXCACHEDNAMES )
  {
   RemoveArrayItem(NameCache,NumCachedNames,sizeof(NameCache[0]),0);
   i = NumCachedNames -1;
  }
 else
  i = NumCachedNames++;

 strcpy(NameCache[i].Name,Name);
 memcpy(&NameCache[i].Address,host->h_addr,sizeof(DWORD));
 Address=NameCache[i].Address;
 return true;
}

//---------------------------------------
// Insert an entry into the Reroute table
//---------------------------------------
void IPAddress::InsertReroute(char* OldName,char* NewName)
{
 int i;

 // First see if it's already there
 for (i=0; i<NumReroutes; ++i)
  if ( stricmp(OldName,RerouteTbl[i].OldName) == 0 )
   {
    if ( stricmp(OldName,NewName) == 0 )
     {
      RemoveArrayItem(RerouteTbl,NumReroutes,sizeof(RerouteTbl[0]),
                     i);
      --NumReroutes;               
     }
    else
     strcpy(RerouteTbl[i].NewName,NewName);
    return;
   }

 // If the table is FULL, this is an error that we need to
 // log.
 if ( NumReroutes == MAXREROUTES )
  {
   IPCLogEvent(ResourceFull,0,"InsertReroute","RerouteTbl");
   RemoveArrayItem(RerouteTbl,NumReroutes,sizeof(RerouteTbl[0]),
                   0);
   i = NumReroutes-1;
  }
 else
   i = NumReroutes++;

 strcpy(RerouteTbl[i].OldName,OldName);
 strcpy(RerouteTbl[i].NewName,NewName);
}

//---------------------------------------------
// Search the ReRoute table for a matching name
//---------------------------------------------
bool IPAddress::SearchReroute(char* Name)
{
 int i;

 // First check the ReRoute Table to see if the Name has
 // been re-routed.
 for (i=0; i<NumReroutes; ++i)
  if ( stricmp(Name,RerouteTbl[i].OldName) == 0 )
   {
    strcpy(Name,RerouteTbl[i].NewName);
    return true;
   }

 return false;
}

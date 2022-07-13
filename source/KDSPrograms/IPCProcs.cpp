//-----------
// IPCProcs.C
//-----------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "ipcmsg.h"
#include "spslog.h"
#include "inetman.h"

extern SPSLogger Logger;
extern INETManager Inet;


//----------------------------------------------
// Send message via Socket interface (INET only)
//----------------------------------------------
bool IPCMessage::Send(char* HostName,char* ProcessName,bool Originate,
                      bool NoReRoute)
{
 struct sockaddr_in addr;
 IPCROUTEINFO* RouteInfo = (IPCROUTEINFO*)MsgData;

 if ( NoReRoute ||
      ! Inet.GetReRoute(HostName,RouteInfo->DestComputer) ) 
  strcpy(RouteInfo->DestComputer,HostName);

 strcpy(RouteInfo->DestProcess,ProcessName);
 if ( Originate )
  {
   strcpy(RouteInfo->SourceComputer,Inet.GetMyHostName());
   strcpy(RouteInfo->SourceProcess,"ANY");
  }

 memset(&addr,0,sizeof(addr));
 addr.sin_family = AF_INET;
 addr.sin_port = Inet.GetPort();
 if ( ! Inet.GetAddress(RouteInfo->DestComputer,
                                   &addr.sin_addr.s_addr) )
  {
   Logger.Log("HOST '%s' NOT RECOGNIZED",RouteInfo->DestComputer);
   return false;
  }
 
 if ( sendto(Inet.GetSocket(),MsgData,Length,
        0,(struct sockaddr*)&addr,sizeof(addr)) != Length )
  {
   Logger.IOError("sendto");
   return false;
  }
 else
   return true;
}

//---------------------------
// Receive message via Socket
//---------------------------
bool IPCMessage::Recv()
{
 struct sockaddr from;
 int fromlen=sizeof(from);

 if ( (Length=recvfrom(Inet.GetSocket(),MsgData,sizeof(MsgData),0,&from,&fromlen)) < 0 )
  {
   Logger.IOError("recvfrom");
   return false;
  }
  
 if ( ! ParseMsg() )
  {
   Logger.Log("Message Is Trash (INET)");
   return false; 
  }

 return true;
}

//--------------------------------------------------
// Return Message to Originator via socket interface
//--------------------------------------------------
bool IPCMessage::Reply()
{
 IPCROUTEINFO* Routeinfo = (IPCROUTEINFO*) MsgData;

 Routeinfo->ReturnInfo.Flags |= RETINFO_REPLY;
 return Send(Routeinfo->SourceComputer,Routeinfo->SourceProcess);
}

//-------------------------------
// Send message via Message Queue
//-------------------------------
bool IPCMessage::Send(int MsgQueue,int MsgType)
{
 mbuf.mtype = MsgType;
 if ( msgsnd(MsgQueue,(struct msgbuf*)&mbuf,Length,0) == -1 )
  {
   Logger.IOError("msgsnd");
   return false;
  }
 else
   return true;
}

//----------------------------------
// Receive message via message queue
//----------------------------------
bool IPCMessage::Recv(int MsgQueue,int MsgType)
{
 if ( (Length=msgrcv(MsgQueue,(struct msgbuf*)&mbuf,sizeof(MsgData),MsgType,0)) == -1 )
  {
   Logger.IOError("msgrcv");
   return false;
  }

 if ( ! ParseMsg() )
  {
   Logger.Log("Message Is Trash (Type=%d)",MsgType);
   return false;
  }

 return true;
}


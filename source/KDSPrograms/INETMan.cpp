//-----------
// NaManage.C
//-----------
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include "inetman.h"
#include "ipckeys.h"
#include "spslog.h"

extern SPSLogger Logger;

#define REROUTETBLSIZE 100

//------------------
// Class Constructor
//------------------
INETManager::INETManager(bool BindSocket,int nameCacheSize) :
  NameCacheSize(nameCacheSize),
  InitOkay(true),
  NameCacheCount(0)
{
 int Id;
 struct servent* Serv=getservbyname("SPSINET",NULL);
 int bufSize=8200;

 // Create the Name Cache
 NameCache = new NAMECACHE[NameCacheSize];

 // Get shared memory segment for REROUTE table
 if ( (Id=shmget(IPCKEY_REROUTETBL,REROUTETBLSIZE*sizeof(ReRouteTbl->Item)+
          sizeof(*ReRouteTbl)-sizeof(ReRouteTbl->Item),IPC_CREAT)) == -1 )
  {
   Logger.IOError("shmget","ReRoute Table");
   ReRouteTbl=NULL;
   InitOkay=false;
  }
 // Attach shared memory segment
 if ( (ReRouteTbl=(REROUTETBL*)shmat(Id,0,0)) == (void*)-1 )
  {
   Logger.IOError("shmat","ReRoute Table");
   ReRouteTbl=NULL;
   InitOkay=false;
  }

 // Create an INET DGRAM socket
 if ( (SockFd=socket(AF_INET,SOCK_DGRAM,0)) == -1 )
  {
   Logger.IOError("socket");
   InitOkay=false;
  }

 // Set Buffer size on Socket
 if ( (setsockopt(SockFd,SOL_SOCKET,SO_SNDBUF,&bufSize,sizeof(bufSize))) == -1 )
   Logger.IOError("setsockopt");

 // Get the SPSINET port number
 if ( Serv==NULL )
  {
   Logger.Log("No entry for SPSINET in /etc/services FILE");
   InitOkay=false;
  }
 else
  {
   Port=Serv->s_port;
   if ( BindSocket )
    {
     struct sockaddr_in addr;
     memset(&addr,0,sizeof(addr));
     addr.sin_family = AF_INET;
     addr.sin_addr.s_addr = htonl (INADDR_ANY) ;
     addr.sin_port =  Port;
     if ( bind(SockFd,(struct sockaddr *)&addr,sizeof(addr)) == -1 )
      {
       Logger.IOError("Bind");
       InitOkay=false;
      }
    }
  } 

 gethostname(MyHostName,sizeof(MyHostName)); 
}

//-----------------
// Class Destructor
//-----------------
INETManager::~INETManager()
{
 delete[] NameCache;
}

//-----------------
// Get an IPAddress
//-----------------
bool INETManager::GetAddress(char* HostName,void* Address)
{
 int i;
 struct hostent* Name;

 // First check the Name Cache
 for (i=0; i<NameCacheCount; ++i)
  if ( strcmp(HostName,NameCache[i].HostName) == 0 )
   {  
    memcpy(Address,&NameCache[i].Address,sizeof(NameCache[i].Address));
    return true;
   }

 // Lookup the host name
 if ( (Name=gethostbyname(HostName)) == NULL )
   return false;

 // If the Cache is full, clear it and start over
 if ( NameCacheCount == NameCacheSize )
   NameCacheCount=i=0;
 memcpy(&NameCache[i].Address,Name->h_addr_list[0],sizeof(DWORD));
 memcpy(Address,Name->h_addr_list[0],sizeof(DWORD));
 ++NameCacheCount;
 return true;
}

//--------------------------
// Check for a ReRoute entry
//--------------------------
bool INETManager::GetReRoute(char *HostName,char* AltName)
{
 int i;

 if ( ReRouteTbl==NULL )
  return false;

 for (i=0; i<ReRouteTbl->Count; ++i)
  if ( strcmp(HostName,ReRouteTbl->Item[i].HostName) == 0 )
    {
     strcpy(AltName,ReRouteTbl->Item[i].AltName);
     return true;
    }

 return false;
}

//-----------------------
// Insert a ReRoute entry
//-----------------------
bool INETManager::InsReRoute(char* HostName,char* AltName)
{
 int i;

 // Make sure I have a table
 if ( ReRouteTbl==NULL )
  {
   Logger.Log("ATTEMPT TO ACCESS NON-EXISTENT REROUTE TABLE");
   return false;
  }

 // Look for host already present
 for (i=0; i<ReRouteTbl->Count; ++i)
  if ( strcmp(HostName,ReRouteTbl->Item[i].HostName) == 0 )
   {
    // If name's same or AltName is NULL, this is cancel reroute
    if ( AltName==NULL ||
         strcmp(HostName,AltName) == 0 )
     {
      --ReRouteTbl->Count;
      memcpy(&ReRouteTbl->Item[i],&ReRouteTbl->Item[i+1],
             (ReRouteTbl->Count-i) * sizeof(ReRouteTbl->Item));
     }
    else
     strcpy(ReRouteTbl->Item[i].AltName,AltName); 

    return true;
   }
  
 // Insert at end unless it's a cancel reroute
 if ( AltName==NULL ||
      strcmp(HostName,AltName) == 0 )
   return true;  

 if ( ReRouteTbl->Count >=  REROUTETBLSIZE )
  {
   Logger.Log("THE REROUTE TABLE IS FULL!!");
   return false;
  }

 i=ReRouteTbl->Count++;
 strcpy(ReRouteTbl->Item[i].HostName,HostName);
 strcpy(ReRouteTbl->Item[i].AltName,AltName); 
 return true;
}


//-------------------
// Clear All ReRoutes
//-------------------
void INETManager::ClearAllReRoutes(void)
{
 if ( ReRouteTbl != NULL )
  ReRouteTbl->Count = 0;
}
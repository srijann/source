#ifndef __INETMAN_H
#define __INETMAN_H

#include "defs.h"
#include "ipcmsg.h"

#define BINDSOCK 1

class INETManager {
  int NameCacheSize;
  int NameCacheCount; 
  typedef struct {
    char HostName[MAX_IPCNAME+1];
    DWORD Address;
   } NAMECACHE;

  NAMECACHE* NameCache;
  char MyHostName[MAX_IPCNAME+1];

  // All processes share a common reroute table
  typedef struct
   {
    int Count;
    struct
     {
      char HostName[MAX_IPCNAME+1];
      char AltName[MAX_IPCNAME+1]; 
     } Item[1];
   } REROUTETBL;
  REROUTETBL* ReRouteTbl;

  int SockFd;
  unsigned short Port;
  int InitOkay;

  public:
  INETManager(bool BindSocket=false,int nameCacheSize=25);
  ~INETManager();
  bool GetAddress(char* HostName,void* Address);
  bool GetReRoute(char *HostName,char* AltName);
  bool InsReRoute(char* HostName,char* AltName);
  void ClearAllReRoutes();
  int GetSocket() { return SockFd; };
  unsigned short GetPort() { return Port; };
  bool IsOkay() { return InitOkay; };
  char* GetMyHostName() { return MyHostName; };
 };

#endif
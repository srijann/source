#ifndef __SERVERAPP_H
#define __SERVERAPP_H
#include "ipc.h"
#include "ipcmsg.h"
#include "messages.h"

class ServerAppInfo {
  public:
  ServerAppInfo();
  char ServerProcessName[MAX_IPCNAME+1];
  char ProgramVersionInfo[10];
 };

#define MAXCACHEDNAMES 100
#define MAXREROUTES    100
class IPAddress {
   WORD NumCachedNames;
   struct
    {
     char  Name[MAX_IPCNAME+1];
     DWORD Address;
    } NameCache[MAXCACHEDNAMES];

   public:

   WORD NumReroutes;
   void InsertReroute(char* OldName,char* NewName);
   bool SearchReroute(char* Name);

   struct
    {
     char OldName[MAX_IPCNAME+1];
     char NewName[MAX_IPCNAME+1];
    } RerouteTbl[MAXREROUTES];

   IPAddress(); 
   bool FetchAddress(char* Name,DWORD& Address);
  };

class ServerApp : public ServerAppInfo, IPAddress {
  SOCKET SendSocket;
  HANDLE ThreadHandle; 

  public:
  short  IPCPort;
  ServerApp();
  ~ServerApp();
  bool ServerInit;
  void DoApplicationMsg();
  bool DoAdminMsg();
  IPCMessage Msg;
  void LogNetworkError(char* CalledFunc,char* CallingFunc,
                       int ErrCode);
 };
#endif


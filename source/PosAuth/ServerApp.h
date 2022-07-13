#ifndef __SERVERAPP_H
#define __SERVERAPP_H
#include "ipc.h"
#include "ipcmsg.h"
#include "messages.h"
#define CHECKAUTHORIZATION
#define NOFLDNAMES
#include "CheckAuth.h"

class ServerAppInfo {
  public:
  ServerAppInfo();
  char ServerProcessName[MAX_IPCNAME+1];
  char ProgramVersionInfo[10];
 };

class ServerApp : public ServerAppInfo {
  public:
  ServerApp();
  ~ServerApp();
  bool ServerInit;
  void DoApplicationMsg();
  bool DoAdminMsg();
  CheckAuth Msg;
 };
#endif


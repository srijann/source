#ifndef __SERVERAPP_H
#define __SERVERAPP_H
#define  STRICT
#include "Win4me.h"
#include "messages.h"
#include "dbdefs.h"
#include "rds.h"
#include "PosFile.h"

#define BADDATA(type)\
  IPCLogEvent(BadData,Msg.GetMsgPointer(),Msg.GetMsgLength(),0,type);

class ServerAppInfo {
  public:
  ServerAppInfo();
  char ServerProcessName[MAX_IPCNAME+1];
  char ProgramVersionInfo[10];
 };

class ServerApp : public ServerAppInfo {
  RDM_SESS SessionHandle;
  RDM_DB   DBHandle;
  char  VelocisErrorBuf[1024];
  void  DBError(char* Source,int Line,int Status,char *Function,
                int RecType=-1, int KeyField=-1);

  void getRecord();
  void addRecord();
  void changeRecord();
  void deleteRecord();
  void update();
  void errorReply(unsigned short Response,char* Text);
  unsigned long nowDate();
  public:
  ServerApp();
  ~ServerApp();
  bool ServerInit;
  void DoApplicationMsg();
  bool DoAdminMsg();
  IPCMessage Msg;
 };
#endif


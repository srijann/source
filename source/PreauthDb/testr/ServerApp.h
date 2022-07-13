#ifndef __SERVERAPP_H
#define __SERVERAPP_H
#define  STRICT
#include "Win4me.h"
#include "messages.h"
#include "dbdefs.h"
#include "rds.h"
#include "PreauthDB.h"

#define BADDATA(type)\
  IPCLogEvent(BadData,Msg.GetMsgPointer(),Msg.GetMsgLength(),0,type);
#define CHECKAUTHDEST "DBMANCHKAUTH"

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
  public:
  ServerApp();
  ~ServerApp();
  bool ServerInit;
  void DoApplicationMsg();
  bool DoAdminMsg();
  void doPreauth();
  void doPostAuth();
  void getPreauths();
  void forcePreauth();
  int  fetchALLPreauths();
  bool getID(char* Buf);
  bool isExpired(PreauthRecord* Rec);
  bool doPurge();
  bool doWrite();
  IPCMessage Msg;

short m_fillnew(PreauthRecord* Rec);
short m_recread1(PreauthRecord* Rec, DWORD Amount, char *MerchantID, char *ID);
int m_recreadAuthID(char *AuthID, PreauthRecord* Rec);
// Really, only Postauth Time gets updated
int m_UpdateTimeOfPostauth(PreauthRecord* Rec, long lTime);
int m_delete(PreauthRecord* Rec);
 };
#endif


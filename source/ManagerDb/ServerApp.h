#ifndef __SERVERAPP_H
#define __SERVERAPP_H
#define  STRICT
#include "Win4me.h"
#include "messages.h"
#include "dbdefs.h"
#define WANTFLDNAMES
#include "Dbmanrec.h"
#include "CheckAuth.h"
#include "rds.h"

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
  char  ProdClassFilename[MAX_PATH+1];
  bool  InitLists();
  bool  InitAuthRules();
  bool  InitDataReq();
  bool  InitUnvFundsRules();
  bool  InitPosResponse();
  bool  InitPositiveRules();
  bool  LogError(char *IOType,char *FileName);
  void  GetAuthRules();
  void  PutAuthRules();
  void  GetList();
  void  PutList();
  void  GetAuthRulesList();
  void  GetUnvFundsRules();
  void  GetPositiveRules();
  void  PutUnvFundsRules();
  void  PutPositiveRules();
  void  GetDataReq();
  void  PutDataReq();
  void  GetResponseRec();
  void  PutResponseRec();
  void  GetResponseText();
  void  GetMerchantRec();
  void  PutMerchantRec();
  void  CheckAuthorization();
  void  FetchAuthRules(MerchantRec& merchRec);
  void  DBReply(WORD ResponseCode,DBTIMESTAMP TimeStamp=0,
                const char* Text=0);
  bool  GetRecord(FLDNUM Fn,void *Key,DBManRecord& Rec,
                  bool* NotFound=0,bool useAltKey=false);
  bool  PutRecord(FLDNUM Fn,DBManRecord& Rec,bool& IsNew,
                  char* AltKeyMsg=0);
  void  DBError(char* Source,int Line,int Status,char *Function,
                int RecType=-1, int KeyField=-1);
  void LogMissingClass(char* MerchantID,char* Type,
                       char* Class,char* SubClass=0);

  public:
  ServerApp();
  ~ServerApp();
  bool ServerInit;
  void DoApplicationMsg();
  bool DoAdminMsg();
  IPCMessage Msg;
  CheckAuth checkMsg;
 };
#endif


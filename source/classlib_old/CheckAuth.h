#ifndef __CHKAUTHMSG_H
#define __CHKAUTHMSG_H

#include "DBManrec.h"
#include "kwrec.h"
#include "CheckAuthFn.h"
#include "MerchantRec.h"
#include "UnvFundsRules.h"
#include "DataReq.h"
#include "AuthRules.h"
#include "ipc.h"
#define NOFLDNAMES
#include "PosFile.h"
#include "RcodeInfo.h"
#include <time.h>

// MAB072302
// Display Posfile values in VoiceAuth
//    Date of first check
//    Data of most recent check
//    Total number of checks from consumer
//    Total amount of checks from consumer
// Do by adding 4 unsigned values to the Class definition to match
// value in positive file.
// in classlib\checkauth.h


class CheckAuth : public IPCMessage {
  char emptyString[1];
  char qmarkString[2];

  time_t NowTs;
  struct tm Now;

  void putRcode(BYTE Rcode);
  bool maxOverrides();

  WORD NetTimeout;
  bool NetUIRetry;

  public:
  CheckAuth();
  ~CheckAuth();

  void setNetUIRetry(bool val=true) { NetUIRetry=val; };
  void setNetTimeout(WORD val) { NetTimeout=val; };

  FLDNUM* OverrideFields;
  uint NumOverrideFields;
  uint MaxOverrideFields;

  enum
   {
    Pos,
    Voice,
    Security
   };

  bool  isGuarantee();
  bool  isVerify();
  bool  isPreauthMerchant();
  bool  isHoldCheckMerchant();
  bool  isVoice();
  FLDNUM needEmployerData(int Type);
  FLDNUM needSpouseCoSignerData(int Type);
  DWORD CheckAmount();
  bool  ProductInfo(uint& Group,uint& SubGroup);
  int   ConsumerAge();
  DWORD CheckNumber();
  bool  ScoreTransaction(int& Score);

  void initClassMembers();
  MerchantRec       merchantRec;
  UnvFundsRules      scoringRules;
  DataRequirements  dataReq;
  AuthRules         authRules;
  RcodeInfo         RCInfo;         
  bool   hasMerchantRec;
  bool   hasScoringRules;
  bool   hasDataReq;
  bool   hasAuthRules;
  bool   hasPositiveFileRecord;
  ACTIVITY* hasActivity(int* ActivityCount=0);
  struct PosfileRecord PosFileRecord;
  char*  MerchantName();
  char*  MerchantID();
  void   CopyMsg(IPCMessage*);
  void   Reset();

  bool   getMerchantData(char* MerchantID=0);
  bool   getActivityData();
  bool   getReferralData();
  bool   doRules();
  WORD   GenerateApprovalNumber(BYTE Rcode=0);
  BYTE   QueueMsg();
  bool   getRcodes(BYTE& Decline,BYTE& Approve);
  bool   isBadChecks();
  void   doPosReply(BYTE Rcode,const char* Text=0);
  BYTE   getApprovalRcode();
  IPCMessage SyncMsgBuf;
  bool doR1();
  bool doR2();
  bool doR3();
  bool doR4();
  bool doR5();
  bool doR6();
  bool doR7();
  bool doR8();
  bool isCheckInRegion();
  bool isPositive();
  void PosFileUpdate();
  int  numRcodesBreached();
  bool getPosfileLimits(uint& OverridePeriod,
                        uint& MaxOverrides,
                        uint& MaxAmount,
                        uint& MaxAccum);

  bool CountActivity(uint Days,uint CountableAmount,
                     uint NonCountableAccumLimit,
                     uint& NumTrans,uint& Amount,
                     char* Account=0,char* Station=0,char* Sic=0,
                     bool flagActivity=false);

  // Field Validation Module
  bool   fieldValidation(int Type,FLDNUM &Fn,bool& isMissing,BYTE& Rcode);
  bool   isValidCheckType(int fn=-1);
  bool   isValidLicense(int statefn=-1,int licfn=-1);
  bool   isValidState(int fn=-1);
  bool   isValidDob(int fn=-1);
  bool   isValidCheckNumber(int fn=-1);
  bool   isValidABA(int fn=-1);
  bool   isValidAccount(int fn=-1);
  bool   isValidName(int fn=-1);
  bool   isValidAddress(int fn=-1);
  bool   isValidResidenceType(int fn=-1);
  bool   isValidCity(int fn=-1);
  bool   isValidConsumerState(int fn=-1);
  bool   isValidZipCode(int fn=-1);
  bool   isValidPhone(int fn=-1);
  bool   isValidSSN(int fn=-1);
  bool   isValidProductClass(int fn=-1);
  bool   isValidTransportMethod(int fn=-1);
  bool   isValidBusinessName(int fn=-1);
  bool   isValidPrivateLabel(int fn=-1);
  bool   isValidTitle(int fn=-1);
  bool   isValidAmount(int fn=-1);
  bool   isValidMerchantID(int fn=-1);
  void   OverrideFieldValidation(FLDNUM);
  bool   isOverrideField(FLDNUM);
  BYTE   doPostAuth();
  BYTE   doPreAuth();
  BYTE   forcePreAuth();
  bool   isPreauth(bool set=false);
  char   EditBuf[100];  // A buffer for editing field data
  int    EditLen;

  // MAB72302 - these match the variables in class Posfile
  unsigned long  LastApproval;
  unsigned long  FirstApproval;
  unsigned short NumApprovals;
  unsigned long  AmountApprovals;

  // MAB060999  #if DO_BANK_ACCOUNT_X_BANK_NUMBER
  // MAB082900  #if DO_BANK_ACCOUNT_X_BANK_NUMBER
  //int iReturnErrorFromRoutingAccountLength;

  // This makes sure that the relationship between the Bank Number and the
  // Bank Account is validated.
//  bool   isValidBankAccountAndBankNumber(int fn=-1);

  char gszBankNumber[16];

//  bool   isValidBankAccountAndBankNumber();
  int    isValidTableBankNumber(char* pszBankNumber, char *pszBankAccount);

  // MAB122899
bool CheckAuth::isValidTableCreditCardMicr(int fn=-1);
int CheckAuth::isThisACreditCardBankMicr(char *pszBankNumber,
                                         char *pszBankAccount);


//  bool   isValidBankAccountAndBankNumber(int fn=-1);
//  int isThisACreditCardBankMicr(char* pszBankNumber,char *pszBankAccount);
//  int isValidTableCreditCardBankMicr(char* pszBankNumber, char *pszBankAccount);

 };
#endif

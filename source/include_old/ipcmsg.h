#ifndef __IPCMSG_H
#define __IPCMSG_H

#ifndef __DEFS_H
#include "defs.h"
#endif

#ifdef WINDOWSNT
typedef HWND MSGQ;
#endif

typedef unsigned char FLDNUM;
typedef unsigned char FLDTYPE;

#define MAX_MSG    8192
#define MAX_FIELDS 256
#define IPCMessageTimedOut         2
#define IPCMessageReceived         1
#define IPCNotifyMessage "IPCNOTIFY"
#define MAX_IPCNAME 8

// The Message Types
#define FLDTYPE_BYTE   (FLDTYPE)0
#define FLDTYPE_WORD   (FLDTYPE)1
#define FLDTYPE_DWORD  (FLDTYPE)2
#define FLDTYPE_STRING (FLDTYPE)3
#define FLDTYPE_VLEN1  (FLDTYPE)4
#define FLDTYPE_VLEN2  (FLDTYPE)5

// This structure containing Routing Information is at the beginning
// of every message.

// Macros for testing/setting Flags bits
#define RETINFO_REPLY 0x01
#define RETINFO_TIMER 0x02
#define RETINFO_NOREROUTE 0x04

#ifdef SCOUNIX
#pragma pack(1)
#endif
typedef struct
 {
  char  SourceProcess[MAX_IPCNAME+1];
  char  SourceComputer[MAX_IPCNAME+1];
  char  DestProcess[MAX_IPCNAME+1];
  char  DestComputer[MAX_IPCNAME+1];
  struct
   {
    WORD  SyncId;
    DWORD LocalInfo;
    WORD  Flags;
   } ReturnInfo;
 } IPCROUTEINFO;

class FUNCTYPE IPCMessage {

 public:
  IPCMessage();
  IPCMessage::IPCMessage(void*,WORD,bool&);
  IPCMessage* IPCMsgPtr() { return this; };
  ~IPCMessage();
  // Put a Null-Terminated String
  bool PutFld(FLDNUM,char* );
  // Put a short integer
  bool PutFld(FLDNUM,WORD);
  // Put an integer
  bool PutFld(FLDNUM,DWORD);
  // Put a variable length field
  bool PutFld(FLDNUM,void* ,int);
  // Put a byte
  bool PutFld(FLDNUM,BYTE);
  // Test the put_fail flag and optionally clear it
  bool PutFldFail(bool clear=false);

  // Query Presence of Field only
  bool GetFld(FLDNUM);
  // Get a Null-Terminated String
  bool GetFld(FLDNUM,int,char*);
  // Get a short integer
  bool GetFld(FLDNUM,WORD &);
  // Get a long integer
  bool GetFld(FLDNUM,DWORD &);
  // Get a variable length field
  bool GetFld(FLDNUM,void* ,int &);
  // Get a byte
  bool GetFld(FLDNUM,BYTE &);
  // Get a pointer to a field in the message (Be careful here!)
  void* GetFldPtr(FLDNUM,int* Len=0,FLDTYPE* Type=0);
  // Get Information About a Field
  bool GetFldInfo(FLDNUM,FLDTYPE&,int&);
  bool GetFldInfo(FLDNUM,FLDTYPE&);
  bool GetFldInfo(FLDNUM);
  int  GetFldLen(FLDNUM);
  // Delete a field
  void DelFld(FLDNUM);
  // Copy the message to another buffer
  void  CopyMsgOut(void*,WORD&);
  void  CopyMsgOut(void* );
  // Copy a Message Into an object and Initialize it
  bool  CopyMsgIn(void*,WORD);
  // Copy a Message into another Message
  void  CopyMsgIn(IPCMessage*);
  // Copy a field from another message
  bool  CopyFld(FLDNUM,IPCMessage*);
  // Copy a field from one FLDNUM to ANOTHER (optionally from a
  // different message
  bool  CopyFld(FLDNUM,FLDNUM,IPCMessage* Msg=0);

  // Clear All Fields in a Message
  void  ClearMsg();

  // Get the maximum size of field that can fit in the message
  int   GetMaxField();

  int   GetMsgLength() { return Length; };
  void  SetMsgLength(WORD n) { Length=n; };
  void* GetMsgPointer() { return MsgData; };
  IPCROUTEINFO* MsgRouteInfo() { return (IPCROUTEINFO*) MsgData; };

  // Interface to IPC I/O procs
  #ifdef WINDOWSNT
  // I want to make these functions virtual but for some
  // reason I get GPF's
  bool SendMsg(char*);
  bool SendMsg(char*,MSGQ,WORD TimeOut=0);
  bool SendRecv(char*,WORD TimeOut,bool UIRetry=false,
                        int Retries=0,char *AltDest=0);
  bool RecvMsg(LPARAM);
  bool RecvReply(LPARAM);
  bool ReplyMsg();
  bool GetMsgSyncId(WORD&);
  void ResetMsgSyncId();
  void IPCDestroy(MSGQ);
  #endif

  bool ParseMsg();
  

 private:

   bool   PutFailFlag;
   WORD   Length;
   WORD   OffsetTbl[MAX_FIELDS];
   BYTE   MsgData[MAX_MSG];

  // Nobody should need any of these
  bool   putfld(FLDNUM,BYTE,void* ,int);
  void  *getfld(FLDNUM);
  bool   vlen_is_binary(BYTE len,char* fld,DWORD &bin);
 };

#endif
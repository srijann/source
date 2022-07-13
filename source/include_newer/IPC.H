#ifndef __IPC_H
#define __IPC_H

// #include "defs.h" MAB061299
#ifndef __DEFS_H
#include "defs.h"
#endif


// These Limits are set by the SYSTEM
#define MAX_DOMAIN_NAME_LEN      40
#define MAX_USERNAME_LEN         40
#define MAX_EVENTLOG_SOURCE_NAME 40

// This is the Maximum length of either a Computer Name or a Process
// Name
#define MAX_IPCNAME          8
// We allow Aliases names to be bigger
#define MAX_IPCLOGICALNAME  30
// This is the Maximum length of a Remote Process name which is in
// the form ComputerName.ProcessName
#define MAX_REMOTEIPCNAME   (MAX_IPCNAME*2+1)

// The LAST TWO FIELD NUMBERS OF ALL IPC Messages are RESERVED for
// Internal Use by the IPC Library
#define IPCFN_ADMINMESSAGE 254
#define IPCFN_DEBUG 255

#include "ipcmsg.h"
#include "ipcserver.h"

// Must call the INIT function before doing IPC. Use this form for a
// Server.
void FUNCTYPE IPCInit(char*,MSGQ);
// Use this form if you want Synchronous I/O
void FUNCTYPE IPCSyncIOInit(char*);
// Use this form for a client
void FUNCTYPE IPCInit(char*);
// Register the Process as a Server
bool FUNCTYPE IPCRegister(char* ProcessName=0,MSGQ Queue=0);
// DeRegister the Server Process
bool FUNCTYPE IPCDeRegister(char* ProcessName=0);
// Get a Point to the Computer Name
char* FUNCTYPE IPCMyComputerName(void);
// Send an IPC message, expecting no reply
bool FUNCTYPE IPCSend(IPCMessage*,char*);
// Send an IPC message, expecting a reply back to the client window
bool FUNCTYPE IPCSend(IPCMessage*,char*,HWND,WORD TimeOut=0);
// Send an IPC message using Synchronous I/O
bool FUNCTYPE IPCSend(IPCMessage*,char*,WORD TimeOut,bool RetryUI=false,
                      int retries=0,char* AltDestName=0);
// Receive an IPC Message
bool FUNCTYPE IPCRecv(IPCMessage*,LPARAM);
// Receive an IPC Message Reply
bool FUNCTYPE IPCRecvReply(IPCMessage*,LPARAM);
// Reply to a Message
bool FUNCTYPE IPCReply(IPCMessage*);
// Query a Message for the SYNCID
bool FUNCTYPE IPCGetSyncId(IPCMessage*,WORD&);
// Reset the SYNCID (set to zero)
void FUNCTYPE IPCResetSyncId(IPCMessage*);
// Purge Messages From the Message Queue
void FUNCTYPE IPCPurgeMessageQueue(MSGQ);
// Count Messages In the Message Queue
int FUNCTYPE IPCCountMessageQueue(MSGQ);
// Log an Event
void FUNCTYPE IPCLogEvent(DWORD msgnum,WORD category=0,
                          char* str1=0, char* str2=0,char* str3=0,
                          char* str4=0, char* str5=0,char* str6=0,
                          char* str7=0, char* str8=0);
// Event Logging with binary dump and up to 8 strings
void FUNCTYPE IPCLogEvent(DWORD msgnum,void* buf,int buflen,
                          WORD category=0,
                          char* str1=0,char* str2=0,char* str3=0,
                          char* str4=0, char* str5=0,char* str6=0,
                          char* str7=0, char* str8=0);
// Function to log an error to a system function
void FUNCTYPE IPCLogSystemError(char* called_funcname,
                                char* calling_funcname,
                                int errcode);
// Function to Enum the Registry for up to 5 Values
bool FUNCTYPE IPCEnumRegistry(HKEY,char*,bool,
                              char*, int, void*,
                              char* name2=0, int len2=0, void* v2=0,
                              char* name3=0, int len3=0, void* v3=0,
                              char* name4=0, int len4=0, void* v4=0,
                              char* name5=0, int len5=0, void* v5=0);
// The IPCNotify Message Number
UINT FUNCTYPE IPCGetNotifyMessageNumber(void);
#endif

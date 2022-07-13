//----------------------------------------------------------------------
// IPCLIB
//----------------------------------------------------------------------

#define STRICT
#include <windows.h>
#include <winsys/registry.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "messages.h"
#include "defs.h"
#include "ipc.h"
#include "splib.h"

#define DEFAULT_LOG_SOURCE "SPS"
#define DEFAULT_LOG_SERVER NULL
#define MAX_SID 200

struct SharedMemDef
 {
  bool   InitOkay;                // Initialization OKAY flag
  WORD   NumBufs;                 // # of I/O Buffers
  BYTE   CacheSyncId;             // Cache Synchronization flag
  UINT   IPCNotifyMessageNumber;  // The registered IPC Message #
  char   ComputerName[MAX_IPCNAME+1];
 } *SharedMem;

// This is the Buffer Pool structure. It follows immediately after the
// Destination Table   
typedef struct
 {
  MSGQ Queue;          // Window Queued To
  IPCMessage Msg;      // The Message Object
 } BUFPOOL;

static BUFPOOL* BufPool;
static HANDLE hMem;    // this is the Shared Memory Handle
static HANDLE hBuf;    // this is the Buffer Management Semaphore
static char   MyProcessName[MAX_IPCNAME+1];
static MSGQ   MyMessageQueue;
static char   UserName[MAX_USERNAME_LEN+1];
static char   Sid[MAX_SID];
static char   EventLogServerName[MAX_IPCNAME+1];
static char   EventLogSourceName[40] = "APPLICATION";
static HKEY   IPCPhysicalNameKey;
static HKEY   IPCLogicalNameKey;
static WORD   NextSyncId;
static HANDLE SyncIOThreadHandle;
static HWND   SyncIODummyWindow;
static HANDLE SyncIOSemaphore;
static IPCMessage* SyncIOMsg;
static bool   SyncIOInProgress=false;

// This is a Name Cache used to store AlIASES and local Server
// Names. It is dynamically allocated and expanded in the increment
// defined below.
#define MAX_NAMECACHE 50
static BYTE NameCacheSyncId;
static BYTE NameCacheCount;
static struct
 {
  // The SourceName can be either a physical process name or an
  // Alias name.
  char Name[MAX_IPCLOGICALNAME+1];
  // If SourceName maps to a local server, we store the MSGQ ID
  // here. If Remote, this value will be zero.
  MSGQ LocalQueue;
  // If Remote, this is the remote Info
  char DestProcessName[MAX_IPCNAME+1];
  char DestComputerName[MAX_IPCNAME+1];
 } NameCache[MAX_NAMECACHE];


// This defines the length of an IPC Message object excluding unused
// bytes of the MsgData.
#define MSGOBJSIZE(msg) (sizeof(IPCMessage)-MAX_MSG+(msg)->GetMsgLength())

// Static Function Prototypes
static WORD _EventType(DWORD);
static void _InitFail(void);
static int  _LocateProcess(char*,bool LogEvent=true);
static int  _CacheInsert(char*,MSGQ,char*,char*);
static bool _Send(IPCMessage*,MSGQ);
static bool _IsValidBufnum(LPARAM);
static bool _IsFullProcessName(char*,char*,char*);
static bool _SearchRegistry(HKEY Key,char* Name,void* Data,
                            DWORD DataSize);
static DWORD SyncIOTask(void*);
static bool _SyncSend(char* DestName,int TimeOut,int Retries);


//-----------------------------
// Initialize the IPC Interface
//-----------------------------
void IPCInit(char* ProcessName)
{
 DWORD NumBufs;
 DWORD size;
 DWORD DomainLen;
 char  DomainName[MAX_DOMAIN_NAME_LEN+1];
 SID_NAME_USE SidType;
 DWORD SidLen;
 LONG  ret;

 // Save the Process Name for Logging Purposes
 memcpy(MyProcessName,ProcessName,min(strlen(ProcessName),
        sizeof(MyProcessName)-1));

 // Get the USER Name and SID (the SID is used for EventLogging)
 size=sizeof(UserName);
 GetUserName(UserName,&size);
 SidLen=GetSidLengthRequired(20);
 DomainLen=sizeof(DomainName);
 LookupAccountName(NULL,
                          UserName,
                          Sid,
                         &SidLen,
                          DomainName,
                         &DomainLen,
                         &SidType);

 // Get Event Logging Info from Registry

 if ( ! IPCEnumRegistry(HKEY_LOCAL_MACHINE,"SOFTWARE\\IPCInfo",true,
               "LogServerName",sizeof(EventLogServerName),
                           EventLogServerName,
               "LogSourceName",sizeof(EventLogSourceName),
                           EventLogSourceName) )
  _InitFail();

 // Create/Open a Semaphore for Buffer Access
 hBuf=CreateSemaphore(NULL,0,1,"IPCBUFPOOL");
 if ( hBuf==NULL )
  {
   IPCLogSystemError("CreateSemaphore","IPCInit",GetLastError());
   _InitFail();
  }

 // Open the shared Memory. If it doesn't exist then I'm the first
 // One up and running and it's my job to initialize
 if ( (hMem=OpenFileMapping(FILE_MAP_WRITE,true,"IPCMEM")) == NULL )
  {
   DWORD Disposition;

   //Get Number of Buffers and Size of Process Name Cache from the
   // Registry
   if ( ! IPCEnumRegistry(HKEY_LOCAL_MACHINE,"SOFTWARE\\IPCInfo",true,
             "NumBufs",sizeof(NumBufs),&NumBufs) )
    {
     ReleaseSemaphore(hBuf,1,NULL);
     _InitFail();
    }

   // Calculate the amount of virtual memory we will need to hold
   // the Shared Memory Segment
   size = sizeof(*SharedMem) + NumBufs * sizeof(BUFPOOL);

   // Allocate the memory
   hMem = CreateFileMapping((HANDLE)0xffffffff,
                               NULL,
                               PAGE_READWRITE,
                               0,size,
                               "IPCMEM");
   if ( hMem==NULL )
    {
     IPCLogSystemError("CreateFileMapping","IPCInit",GetLastError());
     ReleaseSemaphore(hBuf,1,NULL);
     _InitFail();
    }

   // Map a view of the file
   SharedMem = (struct SharedMemDef *)MapViewOfFile(hMem,FILE_MAP_WRITE,0,0,0);
   if ( SharedMem==NULL )
    {
     IPCLogSystemError("MapViewOfFile","IPCInit",GetLastError());
     _InitFail();
    }

   // Initialize the Memory
   memset(SharedMem,0,size);
   SharedMem->NumBufs=(WORD)NumBufs;
  
   // MAB Change from JE 090498
   BufPool=(BUFPOOL *)&SharedMem[ 1 ];
   //BufPool=(BUFPOOL *)&SharedMem[ sizeof(*SharedMem) ];

   if ( ! IPCEnumRegistry(HKEY_LOCAL_MACHINE,
        "SYSTEM\\CurrentControlSet\\Control\\ComputerName\\ComputerName",true,
        "ComputerName",sizeof(SharedMem->ComputerName),SharedMem->ComputerName) )
   {
    ReleaseSemaphore(hBuf,1,NULL);
    _InitFail();
   }

   // Delete the VOLATILE IPCPhysicalNames KEY and Create a new one in
   // the Registry. If There is no previous instance of the
   // Shared memory then there should be no servers left in the local
   // registry.
   RegDeleteKey(HKEY_LOCAL_MACHINE,"SOFTWARE\\IPCPhysicalNames");
   if ( (ret=RegCreateKeyEx(HKEY_LOCAL_MACHINE,
                            "SOFTWARE\\IPCPhysicalNames",0,"IPCPhysicalNames",
                            REG_OPTION_VOLATILE,KEY_ALL_ACCESS,
                            NULL,&IPCPhysicalNameKey,&Disposition)) !=
        ERROR_SUCCESS )
    {
     IPCLogSystemError("RegCreateKeyEx","IPCInit",ret);
     ReleaseSemaphore(hBuf,1,NULL);
     _InitFail();
    }

   // Create/Open the NON-VOLATILE IPCLogicalNames Key
   if ( (ret=RegCreateKeyEx(HKEY_LOCAL_MACHINE,
                            "SOFTWARE\\IPCLogicalNames",0,
                            "IPCLogicalNames",
                            REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,
                            NULL,&IPCLogicalNameKey,&Disposition)) !=
        ERROR_SUCCESS )
    {
     IPCLogSystemError("RegCreateKeyEx","IPCInit",ret);
     ReleaseSemaphore(hBuf,1,NULL);
     _InitFail();
    }

   // Release other Processes
   SharedMem->InitOkay=true;
   ReleaseSemaphore(hBuf,1,NULL);

   // Register the IPC Message Number
   SharedMem->IPCNotifyMessageNumber =
      RegisterWindowMessage(IPCNotifyMessage);
  }
 else
  {
   // If I'm not the first then I'm going to wait here until the
   // above block of code is completed by the first process
   WaitForSingleObject(hBuf,INFINITE);
   ReleaseSemaphore(hBuf,1,NULL);
   // Map a view of the file
   SharedMem = (struct SharedMemDef *)MapViewOfFile(hMem,FILE_MAP_WRITE,0,0,0);
   if ( SharedMem==NULL )
    {
     IPCLogSystemError("MapViewOfFile","IPCInit",GetLastError());
     _InitFail();
    }

   // Make sure there were no problems with the Initialization block
   // above.
   if ( ! SharedMem->InitOkay )
    _InitFail();

   // Get registry Handles
   if ( (ret=RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                "SOFTWARE\\IPCPhysicalNames",0,
                KEY_ALL_ACCESS,&IPCPhysicalNameKey))
       != ERROR_SUCCESS )
    {
     IPCLogSystemError("RegOpenKeyEx","IPCInit",ret);
     _InitFail();
    }
   if ( (ret=RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                "SOFTWARE\\IPCLogicalNames",0,
                KEY_ALL_ACCESS,&IPCLogicalNameKey))
       != ERROR_SUCCESS )
    {
     IPCLogSystemError("RegOpenKeyEx","IPCInit",ret);
     _InitFail();
    }
   
   // MAB Change from JE 090498
   BufPool=(BUFPOOL *)&SharedMem[ 1 ];
   //BufPool=(BUFPOOL*)&SharedMem[ sizeof(*SharedMem) ];

   SharedMem->IPCNotifyMessageNumber =
     RegisterWindowMessage(IPCNotifyMessage);
  }

 // Allocate a Name Cache
 NameCacheSyncId=SharedMem->CacheSyncId;
 NameCacheCount=0;
 MyMessageQueue=0;
}

void IPCSyncIOInit(char* ProcessName)
{
 DWORD ThreadId;

 IPCInit(ProcessName);

 // Create a thread to do the SYNC IO
 if ( (SyncIOThreadHandle=CreateThread(0,0,
  (LPTHREAD_START_ROUTINE)SyncIOTask,0,0,&ThreadId)) == NULL )
 {
  IPCLogSystemError("CreateThread","IPCInit",GetLastError());
  _InitFail();
 }

 // Create a Semaphore. This semaphore is used to signal the primary
 // task when a message reply is received by the SYNCIO task.
 if ( (SyncIOSemaphore=CreateSemaphore(NULL,0,1,NULL)) == NULL )
  {
   IPCLogSystemError("CreateSemaphore","IPCInit",GetLastError());
   _InitFail();
  }

 // Wait until the Task becomes ready
 while( ! SyncIODummyWindow )
  Sleep(100);
}

//-------------------------------
// This is Server Initialization
//------------------------------
void IPCInit(char* ProcessName,MSGQ Queue)
{
 // First do same as client
 IPCInit(ProcessName);

 // Make sure there is not a process with this name already running
 if ( _LocateProcess(MyProcessName,false) != -1 )
  {
   IPCLogEvent(ServerAlreadyRunning);
   _InitFail();
  }

 // If the Queue parameter is not 0, register it now. Otherwise, the
 // caller must register it later.
 if ( (MyMessageQueue=Queue) != 0 &&
       ! IPCRegister() )
    _InitFail();
}

//--------------------------
// Register a Server Process
//--------------------------
bool IPCRegister(char *ProcessName,MSGQ Queue)
{
 DWORD Dval;
 LONG ret;

 if ( ! ProcessName )
  {
   ProcessName=MyProcessName;
   Queue=MyMessageQueue;
  }

 Dval=(DWORD)Queue;

 // Register the QUEUE in the PHYSICAL names queue
 if ( (ret=RegSetValueEx(IPCPhysicalNameKey,ProcessName,0,REG_DWORD,
                         (BYTE *)&Dval,sizeof(DWORD))) != ERROR_SUCCESS )
  {
   IPCLogSystemError("RegSetValueEx","IPCRegisterServer",ret);
   return false;
  }

 return true;
}

//-------------------------
// DeRegister a ProcessName
//-------------------------
bool IPCDeRegister(char *ProcessName)
{
 DWORD Queue;

 if ( ! ProcessName )
   ProcessName=MyProcessName;

 if ( ! _SearchRegistry(IPCPhysicalNameKey,ProcessName,&Queue,
                      sizeof(Queue)) )
  return false;

 // Purge all Queued Messages
 IPCPurgeMessageQueue((HWND)Queue);

 // Delete the Local Registry Key
 RegDeleteValue(IPCPhysicalNameKey,ProcessName);
 return true;
}

//----------------------------------
// Get a Pointer to My Computer Name
//----------------------------------
char* IPCMyComputerName(void)
{
 return SharedMem->ComputerName;
}

//--------------------------------------------------------
// Send a message to a known process - Expecting no answer
//--------------------------------------------------------
bool IPCSend(IPCMessage* msg,char* Name)
{
 IPCROUTEINFO* rtinfo=msg->MsgRouteInfo();
 int       i;
 bool local=true;

 // The Name can be "REPLY"
 if ( stricmp(Name,"REPLY") == 0 )
  return IPCReply(msg);

 // The Name can be a Fully Qualified Process Name. We can tell
 // because it has an embedded '.' in it.
 if (  _IsFullProcessName(Name,
                          rtinfo->DestComputer,
                          rtinfo->DestProcess) )
   if ( stricmp(rtinfo->DestComputer,SharedMem->ComputerName) )
    {
     Name="NETROUTE";
     local=false;
    }
   else
    Name = rtinfo->DestProcess;

 // Locate the Destination Process
 if ( (i=_LocateProcess(Name)) == -1 )
   return false;

 // If it's a Reply Destination, Return it.
 if ( stricmp(NameCache[i].DestProcessName,"REPLY")==0 )
   return IPCReply(msg);

 // Send to Local or Remote destination
 if ( local )
  {
   strcpy(rtinfo->DestComputer,NameCache[i].DestComputerName);
   strcpy(rtinfo->DestProcess,NameCache[i].DestProcessName);
  }

 return  _Send(msg, NameCache[i].LocalQueue );
}

#pragma argsused
//------------------
// Timeout A message
//------------------
static void CALLBACK TimerProc(MSGQ hwnd,UINT umsg,UINT idEvent,DWORD dwTime)
{
 KillTimer(hwnd,idEvent);
 PostMessage(hwnd,SharedMem->IPCNotifyMessageNumber,
             IPCMessageTimedOut,umsg);
}

//--------------------------------------------------------
// Send a message to a known process as Message Originator
//--------------------------------------------------------
bool IPCSend(IPCMessage* msg,char *DestName,HWND Window,WORD TimeOut)
{
 IPCROUTEINFO* rtinfo=msg->MsgRouteInfo();
 int       i;
 bool local = true;

 // Set up the ROUTEINFO structure with SOURCE and RETURN information
 memset(rtinfo,0,sizeof(IPCROUTEINFO));
 strcpy(rtinfo->SourceComputer,SharedMem->ComputerName);
 strcpy(rtinfo->SourceProcess,MyProcessName);
 rtinfo->ReturnInfo.LocalInfo=(DWORD)Window;
 // Set the SYNCID -- We don't use zero
 if ( ++NextSyncId == 0 )
   NextSyncId=1;
 rtinfo->ReturnInfo.SyncId=NextSyncId;
  // Set the TIMER flag
 if ( TimeOut )
   rtinfo->ReturnInfo.Flags |= RETINFO_TIMER;

  // The Name can be a Fully Qualified Process Name. We can tell
 // because it has an embedded '.' in it.
 if (  _IsFullProcessName(DestName,
                          rtinfo->DestComputer,
                          rtinfo->DestProcess) )
   if ( stricmp(rtinfo->DestComputer,SharedMem->ComputerName) )
    {
     DestName="NETROUTE";
     local=false;
    }
   else
    DestName = rtinfo->DestProcess;

 // Locate the Process
 if ( (i=_LocateProcess(DestName)) == -1 )
   return false;

 if ( local )
  {
   strcpy(rtinfo->DestProcess,NameCache[i].DestProcessName);
   strcpy(rtinfo->DestComputer,NameCache[i].DestComputerName);
  }

 // Send It
 if ( ! _Send(msg,NameCache[i].LocalQueue) )
  return false;

 // Set the Window TIMER
 if ( TimeOut )
  SetTimer(Window,rtinfo->ReturnInfo.SyncId,TimeOut,TimerProc);

 return true;
}

//---------------------------------------
// Send a Message and WAIT for a response
//---------------------------------------
bool FUNCTYPE IPCSend(IPCMessage* Msg,char* DestName,
                      WORD TimeOut,bool RetryUI,
                      int retries,char* AltDestName)
{
 for (;;)
  {
   SyncIOMsg=Msg;
   SyncIOInProgress=true;
   if ( _SyncSend(DestName,TimeOut,retries) )
    {
     SyncIOInProgress=false;
     return true;
    }
   if ( AltDestName &&
        _SyncSend(AltDestName,TimeOut,retries) )
    {
     SyncIOInProgress=false;
     return true;
    }
   if ( ! RetryUI ||
        MessageBoxf(0,"NETWORK ERROR",
            MB_TASKMODAL|MB_ICONERROR|MB_RETRYCANCEL,
          "DESTINATION '%s' IS NOT RESPONDING!",DestName) != IDRETRY )
    {
     SyncIOInProgress=false;
     return false;
    }
  }

}

//-------------------
// Reply to a message
//-------------------
bool IPCReply(IPCMessage *msg)
{
 IPCROUTEINFO* rtinfo=msg->MsgRouteInfo();
 int i;

 // Flag the Message for Reply and Set Dest to Source
 rtinfo->ReturnInfo.Flags |= RETINFO_REPLY;
 strcpy(rtinfo->DestComputer,rtinfo->SourceComputer);
 strcpy(rtinfo->DestProcess,rtinfo->SourceProcess);

 // If it's Remote, Send it to the INTRANET Router for delivery
 if ( stricmp(rtinfo->DestComputer,SharedMem->ComputerName) )
   if ( (i=_LocateProcess("NETROUTE")) == -1 )
     return false;
   else
     return _Send(msg,NameCache[i].LocalQueue);

 // If it's Local, Return it to the Window in the Return Info
 // Structure.
 if ( rtinfo->ReturnInfo.LocalInfo )
   return _Send(msg,(MSGQ)rtinfo->ReturnInfo.LocalInfo);

 // If no Return Window, attempt to locate the Source Process by
 // Name and return it there.
 if (  (i=_LocateProcess(rtinfo->SourceProcess)) == -1 ||
      NameCache[i].LocalQueue == 0 )
   return false;
 else
   return _Send(msg,NameCache[i].LocalQueue);
}

//------------------
// Receive a Message
//------------------
bool IPCRecv(IPCMessage* msg,LPARAM BufId)
{
 BUFPOOL *buf;
 IPCROUTEINFO* rtinfo=msg->MsgRouteInfo();

 if ( ! _IsValidBufnum(BufId) )
   return false;

 buf=&BufPool[BufId];

 // If there's a Timer, Kill it now
 if ( (rtinfo->ReturnInfo.Flags & RETINFO_REPLY) &&
      (rtinfo->ReturnInfo.Flags & RETINFO_TIMER) )
  KillTimer((HWND)rtinfo->ReturnInfo.LocalInfo,
             rtinfo->ReturnInfo.SyncId);

 memcpy(msg,&buf->Msg,MSGOBJSIZE(&buf->Msg));
 buf->Queue=0;
 return true;
}

//------------------------
// Receive a Message Reply
//------------------------
bool IPCRecvReply(IPCMessage *msg,LPARAM BufId)
{
 BUFPOOL  *buf;
 IPCROUTEINFO* rtinfo_old=msg->MsgRouteInfo();
 IPCROUTEINFO* rtinfo_new;

 if ( ! _IsValidBufnum(BufId) )
   return false;

 buf=&BufPool[BufId];
 rtinfo_new=buf->Msg.MsgRouteInfo();

 // If there's a Timer, Kill it now
 if ( (rtinfo_new->ReturnInfo.Flags & RETINFO_REPLY) &&
      (rtinfo_new->ReturnInfo.Flags & RETINFO_TIMER) )
  KillTimer((HWND)rtinfo_new->ReturnInfo.LocalInfo,
            rtinfo_new->ReturnInfo.SyncId);

 // Compare the SYNCID's to make sure they match
 if ( rtinfo_new->ReturnInfo.SyncId != rtinfo_old->ReturnInfo.SyncId )
  {
   buf->Queue=0;
   return false;
  }
 else
  {
   memcpy(msg,&buf->Msg,MSGOBJSIZE(&buf->Msg));
   buf->Queue=0;
   return true;
  }

}

//----------------------------------------
// Query a Message for the embedded SYNCID
//----------------------------------------
bool IPCGetSyncId(IPCMessage* msg,WORD& SyncId)
{
 SyncId=msg->MsgRouteInfo()->ReturnInfo.SyncId;
 return true;
}

//-----------------------------
// Reset a SYNCID (set to zero)
//-----------------------------
void IPCResetSyncId(IPCMessage* msg)
{
 msg->MsgRouteInfo()->ReturnInfo.SyncId=0;
}

//---------------------------------------------------------------------
// Cleanup the Buffer Pool, Freeing all buffers Queued to the callers's
// Queue.
//---------------------------------------------------------------------
void IPCPurgeMessageQueue(MSGQ Queue)
{
 register i;
 for (i=0; i<SharedMem->NumBufs; ++i)
  if ( BufPool[i].Queue == Queue )
    BufPool[i].Queue=0;
}

//---------------------------------------------------------------------
// Count the number of messages in A Message queue
//---------------------------------------------------------------------
int IPCCountMessageQueue(MSGQ Queue)
{
 register i, count;
 for (i=count=0; i<SharedMem->NumBufs; ++i)
  if ( BufPool[i].Queue == Queue )
    ++count;

 return count;
}

//----------------------------------
// EventLogging with up to 5 strings
//----------------------------------
void IPCLogEvent(DWORD msgnum,WORD category,
                 char* str1 ,char* str2 ,char* str3 ,
                 char* str4 , char* str5 ,char* str6 ,
                 char* str7 , char* str8)
{
 IPCLogEvent(msgnum,0,0,category,str1,str2,str3,str4,str5,
              str6,str7,str8);
}

//---------------------------------------------------
// Event Logging with binary dump and up to 5 strings
//---------------------------------------------------
void IPCLogEvent(DWORD msgnum,void* buf,int buflen,WORD category,
                 char* str1 ,char* str2 ,char* str3 ,
                 char* str4 , char* str5 ,char* str6 ,
                 char* str7 , char* str8)
{
 HANDLE log=NULL;
 LPCTSTR lpstrings[9];
 WORD    nstrs;

 lpstrings[0]=MyProcessName;
 lpstrings[1]=str1;
 lpstrings[2]=str2;
 lpstrings[3]=str3;
 lpstrings[4]=str4;
 lpstrings[5]=str5;
 lpstrings[6]=str6;
 lpstrings[7]=str7;
 lpstrings[8]=str8;

 for (nstrs=1; nstrs<9 && lpstrings[nstrs] != 0; ++nstrs);

 // Open the log, using default names if globals are not filled in
 if ( strcmp(EventLogServerName,"LOCAL") )
   log = RegisterEventSource(
           (*EventLogServerName==' ' ||
            *EventLogServerName==0 ) ? DEFAULT_LOG_SERVER :
                                      EventLogServerName,
            (*EventLogSourceName==' ' ||
             *EventLogSourceName==0 ) ? DEFAULT_LOG_SOURCE :
                                      EventLogSourceName);

 // If can't open the Server Event Log try the local log
 if ( log==NULL )
  {
   // If can't open the local log, then we can't log
   log=RegisterEventSource(NULL,EventLogSourceName);
   if ( log==NULL )
     return;
  }

 ReportEvent(log,_EventType(msgnum),category,msgnum,Sid,nstrs,
             buflen,lpstrings,buf);
 DeregisterEventSource(log);
}

//------------------------------------------
// Function to Log an Error to a system call
//------------------------------------------
void IPCLogSystemError(char *called_funcname,
                       char *calling_funcname,
                       int errcode)
{
 char errbuf[10];

 itoa(errcode,errbuf,10);
 IPCLogEvent(SysCallError,0,called_funcname,calling_funcname,
             errbuf,strerror(errcode));
}


//-------------------------------------------------------------------
// Enum (Iterate/Read,etc.) the Registry for up to 5 parameter values
//-------------------------------------------------------------------
bool FUNCTYPE IPCEnumRegistry(HKEY Key,char *subkey,bool required,
                              char* name1, int len1, void* v1,
                              char* name2, int len2, void* v2,
                              char* name3, int len3, void* v3,
                              char* name4, int len4, void* v4,
                              char* name5, int len5, void* v5)
{
 HKEY  hKey;
 int   ret;
 DWORD num;
 char  DataBuf[256];
 char  NameBuf[50];
 DWORD DataBufSize;
 DWORD NameBufSize;
 DWORD ValueType;
 bool  present[5] = { 0, 0, 0, 0, 0 };
#define COMPARE(I,NAME,LEN,PTR)\
  if (NAME && stricmp(NAME,NameBuf)==0 && (int) DataBufSize <=LEN)\
   { memcpy(PTR,DataBuf,DataBufSize); present[I-1]=true; }
#define COMPLAIN(I,NAME)\
  if ( NAME && ! present[I-1] )\
   { if (required) IPCLogEvent(MissingParm,0,NAME); return false; }

 if ( (ret=RegOpenKeyEx(Key,subkey,0,KEY_READ,&hKey)) != ERROR_SUCCESS )
  {
   if ( required )
     IPCLogEvent(MissingParm,0,subkey);
   return false;
  }

 for (ret=ERROR_SUCCESS, num=0; ret==ERROR_SUCCESS;++num)
  {
   DataBufSize=sizeof(DataBuf);
   NameBufSize=sizeof(NameBuf);
   if ( (ret=RegEnumValue(hKey,num,(LPTSTR)NameBuf,&NameBufSize,NULL,
                    &ValueType,(LPBYTE)DataBuf,&DataBufSize)) ==
        ERROR_SUCCESS )
    {
     COMPARE(1,name1,len1,v1) else
     COMPARE(2,name2,len2,v2) else
     COMPARE(3,name3,len3,v3) else
     COMPARE(4,name4,len4,v4) else
     COMPARE(5,name5,len5,v5);
    }
  }

 RegCloseKey(hKey);
 COMPLAIN(1,name1);
 COMPLAIN(2,name2);
 COMPLAIN(3,name3);
 COMPLAIN(4,name4);
 COMPLAIN(5,name5);
 return true;
 #undef COMPLAIN
 #undef COMPARE
}

//-----------------------------------------
// Function to Fetch IPCNotifyMessageNumber
//-----------------------------------------
UINT FUNCTYPE IPCGetNotifyMessageNumber(void)
{
 return SharedMem->IPCNotifyMessageNumber;
}


//----------------------------------------------------------------------
//                     BEGIN STATIC FUNCTIONS
//----------------------------------------------------------------------

//--------------------------
// Report on Startup Failure
//--------------------------
static void _InitFail(void)
{
 ::MessageBox(0,
   "IPC Initialization Failed\nSee Event Log for Details",
   MyProcessName,MB_OK|MB_ICONSTOP);
 throw;
}

//------------------------------------------------------------------
// Get Event Type from a Message Number (It's in the first 2 bits -
// See MC compiler docs.
//------------------------------------------------------------------
static WORD _EventType(DWORD msgnum)
{
 static WORD types[4] = { EVENTLOG_AUDIT_SUCCESS,
                          EVENTLOG_INFORMATION_TYPE,
                          EVENTLOG_WARNING_TYPE,
                          EVENTLOG_ERROR_TYPE };

 return types[ msgnum >> 30 ];
}


//------------------
// Allocate a Buffer
//------------------
static WORD _getbuf(MSGQ queue)
{
 bool first_try;
 register i;

 for (first_try=true;;first_try=false)
  {
   WaitForSingleObject(hBuf,INFINITE);
   // First look for a free buffer
   for (i=0; i<SharedMem->NumBufs; ++i)
    if ( BufPool[i].Queue == 0 )
     {
      BufPool[i].Queue=queue;
      ReleaseSemaphore(hBuf,1,NULL);
      return (WORD) i;
     }
   // Attempt to Clean up the queue by marking messages queued to
   // Non-Existent Queues as free
   for (i=0; i<SharedMem->NumBufs; ++i)
    if ( ! IsWindow(BufPool[i].Queue) )
      BufPool[i].Queue=0;
   ReleaseSemaphore(hBuf,1,NULL);
   // Log this event only one time per occurrence
   if ( first_try )
     IPCLogEvent(ResourceFull,0,"IPCGetBuf","Buffer Pool");
     
   // Wait a random amount of time from 1/2 to 1 second before
   // Trying again.
   Sleep( 500 + random(500) );
  }

}

//-------------------------------------
// Allocate a Buffer and Send a Message
//-------------------------------------
static bool _Send(IPCMessage* msg,MSGQ queue)
{
 WORD bufnum;
 int  i;

 // If the QUEUE is ZERO, this means it's a Remote message.
 // Look up NETROUTE in cache.
 if ( queue == 0 )
  {
   if ( (i=_LocateProcess("NETROUTE")) == -1 )
     return false;
   queue = NameCache[i].LocalQueue;
  }

 bufnum=_getbuf(queue);
 memcpy(&BufPool[bufnum].Msg,msg,MSGOBJSIZE(msg));
 if (! PostMessage(queue,SharedMem->IPCNotifyMessageNumber,
                   IPCMessageReceived,bufnum) )
  {
   IPCLogSystemError("PostMessage","IPCSend",GetLastError());
   BufPool[bufnum].Queue=0;
   return false;
  }

 return true;
}

//--------------------------------------------------------------
// Validate that buffer number is valid and return pointer to it
//--------------------------------------------------------------
static bool _IsValidBufnum(LPARAM bufnum)
{
 if ( (bufnum > SharedMem->NumBufs-1) ||
      ! BufPool[bufnum].Queue )
  {
   IPCLogEvent(IllegalFunctionParm,0,"bufnum","_recv");
   return false;
  }

 return true;
}

//----------------------------------------------------------
// Search the Open Registry Key for name requested by caller
//-----------------------------------------------------------
static bool _SearchRegistry(HKEY Key,char* Name,void* Data,
                            DWORD DataSize)
{
 int   n;
 char  NameBuf[MAX_IPCLOGICALNAME+1];
 DWORD NameSize;
 DWORD Dsize;
 DWORD type_code;

 for (NameSize=sizeof(NameBuf), Dsize=DataSize, n=0;

      RegEnumValue(Key,
                   n,
                   NameBuf,
                   &NameSize,
                   0,
                   &type_code,
                   (LPBYTE)Data,
                   &Dsize)==ERROR_SUCCESS;

    ++n, NameSize=MAX_IPCLOGICALNAME+1, Dsize=DataSize)

   if ( stricmp(NameBuf,Name) == 0 )
    return true;

 return false;
}

//------------------------------------------------------------------
// Locate a Server Process and add it's name to the Cache
//------------------------------------------------------------------
static int _LocateProcess(char* name,bool LogEvent)
{
 int   i;
 char  Buf[MAX_IPCLOGICALNAME+1];
 char  ComputerName[MAX_IPCNAME+1];
 char  ProcessName[MAX_IPCNAME+1];
 MSGQ  Queue;

 // Make sure this name is valid
 if ( strlen(name) > MAX_REMOTEIPCNAME )
   return -1;

 // First see if the Cache Needs to be cleared
 if ( NameCacheSyncId != SharedMem->CacheSyncId )
  {
   NameCacheSyncId=SharedMem->CacheSyncId;
   NameCacheCount=0;
  }

 // Search the Name Cache for a Match
 for (i=0; i<NameCacheCount; ++i)
   if ( stricmp(name,NameCache[i].Name)==0 )
    {
     // If it's a local queue that's no longer valid, delete it
     // from the cache and continue.
     if ( NameCache[i].LocalQueue != 0 &&
          ! IsWindow(NameCache[i].LocalQueue) )
      {
       NameCacheCount=(BYTE)RemoveArrayItem(&NameCache,
                 NameCacheCount,sizeof(NameCache[0]),i);
       break;
      }
     else
       return i;
    }

 // Look First for a Local IPC Server
 if ( _SearchRegistry(IPCPhysicalNameKey,name,&Queue,sizeof(Queue)) &&
      IsWindow(Queue) )
  return _CacheInsert(name,Queue,name,SharedMem->ComputerName);

 // Look Next for an IPC Alias
 if ( _SearchRegistry(IPCLogicalNameKey,name,Buf,sizeof(Buf)) )
  {
   // It can be a FULL Process Name in the form
   // ComputerName.ProcessName
   if ( _IsFullProcessName(Buf,ComputerName,ProcessName) )
    {
     if ( stricmp(ComputerName,SharedMem->ComputerName) )
       return _CacheInsert(name,0,ProcessName,ComputerName);
     else
       strcpy(Buf,ComputerName);
    }

   else
   // It can be the word REPLY
   if ( stricmp(Buf,"REPLY") == 0 )
    return _CacheInsert(name,0,Buf,"");

   // It can be an ALIAS for a LOCAL Server Process
   if ( _SearchRegistry(IPCPhysicalNameKey,Buf,&Queue,sizeof(Queue)) &&
        IsWindow(Queue) )
    return _CacheInsert(name,Queue,Buf,SharedMem->ComputerName);
  }

 // No hit. Log an Event for diagnostic purposes
 if ( LogEvent )
   IPCLogEvent(ProcessNotFound,0,name);
 return -1;
}

//------------------------------
// Insert Server Info into cache
//------------------------------
static int _CacheInsert(char *name,MSGQ queue,char* ProcessName,
                        char* ComputerName)
{
 int i;

 // If Full, Remove the first Entry
 if ( NameCacheCount == MAX_NAMECACHE )
   NameCacheCount=(BYTE)RemoveArrayItem(&NameCache,
      NameCacheCount,sizeof(NameCache[0]),0);

 i = NameCacheCount++;
 strcpy(NameCache[i].Name,name);
 NameCache[i].LocalQueue=queue;
 strcpy(NameCache[i].DestProcessName,ProcessName);
 strcpy(NameCache[i].DestComputerName,ComputerName);
 return i;
}

//------------------------------------------------------------------
// Determine if a Remote process name is valid. If not, log an event
//------------------------------------------------------------------
static bool _IsFullProcessName(char* Name,char* ComputerName,
                               char* ProcessName)
{
 char* pt;
 int ComputerNameLen;

 if ( (pt = strchr(Name,'.')) == NULL ||
      strlen(&pt[1]) > MAX_IPCNAME ||
      (ComputerNameLen=(ptrdiff_t)(pt-Name)) > MAX_IPCNAME )
  return false;

 memcpy(ComputerName,Name,ComputerNameLen);
 ComputerName[ComputerNameLen]=0;
 strcpy(ProcessName,&pt[1]);
 return true;
}

static LRESULT CALLBACK WindowProc(
    HWND  hwnd,	// handle of window
    UINT  uMsg,	// message identifier
    WPARAM  wParam,	// first message parameter
    LPARAM  lParam) 	// second message parameter
{
 if (uMsg==SharedMem->IPCNotifyMessageNumber)
  {
   BUFPOOL *buf = &BufPool[lParam];

   if ( SyncIOInProgress &&
        buf->Msg.MsgRouteInfo()->ReturnInfo.SyncId ==
          SyncIOMsg->MsgRouteInfo()->ReturnInfo.SyncId )
    {
     SyncIOMsg->CopyMsgIn(buf->Msg.GetMsgPointer(),
                          (WORD)buf->Msg.GetMsgLength() );
     ReleaseSemaphore(SyncIOSemaphore,1,NULL);
    }

   buf->Queue=0;
   return 0;
  }
 return DefWindowProc( hwnd, uMsg, wParam, lParam );
}

//---------------------------
// Send a Synchronous Message
//---------------------------
static bool _SyncSend(char* DestName,int TimeOut,int Retries)
{
 int N;

 for (N=0; N<=Retries; ++N)
  {
   // Send the Message, expecting the response to come back to the dummy
   // window in the SYNCIO task.
   if ( ! IPCSend(SyncIOMsg,DestName,SyncIODummyWindow,0) )
     return false;
   switch( WaitForSingleObject(SyncIOSemaphore,TimeOut) )
    {
     case WAIT_FAILED:     IPCLogSystemError("WaitForSingleObject",
                                    "IPCSend",GetLastError());
                           return false;

     case WAIT_OBJECT_0:   return true;

     case WAIT_TIMEOUT:    break;
    }
  }

 // Retries exceeded 
 return false;
}


//---------------------------------------------------------------
// This is the Synchronous I/O Task if the caller initialized for
// Synchronous I/O
//---------------------------------------------------------------
static DWORD SyncIOTask(void*)
{
 MSG msg;
 CREATESTRUCT CreateStruct;

 // Register a Window Class
 WNDCLASS wndclass;   // Structure used to register Windows class.
 memset(&wndclass,0,sizeof(wndclass));
 wndclass.style         = CS_GLOBALCLASS;
 wndclass.lpfnWndProc   = WindowProc;
 wndclass.lpszClassName = "IPCSYNCIO";

 if ( ! RegisterClass( &wndclass ) )
  {
   IPCLogSystemError("RegisterClass","SyncIOTask",GetLastError());
   throw;
  }

 SyncIODummyWindow = CreateWindow("IPCSYNCIO","SYNCIO",
   WS_DISABLED|WS_POPUP,0,0,1,1,0,NULL,0,(LPSTR)&CreateStruct);
 if ( ! SyncIODummyWindow )
  {
   IPCLogSystemError("CreateWindow","SyncIOTask",GetLastError());
   throw;
  }

 while( GetMessage( &msg, NULL, 0, 0 ) )
  {
   TranslateMessage( &msg );
   DispatchMessage( &msg );
  }

 return 0;
}



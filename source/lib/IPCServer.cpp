//----------------------------------------------------------------------------
//  Project ServerApp
//  JHE Software, Inc.
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    ServerApp Application
//  FILE:         IPCServer.cpp
//  AUTHOR:       Jack H Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Encapsulates the Main Window for a Server Application. It creates
//  the main WINDOW as the primary IPC MESSAGE QUEUE for this SERVER
//  and handle's message dispatching.
//----------------------------------------------------------------------------

#define  STRICT
#include <windows.h>
#include <stdio.h>
#include <time.h>
#include "ipc.h"
#include "messages.h"
#include "splib.h"

static LRESULT  CALLBACK  WndProc( HWND hWnd,
                            UINT iMessage,
                            WPARAM wParam,
                            LPARAM lParam )
{
 IPCWindow* pWindow = (IPCWindow*)GetWindowLong(hWnd,0);
 if ( pWindow == 0 )
  {
   if ( iMessage == WM_CREATE )
    {
     LPCREATESTRUCT lpcs;

     lpcs = (LPCREATESTRUCT) lParam;
     pWindow = (IPCWindow *) lpcs->lpCreateParams;
     SetWindowLong(hWnd,0,(LONG)pWindow);
     return pWindow->WndProc( iMessage, wParam, lParam );
    }
   else
    return DefWindowProc( hWnd, iMessage, wParam, lParam );
  }
 else
  return pWindow->WndProc( iMessage, wParam, lParam );
}


IPCServer::IPCServer(HINSTANCE hInstance,
                       char *Name,
                       char *Version,
                       IPCMessage* MsgBuf,
                       void (CALLBACK* Applfunc)(void),
                       int(CALLBACK* Adminfunc)(void))
 {
  DWORD dwStyle=WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX|WS_SIZEBOX;
  DWORD ParentWin=0;

  strcpy(ServerProcessName,Name);
  strcpy(ProgramVersionInfo,Version);
  ApplFunc=Applfunc;
  AdminFunc=Adminfunc;
  Msg=MsgBuf;
  Register(hInstance,ServerProcessName);

  IPCEnumRegistry(HKEY_LOCAL_MACHINE,"SOFTWARE\\IPCServers",
                   false,"IPCShell",sizeof(ParentWin),&ParentWin);
  if ( ! IsWindow((HWND)ParentWin) )
     ParentWin=0;


  hWnd = CreateWindow( ServerProcessName,
         ServerProcessName,
         dwStyle,
         0,
         0,
         300,
         360,
         (HWND)ParentWin,
         NULL,
         hInstance,
         (LPSTR) this );
     if ( ! hWnd )
         exit( FALSE );

  if ( ! IPCRegister(ServerProcessName,hWnd) )
    exit( FALSE );

  MessageCounter=StartDisplayMessageCounter=0;
  ProcessStartedTS=time(NULL);
  AdminMsgTS=ApplicationMsgTS=0;
  MessageCounterTS=ProcessStartedTS;
  strcpy(LastAdminMessage,"None");
  ShowWindow( hWnd, SW_MINIMIZE );
  UpdateWindow(hWnd);

  // Get the Notify Message Number
  IPCNotify = IPCGetNotifyMessageNumber();
 }

IPCServer::~IPCServer()
{
 IPCDeRegister();
}

void IPCServer::Register(HINSTANCE hInstance,char* name)
 {
  WNDCLASS wndclass;   // Structure used to register Windows class.

  wndclass.style         = CS_HREDRAW | CS_VREDRAW;
  wndclass.lpfnWndProc   = ::WndProc;
  wndclass.cbClsExtra    = 0;
  wndclass.cbWndExtra    = sizeof( IPCServer * );
  wndclass.hInstance     = hInstance;
  wndclass.hIcon         = LoadIcon( hInstance, "ServerApp" );
  wndclass.hCursor       = LoadCursor( NULL, IDC_ARROW );
  wndclass.hbrBackground = (HBRUSH)GetStockObject( WHITE_BRUSH );
  wndclass.lpszMenuName  = NULL;
  wndclass.lpszClassName = name;

  if ( ! RegisterClass( &wndclass ) )
      exit( FALSE );
 }


void IPCServer::UpdateWindowText(HDC hdc)
{
 RECT rect;
 char FormatBuf[512];

 // Display the Program Version
 strcpy(FormatBuf,"Program Version\n");
 strcat(FormatBuf,ProgramVersionInfo);
 strcat(FormatBuf,"\n--------------\n");

 // Display what time the process started
 strcat(FormatBuf,"Process Started\n");
 strcat(FormatBuf,asctime(localtime((time_t*)&ProcessStartedTS)));
 strcat(FormatBuf,"--------------\n");

 // Display Queue Count
 strcat(FormatBuf,"Messages Queued\n");
 ultoa(IPCCountMessageQueue(hWnd),&FormatBuf[strlen(FormatBuf)],10);
 strcat(FormatBuf,"\n--------------\n");

 // Display Application Message Summary, if any
 if ( ApplicationMsgTS )
  {
   strcat(FormatBuf,"Application Messages Received\n");
   ultoa(MessageCounter,&FormatBuf[strlen(FormatBuf)],10);
   strcat(FormatBuf,"\nSince\n");
   strcat(FormatBuf,asctime(localtime((time_t*)&MessageCounterTS)));
   strcat(FormatBuf,"Last\n");
   strcat(FormatBuf,asctime(localtime((time_t*)&ApplicationMsgTS)));
   strcat(FormatBuf,"--------------\n");
  }


 // Display Admin Message Summary, if any 
 if ( AdminMsgTS )
  {
   strcat(FormatBuf,"Last Admin Message Received\n");
   strcat(FormatBuf,asctime(localtime((time_t*)&AdminMsgTS)));
   strcat(FormatBuf,"Text\n");
   strcat(FormatBuf,LastAdminMessage);
  }

 // Paint the window 
 GetClientRect( hWnd, (LPRECT) &rect );
 DrawText(hdc,FormatBuf,-1,&rect,DT_CENTER|DT_NOCLIP);
}

void IPCServer::Paint( void )
{
 PAINTSTRUCT ps;
 BeginPaint( hWnd, &ps );
 UpdateWindowText(ps.hdc);
 EndPaint( hWnd, &ps );
 if ( ! StartDisplayMessageCounter )
   StartDisplayMessageCounter = MessageCounter;
}

void IPCServer::Display(void)
{
 RECT rect;

 if ( IsIconic(hWnd) )
   return;

 GetClientRect( hWnd, (LPRECT) &rect );
 InvalidateRect( hWnd, &rect, true );
 UpdateWindow(hWnd);
}

//------------------------
// Handle an ADMIN Message
//
// An ADMIN Message is a Message with a text Command in the
// IPCFN_ADMINMESSAGE field.
//
// Returns true if Message Should be Replied to, otherwise
// returns false.
//------------------------
bool IPCServer::ProcessAdminMessage()
{
 char MsgText[256];
 int  len=sizeof(MsgText);
 IPCROUTEINFO* rtinfo=(IPCROUTEINFO*)Msg->GetMsgPointer();

 // If the Message is not specifically addressed to me then
 // it's none of my business.
 if ( stricmp(rtinfo->DestProcess,ServerProcessName) ||
      stricmp(rtinfo->DestComputer,IPCMyComputerName()) ||
      ! Msg->GetFld(IPCFN_ADMINMESSAGE,len,MsgText) )
  return false;

 // Save the Text of the message and the time for the Display
 memcpy(LastAdminMessage,MsgText,(int)min(len,(int)sizeof(LastAdminMessage)-1));
 LastAdminMessage[sizeof(LastAdminMessage)-1] = 0;
 AdminMsgTS=time(NULL);

 // Give Server APP first crack at it. Server APP returns
 // true if he handled it himself, false if he wants us to
 // implement the default action.
 if ( AdminFunc() )
   return true;

 // If It's HELP, we let the Server Object put in it's Command
 // Summary and then we append the default commands to it.
 if ( memcmp(MsgText,"HELP",4)==0 )
  {
   Msg->GetFld(IPCFN_ADMINMESSAGE,len=sizeof(MsgText),MsgText);
   strcat(MsgText,", MSGCOUNT, MSGCOUNTRESET,"
           "REGISTER, HELLO, SHUTDOWN, VERSION");
   Msg->PutFld(IPCFN_ADMINMESSAGE,MsgText);
   return true;
  }

 // Get the Message Counter and Reset it to Zero
 if ( memcmp(MsgText,"MSGCOUNTRESET",14) == 0 )
  {
   sprintf(MsgText,"MSGCOUNT = %d",MessageCounter);
   Msg->PutFld(IPCFN_ADMINMESSAGE,MsgText);
   MessageCounter=0;
   MessageCounterTS=time(NULL);
   return true;
  }

 // Get the Message Counter
 if ( memcmp(MsgText,"MSGCOUNT",8) == 0 )
  {
   sprintf(MsgText,"MSGCOUNT = %d",MessageCounter);
   Msg->PutFld(IPCFN_ADMINMESSAGE,MsgText);
   return true;
  }

 // Echo ALIVE Message Back in Response to HELLO
 if ( memcmp(MsgText,"HELLO",5) == 0 )
  {
   sprintf(MsgText,"%s.%s ALIVE",IPCMyComputerName(),
           ServerProcessName);
   Msg->PutFld(IPCFN_ADMINMESSAGE,MsgText);
   return true;
  }

 // Re-Register the Server
 if ( memcmp(MsgText,"REGISTER",8) == 0 )
  {
   if ( IPCRegister(ServerProcessName,hWnd) )
    strcpy(MsgText,"OKAY");
   else
    strcpy(MsgText,"REGISTRATION FAILED");
   Msg->PutFld(IPCFN_ADMINMESSAGE,MsgText);
   return true;
  }

 // Get VERSION Info
 if ( memcmp(MsgText,"VERSION",7) == 0 )
  {
   strcpy(MsgText,ServerProcessName);
   strcat(MsgText," VERSION = ");
   strcat(MsgText,ProgramVersionInfo);
   Msg->PutFld(IPCFN_ADMINMESSAGE,MsgText);
   return true;
  }

 // SHUTDOWN the Server NOW
 if ( memcmp(MsgText,"SHUTDOWN",8) == 0 )
  {
   strcpy(MsgText,"OKAY");
   Msg->PutFld(IPCFN_ADMINMESSAGE,MsgText);
   PostQuitMessage(0);
   return true;
  }

 // ADMIN COMMAND UNRECOGNIZED
 strcat(MsgText," (COMMAND UNRECOGNIZED)");
 Msg->PutFld(IPCFN_ADMINMESSAGE,MsgText);
 return true;
}

//------------------------
// Handle a DEBUG COMMAND
//
// A DEBUG COMMAND is a Message with a text Command in the
// IPCFN_DEBUG field. After the DEBUG action is taken, it
// is HANDLED as an APPLICATION Message.
//------------------------
void IPCServer::ProcessDebugCommand()
{
 char MsgText[128];
 int  len=sizeof(MsgText);
 char *command;
 char *scategory;
 WORD  category;

 // Get the COMMAND
 if ( ! Msg->GetFld(IPCFN_DEBUG,len,MsgText) )
  return;

 // This is Debugging Feature to simulate a transaction timeout.
 // The Sender simply tells us how long to wait in Milliseconds
 // The form is WAITnnn...
 if (memcmp(MsgText,"WAIT",4)==0)
  {
   Sleep(atol(&MsgText[4]));
   return;
  }

 // We display the DEBUG command in the Last Admin field on
 // the display window.
 memcpy(LastAdminMessage,MsgText,(int)min(len,(int)sizeof(LastAdminMessage)-1));
 LastAdminMessage[sizeof(LastAdminMessage)-1] = 0;
 AdminMsgTS=time(NULL);

 if ( (command=strtok(MsgText," ")) != NULL )
  {
   if ( (scategory=strtok(NULL," ")) != NULL )
     category=(WORD)atoi(scategory);
   else
     category=0;

   if ( stricmp(command,"TRACE") == 0 )
     IPCLogEvent(MessageTrace,category,MsgText);
   else
   
   if ( stricmp(command,"TRACEDUMP") == 0 )
     IPCLogEvent(MessageTraceDump,Msg->GetMsgPointer(),
               (int)Msg->GetMsgLength(),category,MsgText);
  }

 // ANYTHING else just displays in window
}


LRESULT IPCServer::WndProc( UINT iMessage, WPARAM wParam, LPARAM lParam )
{
 if (iMessage==IPCNotify)
  {
   if ( Msg->RecvMsg(lParam) )
    {
     if ( Msg->GetFld(IPCFN_DEBUG) )
      ProcessDebugCommand();

     if ( Msg->GetFld(IPCFN_ADMINMESSAGE) &&
          ProcessAdminMessage() )
       Msg->ReplyMsg();
     else
      {
       ++MessageCounter;
       ApplicationMsgTS=time(NULL);
       ApplFunc();
      }

     if ( ! IsIconic(hWnd) )
       Display();
    }

   return 0; 
  }

 switch (iMessage)
  {
   case WM_SYSCOMMAND:
     if ( (wParam & 0xfff0) == SC_CLOSE )
       if ( MessageBox(hWnd,
                  "YOU ARE ABOUT TO SHUTDOWN\n"
                  "A SERVER PROCESS WHICH MAY\n"
                  "BE PROCESSING TRANSACTIONS.\n\n"
                  "ARE YOU SURE YOU WANT TO DO THIS?",
                  "WARNING",MB_ICONEXCLAMATION|MB_YESNO) != IDYES )
        return 1;
   default:
      return DefWindowProc( hWnd, iMessage, wParam, lParam );

   case WM_CREATE:
      break;

   case WM_PAINT:
      Paint();
      break;

   case WM_DESTROY:
      PostQuitMessage( 0 );
      break;

  }

 return 0;
}


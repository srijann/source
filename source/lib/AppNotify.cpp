//----------------------------------------------------------------------------
//  Copyright © 1996. All Rights Reserved.
//
//  FILE:         AppNotify.cpp
//  AUTHOR:       Jack Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of AppNotify
//
//  Some functions for handling application-level notification
//  messages.
//
//  This is not an object, so you can only have one of these in
//  your program.
//----------------------------------------------------------------------------

#include <owl/pch.h>
#include "splib.h"

static UINT MsgNum=0;

//---------------------------------------------------------------
// Call this function to register the message and save the Number
//---------------------------------------------------------------
UINT SetAppNotifyMsg(char *AppName)
{
 return MsgNum=RegisterWindowMessage(AppName);
}

//---------------
// Get the Number
//---------------
UINT GetAppNotifyMsg()
{
 return MsgNum;
}

//---------------------------------------
// To Send Notification Message to a HWND
//---------------------------------------
void AppNotifyMsg(HWND Window,WPARAM MsgType, LPARAM Id)
{
 PostMessage(Window,MsgNum,MsgType,Id);
}

//----------------------------------------
// To Send Notification Message to TWindow
//----------------------------------------
void AppNotifyMsg(TWindow *Win,WPARAM MsgType, LPARAM Id)
{
 AppNotifyMsg(Win->HWindow,MsgType,Id);
}




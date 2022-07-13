#include "win4me.h"

static UINT MsgNum=0;

//---------------------------------------------------------------------
// Fetch the SpsMan Notify Message Number. Register if not registered.
//---------------------------------------------------------------------
UINT SpsManNotify()
{
 if ( ! MsgNum )
  MsgNum=RegisterWindowMessage("SPSMANNOTIFY");
 return MsgNum;
}

//---------------------------------------
// To Send Notification Message to a HWND
//---------------------------------------
void SpsManNotify(HWND Window,WPARAM MsgType, LPARAM Id)
{
 if (! MsgNum)
  SpsManNotify();
 PostMessage(Window,MsgNum,MsgType,Id);
}

//----------------------------------------
// To Send Notification Message to TWindow
//----------------------------------------
void SpsManNotify(TWindow *Win,WPARAM MsgType, LPARAM Id)
{
 SpsManNotify(Win->HWindow,MsgType,Id);
}




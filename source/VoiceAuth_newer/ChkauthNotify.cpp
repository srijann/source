#include "win4me.h"
#include "chkauth.h"

static UINT MsgNum=0;

//---------------------------------------------------------------------
// Fetch the CHKAUTH Notify Message Number. Register if not registered.
//---------------------------------------------------------------------
UINT ChkauthNotify()
{
 if ( ! MsgNum )
  MsgNum=RegisterWindowMessage(CHKAUTHNOTIFYMSG);
 return MsgNum;
}

//---------------------------------------
// To Send Notification Message to a HWND
//---------------------------------------
void ChkauthNotify(HWND Window,WPARAM MsgType, LPARAM Id)
{
 if (! MsgNum)
  ChkauthNotify();
 PostMessage(Window,MsgNum,MsgType,Id);
}

//----------------------------------------
// To Send Notification Message to TWindow
//----------------------------------------
void ChkauthNotify(TWindow *Win,WPARAM MsgType, LPARAM Id)
{
 ChkauthNotify(Win->HWindow,MsgType,Id);
}




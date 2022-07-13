#ifndef __APPNOTIFY_H
#define __APPNOTIFY_H
extern UINT SetAppNotifyMsg(char* AppName);
extern UINT GetAppNotifyMsg();
extern void AppNotifyMsg(HWND Window,WPARAM MsgType, LPARAM Id);
extern void AppNotifyMsg(TWindow *Win,WPARAM MsgType, LPARAM Id);
#endif

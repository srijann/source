//------------------------------------------------
// Does a Message Box with printf Style formatting
//------------------------------------------------
#define STRICT
#include <windows.h>
#include <stdarg.h>
#include <stdio.h>

int MessageBoxf(HWND hwnd,LPCTSTR caption,UINT type,char *fmtstr,...)
{
 char buf[255];
 va_list argptr;

 va_start(argptr,fmtstr);
 vsprintf(buf,fmtstr,argptr);
 va_end(argptr);
 return MessageBox(hwnd,buf,caption,type);
} 
#ifndef __SPLIB_H
#define __SPLIB_H
extern bool  ShareData(void *data,int len,char *name);
extern void *ShareData(char *name);
extern int RemoveArrayItem(void *array,int numitems,int itemsize,int delitem);
extern int MessageBoxf(HWND hwnd,LPCTSTR caption,UINT type,char *fmtstr,...);
extern char* SocStrerror(int);
extern int _atoi(char* String,int Len);
#endif

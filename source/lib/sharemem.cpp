#include <windows.h>

bool ShareData(void *data,int len,char *name)
{
 char *mem;
 HANDLE hMem = CreateFileMapping((HANDLE)0xffffffff,
                               NULL,
                               PAGE_READWRITE,
                               0,len,
                               name);
 if ( hMem==NULL )
  return false;

 mem = (char *)MapViewOfFile(hMem,FILE_MAP_WRITE,0,0,0);
 if ( mem==NULL )
  return false;

 memcpy(mem,data,len);
 return true;
}

void *ShareData(char *name)
{
 HANDLE hMem=OpenFileMapping(FILE_MAP_WRITE,true,name);

 if ( hMem==NULL )
  return false;

 return( MapViewOfFile(hMem,FILE_MAP_WRITE,0,0,0) );
}

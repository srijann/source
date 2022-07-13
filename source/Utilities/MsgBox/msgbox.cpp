#include <windows.h>
#include <time.h>

void main(int argc,char* argv[])
{
 int Type=MB_OK;
 time_t now=time(NULL);
 struct tm* t=localtime(&now);
 char Buf[50];
 int  Len;

 strcpy(Buf,asctime(t));
 Len=strlen(Buf);
 if ( Buf[Len-1] == '\n' )
  Buf[Len-1] = 0;

 if ( argc > 2 )
  {
   if ( stricmp(argv[2],"ERROR")==0 )
    Type |= MB_ICONERROR;
   else
    if ( stricmp(argv[2],"WARN")== 0 )
     Type |= MB_ICONEXCLAMATION;
    else
     Type |= MB_ICONINFORMATION;
  }
 else
  Type |= MB_ICONINFORMATION;     

 MessageBox(0,argv[1],Buf,Type);
}
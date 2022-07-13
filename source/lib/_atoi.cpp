#include <stdlib.h>
#include <string.h>

//------------------------------------------------------------
// Convert Ascii to Integer without requiring null-terminator.
// (Caller supplies length)
//------------------------------------------------------------
int _atoi(char *String,int Len)
{
 char Buf[20];

 if ( Len > sizeof(Buf) -1 )
  return 0;

 memcpy(Buf,String,Len);
 Buf[Len]=0;
 return atoi(Buf);
}

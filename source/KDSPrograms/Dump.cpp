#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>

//--------------------------------
// Print a dump to file SPSOut.DMP
//--------------------------------
void Dump(const char* Text,const char* Buf,int Len)
{
 unsigned char* sBuf=(unsigned char*)Buf;
 register i, j, k;
 FILE* f=fopen("SPSOUT.DMP","a");
 time_t now;
 struct tm* tblock;

 now = time(NULL);
 tblock = localtime( &now );

 if ( f==NULL )
  return;

 fprintf(f,"%s",asctime(tblock));
 fprintf(f,"%s\n",Text);
 fprintf(f,"Message Length=%d\n",Len);

 #define CHARS_PER_LINE 10
 char  hexBuf[CHARS_PER_LINE*3+1];
 char  charBuf[CHARS_PER_LINE+1];

 for (i=0; i<Len; i += CHARS_PER_LINE)
  {
   memset(hexBuf,0,sizeof(hexBuf));
   memset(charBuf,0,sizeof(charBuf));
   for ( j=0, k=i+j; k<Len && j < CHARS_PER_LINE; ++j, ++k)
    {
     sprintf(&hexBuf[j*3],"%02X ",sBuf[k]);
     if ( isprint(sBuf[k]) )
      charBuf[j] = sBuf[k];
     else
      charBuf[j] = '.';
    }
   fprintf(f,"%05d %-*s %-*s\n",i,CHARS_PER_LINE*3,hexBuf,
           CHARS_PER_LINE,charBuf);
  }

 fclose(f); 
}

char Buf[500];

void main(void)
{
 int i;
 for (i=0; i<sizeof(Buf); ++i)
   Buf[i] = (unsigned char) i;

 Dump("This is a Test",Buf,sizeof(Buf));
 exit(0);
}

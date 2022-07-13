#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>

// Testing sockets in UNIX Domain for buffering capabilities

char inbuf[4097];
char outbuf[4097];

void Abort(char*Str)
{
 perror(Str);
 exit(1);
}

void main(void)
{
 int so=socket(AF_UNIX,SOCK_DGRAM,0);
 int si=socket(AF_UNIX,SOCK_DGRAM,0);
 int len;
 struct sockaddr_un addr;
 

 if ( so==-1 || si==-1 )
    Abort("SOCKET CREATION ERROR");
 else
    puts("SOCKETS CREATED OKAY");  

 memset(&addr,0,sizeof(addr));
 addr.sun_family = AF_INET;
 strcpy(addr.sun_path,"SOCIN");
 unlink(addr.sun_path);
  if ( bind(si,(struct sockaddr*)&addr,sizeof(addr)) == -1 )
   Abort("BIND FAILED");
 else
   puts("SI BIND OKAY"); 

 strcpy(outbuf,"HELLO");
 if ( sendto(so,outbuf,sizeof(outbuf),0,(struct sockaddr*)&addr,sizeof(addr)) < 0 )
   Abort("SENDTO ERROR");
 else
   puts("SEND OKAY");

 if ( read(si,inbuf,sizeof(inbuf)) < 0 )
   Abort("READ ERROR");
 else
   puts(inbuf);

 close(so);
 close(si);
} 

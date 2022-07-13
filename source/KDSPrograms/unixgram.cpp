//#define WINDOWSNT
#define SCOUNIX

#ifdef WINDOWSNT
#include <winsock.h>
#endif

#ifdef SCOUNIX
#define INVALID_SOCKET (-1)
#define SOCKET_ERROR (-1)
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/un.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int Started=0;

void Startup(void)
{
 #ifdef WINDOWSNT
 WORD wVersionRequested;
 WSADATA wsaData;
 int err;
 // struct servent *srv;
 wVersionRequested = MAKEWORD(2,0);
 // Do WINSOCK initialization
 err = WSAStartup(wVersionRequested, &wsaData);
 if ( err != 0 )
  {
   puts("WSAStartup failed");
   exit(0);
  }
 Started=1;
 #endif
}

void Shutdown(char* msg)
{
 if ( msg != NULL )
  puts(msg);
  
 #ifdef WINDOWSNT
 if (Started)
   WSACleanup();
 #endif
 exit(0);
}

void Error(char* String)
{
 #ifdef SCOUNIX
 perror(String);
 #endif
 #ifdef WINDOWSNT
 printf("%s: WSAGETLASTERROR=%d\n",String,WSAGetLastError());
 #endif
 Shutdown(0);
}

void UsageIs(void)
{
 puts("Usage is UNIXGRAM S[end] sourceportname destportname length or\n"
      "                  R[ecv] portname sourceportname destporname maxlength");
 Shutdown(0);
}

//--------------
// Send Messages
//--------------
void Send(char** args)
{
 int Length=atoi(args[4]);
 int s;
 char *BufOut;
 char *BufIn;
 int i;
 struct sockaddr_un addr;
 struct sockaddr from;
 int fromlen;
 int count;

 // Create Socket
 if ( (s=socket(AF_UNIX,SOCK_DGRAM,0)) == INVALID_SOCKET )
  Error("SOCKET CREATION FAILED");
 // Get a buffer
 if ( (BufOut=malloc(Length)) == NULL ||
      (BufIn=malloc(Length)) == NULL )
  Shutdown("NOT ENOUGH MEMORY");

 // bind to the port
  // Bind the socket to OUR port
 memset(&addr,0,sizeof(addr));
 addr.sun_family = AF_UNIX;
 strcpy(addr.sun_path,args[2]);
 unlink(addr.sun_path);

 if ( bind(s,(struct sockaddr *)&addr,
           sizeof(addr)) == SOCKET_ERROR )
  Error("BIND ERROR");


 memset(&addr,0,sizeof(addr));
 addr.sun_family = AF_UNIX;
 strcpy(addr.sun_path,args[3]);

 printf("SENDING DATAGRAMS ...\n");
 #ifdef SCOUNIX
 fflush(stdout);
 #endif
 for(count=0;;++count)
  {
   for (i=0; i<Length; ++i)
    BufOut[i] = (unsigned char)rand();
   if ( sendto(s,BufOut,Length,
        0,(struct sockaddr*)&addr,sizeof(addr)) != Length )
     Error("SENDTO ERROR");
   fromlen=sizeof(from);
   if ( recvfrom(s,BufIn,Length,0,&from,&fromlen) != Length )
     Error("RECV ERROR");
   if ( memcmp(BufIn,BufOut,Length) )
     Shutdown("Buffer Mismatch");
   printf("\r%d",count);
   #ifdef SCOUNIX
   fflush(stdout);
   #endif
  }

}


//-----------------
// Receive messages
//-----------------
void Recv(char** args)
{
 int MaxLength=atoi(args[4]);
 int Length;
 int s;
 char *BufIn;
 struct sockaddr_un addr;
 struct sockaddr from;
 int fromlen;

 // Create Socket
 if ( (s=socket(AF_UNIX,SOCK_DGRAM,0)) == INVALID_SOCKET )
  Error("SOCKET CREATION FAILED");
 // Get a buffer
 if ( (BufIn=malloc(MaxLength)) == NULL )
  Shutdown("NOT ENOUGH MEMORY");

 // bind to the port
  // Bind the socket to OUR port
 memset(&addr,0,sizeof(addr));
 addr.sun_family = AF_UNIX;
 strcpy(addr.sun_path,args[2]);
 unlink(addr.sun_path);

 if ( bind(s,(struct sockaddr *)&addr,
           sizeof(addr)) == SOCKET_ERROR )
  Error("BIND ERROR");

 printf("RECEIVING DATAGRAMS....\n");
 for(;;)
  {
   fromlen=sizeof(from);
   if ( (Length=recvfrom(s,BufIn,MaxLength,0,&from,&fromlen)) < 0 )
     Error("RECV ERROR");
   addr.sun_family = AF_UNIX;
   strcpy(addr.sun_path,args[3]); 
   if ( sendto(s,BufIn,Length,0,(struct sockaddr*)&addr,sizeof(addr)) != Length )
     Error("SENDTO ERROR");
  }

}

void main(int argc,char *argv[])
{
 Startup();

 if ( argc < 2 )
  UsageIs();

 switch( argv[1][0] )
  {
   case 'S':
   case 's':
             if ( argc != 5 )
               UsageIs();
             Send(argv);
             break;
   case 'R':
   case 'r':
             if ( argc != 5 )
               UsageIs();
             Recv(argv);
             break;
   default: UsageIs();
  }

 Shutdown(0);
}

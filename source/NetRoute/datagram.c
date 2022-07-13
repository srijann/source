#define WINDOWSNT
//#define SCOUNIX

#ifdef WINDOWSNT
#include <winsock.h>
#endif

#ifdef SCOUNIX
#define INVALID_SOCKET (-1)
#define SOCKET_ERROR (-1)
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
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
 puts("Usage is DATAGRAM S[end] host port length or\n"
      "                  R[ecv] port maxlength");
 Shutdown(0);
}

//--------------
// Send Messages
//--------------
void Send(char** args)
{
 struct hostent* Name=gethostbyname(args[2]);
 unsigned short port=(unsigned short)atoi(args[3]);
 int Length=atoi(args[4]);
 int s;
 char *BufOut;
 char *BufIn;
 int i;
 struct sockaddr_in addr;
 struct sockaddr from;
 int fromlen;
 int count;

 if ( Name==NULL )
  {
   printf("HOST '%s' NOT FOUND",args[2]);
   Shutdown(0);
  }

 // Create Socket
 if ( (s=socket(AF_INET,SOCK_DGRAM,0)) == INVALID_SOCKET )
  Error("SOCKET CREATION FAILED");
 // Get a buffer
 if ( (BufOut=malloc(Length)) == NULL ||
      (BufIn=malloc(Length)) == NULL )
  Shutdown("NOT ENOUGH MEMORY");

 // bind to the port
  // Bind the socket to OUR port
 //memset(&addr,0,sizeof(addr));
 //addr.sin_family = AF_INET;
 //addr.sin_addr.s_addr = htonl (INADDR_ANY) ;
 //addr.sin_port = htons(port);

 //if ( bind(s,(struct sockaddr *)&addr,
 //          sizeof(addr)) == SOCKET_ERROR )
 // Error("BIND ERROR");

 memset(&addr,0,sizeof(addr));
 addr.sin_family = AF_INET;
 addr.sin_port = htons(port);
 memcpy(&addr.sin_addr.s_addr,Name->h_addr_list[0],4);

 printf("SENDING DATAGRAMS ...\n");
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
  }

}


//-----------------
// Receive messages
//-----------------
void Recv(char** args)
{
 unsigned short port=(unsigned short)atoi(args[2]);
 int MaxLength=atoi(args[3]);
 int Length;
 int s;
 char *BufIn;
 struct sockaddr_in addr;
 struct sockaddr from;
 int fromlen;

 // Create Socket
 if ( (s=socket(AF_INET,SOCK_DGRAM,0)) == INVALID_SOCKET )
  Error("SOCKET CREATION FAILED");
 // Get a buffer
 if ( (BufIn=malloc(MaxLength)) == NULL )
  Shutdown("NOT ENOUGH MEMORY");

 // bind to the port
  // Bind the socket to OUR port
 memset(&addr,0,sizeof(addr));
 addr.sin_family = AF_INET;
 addr.sin_addr.s_addr = htonl (INADDR_ANY) ;
 addr.sin_port = htons(port);

 if ( bind(s,(struct sockaddr *)&addr,
           sizeof(addr)) == SOCKET_ERROR )
  Error("BIND ERROR");

 printf("RECEIVING DATAGRAMS....\n");
 for(;;)
  {
   fromlen=sizeof(from);
   if ( (Length=recvfrom(s,BufIn,MaxLength,0,&from,&fromlen)) < 0 )
     Error("RECV ERROR");
   if ( sendto(s,BufIn,Length,0,(struct sockaddr*)&from,fromlen) != Length )
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
             if ( argc != 4 )
               UsageIs();
             Recv(argv);
             break;
   default: UsageIs();
  }

 Shutdown(0);
}

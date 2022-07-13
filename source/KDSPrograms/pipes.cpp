//-----------------------------------------------
// Pipes.C
//
//  Encapsulate behavior of Pipes in a C++ object
//-----------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stropts.h>
#include <poll.h>
#include <errno.h>
#include "defs.h"
#include "pipes.h"
#include "spslog.h"

// There must be a global declaration of this object for error logging
extern SPSLogger Logger;
#define BUFLEN 300

//------------------
// Class constructor
//------------------
Pipe::Pipe(char* Name) : Fd(-1)
{
 PipeName=new char[strlen(Name)+1];
 strcpy(PipeName,Name);
 IOBuf=new char[BUFLEN];
 Open();
}

//-----------------
// Class destructor
//-----------------
Pipe::~Pipe()
{
 delete[] PipeName;
 delete[] IOBuf;
}

//-------------------------------------------------------------
// Open the pipe, retrying up to 5 times with a 1 second delay.
// Return true or false
//
// This is a private member function
//-------------------------------------------------------------
int Pipe::Open()
{
 int Retries;

 // If already open, do nothing
 if ( Fd > 0 )
   return true;

 // Loop until it's open
 for (Retries=0; Retries<5; ++Retries)
   if ( (Fd=open(PipeName,O_RDWR)) == -1 )
    sleep(1);

 if ( Fd == -1 )
  {
   Logger.IOError("open",PipeName);
   return false;
  }

 return true;
}

//---------------
// Flush the pipe
//
//  Return true or false to indicate success
//---------------
bool Pipe::Flush()
{
 struct pollfd fds;

 if ( ! Open() )
   return false;

 for (;;)
  {
   fds.fd=Fd;
   fds.events = POLLIN;
   switch( poll(&fds,1,0) )
    {
     case 0:  return true;
     case -1: Logger.IOError("poll",PipeName);
              close(Fd);
              Fd=-1;
              return false;
     default: if ( read(Fd,IOBuf,BUFLEN) == -1 )
               {
                Logger.IOError("read",PipeName);
                close(Fd);
                Fd=-1;
                return false;
               }
              break;
    }
  }

}

//-------------------------------------------
// Read from Pipe, waiting specified interval
//-------------------------------------------
int Pipe::Read(char* Buf,int Len,int Wait)
{
 struct pollfd fds;
 int ret;

 for (;;)
  {
   if ( ! Open() )
     return false;
   fds.fd=Fd;
   fds.events = POLLIN;
   while( (ret=poll(&fds,1,Wait)) == -1 && errno==EAGAIN ) sleep(1);
   if ( ret==-1 )
    {
     Logger.IOError("poll",PipeName);
     Fd = -1;
     return -1;
    }
   if ( ret==0 )
    return 0;
    
   if ( (Len=read(Fd,Buf,Len)) == -1 )
    {
     Logger.IOError("read",PipeName);
     close(Fd);
     Fd = -1;
     return -1;
    }

   // This is an End sentinel that tells us to close the Pipe and
   // start over.
   if ( memcmp(Buf,"$END$",5) ==0 )
    {
     close(Fd);
     Fd=-1;
     continue;
    }
    
   return Len;
  }

}

//--------------
// Write to Pipe
//--------------
bool Pipe::Write(char* Buf,int Len)
{
 int n;

 if ( ! Open() )
   return false;

 memset(IOBuf,' ',257);
 if ( Len > 257 )
   Len=257;
 memcpy(IOBuf,Buf,Len);
 Len=257;

 while( Len )
  {
   if ( (n = write(Fd,IOBuf,Len)) == -1 )
    {
     Logger.IOError("write",PipeName);
     close(Fd);
     Fd=-1;
     return false;
    }
   Len -= n;
  }

 return true;
}

//----------------------------------
// Send an Abort message on the pipe
//----------------------------------
bool Pipe::Abort()
{
 return Write("$ABT$",5);
}


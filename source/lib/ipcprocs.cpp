
#include <windows.h>
#include <dir.h>
#include <owl/profile.h>
#include <string.h>
#include "ipcmsg.h"

#define ERRMSG(text) ::MessageBox(0,text,"APPLICATION MANAGER",MB_OK|MB_ICONSTOP)

static HANDLE hMem;         // Handle for shared memory
static HANDLE hMbox;        // Handle for Mailbox Table
static HANDLE hBuf;         // Handle for Buffer Pool

bool IPCInit(void)
{
 TProfile *profile;
 int Bufs,MailBoxes;
 int size;

 hMem=OpenFileMapping(FILE_MAP_WRITE,true,"APPMEM");
 if ( hMem==NULL )
  {
   profile = new TProfile("ApplMan","APPL.INI");
   if ( (Bufs=profile->GetInt("Bufs",-1)) == -1 )
    {
     ERRMSG("Missing Parameter in APPL.INI file - 'Bufs'");
     return(false);
    }
   if ( (MailBoxes=profile->GetInt("MailBoxes",-1)) == -1 )
    {
     ERRMSG("Missing Parameter in APPL.INI file - 'MailBoxes'");
     return(false);
    }

   delete profile;

   size = sizeof(appmem->hdr) + MailBoxes * sizeof(struct mailbox_def) +
          Bufs * (sizeof(class MessageBase));

   hMem = CreateFileMapping((HANDLE)0xffffffff,
                               NULL,
                               PAGE_READWRITE,
                               0,size,
                               "APPMEM");
    if ( hMem==NULL )
     {
      ERRMSG("CreateFileMapping FAILED");
      return(false);
     }

    appmem = (struct appmem_def *) MapViewOfFile(hMem,FILE_MAP_WRITE,0,0,0);
    if ( appmem==NULL )
     {
      ERRMSG("MapViewOfFile FAILED");
      return(false);
     }

   memset(appmem,0,size);

   // Set up the Header
   appmem->hdr.MaxMailBoxes=MailBoxes;
   appmem->hdr.MaxBuffers=Bufs;

   // Set pointer to Mailbox Entries (after header)
   appmem->mbox=(struct mailbox_def *)
     &((char *)appmem)[sizeof(appmem->hdr)];

   // Set pointer to Buffers (after mailbox entries)
   appmem->buf=(struct buf_def *)
     &((char*)appmem->mbox)[sizeof(appmem->mbox) * MailBoxes];

   memcpy(appmem->hdr.id,"APPMEM",sizeof(appmem->hdr.id));
   ERRMSG("CREATED SHARED MEMORY");
  }

 // Create a the Mailbox Table Semaphore
 hMbox=CreateSemaphore(NULL,1,1,"APPMEM_MBOX");
 if ( hMbox==NULL )
  {
   ERRMSG("CreateSemaphore FAILED");
   return(false);
  }
 if ( GetLastError() != ERROR_ALREADY_EXISTS )
  ERRMSG("CREATED MAILBOX SEMAPHORE");

 // Create a Semaphore for Buffer Access
 hBuf=CreateSemaphore(NULL,1,1,"APPMEM_BUFS");
 if ( hBuf==NULL )
  {
   ERRMSG("CreateSemaphore FAILED");
   return(false);
  }
 if ( GetLastError() != ERROR_ALREADY_EXISTS )
   ERRMSG("CREATED BUFFER SEMAPHORE");

 return(true);
}



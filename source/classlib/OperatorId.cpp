#include "win4me.h"
#include "OperatorId.h"
#include <stdlib.h>
#include <stdio.h>

//----------------------------------------------------------------------
// Class User ID encapsulates User Id Info
//
// OperatorId is a number that comes from the current logon. All logons must
// begin with a number for this to work correctly.
//
// If logon doesn't begin with a number, 1 is returned by both of these
// functions.
//----------------------------------------------------------------------

// Get as DWORD
DWORD COperatorId::Get()
{
 char UserName[50];
 DWORD size=sizeof(UserName);
 if ( ! GetUserName(UserName,&size) ||
      ! isdigit(UserName[0]) )
  return 1;
 else
  return atoi(UserName);
}

// Get as fixed-width Character string (not null-terminated)
void COperatorId::Get(char* Buf,int BufLen)
{
 char fmtBuf[25];
 BufLen=min((uint)BufLen,sizeof(fmtBuf)-1);
 sprintf(fmtBuf,"%0*d",BufLen,Get());
 memcpy(Buf,fmtBuf,BufLen);
}

#include "win4me.h"

DWORD GetUserId()
{
 char UserName[50];
 DWORD size=sizeof(UserName);
 if ( ! GetUserName(UserName,&size) ||
      ! isdigit(UserName[0]) )
  return 1;
 else
  return atoi(UserName);
}

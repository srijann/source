#define STRICT
#include <windows.h>
#include <iostream.h>
#include <conio.h>
#include <stdio.h>
#include "rds.h"

static RDM_SESS sessionHandle;

DBD_DBF regdbase;
FILEDEV filedev[2];

struct device dev;

void quit(int ecode)
{
 if ( sessionHandle )
  {
   d_close(sessionHandle);
   s_logout(sessionHandle);
  }
 exit(ecode);
}

void main(void)
{
 short status;

 // LOGIN
 if ( (status=s_login("rds","admin","secret",&sessionHandle)) != S_OKAY )
  {
   cout << "s_login returned " << status << "\n";
   quit(0);
  }

 // Add the new Database
 strcpy(regdbase.dbdfiledev,"catdev");
 strcpy(regdbase.db.dbname,"BankPhoneDB");
 regdbase.db.dbaccess = 0;

 strcpy(filedev[0].fname,"BankPhon.dat");
 strcpy(filedev[0].fdevice,"catdev");
 strcpy(filedev[1].fname,"BankNum.key");
 strcpy(filedev[1].fdevice,"catdev");

 if ( (status=s_dbAdd(&regdbase,filedev,2 * sizeof(FILEDEV),
                       sessionHandle)) != S_OKAY )
  {
   cout << "s_dbAdd returned " << status << "\n";
   quit(0);
  }

 // Initialize the New Database
 if ( (status=s_dbInit("BankPhoneDB",sessionHandle)) != S_OKAY )
  {
   cout << "s_dbinit returned " << status << "\n";
   quit(0);
  }

 d_close(sessionHandle);

 // Success
 cout << "Database Created\n";
 quit(1);
}

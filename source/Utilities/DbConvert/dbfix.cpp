#define STRICT
#include <windows.h>
#include <iostream.h>
#include <conio.h>
#include <stdio.h>
#define NOFLDNAMES
#include "DataReq.h"
#include "rds.h"

static RDM_SESS SessionHandle;
static RDM_DB   DBHandle;

DataRequirements DataReq;

void quit(int ecode)
{
 if ( SessionHandle )
  {
   d_close(SessionHandle);
   s_logout(SessionHandle);
  }
 exit(ecode);
}

void fix()
{
 int Type;
 int Field;

 for( Type=0; Type<3; ++Type)
  {
   for (Field=DataReq.FINANCEAMOUNT; Field<32; ++Field)
    DataReq.setRequired(Type,Field,false);
   for (Field=DataReq.SPOUSESALARY+1; Field<64; ++Field)
    DataReq.setRequired(Type,Field,false);
  }

}


void main(void)
{
 short status;

 // LOGIN
 if ( (status=s_login("rds","admin","secret",&SessionHandle)) != S_OKAY )
  {
   cout << "s_login returned " << status << "\n";
   quit(0);
  }

 // Open Old Manager Database
 if ( (status=d_open("SPSManV21","xq",SessionHandle,&DBHandle)) != S_OKAY )
  {
   cout << "d_open of SPSManV21 returned " << status << "\n";
   quit(0);
  }


 // Position old DB to DataRequirements Record
 if ( (status=d_keyfrst(DataReq.KeyType(),DBHandle)) != S_OKAY )
  {
   cout << "d_recfrst returned " << status << "\n";
   quit(0);
  }

 // Read each record, fix, and add to new file
 while( d_keynext(DataReq.KeyType(),DBHandle) == S_OKAY )
  {
   if ( (status=d_recread(DATAREQRECORD,&DataReq.Data,DBHandle)) != S_OKAY )
    {
     cout << "d_recread returned " << status << "\n";
     quit(0);
    }

   cout << "fixing " << DataReq.Data.DataReqClassName << "\n";
   fix();

   if ( (status=d_recwrite(DATAREQRECORD,&DataReq.Data,DBHandle)) != S_OKAY )
    {
     cout << "d_recwrite returned " << status << "\n";
     quit(0);
    }
  }

 // Close the database


 // Success
 cout << "Fixup Complete\n";
 quit(1);
}

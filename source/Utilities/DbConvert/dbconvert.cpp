#define STRICT
#include <windows.h>
#include <iostream.h>
#include <conio.h>
#include <stdio.h>
#define NOFLDNAMES
#include "SPSManV21.h"
#include "SPSManDB.h"
#include "rds.h"
#include "DataReq.h"

static RDM_SESS oldSessionHandle;
static RDM_SESS newSessionHandle;
static RDM_DB   oldDBHandle;
static RDM_DB   newDBHandle;

DBD_DBF regdbase;
FILEDEV filedev[20];

struct DataRequirements DataReq;
struct OldAuthRulesRecord oldRec;
struct AuthRulesRecord newRec;
struct device dev;

void quit(int ecode)
{
 if ( oldSessionHandle )
  {
   d_close(oldSessionHandle);
   s_logout(oldSessionHandle);
  }
 if ( newSessionHandle )
  {
   d_close(newSessionHandle);
   s_logout(newSessionHandle);
  }
 exit(ecode);
}

//--------------------------------------------
// Do a FIX up on the DataRequirements Records
//--------------------------------------------
void fixDataReq(RDM_DB DBHandle)
{
 int Type;
 int Field;
 short status;

 cout << "FIXING DATA REQUIREMENT RECORDS....\n";

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

   for( Type=0; Type<3; ++Type)
    {
     for (Field=DataReq.FINANCEAMOUNT; Field<32; ++Field)
      DataReq.setRequired(Type,Field,false);
     for (Field=DataReq.SPOUSESALARY+1; Field<64; ++Field)
      DataReq.setRequired(Type,Field,false);
     if ( ! DataReq.isRequired(Type,DataReq.EMPLOYMENTDATA) )
      for (Field=DataReq.EMPLOYERNAME; Field <= DataReq.SALARY; ++Field)
        DataReq.setRequired(Type,Field,false);
     if ( ! DataReq.isRequired(Type,DataReq.SPOUSEDATA) )
      for (Field=DataReq.SPOUSENAME; Field <= DataReq.SPOUSESALARY; ++Field)
        DataReq.setRequired(Type,Field,false);
    }

   if ( (status=d_recwrite(DATAREQRECORD,&DataReq.Data,DBHandle)) != S_OKAY )
    {
     cout << "d_recwrite returned " << status << "\n";
     quit(0);
    }
  }

}

//----------------------------------
// Do a conversion on the Auth Rules
//-----------------------------------
void convertAuthRules()
{
 short status;

 cout << "CONVERTING AUTHRULES...\n";

 // Position old DB to DataRequirements Record
 if ( (status=d_keyfrst(AUTHRULESKEY,oldDBHandle)) != S_OKAY )
  {
   cout << "d_recfrst returned " << status << "\n";
   quit(0);
  }

 memset(&newRec,0xff,sizeof(newRec));


 // Read each record, convert, and add to new file
 while( d_keynext(AUTHRULESKEY,oldDBHandle) == S_OKAY )
  {
   if ( (status=d_recread(AUTHRULESRECORD,&oldRec,oldDBHandle)) != S_OKAY )
    {
     cout << "d_recread returned " << status << "\n";
     quit(0);
    }

   cout << "Converting " << oldRec.AuthRulesKey.ClassName;
   if ( oldRec.AuthRulesKey.SubClassName[0] )
    cout << "." << oldRec.AuthRulesKey.SubClassName;
   cout << "\n";

   // All we're actually doing is making more room for product
   // adjustment values
   memcpy(&newRec,&oldRec,sizeof(oldRec));

   if ( (status=d_fillnew(AUTHRULESRECORD,&newRec,newDBHandle)) != S_OKAY )
    {
     cout << "d_fillnew returned " << status << "\n";
     quit(0);
    }
  }

}

void main(void)
{
 short status;

 // LOGIN
 if ( (status=s_login("rds","admin","secret",&oldSessionHandle)) != S_OKAY ||
      (status=s_login("rds","admin","secret",&newSessionHandle)) != S_OKAY )
  {
   cout << "s_login returned " << status << "\n";
   quit(0);
  }

 // Open Old Manager Database
 if ( (status=d_open("SPSManV21","xq",oldSessionHandle,&oldDBHandle)) != S_OKAY )
  {
   cout << "d_open of SPSManV21 returned " << status << "\n";
   quit(0);
  }

 // Add Temp Device to Catalog
 strcpy(dev.devname,"tempdev");
 strcpy(dev.devpath,"C:\\VELOCIS\\TEMP");
 if ( (status=s_devAdd(&dev,newSessionHandle)) != S_OKAY &&
      status != S_DEVEXISTS )
  {
   cout << "s_devAdd of tempdev returned " << status << "\n";
   quit(0);
  }


 // Add the new Database
 strcpy(regdbase.dbdfiledev,"catdev");
 strcpy(regdbase.db.dbname,"SPSManV22");
 regdbase.db.dbaccess = 0;

 strcpy(filedev[0].fname,"AuthRule.dat");
 strcpy(filedev[0].fdevice,"tempdev");
 strcpy(filedev[1].fname,"AuthRule.key");
 strcpy(filedev[1].fdevice,"tempdev");
 strcpy(filedev[2].fname,"DataReq.dat");
 strcpy(filedev[2].fdevice,"tempdev");
 strcpy(filedev[3].fname,"DataReq.key");
 strcpy(filedev[3].fdevice,"tempdev");
 strcpy(filedev[4].fname,"SecRules.dat");
 strcpy(filedev[4].fdevice,"tempdev");
 strcpy(filedev[5].fname,"SecRules.key");
 strcpy(filedev[5].fdevice,"tempdev");
 strcpy(filedev[6].fname,"Merchant.dat");
 strcpy(filedev[6].fdevice,"tempdev");
 strcpy(filedev[7].fname,"Merchant.key");
 strcpy(filedev[7].fdevice,"tempdev");
 strcpy(filedev[8].fname,"Merchnam.key");
 strcpy(filedev[8].fdevice,"tempdev");
 strcpy(filedev[9].fname,"Response.dat");
 strcpy(filedev[9].fdevice,"tempdev");
 strcpy(filedev[10].fname,"Response.key");
 strcpy(filedev[10].fdevice,"tempdev");
 strcpy(filedev[11].fname,"PosRules.dat");
 strcpy(filedev[11].fdevice,"tempdev");
 strcpy(filedev[12].fname,"PosRules.key");
 strcpy(filedev[12].fdevice,"tempdev");
 strcpy(filedev[13].fname,"Lists.dat");
 strcpy(filedev[13].fdevice,"tempdev");
 strcpy(filedev[14].fname,"Lists.key");
 strcpy(filedev[14].fdevice,"tempdev");

 s_dbDel("SPSManV22",newSessionHandle);  /* In case it already exists */
 if ( (status=s_dbAdd(&regdbase,filedev,15 * sizeof(FILEDEV),
                       oldSessionHandle)) != S_OKAY )
  {
   cout << "s_dbAdd of SPSManV22 returned " << status << "\n";
   quit(0);
  }

 // Initialize the New Database
 if ( (status=s_dbInit("SPSManV22",newSessionHandle)) != S_OKAY )
  {
   cout << "s_dbinit returned " << status << "\n";
   quit(0);
  }

 // Open the New Database
 if ( (status=d_open("SPSManV22","xq",newSessionHandle,&newDBHandle)) != S_OKAY )
  {
   cout << "d_open of TempDb returned " << status << "\n";
   quit(0);
  }

 // Do Conversion/Fix stuff
 fixDataReq(oldDBHandle);
 convertAuthRules();

 // Success
 cout << "Conversion Complete\n";
 // Database files must now be copied from VELOCIS Catalog to
 // the new device SPSDev1
 quit(1);
}

#include "dbload.h"
#include "SPSLogDB.h"

#define MAX_OVERFLOWSEGMENTS 6

static struct LogRecord Rec;

//---------------------
// Load the Log File
//---------------------
void LoadLogFile()
{
 Open("SPSLogDB");
 cout << "Loading Log File..." << endl;
 CopyTemp("LogFile\\SPSLog");

 cout << " Rebuilding Database..." << endl;
 int Status;
 int RestoredCount=0;
 int Cnt;
 while ( (Cnt=fread(&Rec,1,sizeof(Rec),SaveFile)) != 0)
 {
   if ( Cnt != sizeof(Rec) )
    {
     cout << endl << "  fread failed: " << strerror(errno) << endl;
     quit(EXIT_FAILURE);
    }
   if ( (Status=d_fillnew(LOGRECORD,&Rec,DBHandle)) != S_OKAY )
    {
     if ( Status==S_DUPLICATE )
      continue;
     cout << endl << "  d_fillnew returned " << Status << endl;
     quit(EXIT_FAILURE);
    }
   // Store the optional keys
   if ( Rec.PhoneNumber[0] &&
        (Status=d_keystore(PHONEKEY,DBHandle)) != S_OKAY )
    {
     cout << "  d_keystore returned " << Status << endl;
     quit(EXIT_FAILURE);
    }

   if ( Rec.BankNumber[0] &&
       (Status=d_keystore(MICRKEY,DBHandle)) != S_OKAY )
    {
     cout << "  d_keystore returned " << Status << endl;
     quit(EXIT_FAILURE);
    }

   if ( Rec.DriversLicense[0] &&
       (Status=d_keystore(LICENSEKEY,DBHandle)) != S_OKAY )
    {
     cout << "  d_keystore returned " << Status << endl;
     quit(EXIT_FAILURE);
    }

   if ( Rec.SSn &&
       (Status=d_keystore(SSNKEY,DBHandle)) != S_OKAY )
    {
     cout << "  d_keystore returned " << Status << endl;
     quit(EXIT_FAILURE);
    }

   if ( Rec.ApprovalNumber &&
       (Status=d_keystore(APPROVALNUMBERKEY,DBHandle)) != S_OKAY )
    {
     cout << "  d_keystore returned " << Status << endl;
     quit(EXIT_FAILURE);
    }

   cout << "  " << ++RestoredCount << "\r";
  }

 cout << "  " << RestoredCount << " Records Restored" << endl;

 CloseAll();
}




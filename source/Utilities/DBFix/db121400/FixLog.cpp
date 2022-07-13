#include "dbfix.h"

#include "SPSLogDB.h"
#include "ipc.h"
#include "CheckAuthFn.h"
#include "time.h"

//-----------------
// Fix the Log File
//-----------------
#include "PreauthDB.h"
void FixLogFile()
{
 struct LogRecord Rec;

 Open("SPSLogDB");
 cout << "Fixing Log File..." << endl;
 CopyTemp(LOGRECORD,"Logfile\\SPSLog",(char*)&Rec,sizeof(Rec));
 Init("SPSLogDB");

 cout << " Rebuilding Database..." << endl;
 int N;
 int Status;
 int RestoredCount=0;
 for (N=0; N<RecordCount; ++N)
  {
   if ( fread(&Rec,1,sizeof(Rec),SaveFile) != sizeof(Rec) )
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
   cout << "  " << ++RestoredCount << "\r";
  }
 cout << "  " << RestoredCount << " Records Restored" << endl;

 CloseAll();
}







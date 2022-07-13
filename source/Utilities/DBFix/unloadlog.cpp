#include "dbunload.h"
//---------------------
// Unload the Log File
//---------------------
#include "SPSLogDB.h"
void UnloadLogFile()
{
 struct LogRecord Rec;

 Open("SPSLogDB");
 cout << "Unloading Log File..." << endl;
 CopyTemp(LOGRECORD,"LogFile\\SPSLog",(char*)&Rec,sizeof(Rec));
 CloseAll();
}


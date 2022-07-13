#include "dbunload.h"
#include "OLD_SPSLogDB.h"

#define MAX_OVERFLOWSEGMENTS 6

static struct
 {
  LogRecord primary;
  OverflowRecord segment[MAX_OVERFLOWSEGMENTS];
 } logRec;

//----------------------------------------------------------
// Read Log record and all sets into global logRec buffer
//----------------------------------------------------------
bool dbRead()
{
 short status=d_recread(LOGRECORD,&logRec,DBHandle);
 int   Len;
 int   i;

 if ( status != S_OKAY )
  {
   cout << "d_recread returned " << status << endl;
   quit(EXIT_FAILURE);
  }

 // Check the time to see if it's 90 days old
 

 // Overflow Segments?
 if ( (Len=logRec.primary.MsgLength -
           sizeof(logRec.primary.PrimarySegment)) <= 0 )
  return true;

 // Read Overflow Segments
 d_setor(OVERFLOWSEGMENT,DBHandle);
 for(i=0; Len >0; Len-=sizeof(OverflowRecord), ++i)
  {
   if ( (status=d_findnm(OVERFLOWSEGMENT,DBHandle)) != S_OKAY )
     return false;
   if ( (status=d_recread(SEGMENT,&logRec.segment[i],DBHandle)) != S_OKAY )
     return false;
  }

 return true;
}


//--------------------------------------
// Copy all records of file to temp file
//--------------------------------------
static void CopyTemp(short Type,const char* Name)
{
 int Status;
 int RecordsDiscarded=0;
 RecordCount=0;

 strcpy(SaveFileName,DBPATH);
 strcat(SaveFileName,Name);
 strcat(SaveFileName,".tmp");

 cout << " Unloading Records to " << SaveFileName << "..." << endl;

 if ( (SaveFile=fopen(SaveFileName, "w+b")) == NULL )
  {
   cout << "fopen of " << Name << " failed: " << strerror(errno) << endl;
   quit(EXIT_FAILURE);
  }

 if ( (Status=d_recfrst(Type,DBHandle)) != S_OKAY )
  {
   if ( Status==S_NOTFOUND )
    {
     cout << "  " << RecordCount << " Records Saved" << endl;
     return;
    }
   cout << "  d_recfrst returned " << Status << endl;
   quit(EXIT_FAILURE);
  }

 do
  {
   if ( ! dbRead() )
     ++RecordsDiscarded;
   else
    {
     if ( fwrite(&logRec,1,sizeof(logRec),SaveFile) != sizeof(logRec) )
      {
       cout << endl << "  fwrite failed: " << strerror(errno) << endl;
       quit(EXIT_FAILURE);
      }
     cout << "  " << ++RecordCount << "\r";
    }
  }
 while( (Status=d_recnext(DBHandle)) == S_OKAY );

 cout << "  " << RecordCount << " Records Saved. " <<
      RecordsDiscarded << " Records Discarded" << endl;

 if ( Status != S_NOTFOUND )
  {
   cout << "d_recnext returned " << Status << endl;
   quit(EXIT_FAILURE);
  }

 if ( fseek(SaveFile,0,SEEK_SET) == -1 )
  {
   cout << "  fseek failed: " << strerror(errno) << endl;
   quit(EXIT_FAILURE);
  }
}

void dbWrite()
{
 short status;
 //char* RecPtr=(char*)Msg.GetMsgPointer() + sizeof(IPCROUTEINFO);
 int   Index;

  // Write Primary record
 if ( (status = d_fillnew(LOGRECORD,&logRec,DBHandle)) != S_OKAY )
  {
   cout << "  d_fillnew returned " << status << endl;
   quit(EXIT_FAILURE);
  }

 // Store the optional keys
 if ( logRec.primary.PhoneNumber[0] &&
      (status=d_keystore(PHONEKEY,DBHandle)) != S_OKAY )
  {
   cout << "  d_keystore returned " << status << endl;
   quit(EXIT_FAILURE);
  }

 if ( logRec.primary.BankNumber[0] &&
     (status=d_keystore(MICRKEY,DBHandle)) != S_OKAY )
  {
   cout << "  d_keystore returned " << status << endl;
   quit(EXIT_FAILURE);
  }

 if ( logRec.primary.DriversLicense[0] &&
     (status=d_keystore(LICENSEKEY,DBHandle)) != S_OKAY )
  {
   cout << "  d_keystore returned " << status << endl;
   quit(EXIT_FAILURE);
  }

 if ( logRec.primary.SSn &&
     (status=d_keystore(SSNKEY,DBHandle)) != S_OKAY )
  {
   cout << "  d_keystore returned " << status << endl;
   quit(EXIT_FAILURE);
  }

 if ( logRec.primary.ApprovalNumber &&
     (status=d_keystore(APPROVALNUMBERKEY,DBHandle)) != S_OKAY )
  {
   cout << "  d_keystore returned " << status << endl;
   quit(EXIT_FAILURE);
  }

 // Write Overflow Segments, if any
 int SegNum=0;
 if ( logRec.primary.MsgLength > sizeof(logRec.primary.PrimarySegment) )
  {
   d_setor(OVERFLOWSEGMENT,DBHandle);
   Index = sizeof(logRec.primary.PrimarySegment);
   while( Index < logRec.primary.MsgLength )
    {
     int Len;
     Len = min( (int)sizeof(OverflowRecord),
                (int)logRec.primary.MsgLength-Index);
     if ( (status=d_fillnew(OVERFLOWRECORD,&logRec.segment[SegNum],DBHandle)) != S_OKAY )
      {
       cout << "  d_fillnew returned " << status << endl;
       quit(EXIT_FAILURE);
      }
     if ( (status=d_connect(OVERFLOWSEGMENT,DBHandle)) != S_OKAY )
      {
       cout << "  d_connect returned " << status << endl;
       quit(EXIT_FAILURE);
      }
     Index += Len;
     ++SegNum;
    }
  }
}

//-----------------
// Unload the Log File
//-----------------
void UnloadLogFile()
{
 Open("SPSLogDB");
 cout << "Unloading Log File..." << endl;
 CopyTemp(LOGRECORD,"LOGFILE\\SPSLog");
 CloseAll();
}


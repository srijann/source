//---------
// SPSLog.C
//---------
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "spslog.h"

//------------
// Constructor
//------------
SPSLogger::SPSLogger(char* ProgName) : ErrorCount(0) 
{
 ProgramName = new char[strlen(ProgName)+1];
 strcpy(ProgramName,ProgName);
}

//-----------
// Destructor
//-----------
SPSLogger::~SPSLogger()
{
 delete[] ProgramName;
}

//--------------
// Log a message
//--------------
void SPSLogger::Log(char *FmtStr,...)
{
 va_list argptr;
 FILE *f=fopen(LOGFILENAME,"a");
 time_t now;
 struct tm* tblock;

 now = time(NULL);
 tblock = localtime( &now ); 

 if ( f==NULL )
   return;

 if ( ErrorCount < NOLOGTHRESHOLD )
  {
   fprintf(f,"%s @ %s",ProgramName,asctime(tblock));
   va_start(argptr,FmtStr);
   vfprintf(f,FmtStr,argptr);
   if ( FmtStr[strlen(FmtStr)-1] == '\n' )
     fprintf(f,"\n");
   else
     fprintf(f,"\n\n");
   fclose(f);    
  } 

 if ( ErrorCount == 0 ||
      now - ErrorTS >= 60*60 )
    {
     ErrorTS=now;
     ErrorCount=1;  
    } 
 else
   if ( ++ErrorCount > ABORTTHRESHOLD )
     {
      fprintf(f,"ABORTED DUE TO TOO MANY ERRORS");
      exit(1); 
     }
         
 fclose(f);
}

//-------------------------
// Log an I/O Error message
//-------------------------
void SPSLogger::IOError(char* Function,char* FileName)
{
 if ( FileName )
  Log("I/O ERROR: Function=%s File=%s\n%s\n",Function,FileName,strerror(errno));
 else
  Log("I/O ERROR: Function=%s\n%s\n",Function,strerror(errno));
}

//-------------------------------------------
// Delete the current log file and start over
//-------------------------------------------
bool SPSLogger::Restart()
{
 if (unlink(LOGFILENAME) ==-1)
   return false;
 else
   return true; 
}

//---------------------------
// Log Abort message and exit
//---------------------------
void SPSLogger::Abort()
{
 Log("Aborted");
 exit(1);
}



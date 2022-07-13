#include "ServerApp.h"


static struct
 {
  int  Num;
  char *Text;
 } RecTypes[] = { {POSFILERECORD, "POSFILERECORD"},
                  {-1} };

static struct
 {
  int Num;
  char *Text;
 } KeyFields[] = { {MICRKEY, "MICRKEY"},
                   {LICENSEKEY, "LICENSEKEY"},
                   {-1}
                 };

void ServerApp::DBError(char* Source,int Line,int Status,char *Function,
                        int RecType, int KeyType)
{
 char  RecTypeFld[50];
 char  KeyTypeFld[50];
 char  StatusFld[10];
 char  LineFld[10];
 static char VelocisErrorBuf[1024];

 int i;

 // Get Record Type Text
 if ( RecType == -1 )
  strcpy(RecTypeFld,"N/A");
 else
  {
   for (i=0; RecTypes[i].Num != -1 && RecTypes[i].Num != RecType; ++i);
   if ( RecTypes[i].Num == -1)
    sprintf(RecTypeFld,"%d",RecType);
   else
    strcpy(RecTypeFld,RecTypes[i].Text);
  }

 // Get Key Field Text
 if ( KeyType == -1 )
  strcpy(KeyTypeFld,"N/A");
 else
  {
   for (i=0; KeyFields[i].Num != -1 && KeyFields[i].Num != KeyType; ++i);
   if ( KeyFields[i].Num == -1 )
    sprintf(KeyTypeFld,"%d",KeyType);
   else
    strcpy(KeyTypeFld,KeyFields[i].Text);
  }


 sprintf(LineFld,"%d",Line);
 sprintf(StatusFld,"%d",Status);

 memset(VelocisErrorBuf,0,sizeof(VelocisErrorBuf));
 d_errinfo(VelocisErrorBuf,sizeof(VelocisErrorBuf),SessionHandle);
 //  %1: A Velocis Error has occurred in source module %2 at line %3.
 //  Status=%4 Function=%5 RecType=%6 KeyType=%7
 //  d_errinfo = %8
 IPCLogEvent(VelocisError,0,Source,LineFld,StatusFld,
             Function,RecTypeFld,KeyTypeFld,VelocisErrorBuf);
}



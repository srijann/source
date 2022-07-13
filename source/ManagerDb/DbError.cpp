#include "ServerApp.h"


static struct
 {
  int  Num;
  char *Text;
 } RecTypes[] = { {DATAREQRECORD, "DATAREQRECORD"},
                  {AUTHRULESRECORD, "AUTHRULESRECORD"},
                  {UNVFUNDSRULESRECORD, "UNVFUNDSRULESRECORD"},
                  {MERCHANTRECORD, "MERCHANTRECORD"},
                  {RESPONSERECORD, "RESPONSERECORD"},
                  {LISTRECORD, "LISTRECORD" },
                  {-1} };

static struct
 {
  int Num;
  char *Text;
 } KeyFields[] = { {AUTHRULESKEY, "AUTHRULESKEY"},
                   {DATAREQCLASSNAME,"DATAREQCLASSNAME"},
                   {UNVFUNDSRULESCLASSNAME,"UNVFUNDSRULESCLASSNAME"},
                   {MERCHANTID,"MERCHANTID"},
                   {MERCHANTNAME,"MERCHANTNAME"},
                   {RESPONSECLASSNAME,"RESPONSECLASSNAME"},
                   {LISTNAME,"LISTNAME"},
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

 d_errinfo(VelocisErrorBuf,sizeof(VelocisErrorBuf),SessionHandle);
 //  %1: A Velocis Error has occurred in source module %2 at line %3.
 //  Status=%4 Function=%5 RecType=%6 KeyType=%7
 //  d_errinfo = %8
 IPCLogEvent(VelocisError,0,Source,LineFld,StatusFld,
             Function,RecTypeFld,KeyTypeFld,VelocisErrorBuf);
}

void ServerApp::LogMissingClass(char* MerchantID,char* Type,
                                char* Class,char* SubClass)
{
 char Name[80];

 strcpy(Name,Class);
 if ( SubClass )
  {
   strcat(Name,".");
   strcat(Name,SubClass);
  }
 IPCLogEvent(DBManRecordMissing,0,MerchantID,Type,Name);
}

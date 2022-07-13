#include "dbunload.h"

#define _MERCHANT 0
#define _AUTHRULE 0
#define _DATAREQ  1
#define _LISTS    0
#define _POSRULES 0
#define _RESPONSE 0
#define _UNVFUNDS 0
//#define _SECRULES 0 - not in v23

void UnloadManageDbFile();


RDM_SESS SessionHandle;
RDM_DB   DBHandle;
FILE* SaveFile;
char* SaveFileName;
int   RecordCount;

static BOOL Interactive=FALSE;

//---------------------------------
// Come here to quit with exit code
//---------------------------------
void quit(int ecode)
{
 if ( SessionHandle )
  {
   d_close(SessionHandle);
   s_logout(SessionHandle);
  }
 if ( ecode==EXIT_SUCCESS )
  cout << "DBUNLOAD Complete." << endl;
 else
  cout << "  !!!!!!!!!!!!!!!!!!!!!" << endl
       << "  !!! DBUNLOAD Aborted !!!" << endl
       << "  !!!!!!!!!!!!!!!!!!!!!" << endl;

 cout << endl;
 exit(ecode);
}

//--------------
// Open Database
//--------------
void Open(const char* dbName)
{
 int Status;

 if ( (Status=d_open(dbName,"xq",SessionHandle,&DBHandle)) != S_OKAY )
  {
   if ( Status==S_EXOPENED )
    cout << "Can't open " << dbName << ". The server process is still running." << endl;
   else
    cout << "d_open returned " << Status
      << endl;
   quit(EXIT_FAILURE);
  }
}

//--------------------
// Close open database
//--------------------
void Close()
{
 int Status=d_close(SessionHandle);

 if ( Status != S_OKAY )
  {
   cout << "d_close returned " << Status
     << endl;
   quit(EXIT_FAILURE);
  }
}

//--------------------
// Initialize Database
//--------------------
void Init(const char* Name)
{
 int Status;

 // Initialize database
 Close();
 if ( (Status=s_dbInit(Name,SessionHandle)) != S_OKAY )
  {
   cout << "s_dbinit returned " << Status << endl;
   quit(EXIT_FAILURE);
  }
 Open(Name);
}

//--------------------------------------
// Copy all records of file to temp file
//--------------------------------------
void CopyTemp(short Type,const char* Name,char* Buf,int Len)
{
 int Status;
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
   if ( (Status=d_recread(Type,Buf,DBHandle)) != S_OKAY )
    {
     cout << endl << "  d_recread returned " << Status << endl;
     quit(EXIT_FAILURE);
    }
   if ( fwrite(Buf,1,Len,SaveFile) != Len )
    {
     cout << endl << "  fwrite failed: " << strerror(errno) << endl;
     quit(EXIT_FAILURE);
    }
   cout << "  " << ++RecordCount << "\r";
  }
 while( (Status=d_recnext(DBHandle)) == S_OKAY );

 cout << "  " << RecordCount << " Records Saved" << endl;

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

//-----------------------------------
// Do ending steps after database unload
//-----------------------------------
void CloseAll()
{
 cout << " Closing Database...." << endl;
 Close();
 cout << endl;
 fclose(SaveFile);
}

//----------------------
// Unload the Positive File
//----------------------
#include "posfile.h"
void UnloadPositiveFile()
{
 struct PosfileRecord Rec;

 Open("posfile");
 cout << "Unloading Positive File..." << endl;
 CopyTemp(POSFILERECORD,"PosFile\\PosFile",(char*)&Rec,sizeof(Rec));
 CloseAll();
}

//------------------------
// Unload the Bank Phone File
//------------------------
#include "BankPhoneDb.h"
void UnloadBankPhoneFile()
{
 struct BankPhoneRecord Rec;

 Open("BankPhoneDB");
 cout << "Unloading Bank Phone File..." << endl;
 CopyTemp(BANKPHONERECORD,"BankPhone\\BankPhone",(char*)&Rec,sizeof(Rec));
 CloseAll();
}

//---------------------
// Unload the Preauth File
//---------------------
#include "PreauthDB.h"
void UnloadPreauthFile()
{
 struct PreauthRecord Rec;

 Open("PreauthDB");
 cout << "Unloading Preauth File..." << endl;
 CopyTemp(PREAUTHRECORD,"Preauth\\Preauth",(char*)&Rec,sizeof(Rec));
 CloseAll();
}

//------------------------------------------------------
// Main function. Get Selection from Args or else prompt
//------------------------------------------------------
void main(int argc,const char* argv[])
{
 short status;

 SaveFileName = new char[MAX_PATH+1];
 cout << "Start" << endl;
 // LOGIN
 if ( (status=s_login("rds","admin","secret",&SessionHandle)) != S_OKAY )
  {
   cout << "s_login returned " << status << "\n";
   quit(EXIT_FAILURE);
  }

 int Key;
 if ( argc== 1 )
  {
   cout << "  1. Positive File" << endl;
   cout << "  2. Bank Phone File" << endl;
   cout << "  3. Preauth File" << endl;
   cout << "  4. Log File" << endl;
   cout << "  5. Managedb File" << endl;
   cout << "  A. All Files" << endl;
   cout << "Unload which File?";
   Key=getch();
   Interactive = TRUE;
  }
 else
  Key = argv[1][0];

 bool ALL=false;
 cout << endl;
 switch( Key )
  {
   case 'A':
   case 'a': ALL=true;

   case '1': UnloadPositiveFile();
             if ( ! ALL )
              break;

   case '2': UnloadBankPhoneFile();
             if ( ! ALL )
              break;

   case '3': UnloadPreauthFile();
             if ( ! ALL )
              break;

   case '4': UnloadLogFile();
             if ( ! ALL )
              break;

   case '5': UnloadManageDbFile();
             if ( ! ALL )
              break;

   default:  cout << "Unrecognized Selection" << endl;
             exit(EXIT_FAILURE);
  }


 // Success
 quit(EXIT_SUCCESS);
}

//---------------------
// Unload the ManageDb File
//---------------------
#include "spsmanv23.h"
void UnloadManageDbFile()
{
#if _MERCHANT
 struct MerchantRecord Rec;
 Open("spsmanv23");
 cout << "Unloading ManageDb File..." << endl;
 CopyTemp(MERCHANTRECORD,"ManageDB\\ManageDB",(char*)&Rec,sizeof(Rec));
#endif

#if _AUTHRULE
 struct AuthRulesRecord Rec;
 Open("spsmanv23");
 cout << "Unloading ManageDb File..." << endl;
 CopyTemp(AUTHRULESRECORD,"ManageDB\\ManageDB",(char*)&Rec,sizeof(Rec));
#endif

#if _DATAREQ
 struct DataReqRecord Rec;
 Open("spsmanv23");
 cout << "Unloading ManageDb File..." << endl;
 CopyTemp(DATAREQRECORD,"ManageDB\\ManageDB",(char*)&Rec,sizeof(Rec));
#endif

#if _LISTS
 struct ListRecord Rec;
 Open("spsmanv23");
 cout << "Unloading ManageDb File..." << endl;
 CopyTemp(LISTRECORD,"ManageDB\\ManageDB",(char*)&Rec,sizeof(Rec));
#endif

#if _POSRULES
 struct PositiveRulesRecord Rec;
 Open("spsmanv23");
 cout << "Unloading ManageDb File..." << endl;
 CopyTemp(POSITIVERULESRECORD,"ManageDB\\ManageDB",(char*)&Rec,sizeof(Rec));
#endif

#if _RESPONSE
 struct ResponseRecord Rec;
 Open("spsmanv23");
 cout << "Unloading ManageDb File..." << endl;
 CopyTemp(RESPONSERECORD,"ManageDB\\ManageDB",(char*)&Rec,sizeof(Rec));
#endif

#if _UNVFUNDS
 struct UnvFundsRulesRecord Rec;
 Open("spsmanv23");
 cout << "Unloading ManageDb File..." << endl;
 CopyTemp(UNVFUNDSRULESRECORD,"ManageDB\\ManageDB",(char*)&Rec,sizeof(Rec));
#endif

CloseAll();
}



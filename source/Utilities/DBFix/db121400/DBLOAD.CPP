#include "dbload.h"

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
  cout << "DBLOAD Complete." << endl;
 else
  cout << "  !!!!!!!!!!!!!!!!!!!!!" << endl
       << "  !!! DBLOAD Aborted !!!" << endl
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
void CopyTemp(const char* Name)
{
 RecordCount=0;

 strcpy(SaveFileName,DBPATH);
 strcat(SaveFileName,Name);
 strcat(SaveFileName,".tmp");

 cout << " Loading Records from " << SaveFileName << "..." << endl;

 if ( (SaveFile=fopen(SaveFileName, "rb")) == NULL )
  {
   cout << "fopen of " << Name << " failed: " << strerror(errno) << endl;
   quit(EXIT_FAILURE);
  }
 fseek(SaveFile,0,SEEK_SET);
}

//-----------------------------------
// Do ending steps after database load
//-----------------------------------
void CloseAll()
{
 cout << " Closing Database...." << endl;
 Close();
 cout << endl;
 fclose(SaveFile);
}

//----------------------
// Load the Positive File
//----------------------
#include "posfile.h"
void LoadPositiveFile()
{
 struct PosfileRecord Rec;

 Open("posfile");
 cout << "Loading Positive File..." << endl;
 CopyTemp("PosFile\\PosFile");

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
   if ( (Status=d_fillnew(POSFILERECORD,&Rec,DBHandle)) != S_OKAY )
    {
     if ( Status==S_DUPLICATE )
      continue;
     cout << endl << "  d_fillnew returned " << Status << endl;
     quit(EXIT_FAILURE);
    }
 // Add License Key
   if ( Rec.LicenseState[0] &&
      (Status=d_keystore(LICENSEKEY,DBHandle)) != S_OKAY &&
      Status != S_DUPLICATE )
    {
     cout << endl << "  d_keystore returned " << Status << endl;
     quit(EXIT_FAILURE);
    }

   // Add Micr Key
   if ( Rec.BankNumber[0] &&
        (Status=d_keystore(MICRKEY,DBHandle)) != S_OKAY &&
        Status != S_DUPLICATE )
    {
     cout << endl << "  d_keystore returned " << Status << endl;
     quit(EXIT_FAILURE);
    }

   // Add Phone Key
   if ( Rec.PhoneKey[0] &&
        (Status=d_keystore(PHONEKEY,DBHandle)) != S_OKAY &&
        Status != S_DUPLICATE )
    {
     cout << endl << "  d_keystore returned " << Status << endl;
     quit(EXIT_FAILURE);
    }


   cout << "  " << ++RestoredCount << "\r";
  }

 cout << "  " << RestoredCount << " Records Restored" << endl;

 CloseAll();
}

//------------------------
// Load the Bank Phone File
//------------------------
#include "BankPhoneDb.h"
void LoadBankPhoneFile()
{
 struct BankPhoneRecord Rec;

 Open("BankPhoneDB");
 cout << "Loading Bank Phone File..." << endl;
 CopyTemp("BankPhone\\BankPhone");

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
   if ( (Status=d_fillnew(BANKPHONERECORD,&Rec,DBHandle)) != S_OKAY )
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

//---------------------
// Load the Preauth File
//---------------------
#include "PreauthDB.h"
void LoadPreauthFile()
{
 struct PreauthRecord Rec;

 Open("PreauthDB");
 cout << "Loading Preauth File..." << endl;
 CopyTemp("Preauth\\Preauth");

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
   if ( (Status=d_fillnew(PREAUTHRECORD,&Rec,DBHandle)) != S_OKAY )
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

//------------------------------------------------------
// Main function. Get Selection from Args or else prompt
//------------------------------------------------------
void main(int argc,const char* argv[])
{
 short status;

 SaveFileName = new char[MAX_PATH+1];

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
   cout << "  A. All Files" << endl;
   cout << "Load which File?";
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

   case '1': LoadPositiveFile();
             if ( ! ALL )
              break;

   case '2': LoadBankPhoneFile();
             if ( ! ALL )
              break;

   case '3': LoadPreauthFile();
             if ( ! ALL )
              break;

   case '4': LoadLogFile();
             if ( ! ALL )
              break;

   default:  cout << "Unrecognized Selection" << endl;
             exit(EXIT_FAILURE);
  }


 // Success
 quit(EXIT_SUCCESS);
}



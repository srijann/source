#include "dbfix.h"

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
  cout << "DBFIX Complete." << endl;
 else
  cout << "  !!!!!!!!!!!!!!!!!!!!!" << endl
       << "  !!! DBFIX Aborted !!!" << endl
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
// Do ending steps after database fix
//-----------------------------------
void CloseAll()
{
 cout << " Closing Database...." << endl;
 Close();
 cout << endl;
 fclose(SaveFile);
 unlink(SaveFileName);
}

//----------------------
// Fix the Positive File
//----------------------
#include "posfile.h"
void FixPositiveFile()
{
 struct PosfileRecord Rec;

 Open("posfile");
 cout << "Fixing Positive File..." << endl;
 CopyTemp(POSFILERECORD,"PosFile\\PosFile",(char*)&Rec,sizeof(Rec));
 Init("PosFile");

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

   cout << "  " << ++RestoredCount << "\r";
  }

 cout << "  " << RestoredCount << " Records Restored" << endl;

 CloseAll();
}

//------------------------
// Fix the Bank Phone File
//------------------------
#include "BankPhoneDb.h"
void FixBankPhoneFile()
{
 struct BankPhoneRecord Rec;

 Open("BankPhoneDB");
 cout << "Fixing Bank Phone File..." << endl;
 CopyTemp(BANKPHONERECORD,"BankPhone\\BankPhone",(char*)&Rec,sizeof(Rec));
 Init("BankPhoneDB");

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
// Fix the Preauth File
//---------------------
#include "PreauthDB.h"
void FixPreauthFile()
{
 struct PreauthRecord Rec;

 Open("PreauthDB");
 cout << "Fixing Preauth File..." << endl;
 CopyTemp(PREAUTHRECORD,"Preauth\\Preauth",(char*)&Rec,sizeof(Rec));
 Init("PreauthDB");

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
   cout << "Fix which File?";
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

   case '1': FixPositiveFile();
             if ( ! ALL )
              break;

   case '2': FixBankPhoneFile();
             if ( ! ALL )
              break;

   case '3': FixPreauthFile();
             if ( ! ALL )
              break;

   case '4': FixLogFile();
             if ( ! ALL )
              break;

   default:  cout << "Unrecognized Selection" << endl;
             exit(EXIT_FAILURE);
  }


 // Success
 quit(EXIT_SUCCESS);
}

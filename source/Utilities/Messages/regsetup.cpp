// This program Sets up the Registry for the SPS System        
#include <windows.h>
#include <iostream.h>

//---------------------
// Set Up Event Logging
//---------------------
bool SetupEventLogging(void) {

  HKEY  hk;
  DWORD dwData;
  CHAR  szBuf[80];
  int   ret;
  bool  success=true;

/*
 * Add your source name as a subkey under the Application
 * key in the EventLog service portion of the registry.
 */

 if ((ret=RegCreateKey(HKEY_LOCAL_MACHINE,
      "SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\SPS",
      &hk)) != ERROR_SUCCESS)
  {
   cout << "Error Creating EventLog\\Application\\SPS Key - "
        << "RegcreateKey Returned " << ret << "\n";
   success=false;
  }

 /* Set the Event ID message-file name. */


 // strcpy(szBuf, "%SystemRoot%\\System\\SPSMSG.dll");
    strcpy(szBuf, "C:\\PROGRAMS\\MESSAGES.DLL");

  /* Add the Event ID message-file name to the subkey. */


  if ((ret=RegSetValueEx(hk,             /* subkey handle         */
          "EventMessageFile",       /* value name            */
          0,                        /* must be zero          */
          REG_EXPAND_SZ,            /* value type            */
          (LPBYTE) szBuf,           /* address of value data */
          strlen(szBuf) + 1))!=ERROR_SUCCESS)       /* length of value data  */
   {
    cout << "Error Creating EventMessageFile Key - " <<
            "RegSetValueEx returned " << ret << "\n";
    success=false;
   }

/* Set the supported types flags. */


dwData = EVENTLOG_ERROR_TYPE | EVENTLOG_WARNING_TYPE |
    EVENTLOG_INFORMATION_TYPE;

if ((ret=RegSetValueEx(hk,      /* subkey handle                */
        "TypesSupported",  /* value name                   */
        0,                 /* must be zero                 */
        REG_DWORD,         /* value type                   */
        (LPBYTE) &dwData,  /* address of value data        */
        sizeof(DWORD))) != ERROR_SUCCESS)    /* length of value data         */
 {
  cout << "Error Creating TypesSupported Key - " <<
          "RegSetValueEx returned " << ret << "\n";
  success=false;
 }

 RegCloseKey(hk);
 return success;
}

//-------------------------
// SetUp IPCINFO Parameters
//-------------------------
bool SetupIPCInfo(DWORD NumBufs,char *LogServerName,char *LogSourceName)
{
 bool success=true;
 int  ret;
 HKEY hk;

 if ((ret=RegCreateKey(HKEY_LOCAL_MACHINE,
      "SOFTWARE\\IPCInfo",&hk)) != ERROR_SUCCESS)
  {
   cout << "Error Creating IPCInfo Key. RegcreateKey Returned " <<
      ret << "\n";
   success=false;
  }

 /* Set the NumBufs Value. */

 if ((ret=RegSetValueEx(hk,      /* subkey handle                */
        "NumBufs",          /* value name                   */
        0,                  /* must be zero                 */
        REG_DWORD,          /* value type                   */
        (LPBYTE) &NumBufs,  /* address of value data        */
        sizeof(DWORD))) != ERROR_SUCCESS)    /* length of value data         */
 {
  cout << "Error Creating NumBufs Key - " <<
          "RegSetValueEx returned " << ret << "\n";
  success=false;
 }

 /* Set the LogServerName Value. */

 if ((ret=RegSetValueEx(hk,      /* subkey handle                */
        "LogServerName",          /* value name                   */
        0,                  /* must be zero                 */
        REG_SZ,            /* value type                   */
        (LPBYTE)LogServerName,  /* address of value data        */
        strlen(LogServerName)+1)) != ERROR_SUCCESS)    /* length of value data         */
 {
  cout << "Error Creating LogServerName Key - " <<
          "RegSetValueEx returned " << ret << "\n";
  success=false;
 }

 // Set the LogSourceName Value.

 if ((ret=RegSetValueEx(hk,      /* subkey handle                */
        "LogSourceName",          /* value name                   */
        0,                  /* must be zero                 */
        REG_SZ,            /* value type                   */
        (LPBYTE)LogSourceName,  /* address of value data        */
        strlen(LogSourceName)+1)) != ERROR_SUCCESS)    /* length of value data         */
 {
  cout << "Error Creating LogSourceName Key - " <<
          "RegSetValueEx returned " << ret << "\n";
  success=false;
 }


 RegCloseKey(hk);
 return( success );
}

bool setStringValue(HKEY hk,char* ValueName,char* String)
{
 int ret;
 
 // Set the LogSourceName Value.

 if ((ret=RegSetValueEx(hk,      /* subkey handle                */
        ValueName,
        0,                  /* must be zero                 */
        REG_SZ,            /* value type                   */
        (LPBYTE)String,  /* address of value data        */
        strlen(String)+1)) != ERROR_SUCCESS)    /* length of value data         */
 {
  cout << "Error Creating " << ValueName <<
          "RegSetValueEx returned " << ret << "\n";
  return false;
 }
}


#define SET(name,value) if ( ! setStringValue(hk,name,value) ) return false
bool SetupLogicalDestinations()
{
 bool success=true;
 int  ret;
 HKEY hk;

 // Create the key
 if ((ret=RegCreateKey(HKEY_LOCAL_MACHINE,
      "SOFTWARE\\IPCLogicalNames",&hk)) != ERROR_SUCCESS)
  {
   cout << "Error Creating IPCLogicalNames Key. RegcreateKey Returned " <<
      ret << "\n";
   success=false;
  }
 // Add the destinations
 SET("@ManagerDB","SPSSERV1.ManageDB");
 SET("ManagerDB","SPSSERV1.ManageDB");
 SET("PREAUTHDEST","SPSSERV1.PREAUTH");
 SET("REFERRALQUEUE","SPSSERV1.REFQUE");
 SET("VOICEAUTH","SPSSERV1.VOICAUTH");
}

//--------------
// Main Function
//--------------
void main(void)
{
 bool success=true;

 if ( ! SetupEventLogging() )
   success=false;

 if ( ! SetupIPCInfo(10,"SPSSERV1","SPS") )
   success=false;

 if ( ! SetupLogicalDestinations() )
   success=false;

 if ( ! success )
   cout << "*** At Least One Registry Function Failed ***\n";
}

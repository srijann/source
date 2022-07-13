#include <windows.h>
#include <iostream.h>

HKEY hk;
DWORD dwData;
UCHAR szBuf[80];

void ErrorExit(char *str)
{
 cout << str;
 exit(0);
}

void main(void) {
/*
 * Add your source name as a subkey under the Application
 * key in the EventLog service portion of the registry.
 */

if (RegCreateKey(HKEY_LOCAL_MACHINE,
"SYSTEM\\CurrentControlSet\\Services\
\\EventLog\\Application\\SamplApp",
            &hk))
    ErrorExit("could not create registry key");

/* Set the Event ID message-file name. */

strcpy(szBuf, "%SystemRoot%\\System\\SamplApp.dll");

/* Add the Event ID message-file name to the subkey. */


if (RegSetValueEx(hk,             /* subkey handle         */ 
        "EventMessageFile",       /* value name            */ 
        0,                        /* must be zero          */
        REG_EXPAND_SZ,            /* value type            */
        (LPBYTE) szBuf,           /* address of value data */ 
        strlen(szBuf) + 1))       /* length of value data  */ 
    ErrorExit("could not set event message file"); 
 
/* Set the supported types flags. */ 

 
dwData = EVENTLOG_ERROR_TYPE | EVENTLOG_WARNING_TYPE | 
    EVENTLOG_INFORMATION_TYPE; 
 
if (RegSetValueEx(hk,      /* subkey handle                */ 
        "TypesSupported",  /* value name                   */ 
        0,                 /* must be zero                 */ 
        REG_DWORD,         /* value type                   */ 
        (LPBYTE) &dwData,  /* address of value data        */ 
        sizeof(DWORD)))    /* length of value data         */ 

    ErrorExit("could not set supported types");
 
RegCloseKey(hk); 
}
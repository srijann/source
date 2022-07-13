// Adds Programs to the Startup Group

#include <windows.h>
#include <conio.h>
#include <iostream.h>
#include <stdio.h>

DWORD InstId;
HSZ     Service;
HSZ     Topic;
HCONV   HConv;

char commands[1000];


//
// This call back function is the heart of interaction between this program
// and DDEML.  Because Windows doesn't pass C++ 'this' pointers to call
// back functions, a static 'this' pointer was used.  If you wanted to
// create a Client that would allow for more than one conversation, using a
// List of conversations and their associated 'this' pointers would be one
// possible method to try.  The XTYP_ constants are described in detail in
// the online help.
//
HDDEDATA FAR PASCAL _export
CallBackProc(WORD type, WORD, HCONV /*hConv*/, HSZ, HSZ, HDDEDATA /*hData*/,
         DWORD, DWORD)
{
  switch (type) {
    case XTYP_DISCONNECT:
      cout << "Disconnected\n";
      break;

    case XTYP_ERROR:
      cout << "A critical DDE error has occured\n";
      break;
  }
  return 0;
}

void quit(int ecode)
{
 if ( HConv )
   DdeDisconnect(HConv);
 if ( Service )
   DdeFreeStringHandle(InstId, Service);
 if ( Topic )
   DdeFreeStringHandle(InstId, Topic);
 if ( InstId )
   DdeUninitialize(InstId);

 exit(ecode);
}

void main(int argc,char* argv[])
{
 int len;
 bool old=false;

 if ( argc > 1 && stricmp(argv[1],"old") == 0 )
  old=true;

 if (DdeInitialize(&InstId, (PFNCALLBACK)(FARPROC)CallBackProc, APPCMD_CLIENTONLY, 0) == DMLERR_NO_ERROR)
  {
    Service = DdeCreateStringHandle(InstId, "PROGMAN", CP_WINANSI);
    Topic = DdeCreateStringHandle(InstId, "PROGMAN", CP_WINANSI);
     if (!Service || !Topic)
       {
        cout << "Creation of DDE strings failed\n";
        quit(EXIT_FAILURE);
       }
  }
 else
 {
  cout << "DDE Initialization failed\n";
  quit(EXIT_FAILURE);
 }

 HConv = DdeConnect(InstId, Service, Topic, 0);
 if (!HConv)
  {
   cout <<  "Can't start conversation.\nMake sure PROGMAN is running.\n";
   quit(EXIT_FAILURE);
  }

 strcpy(commands,"[ShowGroup(""Startup"",1,0)]");
 if ( ! old )
  {
   strcat(commands,"[AddItem(C:\\SPS\\PROGRAMS\\POSAUTH.EXE,POSAUTH)]");
   strcat(commands,"[AddItem(C:\\SPS\\PROGRAMS\\REFERQUE.EXE,REFQUE)]");
   strcat(commands,"[AddItem(C:\\SPS\\PROGRAMS\\IPCSHELL.EXE,IPCSHELL)]");
   strcat(commands,"[AddItem(C:\\SPS\\PROGRAMS\\BANKPHONE.EXE,BANKPHON)]");
   strcat(commands,"[AddItem(C:\\SPS\\PROGRAMS\\POSFILDB.EXE,POSFILE)]");
   strcat(commands,"[AddItem(C:\\SPS\\PROGRAMS\\PREAUTHDB.EXE,PREAUTH)]");
   strcat(commands,"[AddItem(C:\\SPS\\PROGRAMS\\MANAGERDB.EXE,MANAGEDB)]");
   strcat(commands,"[AddItem(C:\\SPS\\PROGRAMS\\SPSLOG.EXE,SPSLOG)]");
   strcat(commands,"[AddItem(C:\\SPS\\PROGRAMS\\NETROUTE.EXE,NETROUTE)]");
  }
 else
  {
   strcat(commands,"[AddItem(C:\\PROGRAMS\\POSAUTH.EXE,POSAUTH)]");
   strcat(commands,"[AddItem(C:\\PROGRAMS\\REFERQUE.EXE,REFQUE)]");
   strcat(commands,"[AddItem(C:\\PROGRAMS\\IPCSHELL.EXE,IPCSHELL)]");
   strcat(commands,"[AddItem(C:\\PROGRAMS\\BANKPHONE.EXE,BANKPHON)]");
   strcat(commands,"[AddItem(C:\\PROGRAMS\\POSFILDB.EXE,POSFILE)]");
   strcat(commands,"[AddItem(C:\\PROGRAMS\\PREAUTHDB.EXE,PREAUTH)]");
   strcat(commands,"[AddItem(C:\\PROGRAMS\\MANAGERDB.EXE,MANAGEDB)]");
   strcat(commands,"[AddItem(C:\\PROGRAMS\\SPSLOG.EXE,SPSLOG)]");
   strcat(commands,"[AddItem(C:\\PROGRAMS\\NETROUTE.EXE,NETROUTE)]");
  }
 len=strlen(commands);
 if ( ! DdeClientTransaction((LPBYTE)commands,
               len, HConv, 0L, CF_TEXT, XTYP_EXECUTE, 90000, 0) )
  {
   int Err=DdeGetLastError(InstId);
   cout << "DDE TRANSACTION FAILED - Error = " << Err << "\n";
   quit(EXIT_FAILURE);
  }

 quit(EXIT_SUCCESS);
}

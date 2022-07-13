#include <windows.h>
#include <iostream.h>

char *subname="SYSTEM\\CurrentControlSet\\Control\\"
              "ComputerName\\ComputerName\\ComputerName";

void main(int argc,char *argv[])
{
 LONG ret;
 unsigned char buf[200];
 DWORD sizebuf;
 HKEY   keyhandle;
 DWORD  type_code;
 unsigned char databuf[200];
 DWORD  sizedbuf;
 DWORD  n;

 ret=RegOpenKeyEx(HKEY_LOCAL_MACHINE,argv[1],0,KEY_READ,&keyhandle);
 if ( ret != ERROR_SUCCESS )
  {
   cout << "RegOpenKeyEx returned " << ret << "\n";
   exit(0);
  }

 sizebuf=sizeof(buf);
 sizedbuf=sizeof(databuf);

 for (n=0;
      (ret=RegEnumValue(keyhandle,n,buf,&sizebuf,NULL,&type_code,databuf,&sizedbuf)) ==
        ERROR_SUCCESS || ret==234; ++n)
   cout << buf << ":" << databuf << "\n";

 if ( ret != ERROR_NO_MORE_ITEMS )
   cout << "RegEnumValue returned " << ret << "\n";

 RegCloseKey(keyhandle);
}

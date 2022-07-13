#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include "spslog.h"
#include "pipes.h"
#include "kwrec.h"

SPSLogger Logger("kwsimv");
Pipe* RecvPipe;
Pipe* SendPipe;
Pipe* ReplyPipe;
char  IOBuf[8192];
char  DtaName[100];
int   Cluster;

bool RcvTran(void)
{
 if ( RecvPipe->Read(IOBuf,sizeof(IOBuf)) == -1 )
  return false;
 printf("RECV:\n%s\n",IOBuf);
 fflush(stdout);
 return true;
}

bool SendTran(void)
{
 sprintf(IOBuf,"TTY%d%c$INQ$   ",Cluster,GROUP_SEPARATOR);
 return SendPipe->Write(IOBuf,strlen(IOBuf));
}

//----------------------------------
// Copy a string W/O null terminator
//----------------------------------
void cpy(char* DestAddr,char *String)
{
 memcpy(DestAddr,String,strlen(String));
}

//-----------------------
// Write out the DTA file
//-----------------------

MERCHANT Merch;
TINQ     Inq;
CHECK    Check;
AUDIT    Audit;

int WriteData()
{
 char LF=0xa;
 int fd=open(DtaName,O_CREAT|O_TRUNC|O_WRONLY);

 if ( fd==-1 )
  {
   perror("ERROR OPENING DAT FILE");
   return false;
  }

 memset(&Merch,' ',sizeof(Merch));
 memset(&Inq,' ',sizeof(Inq));
 memset(&Check,' ',sizeof(Check));
 memset(&Audit,' ',sizeof(Audit));
 // Write Merch
 Merch.Sentinel='M';
 cpy(Merch.TID,"TIDXXXXX");
 cpy(Merch.Name,"MERCHNAME");
 cpy(Merch.Address1,"7103 SHEHANDOAH CT.");
 cpy(Merch.Address2,"ADDRESS2");
 cpy(Merch.CityState,"TAMPA,FL");
 cpy(Merch.Zip,"33615");
 cpy(Merch.MerchId,"A9999");
 write(fd,&Merch,sizeof(Merch));
 write(fd,&LF,1);
 // Write Check
 Check.Sentinel='C';
 cpy(Check.MerchId,"A9999");
 cpy(Check.BankNumber,"123456789");
 cpy(Check.Account,"222222222222");
 cpy(Check.CheckNumber,"1234");
 cpy(Check.Name,"JOHN DOE");
 cpy(Check.Address1,"5013 W. IdleWild");
 cpy(Check.Address2,"Apt #2");
 cpy(Check.CityState,"SanDiego,CA");
 cpy(Check.Zip,"92067");
 cpy(Check.HomePhone,"9035551212");
 cpy(Check.WorkPhone,"9045551313");
 cpy(Check.CheckDate,"010197");
 cpy(Check.SSN,"268460551");
 cpy(Check.StateCode,"CA");
 cpy(Check.DriversLicense,"A000001");
 cpy(Check.TypeReturnedCheck,"XX");
 cpy(Check.DateReceived,"123196");
 cpy(Check.LastActivity,"113096");
 cpy(Check.BalanceDue,"0000000.00");
 write(fd,&Check,sizeof(Check));
 write(fd,&LF ,1);
 cpy(Check.Account,"333333333333");
 cpy(Check.Name,"JOHN SMITH");
 write(fd,&Check,sizeof(Check));
 write(fd,&LF,1);
 // Write Audit
 Audit.Sentinel = 'A';
 cpy(Audit.InquiryNumber,"111111111");
 cpy(Audit.Misc5,"END#1");
 write(fd,&Audit,sizeof(Audit));
 write(fd,&LF,1);
 cpy(Audit.InquiryNumber,"222222222");
 cpy(Audit.Misc5,"END#2");
 write(fd,&Audit,sizeof(Audit));
 write(fd,&LF,1);
 close(fd);
 return true;
}

void RecvReply(void)
{
 memset(IOBuf,0,sizeof(IOBuf));
 int ret=ReplyPipe->Read(IOBuf,sizeof(IOBuf),1000*60*5);
 if ( ret==0 )
  {
   puts("TIMEOUT");
   fflush(stdout);
  }
 else
 if ( ret > 0 )
  {
   printf("REPLY: %s",IOBuf);
   fflush(stdout);
  }
}

void main(int,char* argv[])
{
 Cluster=atoi(argv[1]);
 sprintf(IOBuf,"/usr/spool/uucp/PST..%d",Cluster);
 RecvPipe=new Pipe(IOBuf);
 sprintf(IOBuf,"/usr/spool/uucp/RST..%d",Cluster);
 SendPipe=new Pipe(IOBuf);
 sprintf(IOBuf,"/usr/spool/uucp/FIN..%d",Cluster);
 ReplyPipe=new Pipe(IOBuf);
 sprintf(DtaName,"/usr/spool/uucp/DTA..%d",Cluster);

 for (;;)
  {
   if ( RcvTran() &&
        WriteData() &&
        SendTran() )
     RecvReply();

   RecvPipe->Flush();
   SendPipe->Flush();
   ReplyPipe->Flush();
  }
}

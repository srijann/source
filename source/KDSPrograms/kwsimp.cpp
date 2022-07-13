#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include "kwrec.h"
#include "pipes.h"
#include "spslog.h"

SPSLogger Logger("KWSimp");
Pipe* SendPipe;
Pipe* RecvPipe;

void cpy(char* DestAdr,char *String);
void WriteData(void);
char PipeBuf[512];
char DtaName[100];

void main(int argc,char *argv[])
{
 int wait=-1;
 int ClusterNumber=1;

 if ( argc >= 3 )
  wait=atoi(argv[2]);
 if ( argc >=2 )
   ClusterNumber=atoi(argv[1]);
   
 sprintf(PipeBuf,"/usr/spool/uucp/TRN..%d",ClusterNumber);
 SendPipe = new Pipe(PipeBuf);
 sprintf(PipeBuf,"/usr/spool/uucp/RSP..%d",ClusterNumber);
 RecvPipe = new Pipe(PipeBuf);
 sprintf(DtaName,"/usr/spool/uucp/DTA..%d",ClusterNumber);

 WriteData();
 sprintf(PipeBuf,"TTY%d%c%s",ClusterNumber,GROUP_SEPARATOR,"KWHELLO");
 if ( ! SendPipe->Write(PipeBuf,strlen(PipeBuf)) )
  {
   perror("ERROR WRITING TO PIPE");
   exit(0);
  }

 memset(PipeBuf,0,sizeof(PipeBuf));
 if ( RecvPipe->Read(PipeBuf,sizeof(PipeBuf),wait) > 0 )
  puts(PipeBuf);
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

void WriteData()
{
 char LF=0xa;
 int fd=open(DtaName,O_CREAT|O_TRUNC|O_WRONLY);

 if ( fd==-1 )
  {
   perror("ERROR OPENING DAT FILE");
   exit(1);
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
 // Write Inq
 Inq.Sentinel='I';
 cpy(Inq.Amount,"9999999.99");
 cpy(Inq.StateCode,"FL");
 cpy(Inq.License,"E40042849209");
 cpy(Inq.DateOfBirth,"060949");
 cpy(Inq.CheckNumber,"123456");
 cpy(Inq.PhoneNumber,"8132439562");
 cpy(Inq.BankNumber,"123456789");
 cpy(Inq.BankAccount,"1111111111111111");
 cpy(Inq.SSN,"268460550");
 cpy(Inq.Misc1,"Additional#1");
 cpy(Inq.Misc2,"Additional#2");
 cpy(Inq.Misc3,"Additional#3");  
 cpy(Inq.Misc4,"Additional#4");  
 cpy(Inq.Misc5,"Additional#5");  
 write(fd,&Inq,sizeof(Inq));
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
 /*
  char Sentinel;
  char InquiryNumber[9];
  char Reference[6];
  char TID[25];
  char Date[6];
  char Time[8];
  char Port[2];
  char BankNumber[10];
  char BankAccount[16];
  char CheckNumber[6];
  char StateCode[4];
  char DriversLicense[15];
  char SSN[9];
  char DOB[6];
  char Amount[10];
  char OutputCode[2];
  char TotalOutput[54];
  char TypeInquiry;
  char BillingGuarantee;
  char Misc1[25];
  char Misc2[25];
  char Misc3[25];
  char Misc4[25];
  char Misc5[25];
 */
 Audit.Sentinel = 'A';
 cpy(Audit.InquiryNumber,"123456789");
 cpy(Audit.Date,"050397");
 cpy(Audit.Time,"12:30:40");
 cpy(Audit.TID,"A0001");
 cpy(Audit.BankNumber,"123456789");
 cpy(Audit.BankAccount,"0987654321654321");
 cpy(Audit.CheckNumber,"001000");
 cpy(Audit.StateCode,"FL");
 cpy(Audit.DriversLicense,"E40042849209");
 cpy(Audit.SSN,"123456789");
 cpy(Audit.DOB,"060949");
 cpy(Audit.Amount,"0000101.51");
 cpy(Audit.OutputCode,"80");
 cpy(Audit.TotalOutput,"THIS IS TOTAL OUTPUT");
 cpy(Audit.Misc5,"Misc5");
 write(fd,&Audit,sizeof(Audit));
 write(fd,&LF,1);
 Audit.Sentinel = 'A';
 write(fd,&Audit,sizeof(Audit));
 write(fd,&LF,1);
 close(fd);
}

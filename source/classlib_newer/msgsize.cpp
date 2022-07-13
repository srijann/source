#include <windows.h>
#include <stdio.h>
#include <iostream.h>
#include <conio.h>
#include "ipcmsg.h"
#include "CheckAuthFn.h"
#include "TranType.h"
#include "Kwrec.h"
#include "SpsManDb.h"
#include "Posfile.h"
#include "BankPhoneDB.h"

IPCMessage Msg;
KENWOOD_TERMINFO TermInfo;
MERCHANT Merch;
ACTIVITY Activity[20];
BADCHECK BadCheck[20];

struct MerchantRecord merchantRecord;
struct UnvFundsRulesRecord secRulesRecord;
struct DataReqRecord dataReqRecord;
struct AuthRulesRecord authRulesRecord;
struct PositiveRulesRecord positiveRulesRecord;
struct PosfileRecord posfileRecord;
struct BankPhoneRecord PhoneRec;

void genPosData(bool average=false)
{
 if ( ! average )
  {
   Msg.PutFld(FN_TRANTYPE,T_POSAUTH);
   Msg.PutFld(FN_TERMINFO,&TermInfo,sizeof(TermInfo));
   Msg.PutFld(FN_TEXTRESPONSE,"123456789012345678901234567890");
   Msg.PutFld(FN_KWMERCHANTRECORD,&Merch,sizeof(Merch));
   Msg.PutFld(FN_AMOUNT,"99999.99");
   Msg.PutFld(FN_LICENSESTATE,"CA");
   Msg.PutFld(FN_LICENSE,"123456789012345");
   Msg.PutFld(FN_DOB,"010101");
   Msg.PutFld(FN_CHECKNUMBER,"1234");
   Msg.PutFld(FN_PHONE,"9999999999");
   Msg.PutFld(FN_BANKNUMBER,"123456789");
   Msg.PutFld(FN_BANKACCOUNT,"1234567890123456");
   Msg.PutFld(FN_SSN,"268460550");
   Msg.PutFld(FN_MERCHID,"99999");
   Msg.PutFld(FN_CHECKSWIPE,(BYTE)'1');
   Msg.PutFld(FN_LICENSESWIPE,(BYTE)'1');
  }
 else
  {
   Msg.PutFld(FN_TRANTYPE,T_POSAUTH);
   Msg.PutFld(FN_TERMINFO,&TermInfo,sizeof(TermInfo));
   Msg.PutFld(FN_TEXTRESPONSE,"123456789012345");
   Msg.PutFld(FN_KWMERCHANTRECORD,&Merch,sizeof(Merch));
   Msg.PutFld(FN_AMOUNT,"99.99");
   Msg.PutFld(FN_LICENSESTATE,"CA");
   Msg.PutFld(FN_LICENSE,"12345678");
   Msg.PutFld(FN_DOB,"010101");
   Msg.PutFld(FN_CHECKNUMBER,"1234");
   Msg.PutFld(FN_PHONE,"9999999999");
   Msg.PutFld(FN_BANKNUMBER,"123456789");
   Msg.PutFld(FN_BANKACCOUNT,"123456789012");
   Msg.PutFld(FN_SSN,"268460550");
   Msg.PutFld(FN_MERCHID,"99999");
   Msg.PutFld(FN_CHECKSWIPE,(BYTE)'1');
   Msg.PutFld(FN_LICENSESWIPE,(BYTE)'1');
  }
}

void genVoiceData(bool average=false)
{
 if ( ! average )
  {
   Msg.PutFld(FN_CHECKTYPE,(BYTE)'P');
   Msg.PutFld(FN_APARTMENT,(BYTE)'Y');
   Msg.PutFld(FN_CITY,"123456789012345");
   Msg.PutFld(FN_CONSUMERSTATE,"CA");
   Msg.PutFld(FN_PRODUCTCLASS,"9999");
   Msg.PutFld(FN_TRANSPORTMETHOD,(BYTE)'1');
   Msg.PutFld(FN_PRIVATELABEL,(BYTE)'1');
   Msg.PutFld(FN_TITLE,(BYTE)'1');
   Msg.PutFld(FN_CONSUMERNAME,"ABCDEFGHIJKLMNOPQRSTUVWXY");
   Msg.PutFld(FN_ADDRESS,"12345678901234567890123456789012345678901234567890123456789012345679901234");
   Msg.PutFld(FN_BUSINESSNAME,"ABCDEFGHIJKLMNOPQRSTUVWXY");
  }
 else
  {
   Msg.PutFld(FN_CHECKTYPE,(BYTE)'P');
   Msg.PutFld(FN_APARTMENT,(BYTE)'Y');
   Msg.PutFld(FN_CITY,"12345678901");
   Msg.PutFld(FN_CONSUMERSTATE,"CA");
   Msg.PutFld(FN_PRODUCTCLASS,"9999");
   Msg.PutFld(FN_TRANSPORTMETHOD,(BYTE)'1');
   Msg.PutFld(FN_TITLE,(BYTE)'1');
   Msg.PutFld(FN_CONSUMERNAME,"ABCDEFGH");
   Msg.PutFld(FN_ADDRESS,"1234567890123");
  }
}

void genHoldCheckData()
{
 Msg.PutFld(FN_SPOUSESSN,"999999999");
 Msg.PutFld(FN_SPOUSENAME,"ABCDEFGHIJKLMNOPQRSTUVWXY");
 Msg.PutFld(FN_SPOUSESALARY,"999999");
 Msg.PutFld(FN_SPOUSEEMPLOYERPHONE,"9999999999");
 Msg.PutFld(FN_SPOUSEEMPLOYERADR3,"ABCDEFGHIJKLMNOPQRSTUVWXY");
 Msg.PutFld(FN_SPOUSEEMPLOYERADR2,"ABCDEFGHIJKLMNOPQRSTUVWXY");
 Msg.PutFld(FN_SPOUSEEMPLOYERADR1,"ABCDEFGHIJKLMNOPQRSTUVWXY");
 Msg.PutFld(FN_SPOUSEEMPLOYERNAME,"ABCDEFGHIJKLMNOPQRSTUVWXY");
 Msg.PutFld(FN_SALARY,"999999");
 Msg.PutFld(FN_EMPLOYERPHONE,"9999999999");
 Msg.PutFld(FN_EMPLOYERADR3,"ABCDEFGHIJKLMNOPQRSTUVWXY");
 Msg.PutFld(FN_EMPLOYERADR2,"ABCDEFGHIJKLMNOPQRSTUVWXY");
 Msg.PutFld(FN_EMPLOYERADR1,"ABCDEFGHIJKLMNOPQRSTUVWXY");
 Msg.PutFld(FN_EMPLOYERNAME,"ABCDEFGHIJKLMNOPQRSTUVWXY");
 Msg.PutFld(FN_VEHICLEVALUE,"999999.99");
 Msg.PutFld(FN_SPOUSEINFO,(BYTE)'Y');
 Msg.PutFld(FN_HOLDCHECKAMOUNT2,"99999.99");
 Msg.PutFld(FN_HOLDCHECKAMOUNT3,"99999.99");
 Msg.PutFld(FN_HOLDCHECKAMOUNT4,"99999.99");
 Msg.PutFld(FN_HOLDCHECKAPPROVALNUM2,(WORD)9999);
 Msg.PutFld(FN_HOLDCHECKAPPROVALNUM3,(WORD)9999);
 Msg.PutFld(FN_HOLDCHECKAPPROVALNUM4,(WORD)9999);
 Msg.PutFld(FN_HOLDCHECKCHECKNUM2,"999999");
 Msg.PutFld(FN_HOLDCHECKCHECKNUM3,"999999");
 Msg.PutFld(FN_HOLDCHECKCHECKNUM4,"999999");
 Msg.PutFld(FN_POSITION,"ABCDEFGHIJKLMNOPQRSTUVWXY");
 Msg.PutFld(FN_SPOUSEPOSITION,"ABCDEFGHIJKLMNOPQRSTUVWXY");
 Msg.PutFld(FN_YEARSEMPLOYED,"99");
 Msg.PutFld(FN_SPOUSEYEARSEMPLOYED,"99");
}

void genDBManagerData()
{
 Msg.PutFld(FN_MERCHANTRECORD,&merchantRecord,sizeof(merchantRecord));
 Msg.PutFld(FN_UNVFUNDSRULESRECORD,&secRulesRecord,sizeof(secRulesRecord));
 Msg.PutFld(FN_AUTHRULES,&authRulesRecord,sizeof(authRulesRecord));
 Msg.PutFld(FN_DATAREQRECORD,&dataReqRecord,sizeof(dataReqRecord));
 Msg.PutFld(FN_POSITIVERULESRECORD,&positiveRulesRecord,sizeof(positiveRulesRecord));
 Msg.PutFld(FN_POSITIVEFILERECORD,&posfileRecord,sizeof(posfileRecord));
 Msg.PutFld(FN_BANKPHONERECORD,&PhoneRec,sizeof(PhoneRec));
}

void genMiscVoiceData()
{
 Msg.PutFld(FN_AMOUNTROUNDED,(DWORD)999999);
 Msg.PutFld(FN_CONSUMERAGE,(BYTE)99);
 Msg.PutFld(FN_TIMESTAMP,(DWORD)1);
 Msg.PutFld(FN_REFERRALNUM,(BYTE)99);
 Msg.PutFld(FN_RCODES,"1234567890");
 Msg.PutFld(FN_RCODE,(BYTE)99);
 Msg.PutFld(FN_APPROVALNUM,(WORD)9999);
 Msg.PutFld(FN_SOURCETYPE,(BYTE)'P');
 Msg.PutFld(FN_FINANCEAMOUNT,"99999");
}

void genActivity(int Num)
{
 Msg.PutFld(FN_ACTIVITY,Activity,sizeof(ACTIVITY)*Num);
}

void genChecks(int Num)
{
 Msg.PutFld(FN_BADCHECKS,BadCheck,sizeof(BADCHECK)*Num);
}

void main(void)
{
 cout << "Empty Length = " << Msg.GetMsgLength() << "\n";

 // Average POS - No Activity
 genPosData(true);
 cout << "AVERAGE POS (Average Data+No Activity+No Checks) = " << Msg.GetMsgLength() << "\n";

 // MAX POS DATA
 genPosData();
 cout << "MAX POS (Max Data+No Activity+No Checks) = " << Msg.GetMsgLength() << "\n";

 // Pos with MAX VELOCITY and MAX checks
 Msg.ClearMsg();
 genPosData();
 genActivity(20);
 genChecks(20);
 cout << "MAX POS (Max Data+Max Activity+Max Checks) = " << Msg.GetMsgLength() << "\n";

  // Figure the Average size of a Voice Transaction
 Msg.ClearMsg();
 genPosData(true);
 genVoiceData(true);
 genDBManagerData();
 genActivity(2);
 genMiscVoiceData();
 cout << "AVERAGE VOICE (Average Data+2 Activities) = " << Msg.GetMsgLength() << "\n";

 // Figure the MAX size of a Voice Transaction
 Msg.ClearMsg();
 genPosData();
 genVoiceData();
 genDBManagerData();
 genHoldCheckData();
 genActivity(20);
 genChecks(20);
 genMiscVoiceData();
 cout << "MAX VOICE (Max Data+Max HoldCheck Data+Max Activity+Max Checks) = " << Msg.GetMsgLength() << "\n";

 if ( Msg.PutFldFail() )
  cout << "*** WARNING PUTFLDFAIL ****";
  
 getch();
}

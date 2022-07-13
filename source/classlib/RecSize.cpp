#include <stdlib.h>
#include <stdio.h>
#include <iostream.h>
#include <conio.h>
#include "defs.h"
#define HWND int
#include "ipcmsg.h"


#include "spsmandb.h"
#include "kwrec.h"

/*
#define FN_MERCHID (FLDNUM) 0
#define FN_AMOUNT (FLDNUM) 1
#define FN_CHECKTYPE (FLDNUM) 2
#define FN_LICENSESTATE (FLDNUM) 3
#define FN_LICENSE (FLDNUM) 4
#define FN_DOB (FLDNUM) 5
#define FN_CHECKNUMBER (FLDNUM) 6
#define FN_MICR (FLDNUM) 7
#define FN_FIRSTNAME (FLDNUM) 8
#define FN_LASTNAME (FLDNUM) 9
#define FN_ADDRESS (FLDNUM) 10
#define FN_APARTMENT (FLDNUM) 11
#define FN_CITY (FLDNUM) 12
#define FN_CONSUMERSTATE (FLDNUM) 13
#define FN_ZIPCODE (FLDNUM) 14
#define FN_AREACODE (FLDNUM) 15
#define FN_PHONE (FLDNUM) 16
#define FN_SSN (FLDNUM) 17
#define FN_PRODUCTCLASS (FLDNUM) 18
#define FN_TRANSPORTMETHOD (FLDNUM) 19
#define FN_BUSINESSNAME (FLDNUM) 20
#define FN_PRIVATELABEL (FLDNUM) 21
#define FN_TITLE (FLDNUM) 22
*/

struct MerchantRecord* mRec;

void main(void)
{
 int Base;
 int FieldsTotal;
 int SPSRecsTotal;

 printf(" sizeof(struct MerchantRecord)=%4d\n",sizeof(struct MerchantRecord));
 printf("             (without Classes)=%4d\n",
     sizeof(struct MerchantRecord)-sizeof(mRec->Classes) );
 printf(" sizeof(struct SecRulesRecord)=%4d\n",sizeof(struct SecRulesRecord));
 printf("  sizeof(struct DataReqRedord)=%4d\n",sizeof(struct DataReqRecord));
 printf("sizeof(struct AuthRulesRecord)=%4d\n",sizeof(struct AuthRulesRecord));
 printf("             (KW)sizeof(CHECK)=%4d\n",sizeof(CHECK));
 printf("             (KW)sizeof(AUDIT)=%4d\n",sizeof(AUDIT));
 printf("         (SPS)sizeof(ACTIVITY)=%4d\n",sizeof(ACTIVITY));
 printf("         (SPS)sizeof(BADCHECK)=%4d\n",sizeof(BADCHECK));
 getch();
 Base=MAX_MSG-sizeof(IPCROUTEINFO);
 printf(" IPCMessage Summary:       Base=%4d\n",Base);
 printf("  - Transaction Fields...\n");
 printf("                    -FN_MERCHID=%4d\n",Base-=8);
 printf("                        -AMOUNT=%4d\n",Base-=11);
 printf("                     -CHECKTYPE=%4d\n",Base-=3);
 printf("                  -LICENSESTATE=%4d\n",Base-=4);
 printf("                       -LICENSE=%4d\n",Base-=17);
 printf("                           -DOB=%4d\n",Base-=10);
 printf("                   -CHECKNUMBER=%4d\n",Base-=9);
 printf("                          -MICR=%4d\n",Base-=26);
 printf("                          -NAME=%4d\n",Base-=19);
 printf("                       -ADDRESS=%4d\n",Base-=61);
 printf("                     -APARTMENT=%4d\n",Base-=3);
 printf("                          -CITY=%4d\n",Base-=18);
 printf("                 -CONSUMERSTATE=%4d\n",Base-=5);
 printf("                       -ZIPCODE=%4d\n",Base-=8);
 printf("                         -PHONE=%4d\n",Base-=12);
 printf("                           -SSN=%4d\n",Base-=12);
 printf("                  -PRODUCTCLASS=%4d\n",Base-=3);
 printf("               -TRANSPORTMETHOD=%4d\n",Base-=3);
 printf("                  -BUSINESSNAME=%4d\n",Base-=18);
 printf("                  -PRIVATELABEL=%4d\n",Base-=3);
 printf("                         -TITLE=%4d\n",Base-=3);
 getch();
 FieldsTotal=MAX_MSG-sizeof(IPCROUTEINFO)-Base;
 printf("   - SPS records\n");
 printf("                -MerchantRecord=%4d\n",Base-=sizeof(struct MerchantRecord) -
                                                      sizeof(mRec->Classes) + 4);
 printf("                     -AuthRules=%4d\n",Base-=sizeof(struct AuthRulesRecord) + 4);
 printf("                       -DataReq=%4d\n",Base-=sizeof(struct DataReqRecord)+ 4);
 printf("                      -SecRules=%4d\n",Base-=sizeof(struct SecRulesRecord) + 4);
 SPSRecsTotal=MAX_MSG-sizeof(IPCROUTEINFO)-Base;
 getch();
 printf(" Summary:\n");
 printf("     Fields Total=%4d\n",FieldsTotal);
 printf("   SPS Recs Total=%4d\n",SPSRecsTotal);
 printf("   MAX AUDIT RECS=%4d\n",Base/sizeof(AUDIT));
 printf("   MAX CHECK RECS=%4d\n",Base/sizeof(CHECK));
 printf("MAX ACTIVITY RECS=%4d\n",Base/sizeof(ACTIVITY));
 printf("MAX BADCHECK RECS=%4d\n",Base/sizeof(BADCHECK));

 getch();
}
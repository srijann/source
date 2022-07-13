#define RECNAME_LEN 11

/* Product Group Macros */
#define NUMPRODUCTGROUPS 12
#define NUMPRODUCTSUBGROUPS 12

/* UnvFunds Macros */
#define NUMCHECKNUMROWS 9
#define NUMAGEROWS 9
#define NUMTIMEOFDAYROWS 7
#define NUMSSNSCOREROWS 7
#define NUMYEARSONJOBROWS 6
#define NUMVELOCITYTRANSROWS 5
#define NUMVELOCITYAMOUNTROWS 5
#define NUMBANKVERIFYAMOUNTROWS 6
#define NUMAMOUNTROWS 9

/* Positive File Macros */
#define NUMAGETBLROWS 9
#define NUMMULTIPLIERS 6

/* List Record Macros */
#define MAXLIST 8000


database SPSManV23 {
  data file "AuthRule.dat" contains AuthRulesRecord;
  key  file "AuthRule.key" contains AuthRulesKey;

  data file "DataReq.dat"  contains DataReqRecord;
  key  file "DataReq.key"  contains DataReqClassName;

  data file "UnvFunds.dat" contains UnvFundsRulesRecord;
  key  file "UnvFunds.key" contains UnvFundsRulesClassName;

  data file "Merchant.dat"  contains MerchantRecord;  
  key  file "Merchant.key"  contains MerchantID;
  key  file "MerchNam.key"  contains MerchantName;

  data file "Response.dat"  contains ResponseRecord;
  key  file "Response.key"  contains ResponseClassName;

  data file "PosRules.dat"  contains PositiveRulesRecord;
  key  file "PosRules.key"  contains PositiveRulesClassName;

  data file "Lists.dat" contains ListRecord;
  key  file "Lists.key" contains ListName;

  record ResponseRecord
   {
    unsigned long ResponseRecRecTStamp;
    unique key char ResponseClassName[RECNAME_LEN];
    char ResponseText[80][33];
   }

  record MerchantRecord
   {
    unsigned long MerchantRecTStamp;
    unique key char MerchantID[6];
    unique key char MerchantName[16];
    unsigned long  FieldEnablerBits;
    unsigned short FedRegionBits;
    char MerchantSic[5];
    char MerchantState[3];
    char MerchantZip[6];
    char MerchantService;
    unsigned char MerchantTimeZone;
    unsigned short VerifyAmount;
    unsigned char  PreauthDays;
    unsigned short PreauthAmount;
    unsigned short HoldCheckAmount;
    unsigned char  HoldCheck1Percent;
    unsigned char  HoldCheckValuePercent;
    unsigned char  ProductGroup;
    char MerchantFiller[34];
   struct {
      char Product[RECNAME_LEN];
      char GUAuthRules[RECNAME_LEN];
      char GUAuthRulesSub[RECNAME_LEN];
      char GUUnvFundsRules[RECNAME_LEN];
      char GUDataReq[RECNAME_LEN];
      char GUPosFileRules[RECNAME_LEN];
      char GURespMsg[RECNAME_LEN];
      char VFYAuthRules[RECNAME_LEN];
      char VFYAuthRulesSub[RECNAME_LEN];
      char VFYUnvFundsRules[RECNAME_LEN];
      char VFYDataReq[RECNAME_LEN];
      char VFYPosFileRules[RECNAME_LEN];
      char VFYRespMsg[RECNAME_LEN];
   } Classes;
  }

  record UnvFundsRulesRecord
   {
    unsigned long UnvFundsRulesTStamp;
    unique key char UnvFundsRulesClassName[RECNAME_LEN];
    unsigned long AmountValue[NUMAMOUNTROWS];
    char AmountPoints[NUMAMOUNTROWS];
    unsigned long CheckNumValue[NUMCHECKNUMROWS];
    char CheckNumPoints[NUMCHECKNUMROWS];
    unsigned char AgeValue[NUMAGEROWS];
    char AgePoints[NUMAGEROWS];
    unsigned short TimeValue[NUMTIMEOFDAYROWS];
    char TimePoints[NUMTIMEOFDAYROWS];
    char DayOfWeekPoints[7];
    unsigned short SSNScoreValue[NUMSSNSCOREROWS];
    char SSNScorePoints[NUMSSNSCOREROWS];
    unsigned char YearsOnJobValue[NUMYEARSONJOBROWS];
    char YearsOnJobPoints[NUMYEARSONJOBROWS];
    unsigned char VelocityDays;
    unsigned char VelocityTransValue[NUMVELOCITYTRANSROWS];
    char VelocityTransPoints[NUMVELOCITYTRANSROWS];
    unsigned long VelocityAmountValue[NUMVELOCITYAMOUNTROWS];
    char VelocityAmountPoints[NUMVELOCITYAMOUNTROWS];
    unsigned char BankVerifyNGDays;
    char BankVerifyNGYesPoints;
    char BankVerifyNGNoPoints;
    unsigned char BankVerifyGoodDays;
    unsigned char BankVerifyGoodNoPoints;
    unsigned long BankVerifyAmountValue[NUMBANKVERIFYAMOUNTROWS];
    char BankVerifyAmountPoints[NUMBANKVERIFYAMOUNTROWS];
    char DeliveryYesPoints;
    char DeliveryNoPoints;
    char LicenseInPoints;
    char LicenseOutPoints;
    char FedRegionInPoints;
    char FedRegionOutPoints;
    char ResidenceHousePoints;
    char ResidenceAptPoints;
    char ConsumerPhoneGoodPoints;
    char ConsumerPhoneNoGoodPoints;
    char EmployerPhoneGoodPoints;
    char EmployerPhoneNoGoodPoints;
    char PositiveFileYesPoints;
    char PositiveFileNoPoints;
    unsigned short TransactionCutoffValue;
    char UnvFundsFiller[40];
  }

  record DataReqRecord
   {
    unsigned long DataReqTStamp;
    unique key char DataReqClassName[RECNAME_LEN];
    /* Assuming 32 fields 0=Pos, 1=Voice, 2=Security */
    struct
     {
      // There are 64 bits but only the first 32 can have Amounts
      unsigned long  Bits[2];
      unsigned short Amounts[32];
     } DataReq[3];
   }

  record AuthRulesRecord {
   unsigned long AuthRulesTStamp;
   unique key struct
    {
     char ClassName[RECNAME_LEN];
     char SubClassName[RECNAME_LEN];
    } AuthRulesKey;
   struct
    {
     unsigned char CountableEnable;
     unsigned short CountableAmt;
     unsigned short CountableAccum;
    } Countable;
   struct
    {
     unsigned char R1Enable;
     unsigned char R1DateTime;
     unsigned char R1Product;
     unsigned char R1OvrdAge1;
     unsigned char R1OvrdAge2;
     unsigned short R1OvrdAmt1;
     unsigned short R1OvrdAmt2;
     unsigned char R1ReferAge1;
     unsigned char R1ReferAge2;
     unsigned short R1ReferAmt1;
     unsigned short R1ReferAmt2;
    } R1;
   struct
    {
     unsigned char R2Enable;
     unsigned char R2DateTime;
     unsigned char R2Product;
     unsigned char R2Sic1Trans;
     unsigned char R2Sic1Days;
     unsigned short R2Sic1Amt;
     unsigned char R2Sic2Trans;
     unsigned char R2Sic2Days;
     unsigned short R2Sic2Amt;
     unsigned char R2Acct1Trans;
     unsigned char R2Acct1Days;
     unsigned short R2Acct1Amt;
     unsigned char R2Acct2Trans;
     unsigned char R2Acct2Days;
     unsigned short R2Acct2Amt;
     unsigned char R2Loc1Trans;
     unsigned char R2Loc1Days;
     unsigned short R2Loc1Amt;
     unsigned char R2Loc2Trans;
     unsigned char R2Loc2Days;
     unsigned short R2Loc2Amt;
    } R2;
   struct
     {
     unsigned char R3Enable;
     unsigned char R3DateTime;
     unsigned char R3Product;
     // Age Referral
     unsigned char  R3AgeReferAge1;
     unsigned char  R3AgeReferAge2;
     unsigned char  R3AgeReferAge3;
     unsigned char  R3AgeReferAge4;
     unsigned long  R3AgeReferNum1From;
     unsigned long  R3AgeReferNum1To;
     unsigned long  R3AgeReferNum2From;
     unsigned long  R3AgeReferNum2To;
     unsigned long  R3AgeReferNum3From;
     unsigned long  R3AgeReferNum3To;
     unsigned long  R3AgeReferNum4From;
     unsigned long  R3AgeReferNum4To;
     unsigned short R3AgeReferAmt1From;
     unsigned short R3AgeReferAmt1To;
     unsigned short R3AgeReferAmt2From;
     unsigned short R3AgeReferAmt2To;
     unsigned short R3AgeReferAmt3From;
     unsigned short R3AgeReferAmt3To;
     unsigned short R3AgeReferAmt4From;
     unsigned short R3AgeReferAmt4To;
     // No Age Referral
     unsigned long  R3ReferNum1From;
     unsigned long  R3ReferNum1To;
     unsigned short R3ReferAmt1From;
     unsigned short R3ReferAmt1To;
     unsigned long  R3ReferNum2From;
     unsigned long  R3ReferNum2To;
     unsigned short R3ReferAmt2From;
     unsigned short R3ReferAmt2To;
     unsigned long  R3ReferNum3From;
     unsigned long  R3ReferNum3To;
     unsigned short R3ReferAmt3From;
     unsigned short R3ReferAmt3To;
     unsigned long  R3ReferNum4From;
     unsigned long  R3ReferNum4To;
     unsigned short R3ReferAmt4From;
     unsigned short R3ReferAmt4To;

     // Age Override
     unsigned char  R3AgeOvrdAge1;
     unsigned long  R3AgeOvrdNum1From;
     unsigned long  R3AgeOvrdNum1To;
     unsigned short R3AgeOvrdAmt1From;
     unsigned short R3AgeOvrdAmt1To;
     unsigned char  R3AgeOvrdAge2;
     unsigned long  R3AgeOvrdNum2From;
     unsigned long  R3AgeOvrdNum2To;
     unsigned short R3AgeOvrdAmt2From;
     unsigned short R3AgeOvrdAmt2To;
     unsigned char  R3AgeOvrdAge3;
     unsigned long  R3AgeOvrdNum3From;
     unsigned long  R3AgeOvrdNum3To;
     unsigned short R3AgeOvrdAmt3From;
     unsigned short R3AgeOvrdAmt3To;
     unsigned char  R3AgeOvrdAge4;
     unsigned long  R3AgeOvrdNum4From;
     unsigned long  R3AgeOvrdNum4To;
     unsigned short R3AgeOvrdAmt4From;
     unsigned short R3AgeOvrdAmt4To;
     // No Age Override
     unsigned long  R3OvrdNum1From;
     unsigned long  R3OvrdNum1To;
     unsigned short R3OvrdAmt1From;
     unsigned short R3OvrdAmt1To;
     unsigned long  R3OvrdNum2From;
     unsigned long  R3OvrdNum2To;
     unsigned short R3OvrdAmt2From;
     unsigned short R3OvrdAmt2To;
     unsigned long  R3OvrdNum3From;
     unsigned long  R3OvrdNum3To;
     unsigned short R3OvrdAmt3From;
     unsigned short R3OvrdAmt3To;
     unsigned long  R3OvrdNum4From;
     unsigned long  R3OvrdNum4To;
     unsigned short R3OvrdAmt4From;
     unsigned short R3OvrdAmt4To;
    } R3;
   struct
    {
     unsigned char R4Enable;
     unsigned char R4DateTime;
     unsigned char R4Product;
     unsigned char R4FwdDays1;
     unsigned short R4FwdGap1;
     unsigned char R4FwdDays2;
     unsigned short R4FwdGap2;
     unsigned char R4FwdDays3;
     unsigned short R4FwdGap3;
     unsigned char R4BackDays;
     unsigned short R4BackGap;
    } R4;
   struct
    {
     unsigned char R5Enable;
     unsigned char R5DateTime;
     unsigned char R5Product;
     unsigned short R5Amt;
    } R5;
   struct
    {
     unsigned char  R6Enable;
     unsigned char  R6DateTime;
     unsigned char  R6Product;
     unsigned short R6Amt;
     unsigned short R6Accum;
    } R6;
   struct
    {
     unsigned char R7Enable;
     unsigned char R7DateTime;
     unsigned char R7Product;
     unsigned char R7DailyNum;
     unsigned short R7DailyAmt;
     unsigned char R7Per1Days;
     unsigned char R7Per2Days;
     unsigned char R7Per3Days;
     unsigned char R7Per1Num;
     unsigned char R7Per2Num;
     unsigned char R7Per3Num;
     unsigned short R7Per1Amt;
     unsigned short R7Per2Amt;
     unsigned short R7Per3Amt;
    } R7;
   struct
    {
     unsigned char R8Enable;
     unsigned char R8DateTime;
     unsigned char R8Product;
     unsigned short R8Amt;
     unsigned short R8Accum;
    } R8;
   struct
    {
     unsigned short DT1From;
     unsigned short DT1To;
     unsigned char  DT1Time[7][1];
     unsigned short DT2From;
     unsigned short DT2To;
     unsigned char  DT2Time[7][1];
     unsigned short DT3From;
     unsigned short DT3To;
     unsigned char  DT3Time[7][1];
     unsigned short DT4From;
     unsigned short DT4To;
     unsigned char  DT4Time[7][1];
    } DT;
   unsigned char ProdAdj[NUMPRODUCTGROUPS][NUMPRODUCTSUBGROUPS];
  }

 record PositiveRulesRecord
   {
    unsigned long PositiveRulesRecTStamp;
    unique key char PositiveRulesClassName[RECNAME_LEN];
    unsigned char  MaxOverrides;
    unsigned long  MaxOverrideAccum;
    unsigned char  OverridePeriod;
    unsigned char  WaitForPositiveDays;
    unsigned char  MultiplyIfNumApprovals;
    unsigned long  MultiplyIfAmountApprovals;
    unsigned char  MultiplyIfLastWithinMonths;
    struct
     {
      unsigned char  MaxAge;
      unsigned short Limit;
      unsigned char  Multiplier[NUMMULTIPLIERS][1];
     } AgeTbl[NUMAGETBLROWS];
   }

  record ListRecord
   {
    unsigned short  ListLen;
    unsigned long   ListTStamp;
    unique key char ListName[RECNAME_LEN];
    char            ListData[MAXLIST][1];
   }

}

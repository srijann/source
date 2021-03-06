#define RECNAME_LEN 11

database SPSManDB {
  data file "AuthRule.dat" contains AuthRulesRecord;
  key  file "AuthRule.key" contains AuthRulesKey;

  data file "DataReq.dat"  contains DataReqRecord;
  key  file "DataReq.key"  contains DataReqClassName;

  data file "SecRules.dat" contains SecRulesRecord;
  key  file "SecRules.key" contains SecRulesClassName;

  data file "Merchant.dat"  contains MerchantRecord;  
  key  file "Merchant.key"  contains MerchantID;
  key  file "MerchNam.key"  contains MerchantName;

  data file "Response.dat"  contains ResponseRecord;
  key  file "Response.key"  contains ResponseClassName;

  record ResponseRecord
   {
    unsigned long ResponseRecRecTStamp;
    unique key char ResponseClassName[RECNAME_LEN];
    char Text[80][33];
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
    char PrivateLabelName[16];
    char MerchantFiller[25];
    struct {
      char Product[RECNAME_LEN];
      char GUAuthRules[RECNAME_LEN];
      char GUAuthRulesSub[RECNAME_LEN];
      char GUSecRules[RECNAME_LEN];
      char GUDataReq[RECNAME_LEN];
      char GUPosFileRules[RECNAME_LEN];
      char GURespMsg[RECNAME_LEN];
      char VFYAuthRules[RECNAME_LEN];
      char VFYAuthRulesSub[RECNAME_LEN];
      char VFYSecRules[RECNAME_LEN];
      char VFYDataReq[RECNAME_LEN];
      char VFYPosFileRules[RECNAME_LEN];
      char VFYRespMsg[RECNAME_LEN];
   } Classes;
  }

  record SecRulesRecord
   {
    unsigned long SecRulesTStamp;
    unique key char SecRulesClassName[RECNAME_LEN];
    unsigned long RuleEnablerBits;
    unsigned short HiRiskProduct1ToTime;
    unsigned short HiRiskProduct1FromTime;
    unsigned char  HiRiskProduct1DayOfWeek;
    unsigned short HiRiskProduct1Amount;
    unsigned char  HiRiskProduct1Value;
    unsigned short HiRiskProduct2ToTime;
    unsigned short HiRiskProdcut2FromTime;
    unsigned short HiRiskProduct2FromTime;
    unsigned char  HiRiskProduct2DayOfWeek;
    unsigned short HiRiskProduct2Amount;
    unsigned char  HiRiskProduct2Value;
    unsigned long  HiRiskCheckNumberValue;
    unsigned short HiRiskAmountValue;
    unsigned char  MaxRcodesNum;
    char HiRiskCALicenses[10][2];
    unsigned char  MaxBankVerifyDays;
    unsigned char  MaxBankVerifyNumber;
    unsigned short LowCreditScoreValue;
    unsigned short HiRiskDemographicClass;
    unsigned char  HiRiskAgeValue;
    char SecRulesFiller[20];
  }

  record DataReqRecord
   {
    unsigned long DataReqTStamp;
    unique key char DataReqClassName[RECNAME_LEN];
    /* Assuming 32 fields 0=Pos, 1=Voice, 2=Security */
    struct
     {
      unsigned long  Bits;
      unsigned short Amounts[32];
     } DataReq[3];
   }

  record AuthRulesRecord {
   unsigned long AuthRulesTStamp;
   char AuthRulesClassName[RECNAME_LEN];
   char AuthRulesSubClassName[RECNAME_LEN];
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
     unsigned long R3ReferNum1;
     unsigned short R3ReferAmt1;
     unsigned long R3ReferNum2;
     unsigned short R3ReferAmt2;
     unsigned char R3AgeReferAge1;
     unsigned long R3AgeReferNum1;
     unsigned short R3AgeReferAmt1;
     unsigned char R3AgeReferAge2;
     unsigned long R3AgeReferNum2;
     unsigned short R3AgeReferAmt2;
     unsigned long R3OvrdNum1;
     unsigned short R3OvrdAmt1;
     unsigned long R3OvrdNum2;
     unsigned short R3OvrdAmt2;
     unsigned char R3AgeOvrdAge1;
     unsigned long R3AgeOvrdNum1;
     unsigned short R3AgeOvrdAmt1;
     unsigned char R3AgeOvrdAge2;
     unsigned long R3AgeOvrdNum2;
     unsigned short R3AgeOvrdAmt2;
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
   unsigned short NetLossFrom;
   unsigned short NetLossTo;
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
   unsigned char ProdAdj[20][1];
   char AuthRulesFiller[20];

   compound unique key AuthRulesKey {
     AuthRulesClassName ascending;
     AuthRulesSubClassName ascending;
    }

  }

}

#ifndef SPSMANDB_H
#define SPSMANDB_H

#define RECNAME_LEN 11


/* database SPSManDB record/key structure declarations */

struct ResponseRecord {
   unsigned long ResponseRecRecTStamp;
   char ResponseClassName[RECNAME_LEN];
   char Text[80][33];
};

struct MerchantRecord {
   unsigned long MerchantRecTStamp;
   char MerchantID[6];
   char MerchantName[16];
   unsigned long FieldEnablerBits;
   unsigned short FedRegionBits;
   char MerchantSic[5];
   char MerchantState[3];
   char MerchantZip[6];
   char MerchantService;
   unsigned char MerchantTimeZone;
   unsigned short VerifyAmount;
   unsigned char PreauthDays;
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
};

struct SecRulesRecord {
   unsigned long SecRulesTStamp;
   char SecRulesClassName[RECNAME_LEN];
   unsigned long RuleEnablerBits;
   unsigned short HiRiskProduct1ToTime;
   unsigned short HiRiskProduct1FromTime;
   unsigned char HiRiskProduct1DayOfWeek;
   unsigned short HiRiskProduct1Amount;
   unsigned char HiRiskProduct1Value;
   unsigned short HiRiskProduct2ToTime;
   unsigned short HiRiskProdcut2FromTime;
   unsigned short HiRiskProduct2FromTime;
   unsigned char HiRiskProduct2DayOfWeek;
   unsigned short HiRiskProduct2Amount;
   unsigned char HiRiskProduct2Value;
   unsigned long HiRiskCheckNumberValue;
   unsigned short HiRiskAmountValue;
   unsigned char MaxRcodesNum;
   char HiRiskCALicenses[10][2];
   unsigned char MaxBankVerifyDays;
   unsigned char MaxBankVerifyNumber;
   unsigned short LowCreditScoreValue;
   unsigned short HiRiskDemographicClass;
   unsigned char HiRiskAgeValue;
   char SecRulesFiller[20];
};

struct DataReqRecord {
   unsigned long DataReqTStamp;
   char DataReqClassName[RECNAME_LEN];
   struct {
      unsigned long Bits;
      unsigned short Amounts[32];
   } DataReq[3];
};

struct AuthRulesRecord {
   unsigned long AuthRulesTStamp;
   char AuthRulesClassName[RECNAME_LEN];
   char AuthRulesSubClassName[RECNAME_LEN];
   struct {
      unsigned char CountableEnable;
      unsigned short CountableAmt;
      unsigned short CountableAccum;
   } Countable;
   struct {
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
   struct {
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
   struct {
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
   struct {
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
   struct {
      unsigned char R5Enable;
      unsigned char R5DateTime;
      unsigned char R5Product;
      unsigned short R5Amt;
   } R5;
   struct {
      unsigned char R6Enable;
      unsigned char R6DateTime;
      unsigned char R6Product;
      unsigned short R6Amt;
      unsigned short R6Accum;
   } R6;
   struct {
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
   struct {
      unsigned char R8Enable;
      unsigned char R8DateTime;
      unsigned char R8Product;
      unsigned short R8Amt;
      unsigned short R8Accum;
   } R8;
   unsigned short NetLossFrom;
   unsigned short NetLossTo;
   struct {
      unsigned short DT1From;
      unsigned short DT1To;
      unsigned char DT1Time[7][1];
      unsigned short DT2From;
      unsigned short DT2To;
      unsigned char DT2Time[7][1];
      unsigned short DT3From;
      unsigned short DT3To;
      unsigned char DT3Time[7][1];
      unsigned short DT4From;
      unsigned short DT4To;
      unsigned char DT4Time[7][1];
   } DT;
   unsigned char ProdAdj[20][1];
   char AuthRulesFiller[20];
};

struct AuthRulesKey {
   char AuthRulesClassName[RECNAME_LEN];
   char AuthRulesSubClassName[RECNAME_LEN];
};

/* record, field and set table entry definitions */

/* File Id Constants */

/* Record Name Constants */
#define RESPONSERECORD 10000
#define MERCHANTRECORD 10001
#define SECRULESRECORD 10002
#define DATAREQRECORD 10003
#define AUTHRULESRECORD 10004

/* Field Name Constants */
#define RESPONSERECRECTSTAMP 0L
#define RESPONSECLASSNAME 1L
#define TEXT 2L
#define MERCHANTRECTSTAMP 1000L
#define MERCHANTID 1001L
#define MERCHANTNAME 1002L
#define FIELDENABLERBITS 1003L
#define FEDREGIONBITS 1004L
#define MERCHANTSIC 1005L
#define MERCHANTSTATE 1006L
#define MERCHANTZIP 1007L
#define MERCHANTSERVICE 1008L
#define MERCHANTTIMEZONE 1009L
#define VERIFYAMOUNT 1010L
#define PREAUTHDAYS 1011L
#define PRIVATELABELNAME 1012L
#define MERCHANTFILLER 1013L
#define CLASSES 1014L
#define PRODUCT 1015L
#define GUAUTHRULES 1016L
#define GUAUTHRULESSUB 1017L
#define GUSECRULES 1018L
#define GUDATAREQ 1019L
#define GUPOSFILERULES 1020L
#define GURESPMSG 1021L
#define VFYAUTHRULES 1022L
#define VFYAUTHRULESSUB 1023L
#define VFYSECRULES 1024L
#define VFYDATAREQ 1025L
#define VFYPOSFILERULES 1026L
#define VFYRESPMSG 1027L
#define SECRULESTSTAMP 2000L
#define SECRULESCLASSNAME 2001L
#define RULEENABLERBITS 2002L
#define HIRISKPRODUCT1TOTIME 2003L
#define HIRISKPRODUCT1FROMTIME 2004L
#define HIRISKPRODUCT1DAYOFWEEK 2005L
#define HIRISKPRODUCT1AMOUNT 2006L
#define HIRISKPRODUCT1VALUE 2007L
#define HIRISKPRODUCT2TOTIME 2008L
#define HIRISKPRODCUT2FROMTIME 2009L
#define HIRISKPRODUCT2FROMTIME 2010L
#define HIRISKPRODUCT2DAYOFWEEK 2011L
#define HIRISKPRODUCT2AMOUNT 2012L
#define HIRISKPRODUCT2VALUE 2013L
#define HIRISKCHECKNUMBERVALUE 2014L
#define HIRISKAMOUNTVALUE 2015L
#define MAXRCODESNUM 2016L
#define HIRISKCALICENSES 2017L
#define MAXBANKVERIFYDAYS 2018L
#define MAXBANKVERIFYNUMBER 2019L
#define LOWCREDITSCOREVALUE 2020L
#define HIRISKDEMOGRAPHICCLASS 2021L
#define HIRISKAGEVALUE 2022L
#define SECRULESFILLER 2023L
#define DATAREQTSTAMP 3000L
#define DATAREQCLASSNAME 3001L
#define DATAREQ 3002L
#define BITS 3003L
#define AMOUNTS 3004L
#define AUTHRULESTSTAMP 4000L
#define AUTHRULESCLASSNAME 4001L
#define AUTHRULESSUBCLASSNAME 4002L
#define COUNTABLE 4003L
#define COUNTABLEENABLE 4004L
#define COUNTABLEAMT 4005L
#define COUNTABLEACCUM 4006L
#define R1 4007L
#define R1ENABLE 4008L
#define R1DATETIME 4009L
#define R1PRODUCT 4010L
#define R1OVRDAGE1 4011L
#define R1OVRDAGE2 4012L
#define R1OVRDAMT1 4013L
#define R1OVRDAMT2 4014L
#define R1REFERAGE1 4015L
#define R1REFERAGE2 4016L
#define R1REFERAMT1 4017L
#define R1REFERAMT2 4018L
#define R2 4019L
#define R2ENABLE 4020L
#define R2DATETIME 4021L
#define R2PRODUCT 4022L
#define R2SIC1TRANS 4023L
#define R2SIC1DAYS 4024L
#define R2SIC1AMT 4025L
#define R2SIC2TRANS 4026L
#define R2SIC2DAYS 4027L
#define R2SIC2AMT 4028L
#define R2ACCT1TRANS 4029L
#define R2ACCT1DAYS 4030L
#define R2ACCT1AMT 4031L
#define R2ACCT2TRANS 4032L
#define R2ACCT2DAYS 4033L
#define R2ACCT2AMT 4034L
#define R2LOC1TRANS 4035L
#define R2LOC1DAYS 4036L
#define R2LOC1AMT 4037L
#define R2LOC2TRANS 4038L
#define R2LOC2DAYS 4039L
#define R2LOC2AMT 4040L
#define R3 4041L
#define R3ENABLE 4042L
#define R3DATETIME 4043L
#define R3PRODUCT 4044L
#define R3REFERNUM1 4045L
#define R3REFERAMT1 4046L
#define R3REFERNUM2 4047L
#define R3REFERAMT2 4048L
#define R3AGEREFERAGE1 4049L
#define R3AGEREFERNUM1 4050L
#define R3AGEREFERAMT1 4051L
#define R3AGEREFERAGE2 4052L
#define R3AGEREFERNUM2 4053L
#define R3AGEREFERAMT2 4054L
#define R3OVRDNUM1 4055L
#define R3OVRDAMT1 4056L
#define R3OVRDNUM2 4057L
#define R3OVRDAMT2 4058L
#define R3AGEOVRDAGE1 4059L
#define R3AGEOVRDNUM1 4060L
#define R3AGEOVRDAMT1 4061L
#define R3AGEOVRDAGE2 4062L
#define R3AGEOVRDNUM2 4063L
#define R3AGEOVRDAMT2 4064L
#define R4 4065L
#define R4ENABLE 4066L
#define R4DATETIME 4067L
#define R4PRODUCT 4068L
#define R4FWDDAYS1 4069L
#define R4FWDGAP1 4070L
#define R4FWDDAYS2 4071L
#define R4FWDGAP2 4072L
#define R4FWDDAYS3 4073L
#define R4FWDGAP3 4074L
#define R4BACKDAYS 4075L
#define R4BACKGAP 4076L
#define R5 4077L
#define R5ENABLE 4078L
#define R5DATETIME 4079L
#define R5PRODUCT 4080L
#define R5AMT 4081L
#define R6 4082L
#define R6ENABLE 4083L
#define R6DATETIME 4084L
#define R6PRODUCT 4085L
#define R6AMT 4086L
#define R6ACCUM 4087L
#define R7 4088L
#define R7ENABLE 4089L
#define R7DATETIME 4090L
#define R7PRODUCT 4091L
#define R7DAILYNUM 4092L
#define R7DAILYAMT 4093L
#define R7PER1DAYS 4094L
#define R7PER2DAYS 4095L
#define R7PER3DAYS 4096L
#define R7PER1NUM 4097L
#define R7PER2NUM 4098L
#define R7PER3NUM 4099L
#define R7PER1AMT 4100L
#define R7PER2AMT 4101L
#define R7PER3AMT 4102L
#define R8 4103L
#define R8ENABLE 4104L
#define R8DATETIME 4105L
#define R8PRODUCT 4106L
#define R8AMT 4107L
#define R8ACCUM 4108L
#define NETLOSSFROM 4109L
#define NETLOSSTO 4110L
#define DT 4111L
#define DT1FROM 4112L
#define DT1TO 4113L
#define DT1TIME 4114L
#define DT2FROM 4115L
#define DT2TO 4116L
#define DT2TIME 4117L
#define DT3FROM 4118L
#define DT3TO 4119L
#define DT3TIME 4120L
#define DT4FROM 4121L
#define DT4TO 4122L
#define DT4TIME 4123L
#define PRODADJ 4124L
#define AUTHRULESFILLER 4125L
#define AUTHRULESKEY 4126L

/* Set Name Constants */

#endif    /* SPSMANDB_H */

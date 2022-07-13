#ifndef SPSMANV21_H
#define SPSMANV21_H

#define RECNAME_LEN 11


/* database SPSManV20 record/key structure declarations */

struct OldResponseRecord {
   unsigned long ResponseRecRecTStamp;
   char ResponseClassName[RECNAME_LEN];
   char Text[80][33];
};

struct OldMerchantRecord {
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
   unsigned short PreauthAmount;
   unsigned short HoldCheckAmount;
   unsigned char HoldCheck1Percent;
   unsigned char HoldCheckValuePercent;
   char MerchantFiller[35];
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

struct OldSecRulesRecord {
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

struct OldDataReqRecord {
   unsigned long DataReqTStamp;
   char DataReqClassName[RECNAME_LEN];
   struct {
      unsigned long Bits[2];
      unsigned short Amounts[32];
   } DataReq[3];
};

struct OldAuthRulesRecord {
   unsigned long AuthRulesTStamp;
   struct {
      char ClassName[RECNAME_LEN];
      char SubClassName[RECNAME_LEN];
   } AuthRulesKey;
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
      unsigned char R3AgeReferAge1;
      unsigned char R3AgeReferAge2;
      unsigned char R3AgeReferAge3;
      unsigned char R3AgeReferAge4;
      unsigned long R3AgeReferNum1From;
      unsigned long R3AgeReferNum1To;
      unsigned long R3AgeReferNum2From;
      unsigned long R3AgeReferNum2To;
      unsigned long R3AgeReferNum3From;
      unsigned long R3AgeReferNum3To;
      unsigned long R3AgeReferNum4From;
      unsigned long R3AgeReferNum4To;
      unsigned short R3AgeReferAmt1From;
      unsigned short R3AgeReferAmt1To;
      unsigned short R3AgeReferAmt2From;
      unsigned short R3AgeReferAmt2To;
      unsigned short R3AgeReferAmt3From;
      unsigned short R3AgeReferAmt3To;
      unsigned short R3AgeReferAmt4From;
      unsigned short R3AgeReferAmt4To;
      unsigned long R3ReferNum1From;
      unsigned long R3ReferNum1To;
      unsigned short R3ReferAmt1From;
      unsigned short R3ReferAmt1To;
      unsigned long R3ReferNum2From;
      unsigned long R3ReferNum2To;
      unsigned short R3ReferAmt2From;
      unsigned short R3ReferAmt2To;
      unsigned long R3ReferNum3From;
      unsigned long R3ReferNum3To;
      unsigned short R3ReferAmt3From;
      unsigned short R3ReferAmt3To;
      unsigned long R3ReferNum4From;
      unsigned long R3ReferNum4To;
      unsigned short R3ReferAmt4From;
      unsigned short R3ReferAmt4To;
      unsigned char R3AgeOvrdAge1;
      unsigned long R3AgeOvrdNum1From;
      unsigned long R3AgeOvrdNum1To;
      unsigned short R3AgeOvrdAmt1From;
      unsigned short R3AgeOvrdAmt1To;
      unsigned char R3AgeOvrdAge2;
      unsigned long R3AgeOvrdNum2From;
      unsigned long R3AgeOvrdNum2To;
      unsigned short R3AgeOvrdAmt2From;
      unsigned short R3AgeOvrdAmt2To;
      unsigned char R3AgeOvrdAge3;
      unsigned long R3AgeOvrdNum3From;
      unsigned long R3AgeOvrdNum3To;
      unsigned short R3AgeOvrdAmt3From;
      unsigned short R3AgeOvrdAmt3To;
      unsigned char R3AgeOvrdAge4;
      unsigned long R3AgeOvrdNum4From;
      unsigned long R3AgeOvrdNum4To;
      unsigned short R3AgeOvrdAmt4From;
      unsigned short R3AgeOvrdAmt4To;
      unsigned long R3OvrdNum1From;
      unsigned long R3OvrdNum1To;
      unsigned short R3OvrdAmt1From;
      unsigned short R3OvrdAmt1To;
      unsigned long R3OvrdNum2From;
      unsigned long R3OvrdNum2To;
      unsigned short R3OvrdAmt2From;
      unsigned short R3OvrdAmt2To;
      unsigned long R3OvrdNum3From;
      unsigned long R3OvrdNum3To;
      unsigned short R3OvrdAmt3From;
      unsigned short R3OvrdAmt3To;
      unsigned long R3OvrdNum4From;
      unsigned long R3OvrdNum4To;
      unsigned short R3OvrdAmt4From;
      unsigned short R3OvrdAmt4To;
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
};

struct OldPositiveRulesRecord {
   unsigned long PositiveRulesRecTStamp;
   char PositiveRulesClassName[RECNAME_LEN];
   unsigned char MaxOverrides;
   unsigned long MaxOverrideAccum;
   unsigned char OverridePeriod;
   unsigned char WaitForPositiveDays;
   unsigned char MultiplyIfNumApprovals;
   unsigned long MultiplyIfAmountApprovals;
   unsigned char MultiplyIfLastWithinMonths;
   struct {
      unsigned char MaxAge;
      unsigned short Limit;
      unsigned char Multiplier[6][1];
   } AgeTbl[9];
};

/* record, field and set table entry definitions */

/* File Id Constants */

/* Record Name Constants */
#define RESPONSERECORD 10000
#define MERCHANTRECORD 10001
#define SECRULESRECORD 10002
#define DATAREQRECORD 10003
#define AUTHRULESRECORD 10004
#define POSITIVERULESRECORD 10005
#define RESPONSECLASSNAME 1L
#define MERCHANTID 1001L
#define MERCHANTNAME 1002L
#define SECRULESCLASSNAME 2001L
#define DATAREQCLASSNAME 3001L
#define AUTHRULESKEY 4001L
#define POSITIVERULESCLASSNAME 5001L

#ifndef NOFLDNAMES
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
#define PREAUTHAMOUNT 1012L
#define HOLDCHECKAMOUNT 1013L
#define HOLDCHECK1PERCENT 1014L
#define HOLDCHECKVALUEPERCENT 1015L
#define MERCHANTFILLER 1016L
#define CLASSES 1017L
#define PRODUCT 1018L
#define GUAUTHRULES 1019L
#define GUAUTHRULESSUB 1020L
#define GUSECRULES 1021L
#define GUDATAREQ 1022L
#define GUPOSFILERULES 1023L
#define GURESPMSG 1024L
#define VFYAUTHRULES 1025L
#define VFYAUTHRULESSUB 1026L
#define VFYSECRULES 1027L
#define VFYDATAREQ 1028L
#define VFYPOSFILERULES 1029L
#define VFYRESPMSG 1030L
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
#define AUTHRULESKEY 4001L
#define CLASSNAME 4002L
#define SUBCLASSNAME 4003L
#define COUNTABLE 4004L
#define COUNTABLEENABLE 4005L
#define COUNTABLEAMT 4006L
#define COUNTABLEACCUM 4007L
#define R1 4008L
#define R1ENABLE 4009L
#define R1DATETIME 4010L
#define R1PRODUCT 4011L
#define R1OVRDAGE1 4012L
#define R1OVRDAGE2 4013L
#define R1OVRDAMT1 4014L
#define R1OVRDAMT2 4015L
#define R1REFERAGE1 4016L
#define R1REFERAGE2 4017L
#define R1REFERAMT1 4018L
#define R1REFERAMT2 4019L
#define R2 4020L
#define R2ENABLE 4021L
#define R2DATETIME 4022L
#define R2PRODUCT 4023L
#define R2SIC1TRANS 4024L
#define R2SIC1DAYS 4025L
#define R2SIC1AMT 4026L
#define R2SIC2TRANS 4027L
#define R2SIC2DAYS 4028L
#define R2SIC2AMT 4029L
#define R2ACCT1TRANS 4030L
#define R2ACCT1DAYS 4031L
#define R2ACCT1AMT 4032L
#define R2ACCT2TRANS 4033L
#define R2ACCT2DAYS 4034L
#define R2ACCT2AMT 4035L
#define R2LOC1TRANS 4036L
#define R2LOC1DAYS 4037L
#define R2LOC1AMT 4038L
#define R2LOC2TRANS 4039L
#define R2LOC2DAYS 4040L
#define R2LOC2AMT 4041L
#define R3 4042L
#define R3ENABLE 4043L
#define R3DATETIME 4044L
#define R3PRODUCT 4045L
#define R3AGEREFERAGE1 4046L
#define R3AGEREFERAGE2 4047L
#define R3AGEREFERAGE3 4048L
#define R3AGEREFERAGE4 4049L
#define R3AGEREFERNUM1FROM 4050L
#define R3AGEREFERNUM1TO 4051L
#define R3AGEREFERNUM2FROM 4052L
#define R3AGEREFERNUM2TO 4053L
#define R3AGEREFERNUM3FROM 4054L
#define R3AGEREFERNUM3TO 4055L
#define R3AGEREFERNUM4FROM 4056L
#define R3AGEREFERNUM4TO 4057L
#define R3AGEREFERAMT1FROM 4058L
#define R3AGEREFERAMT1TO 4059L
#define R3AGEREFERAMT2FROM 4060L
#define R3AGEREFERAMT2TO 4061L
#define R3AGEREFERAMT3FROM 4062L
#define R3AGEREFERAMT3TO 4063L
#define R3AGEREFERAMT4FROM 4064L
#define R3AGEREFERAMT4TO 4065L
#define R3REFERNUM1FROM 4066L
#define R3REFERNUM1TO 4067L
#define R3REFERAMT1FROM 4068L
#define R3REFERAMT1TO 4069L
#define R3REFERNUM2FROM 4070L
#define R3REFERNUM2TO 4071L
#define R3REFERAMT2FROM 4072L
#define R3REFERAMT2TO 4073L
#define R3REFERNUM3FROM 4074L
#define R3REFERNUM3TO 4075L
#define R3REFERAMT3FROM 4076L
#define R3REFERAMT3TO 4077L
#define R3REFERNUM4FROM 4078L
#define R3REFERNUM4TO 4079L
#define R3REFERAMT4FROM 4080L
#define R3REFERAMT4TO 4081L
#define R3AGEOVRDAGE1 4082L
#define R3AGEOVRDNUM1FROM 4083L
#define R3AGEOVRDNUM1TO 4084L
#define R3AGEOVRDAMT1FROM 4085L
#define R3AGEOVRDAMT1TO 4086L
#define R3AGEOVRDAGE2 4087L
#define R3AGEOVRDNUM2FROM 4088L
#define R3AGEOVRDNUM2TO 4089L
#define R3AGEOVRDAMT2FROM 4090L
#define R3AGEOVRDAMT2TO 4091L
#define R3AGEOVRDAGE3 4092L
#define R3AGEOVRDNUM3FROM 4093L
#define R3AGEOVRDNUM3TO 4094L
#define R3AGEOVRDAMT3FROM 4095L
#define R3AGEOVRDAMT3TO 4096L
#define R3AGEOVRDAGE4 4097L
#define R3AGEOVRDNUM4FROM 4098L
#define R3AGEOVRDNUM4TO 4099L
#define R3AGEOVRDAMT4FROM 4100L
#define R3AGEOVRDAMT4TO 4101L
#define R3OVRDNUM1FROM 4102L
#define R3OVRDNUM1TO 4103L
#define R3OVRDAMT1FROM 4104L
#define R3OVRDAMT1TO 4105L
#define R3OVRDNUM2FROM 4106L
#define R3OVRDNUM2TO 4107L
#define R3OVRDAMT2FROM 4108L
#define R3OVRDAMT2TO 4109L
#define R3OVRDNUM3FROM 4110L
#define R3OVRDNUM3TO 4111L
#define R3OVRDAMT3FROM 4112L
#define R3OVRDAMT3TO 4113L
#define R3OVRDNUM4FROM 4114L
#define R3OVRDNUM4TO 4115L
#define R3OVRDAMT4FROM 4116L
#define R3OVRDAMT4TO 4117L
#define R4 4118L
#define R4ENABLE 4119L
#define R4DATETIME 4120L
#define R4PRODUCT 4121L
#define R4FWDDAYS1 4122L
#define R4FWDGAP1 4123L
#define R4FWDDAYS2 4124L
#define R4FWDGAP2 4125L
#define R4FWDDAYS3 4126L
#define R4FWDGAP3 4127L
#define R4BACKDAYS 4128L
#define R4BACKGAP 4129L
#define R5 4130L
#define R5ENABLE 4131L
#define R5DATETIME 4132L
#define R5PRODUCT 4133L
#define R5AMT 4134L
#define R6 4135L
#define R6ENABLE 4136L
#define R6DATETIME 4137L
#define R6PRODUCT 4138L
#define R6AMT 4139L
#define R6ACCUM 4140L
#define R7 4141L
#define R7ENABLE 4142L
#define R7DATETIME 4143L
#define R7PRODUCT 4144L
#define R7DAILYNUM 4145L
#define R7DAILYAMT 4146L
#define R7PER1DAYS 4147L
#define R7PER2DAYS 4148L
#define R7PER3DAYS 4149L
#define R7PER1NUM 4150L
#define R7PER2NUM 4151L
#define R7PER3NUM 4152L
#define R7PER1AMT 4153L
#define R7PER2AMT 4154L
#define R7PER3AMT 4155L
#define R8 4156L
#define R8ENABLE 4157L
#define R8DATETIME 4158L
#define R8PRODUCT 4159L
#define R8AMT 4160L
#define R8ACCUM 4161L
#define DT 4162L
#define DT1FROM 4163L
#define DT1TO 4164L
#define DT1TIME 4165L
#define DT2FROM 4166L
#define DT2TO 4167L
#define DT2TIME 4168L
#define DT3FROM 4169L
#define DT3TO 4170L
#define DT3TIME 4171L
#define DT4FROM 4172L
#define DT4TO 4173L
#define DT4TIME 4174L
#define PRODADJ 4175L
#define POSITIVERULESRECTSTAMP 5000L
#define POSITIVERULESCLASSNAME 5001L
#define MAXOVERRIDES 5002L
#define MAXOVERRIDEACCUM 5003L
#define OVERRIDEPERIOD 5004L
#define WAITFORPOSITIVEDAYS 5005L
#define MULTIPLYIFNUMAPPROVALS 5006L
#define MULTIPLYIFAMOUNTAPPROVALS 5007L
#define MULTIPLYIFLASTWITHINMONTHS 5008L
#define AGETBL 5009L
#define MAXAGE 5010L
#define LIMIT 5011L
#define MULTIPLIER 5012L
#endif
/* Set Name Constants */

#endif    /* SPSMANV21_H */

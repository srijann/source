#ifndef SPSLOGDB_H
#define SPSLOGDB_H


/* database SPSLogDB record/key structure declarations */

struct LogRecord {
   unsigned long Date;
   unsigned long Time;
   char MerchantID[6];
   char BankNumber[10];
   char BankAccount[17];
   char DriversLicense[18];
   unsigned long SSn;
   char PhoneNumber[11];
   unsigned short ApprovalNumber;
   unsigned short Operator;
   unsigned short MsgLength;
   char Message[8192][1];
};

struct DateKey {
   unsigned long Date;
   unsigned long Time;
};

struct MerchantKey {
   char MerchantID[6];
   unsigned long Date;
   unsigned long Time;
};

struct OperatorKey {
   unsigned short Operator;
   unsigned long Date;
   unsigned long Time;
};

struct MicrKey {
   char BankNumber[10];
   char BankAccount[17];
   unsigned long Date;
   unsigned long Time;
};

struct LicenseKey {
   char DriversLicense[18];
   unsigned long Date;
   unsigned long Time;
};

struct SSNKey {
   unsigned long SSn;
   unsigned long Date;
   unsigned long Time;
};

struct PhoneKey {
   char PhoneNumber[11];
   unsigned long Date;
   unsigned long Time;
};

struct ApprovalNumberKey {
   unsigned short ApprovalNumber;
   unsigned long Date;
   unsigned long Time;
};

/* record, field and set table entry definitions */

/* File Id Constants */

/* Record Name Constants */
#define LOGRECORD 10000

/* Field Name Constants */
#define DATE 0L
#define TIME 1L
#define MERCHANTID 2L
#define BANKNUMBER 3L
#define BANKACCOUNT 4L
#define DRIVERSLICENSE 5L
#define SSN 6L
#define PHONENUMBER 7L
#define APPROVALNUMBER 8L
#define OPERATOR 9L
#define MSGLENGTH 10L
#define MESSAGE 11L
#define DATEKEY 12L
#define MERCHANTKEY 13L
#define OPERATORKEY 14L
#define MICRKEY 15L
#define LICENSEKEY 16L
#define SSNKEY 17L
#define PHONEKEY 18L
#define APPROVALNUMBERKEY 19L

/* Set Name Constants */

#endif    /* SPSLOGDB_H */

#ifndef __KWREC_H
#define __KWREC_H

#include "defs.h"

#pragma pack(1)
typedef struct
 {
  int  IPCKey;
  char TTYName[12];
  WORD SyncId;
 } KENWOOD_TERMINFO;

#define GROUP_SEPARATOR 29
#define FIELD_SEPARATOR 28

typedef struct {
  char Sentinel;             // M
  char TID[25];
  char Name[25];
  char Address1[25];
  char Address2[25];
  char CityState[17];
  char Zip[5];
  char MerchId[5];
  char Sic[4];
 } MERCHANT;

typedef struct {
  char Sentinel;           // 1
  char Amount[10];         // 2-11
  char StateCode[4];       // 12-15
  char License[15];        // 16-30
  char DateOfBirth[6];     // 31-36
  char CheckNumber[6];     // 37-42
  char PhoneNumber[12];    // 43-54
  char BankNumber[9];      // 55-63
  char BankAccount[16];    // 64-79
  char SSN[9];             // 80-88
  char LicenseSwipe[1];    // 89
  char CheckSwipe[1];      // 90
  char ProductClass[6];    // 91-96
  char ZipCode[5];         // 97-101
  char Misc1[12];
  char Misc2[25];
  char Misc3[25];
  char Misc4[25];
  char Misc5[25];
 } TINQ;

typedef struct {
  char Sentinel;              // 1
  char MerchId[5];            // 2-6
  char BankNumber[10];        // 7-16
  char Account[17];           // 17-33
  char CheckNumber[4];        // 34-37
  char Name[25];              // 38-62
  char Address1[25];          // 63-87
  char Address2[25];          // 88-112
  char CityState[17];         // 113-129
  char Zip[5];                // 130-134
  char HomePhone[12];         // 135-146
  char WorkPhone[12];         // 147-158
  char CheckDate[6];          // 159-164
  char SSN[9];                // 165-173
  char StateCode[2];          // 174-175
  char DriversLicense[16];    // 176-191
  char TypeReturnedCheck[2];  // 192-193
  char DateReceived[6];       // 194-199
  char MailReturned;          // 200
  char CurrentStatus[2];      // 201-202
  char LastActivity[6];       // 203-208
  char Listed;                // 209
  char Guaranteed;            // 210
  char CreditBureau;          // 211
  char RemitDate[6];          // 212-217
  char Verify;                // 218
  char DebtorNumber[9];       // 219-227
  char AdditionalInfo[25];    // 228-252
  char Amount[10];
  char ServiceCharges[10];
  char BalanceDue[10];
 } CHECK;

typedef struct {
  char MerchId[5];
  char CheckNumber[4];
  char CheckDate[6];
  char TypeReturnedCheck[2];
  char Amount[10];
  char Ids[4];                // Which transaction ID's apply
 } BADCHECK;

/* OLD
typedef struct {
  char Sentinel;          // C
  char MerchId[5];
  char BankNumber[10];
  char Account[17];
  char CheckNumber[4];
  char Name[25];
  char Address1[25];
  char Address2[25];
  char CityState[17];
  char Zip[5];
  char HomePhone[12];
  char WorkPhone[12];
  char CheckDate[6];
  char SSN[9];
  char StateCode[2]; 
  char DriversLicense[16];
  char TypeReturnedCheck[2];
  char DateReceived[6];
  char MailReturned;
  char CurrentStatus[2];
  char LastActivity[6];
  char Listed;
  char Guaranteed;
  char CreditBureau;
  char RemitDate[6];
  char Verify;
  char DebtorNumber[9];
  char AdditionalInfo[25];
  char Amount[10];
  char ServiceCharges[10];
  char BalanceDue[10];
 } CHECK;
*/

 typedef struct {
  char Sentinel;               // 1 A
  char InquiryNumber[9];       // 2-10
  char Reference[6];           // 11-16
  char TID[25];                // 17-41
  char Date[6];                // 42-47
  char Time[8];                // 48-55
  char Port[2];                // 56-57
  char BankNumber[10];         // 58-67
  char BankAccount[16];        // 68-83
  char CheckNumber[6];         // 84-89
  char StateCode[4];           // 90-93
  char DriversLicense[15];     // 94-108
  char SSN[9];                 // 109-117
  char DOB[6];                 // 118-123
  char Amount[10];             // 124-133
  char OutputCode[2];          // 134-135
  char TotalOutput[54];        // 136-189
  char DeliveryMethod;         // 190
  char InquiryType;            // 191
  char InquiryStatus;          // 192
  char InquiryUsed;            // 193
  char RetryOutputCode[2];     // 194-195
  char LicenseSwipe;           // 196
  char CheckSwipe;             // 197
  char Sic[4];                 // 198-201
  char ProductClass[6];        // 202-207
  char ZipCode[5];             // 208-212
  char RockyMountainData[70];  // 213-282
  char PhoneNumber[12];        // 283-294
  char Unused[22];             // 295-316
  char HostName[10];           // 317-326
 } AUDIT;

// This is mapping of Total Output field for Voice transactions
typedef struct {
  char  Unused[37];
  char  OperatorId[4];
  char  BVAverageBalance[5];   // Average balance
  char  BVAccountDate[6];      // Account Open or Close date depending
                               // on BV status
  char  BVStatus[2];           //   IF  = Insufficient funds
                               //   AC  = Account Closed
                               //   VG  = Verified Good
                               //   UV  = Unverifiable
                               //  '  ' = Not Bank Verified
 } VOICEOUTPUT;

// This is AUDIT formatted to SPS ACTIVITY
typedef struct {
  BYTE  Version;             // Record Version. Added 1/98. Starting at
                             // Version 1. This is 1 byte binary so
                             // if it's not present, it will be an
                             // ascii character (ie., 1st byte of MerchId)
  char  MerchID[5];
  char  Date[6];
  DWORD Time;                // Time in seconds since midnight
  BYTE  Age;                 // How many days ago this transaction was
  char  BankNumber[9];
  char  AccountNumber[16];
  DWORD CheckNumber;
  DWORD Amount;              // Dollars rounded
  char  Sic[4];
  BYTE  Rcode;
  VOICEOUTPUT Output;
  BYTE  PriorRcode;
  BYTE  ProductCode;
  char  Ids[4];              // The IDS associated
 } ACTIVITY;

// This is AUDIT formatted to SPS ACTIVITY
typedef struct {
  char  MerchID[5];
  char  Date[6];
  DWORD Time;                // Time in seconds since midnight
  BYTE  Age;                 // How many days ago this transaction was
  char  BankNumber[9];
  char  AccountNumber[16];
  DWORD CheckNumber;
  DWORD Amount;              // Dollars rounded
  char  Sic[4];
  BYTE  Rcode;
  VOICEOUTPUT Output;
  BYTE  PriorRcode;
  BYTE  ProductCode;
 } ACTIVITY_VERSION_0;

// This is AUDIT formatted to SPS ACTIVITY
/* OLD
typedef struct {
  char  MerchID[5];
  char  Date[6];
  char  Time[6];
  BYTE  Age;                 // How many days ago this transaction was
  char  BankNumber[9];
  char  AccountNumber[16];
  DWORD CheckNumber;
  char  StateCode[2];
  char  DriversLicense[15];
  char  SSN[9];
  char  DOB[6];
  DWORD Amount;              // Dollars rounded
  char  Sic[4];
  BYTE  Rcode;
  char  TotalOutput[54];
  BYTE  PriorRcode;
  char  Filler[15];
 } ACTIVITY;
*/

#pragma pack()
#endif

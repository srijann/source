#ifndef POSFILE_H
#define POSFILE_H

#define POSFILEFLAGS_MANUALENTRY       1
#define POSFILEFLAGS_HITKEYMICR        2
#define POSFILEFLAGS_HITKEYLICENSE     4
#define POSFILEFLAGS_HITKEYPHONE       8


/* database PosFile record/key structure declarations */

struct PosfileRecord {
   char BankNumber[10];
   char BankAccount[17];
   char LicenseState[3];
   char License[16];
   unsigned long LastApproval;
   unsigned long FirstApproval;
   unsigned short NumApprovals;
   unsigned long AmountApprovals;
   unsigned long DateOfBirth;
   unsigned short Flags;
   struct {
      unsigned short NumberOfOverridesAllowed;
      unsigned short OverridePeriod;
      unsigned short MaxOverrideAmount;
      unsigned short MaxOverrideAccum;
      unsigned short SicCodeRestrictions[3];
      char AccountRestrictions[3][4];
   } Parms;
   struct {
      char ConsumerName[26];
      char BusinessName[26];
   } ConsumerInfo;
   char PhoneKey[11];
};

struct MicrKey {
   char BankNumber[10];
   char BankAccount[17];
};

struct LicenseKey {
   char LicenseState[3];
   char License[16];
};

/* record, field and set table entry definitions */

/* File Id Constants */

#ifndef NOFLDNAMES
/* Record Name Constants */
#define POSFILERECORD 10000

/* Field Name Constants */
#define BANKNUMBER 0L
#define BANKACCOUNT 1L
#define LICENSESTATE 2L
#define LICENSE 3L
#define LASTAPPROVAL 4L
#define FIRSTAPPROVAL 5L
#define NUMAPPROVALS 6L
#define AMOUNTAPPROVALS 7L
#define DATEOFBIRTH 8L
#define FLAGS 9L
#define PARMS 10L
#define NUMBEROFOVERRIDESALLOWED 11L
#define OVERRIDEPERIOD 12L
#define MAXOVERRIDEAMOUNT 13L
#define MAXOVERRIDEACCUM 14L
#define SICCODERESTRICTIONS 15L
#define ACCOUNTRESTRICTIONS 16L
#define CONSUMERINFO 17L
#define CONSUMERNAME 18L
#define BUSINESSNAME 19L
#define PHONEKEY 20L
#define MICRKEY 21L
#define LICENSEKEY 22L
#endif

/* Set Name Constants */

#endif    /* POSFILE_H */

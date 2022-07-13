#define POSFILEFLAGS_MANUALENTRY       1
#define POSFILEFLAGS_CHANGEDMICRKEY    2

database PosFile {
  data file "PosFile.dat" contains PosfileRecord;
  key file  "PosFile.LIC" contains LicenseKey;
  key file  "PosFile.MIC" contains MicrKey;

  record PosfileRecord
   {
    char BankNumber[10];
    char BankAccount[17];
    char LicenseState[3];
    char License[16];
    unsigned long  LastApproval;
    unsigned long  FirstApproval;
    unsigned short NumApprovals;
    unsigned long  AmountApprovals;
    /* Value is zero if we don't have the date of birth */
    unsigned long  DateOfBirth;
    /* Flags Bits .. See macros above */
    unsigned short Flags;

    struct
     {
      unsigned short NumberOfOverridesAllowed;
      unsigned short OverridePeriod;
      unsigned short MaxOverrideAmount;
      unsigned short MaxOverrideAccum;
      unsigned short SicCodeRestrictions[3];
      char  AccountRestrictions[3][4];
     } Parms;

    struct
     {
      char  ConsumerName[26];
      char  BusinessName[26];
      char  Phone[11];
     } ConsumerInfo;

    compound optional unique key MicrKey  
     {
      BankNumber;
      BankAccount;
     }

    compound optional unique key LicenseKey
     {
      LicenseState;
      License;
     }

   }

}

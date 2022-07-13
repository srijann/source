#ifndef BANKPHONEDB_H
#define BANKPHONEDB_H


/* database BankPhoneDB record/key structure declarations */

struct BankPhoneRecord {
   char BankNumber[10];
   char BankName[26];
   char PhoneNumber[11];
   char Comment[31];
   char Filler[51];
};

#ifndef NOFLDNAMES
/* record, field and set table entry definitions */

/* File Id Constants */

/* Record Name Constants */
#define BANKPHONERECORD 10000

/* Field Name Constants */
#define BANKNUMBER 0L
#define BANKNAME 1L
#define PHONENUMBER 2L
#define COMMENT 3L
#define FILLER 4L

/* Set Name Constants */
#endif

#endif    /* BANKPHONEDB_H */

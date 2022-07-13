#ifndef PREAUTHDB_H
#define PREAUTHDB_H


/* database PreauthDB record/key structure declarations */

struct PreauthRecord {
   unsigned long TimeofPreauth;
   unsigned long TimeofPostauth;
   unsigned char PreauthRetention;
   unsigned long PostAuthAmount;
   char PreauthMerchantID[6];
   char PreauthConsumerID[30];
};

/* record, field and set table entry definitions */

/* File Id Constants */

/* Record Name Constants */
#define PREAUTHRECORD 10000

/* Field Name Constants */
#define TIMEOFPREAUTH 0L
#define TIMEOFPOSTAUTH 1L
#define PREAUTHRETENTION 2L
#define POSTAUTHAMOUNT 3L
#define PREAUTHMERCHANTID 4L
#define PREAUTHCONSUMERID 5L

/* Set Name Constants */

#endif    /* PREAUTHDB_H */

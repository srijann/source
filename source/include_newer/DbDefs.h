#ifndef __DBDEFS_H
#define __DBDEFS_H
#define DB_OKAY       0
#define DB_NOTFOUND   1
#define DB_ERROR      2
#define DB_DUPLICATE  3
#define DB_MODIFIED   4
typedef unsigned long DBTIMESTAMP;
typedef struct
 {
  unsigned short ResponseCode;
  DBTIMESTAMP    TimeStamp;
  char           Text[40];         // Optional Error Text
 } DBRESULT;

#define MAX_LISTNAME 10
 
typedef struct
 {
  DBTIMESTAMP TimeStamp;
  char ListName[MAX_LISTNAME+1];
  unsigned int ListLen;
  unsigned int SegmentLen;
  unsigned int SegmentOffset;
 } DBLISTINFO;
 
typedef struct
 {
  unsigned int KeyField;             // Identifier of Current Key
  char KeyState[1];                  // Variable Length State Info
 } DBKEYINFO;
#endif

#ifndef __SAVEFILE_H
#define __SAVEFILE_H
#define SAVEFILE_CURRENT_VERSION 1
// A Valid SAVE file will have this at the beginning
typedef struct {
  short Version;         // A version Number. Must match the Version #
                         // above to be valid.
  char  Id[6];           // Should be SPSMAN
  unsigned long TimeStamp;      // When Saved
 } SAVEFILE_SENTINEL;

// Corresponds to the enum struct
short SaveRecordVersion[] = { 3, 3, 3, 3, 3, 3, 3 };

// This precedes the records
typedef struct {
  short Type;
  int   Len;
  short Version;
 } SAVERECORD_SENTINEL;
#endif

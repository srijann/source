#ifndef __SEARCHINFO_H
#define __SEARCHINFO_H

typedef struct
 {
  char   KeyFld1[50];
  char   KeyFld2[50];
  FLDNUM FldNum1;
  FLDNUM FldNum2;
  bool   noKeyFlds;
  char   FromDate[9];
  char   ToDate[9];
  bool   AllDates;
  bool   PartialKeyMatch;
  bool   CurrentDay;
  bool   CurrentYear;
  char   LastNDays[3];
 } SEARCHINFO;

#endif
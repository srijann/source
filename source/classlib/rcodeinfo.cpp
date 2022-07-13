//----------------------------------------------------------------------------
//  Project ClassLib
//
//  Copyright © 1996. All Rights Reserved.
//
//  FILE:         RcodeInfo.cpp
//  AUTHOR:       Jack Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of RcodeInfo
//
//  Encapsulates information about SPS response codes.
//
//  Most of the info is inline (see the header).
//----------------------------------------------------------------------------

#include "RcodeInfo.h"
#include "Rcodes.h"

static char* rcText[100] =
 {
  "SYSTEM ERROR",
#if 0
  "KENWOOD NEG 01",
#else
  "DECL FORCED",
#endif
  "KENWOOD NEG 02",
  "KENWOOD NEG 03",
  "KENWOOD NEG 04",
  "KENWOOD NEG 05",
  "UNDEFINED 06",
  "UNDEFINED 07",
  "UNDEFINED 08",
  "UNDEFINED 09",
  "UNDEFINED 10",
  "UNDEFINED 11",
  "UNDEFINED 12",
  "UNDEFINED 13",
  "UNDEFINED 14",
  "UNDEFINED 15",
  "UNDEFINED 16",
  "UNDEFINED 17",
  "UNDEFINED 18",
  "UNDEFINED 19",
  "UNDEFINED 20",
  "R1 AGE",
  "R2 SIC",
  "R3 CKNUM",
  "R4 CHK SEQ",
  "R5 SNGL CHK",
  "R6 OUT OF ST",
  "R7 ACTIVITY",
  "R8 OUT OF REG",
#if 0
  "UNDEFINED 29",
#else
  "DECL NEGATIVE",
#endif
  "MULTI PREAUTHS",
  "EXCEEDS PREAUTH",
  "UNDEFINED 32",
  "UNDEFINED 33",
  "UNDEFINED 34",
  "UNDEFINED 35",
  "UNDEFINED 36",
  "UNDEFINED 37",
  "UNDEFINED 38",
  "UNDEFINED 39",
  "LOST STOLEN",
  "UNDEFINED 41",
  "UNDEFINED 42",
  "UNDEFINED 43",
  "UNDEFINED 44",
  "UNDEFINED 45",
  "UNDEFINED 46",
  "UNDEFINED 47",
  "UNDEFINED 48",
  "UNDEFINED 49",
  "INVALID LIC",
  "INVALID ST",
  "INVALID MICR",
  "INVALID DOB",
  "INVALID PHONE",
  "INVALID SSN",
  "INVALID CKNUM",
  "INVALID AMT",
  "INVALID ZIP",
  "INVALID DATA",
  "UNDEFINED 60",
  "UNDEFINED 61",
  "UNDEFINED 62",
  "UNDEFINED 63",
  "UNDEFINED 64",
  "UNDEFINED 65",
  "UNDEFINED 66",
  "UNDEFINED 67",
  "UNDEFINED 68",
  "UNDEFINED 69",
  "CALL CANCELLED",
  "UNDEFINED 71",
  "UNDEFINED 72",
  "UNDEFINED 73",
  "UNDEFINED 74",
  "UNDEFINED 75",
  "UNDEFINED 76",
  "UNDEFINED 77",
  "UNDEFINED 78",
  "UNDEFINED 79",
  "APPRV GUAR",
  "APPRV AGE",
  "APPRV PREAUTH",
  "APPRV CHKNUM",
  "APPRV HOLD CHK",
  "APPRV VERIFY",
  "APPRV FORCED",
  "APPRV POSITIVE",
  "UNDEFINED 88",
  "UNDEFINED 89",
  "MISSING LIC",
  "MISSING ST",
  "MISSING MICR",
  "MISSING DOB",
  "MISSING PHONE",
  "MISSING SSN",
  "MISSING CKNUM",
  "MISSING AMT",
  "MISSING ZIP",
  "MISSING DATA" };

static char* undefined = "UNDEFINED";

//---------------------------------------------------------
// Get a pointer to the text string describing the response
//---------------------------------------------------------
char* RcodeInfo::rcodeText(int rcode)
{
 if ( rcode > 99 )
  return undefined;
 else
  return rcText[rcode];
}

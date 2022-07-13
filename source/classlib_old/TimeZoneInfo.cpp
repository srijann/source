//----------------------------------------------------------------------------
//  Project Classlib
//
//  Copyright © 1996. All Rights Reserved.
//
//  FILE:         TimeZoneInfo.cpp
//  AUTHOR:       Jack Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of TineZoneInfo
//
//  Encapsulates information about Time Zones
//----------------------------------------------------------------------------

#include "TimeZoneInfo.h"

#define ZONESDEFINED 10

#include <time.h>

static char* ZoneList[] = { "Hawaii(-10)",
                            "Alaska(-9)",
                            "Pacific(-8)",
                            "Arizona(-7)",
                            "Mountain(-7)",
                            "Central(-6)",
                            "Eastern(-5)",
                            "Indiana(-5)",
                            "Atlantic(-4)",
                            "Newfndlnd(-3)",
                            "" };

static struct
 {
  short Offset;
  bool  DaylightSavings;
 } Tbl[ZONESDEFINED+1] =
               {{-10,true},
               {-9,true},
               {-8,true},
               {-7,false},
               {-7,true},
               {-6,true},
               {-5,true},
               {-5,false},
               {-4,true},
               {-3,true},
               {0,false} };

//-------------------------------------------------------------
// Get Pointer to TimeZone name, the Zone, the Offset from GMT
// and whether it observes Daylight savings
//-------------------------------------------------------------
char* TimeZoneInfo::GetInfo(int Zone,int& Offset,bool& Dst)
{
 if ( Zone > ZONESDEFINED )
   Zone=ZONESDEFINED+1;

 Dst=Tbl[Zone].DaylightSavings;
 Offset=Tbl[Zone].Offset;

 return ZoneList[Zone];
}

//------------------------------------------
// Get the Time Zone and pointer to the name
//-------------------------------------------
char* TimeZoneInfo::GetInfo(int Zone)
{
 if ( Zone > ZONESDEFINED )
  Zone=ZONESDEFINED+1;

 return ZoneList[Zone];
}

//-------------------------------------
// Get the Number of Time Zones defined
//-------------------------------------
int TimeZoneInfo::NumTimeZones()
{
 return ZONESDEFINED;
}

//----------------------------------------------
// Adjust a TimeStamp to the Time Zone specified
//----------------------------------------------
struct tm* TimeZoneInfo::LocalTime(int Zone,time_t TimeStamp)
{
 int Offset;
 bool DST;
 struct tm* t;
 time_t ts;

 // Time behavior of Borland C++  5.01 and Windows NT at the writing
 // of this code was as follows:
 //
 // Windows NT, if so-instructed (and it should be), automatically
 // changes the system time for daylight savings.
 //
 // The _timezone paramater does not vary as the system shifts on/off
 // daylight savings.
 //
 // The isdst parameter of the tm structure is set for daylight
 // savings.

 // Convert to DST and back to Local
 GetInfo(Zone,Offset,DST);
 if ( TimeStamp )
  ts=TimeStamp;
 else
  ts=time(0);

 ts +=  _timezone + ( 3600 * Tbl[Zone].Offset );
 t=localtime(&ts);

 // We need not be concerned with daylight savings time unless we're
 // on daylight savings time and the time zone doesn't observe it.
 // In this case, the local time needs to be adjusted by another hour.
 if ( t->tm_isdst &&
      ! DST )
  {
   ts -= 3600;
   t = localtime(&ts);
  }


 return t;
}

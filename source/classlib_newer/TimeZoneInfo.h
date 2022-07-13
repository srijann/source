#ifndef __TIMEZONEINFO_H
#define __TIMEZONEINFO_H

#include <time.h>

class TimeZoneInfo {

  public:
  //Get info about zone, returns GMT offset

  char*  GetInfo(int Zone);
  char*  GetInfo(int Zone,int& Offset,bool& Dst);
  int    NumTimeZones();
  // Get the current local time in a time zone
  struct tm* LocalTime(int Zone,time_t TimeStamp=0);

  enum
   {
    Hawaii,
    Alaska,
    Pacific,
    Arizona,
    Mountain,
    Central,
    Eastern,
    Indiana,
    Atlantic,
    Newfndlnd
   };

 };

#endif
#include "TimeZones.h"

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
                             0 };

static struct
 {
  short Offset;
  bool  DaylightSavings;
 } Tbl[10] = { {-10,true},
               {-9,true},
               {0,false} };

int TimeZoneInfo::GetInfo(int Zone,bool& Dst)
{
 if ( Zone > 9 )
  Zone=10;
 Dst=Tbl[Zone].DaylightSavings;
 return Tbl[Zone].Offset; 
}

char** TimeZoneInfo::GetList()
{
 return ZoneList;
}


#ifndef __TIMEZONES_H
#define __TIMEZONES_H

class TimeZoneInfo {

  public:
  //Get info about zone, returns GMT offset

  int GetInfo(int Zone,bool& Dst);
  char** GetList();

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
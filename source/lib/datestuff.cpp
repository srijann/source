// Convert Year and yearday into Month and monthday.
// Year day is Base 1, not 0 as in tm structure
static int montbl[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
void YdayToMMDD(int year,int yday,int& month,int& day)
{
 // Leap Year calculation is accurate until year 2100
 montbl[2] = ((year%4) ? 28 : 29);
 month=1;
 day=yday;

 while( month < 12 && day > montbl[month] )
  {
   day -= montbl[month];
   ++month;
  }

}

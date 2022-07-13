#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mem.h>
#include <ctype.h>

#define DO_LICENCE_CHECK 1

#define WAS_DISABLED           -1
#define FILE_OPEN_ERROR      1001
#define STATE_NOT_FOUND      1002
#define LENGTH_ERROR         1003
#define LEADING_ALPHA_ERROR  1004
#define ALPHA_COUNT_ERROR    1005
#define WAS_NOT_SSN          1006
#define NUM_STATES             61
#define LEN_NUM_BUF            71

#define false               0
#define true                1

typedef struct _STATE_LICENSE_DATA
    {   long lState;
        int  iMaxChars;
        int  iMinChars;
        int  iMaxAlphas;
        int  iMinAlphas;
        int  iHasLeadAlpha;
        int  iIsSSNType;
        int  iMoreFlag;
        int  iIsDisabled;
    }STATE_LICENSE_DATA;

static int isValidGenericLicense(char* License, char *szState);
static int isValidTableLicense(char* License, long lState, int* piMoreFlag);
static int isValidStatesLicense(char* License, long lState);
static int iIsCorrectNumberOfDigits(char* License, int iCount);

void main()
{
    char szState[3];
    char szbuf[80];
    char szLicense[30];
    int iReturn;

    printf("\n\n\n\n\n\n\n\n\n\n\n   Welcome to another fine Data Transmogrifier");
    printf("\n    from those funny people of the EIC.");
    while(1)
    {
    printf("\n\n    This is a Drivers License validator.");
    printf("\n    Enter 'q' in the state field to quit");
    printf("\n\n    Enter the State of the License:");
    gets(szbuf);if(szbuf[0]=='q') break;
    szState[0]=szbuf[0];szState[1]=szbuf[1];szState[2]='\0';
    while(1)
    {
    printf("\n    Enter the License number      :");
    gets(szLicense);
    printf("\n\n    OK, lets Do it.\n\n");

   // MAB 112897 - if licence is all '0's, make blank

    iReturn = strspn(szLicense, "0 ");   // '0' and ' '
    if(szLicense[0] == 'q')
       return;
    if(iReturn == strlen(szLicense))  // only '0' or ' ' found
        printf("\nOnly '0' or ' ' was found. Length=%d.",iReturn);
    else
        printf("\nSomething else was found at Length=%d.",iReturn);

    }

    iReturn = isValidGenericLicense(szLicense,szState);
    printf("\n    The License Number %s for State %s is ", szLicense, szState);
    if (iReturn==true)
         printf("valid.\n");
    else if (iReturn==false)
         printf("invalid.\n");
    }
}

#if DO_LICENCE_CHECK
//--------------------------------------------------------------------
// Generic License Validation
// This function calls the two functions that actually do the
// validation and it interprets their error returns into true
// or false for the calling application
//--------------------------------------------------------------------
static int isValidGenericLicense(char* License, char *szState)
{
    int  iReturn = 0;
    int  iMoreFlag = 0;
    int* piMoreFlag = &iMoreFlag;
    long lState;

    lState = (long)((szState[0] - 64) * 100);
    lState += (long)(szState[1] - 64);

    iReturn = isValidTableLicense(License, lState, piMoreFlag);
    if( ((iReturn == true) && (iMoreFlag == 0) )  ||
        (iReturn == WAS_DISABLED)
      )
        return true;

    if(iReturn == FILE_OPEN_ERROR)
        return iReturn;
    if( (iReturn == ALPHA_COUNT_ERROR)   ||
        (iReturn == LEADING_ALPHA_ERROR) ||
        (iReturn == LENGTH_ERROR)        ||
        (iReturn == STATE_NOT_FOUND)     ||
        (iReturn == WAS_NOT_SSN)         ||  // needs SSN
        ( (iReturn == false) && (iMoreFlag == 0) )
      )
        return false;

    if(iMoreFlag == 1)   // try the rest of the functions
       iReturn = isValidStatesLicense(License,lState);

    if(iReturn == true)
        return true;

    //if(iReturn == false)
    //     return false;

    return false;  // should never reach here
}
                               
//----------------------------------------------------------------------
// Generic Table License Validation
// This function validates licences acording to values in the
// statelic.dat file.  The first time it is called, it reads the
// data from file, into static memory. Subsequent calls, do not read
// from file.  It then checks length, alpha content, is it a SSN type,
// does it have a lead alpha, etc.  It also returns a value that will
// be interpreted as true if a state validation is set to disabled.
//----------------------------------------------------------------------
static int isValidTableLicense(char* License, long lState, int* piMoreFlag)
{
    char szStateBuf[4];
    char szNumBuf[LEN_NUM_BUF];
    char szbuf[LEN_NUM_BUF];
    FILE *fin;
    int ii = 0;
    int iReturn = 0;
    int iCount = 0;
    int iAlphaCounter;
    static int iStateDataHasBeenReadFromFile;
    static STATE_LICENSE_DATA dat[NUM_STATES];

    // get the data from file to memory once
    if(!( iStateDataHasBeenReadFromFile))  // file has not been read yet
    {
       if((fin = fopen("statelic.dat", "r")) == NULL)  // couldn't open file
          return FILE_OPEN_ERROR;

       for(ii = 0; ii < NUM_STATES; ii++)
       {
          memset(szbuf,'\0',sizeof(szbuf));
          memset(szStateBuf,'\0',sizeof(szStateBuf));
          memset(szNumBuf,'\0',sizeof(szNumBuf));

          fgets(szbuf, LEN_NUM_BUF, fin);
          strncpy(szStateBuf,szbuf,3);
          strcpy(szNumBuf,&szbuf[2]);

          sscanf(szNumBuf,"%ld %d %d %d %d %d %d %d %d", //szStateBuf,
            &dat[ii].lState,
            &dat[ii].iMaxChars, &dat[ii].iMinChars, &dat[ii].iMaxAlphas,
            &dat[ii].iMinAlphas, &dat[ii].iHasLeadAlpha, &dat[ii].iIsSSNType,
            &dat[ii].iMoreFlag, &dat[ii].iIsDisabled);
       }
       fclose(fin);
       iStateDataHasBeenReadFromFile = 1;
    }

    // find data for state name
    for(iCount = 0; iCount < NUM_STATES; iCount++)
       if(dat[iCount].lState == lState)
          break;

    if(iCount == NUM_STATES)
       return STATE_NOT_FOUND;

    if(dat[iCount].iIsDisabled)  // has been turned off
       return WAS_DISABLED;

    if(dat[iCount].iIsSSNType)  // if it could be a SSN
    {
       iReturn = iIsCorrectNumberOfDigits(License, 9);   // is it 9 digits?
       if(iReturn == true)   // is it 9 digits?
           return true;       // definate true

       if(dat[iCount].iIsSSNType == 2)  // if it must be a SSN
          if(iReturn == false)
             return WAS_NOT_SSN;  // false that overrides MoreFlag
    }

    if(dat[iCount].iMoreFlag)  // save the more flag for the next function up
       *piMoreFlag = true;

    // check length
    if ( (strlen(License) > dat[iCount].iMaxChars) ||
         (strlen(License) < dat[iCount].iMinChars) )
       return LENGTH_ERROR;

    if(dat[iCount].iHasLeadAlpha)  // check for leading alpha
       if(!(isalpha(License[0])))
          return LEADING_ALPHA_ERROR;

    // check for number of alphas
    iAlphaCounter = 0;
    for(ii = 0; ii < strlen(License); ii++)
       if( (isalpha(License[ii])) || (License[ii] == '*') )
          iAlphaCounter++;
    if( (iAlphaCounter > dat[iCount].iMaxAlphas)  ||
        (iAlphaCounter < dat[iCount].iMinAlphas) )
        return ALPHA_COUNT_ERROR;

    return true;
}

//----------------------------------------------------------------------
// These are the functions for each state that cannot be validated using
// information in the table.
//----------------------------------------------------------------------
static int isValidStatesLicense(char* License, long lState)
{
   register ii;
   int iReturn = 0;
   char szbuf[100];
   int  iNumberOfDigits = 0;
   int  iNumberOfAlphas = 0;

   for(ii=0; ii<strlen(License); ii++)  // get number of digits
       if(isdigit(License[ii]))
          iNumberOfDigits++;

   for(ii=0; ii<strlen(License); ii++)  // get number of alphas
       if(isalpha(License[ii])) // || (License[ii] == '*') )
          iNumberOfAlphas++;

   if (lState == 126 )  // Arizona
   {
      if( (isalpha(License[0])) && (iNumberOfDigits == 8))  // A########
         return true;

      if(strlen(License) == 7)
      {
         if( (isalpha(License[0])) && (iNumberOfDigits == 6) )  // A######
            return true;

         if( ( (isalpha(License[0])) && (isalpha(License[1])) ) &&
             (iNumberOfDigits == 5)  //AA#####
           )
         return true;
      }
      return false;
   }

   else if (lState == 315 )  // Colorado - ssn already checked
   {
       if( (iNumberOfDigits > 6) || (iNumberOfAlphas > 5) )
           return false;
   }

   else if (lState == 320 )  // Connecticut
   {
       szbuf[0] = License[0]; szbuf[1] = License[1]; szbuf[2] ='\0';
       iReturn = atoi(szbuf);
       if((iReturn > 24) || (iReturn < 1))
           return false;
       // ### Must get birthday for this
       return true;
   }

   else if (lState == 904 )  // Idaho
   {
      if(isalpha(License[0]))  //AA######A
      {
         if( (isalpha(License[1])) && (isalpha(License[8])) )
             return true;  // it has already been checked for 6# minimum
      }
      return false;
   }

   //else if ( memcmp(State,"IL",2) == 0 )  // Illinois
   //{ all kinds of stuff }

   else if (lState == 901 )  // Iowa
   {
      if( (isalpha(License[3])) && (isalpha(License[4])) )  //###AA####
         return true;  // it has already been checked for 7# minimum
      return false;
   }

   else if (lState == 1118 )  // Kansas
   {
      if(License[0] == 'K')   //K########
         return true;  // it has already been checked for 8# minimum
      return false;
   }

   else if (lState == 1125 )  // Kentucky
   {
      if(iNumberOfDigits == 10)  // was already checked for 10 max
         return true;
      if( (isalpha(License[0])) && (iNumberOfDigits == 8) )  //A########
         return true;  // it has already been checked for 8# minimum
      return false;
   }

   else if (lState == 1201 )  // Louisiana
   {
      if(memcmp(License,"00",2))
         return false;
      return true;
   }

   else if (lState == 1305 )  // Maine ####### or #######X
   {
      if( (strlen(License) == 8)  &&
          (License[7] == 'X') )   //#######X - proqavisional or under 21
             return true;  // it has already been checked for 7# minimum

      if(strlen(License) == 7)
         if(iIsCorrectNumberOfDigits(License, 7))
            return true;

      return false;
   }

   //else if ( memcmp(State,"MD",2) == 0 )  // Maryland A############
   //{;}   // complicated

   else if (lState == 1301 )  // Massachusetts
   {
      if(License[0] == 'S')   //S########
         return true;  // it has already been checked for 8# minimum
      return false;
   }

   //else if ( memcmp(State,"MI",2) == 0 )  // Michigan A############
   //{;}   // first letter of last name and other complicated stuff

   //else if ( memcmp(State,"MN",2) == 0 )  // Minnesota A############
   //{;}   // first letter of last name plus other

   else if (lState == 1315 )
   {  // Missouri ######### for CDL A########
      if(isalpha(License[0]))   //S########
         return true;  // it has already been checked for 1# minimum
      return false;
   }

   else if (lState == 1405 )  //Nebraska
   {  //  H,V,G,E,C,B or A followed by up to 8#
      if( (License[0] == 'H') ||
          (License[0] == 'V') ||
          (License[0] == 'G') ||
          (License[0] == 'E') ||
          (License[0] == 'C') ||
          (License[0] == 'B') ||
          (License[0] == 'A') )
         return true;
      return false;
   }

   else if (lState == 1422 )  //Nevada
   {  //  ############ or X########
      if(iIsCorrectNumberOfDigits(License, 12))  // last 2 digits will be year OB ###
          return true;
      //if( (isalpha(License[0])) && (strlen(License) == 9) )  //S########
      if( (License[0] == 'X') && (strlen(License) == 9) &&  //S########
          (iNumberOfAlphas == 1)
        )
         return true;
      return false;
   }

   else if (lState == 1408 )  // New Hampshire
   {  //  ##AAA##### = MOB, letters from name, YOB, DOB, #
      for (ii=0; ii<2; ii++)
         if ( ! isdigit(License[ii]) )
            return false;
      for (ii=2; ii<5; ii++)
         if ( ! isalpha(License[ii]) )
            return false;
      for (ii=5; ii<10; ii++)
         if ( ! isdigit(License[ii]) )
            return false;
      return true;
   }

   //else if ( memcmp(State,"NJ",2) == 0 )  // New Jersey
   //{ Uses Date }

   else if (lState == 1425 )  // New York
   {
      if( (isalpha(License[0])) &&
          ( (strlen(License) == 19) || (strlen(License) == 22) )
        )
          return true;
      return false;
   }

   else if (lState == 1508 )  // Ohio
   {
      if( (isalpha(License[0])) &&
          (isalpha(License[1])) &&
          (strlen(License) == 8)
        )
          return true;
      return false;
   }

   //else if ( memcmp(State,"RI",2) == 0 )  // Rhode Island

   else if (lState == 2201 )    // VA Virginia
   {
      if( (License[0] == 'R') ||   // R######## or T#######
          (License[0] == 'T') )
         return true;  // it has already been checked for 8# minimum
      return false;
   }

   else if (lState == 2301 )    // Washington   AAAAAAA##---
   {
      iReturn = 0;
      for (ii=0; ii<7; ii++)
         if( (isalpha(License[ii])) || (License[ii] == '*') )
            iReturn++;
      if(iReturn != 7)
            return false;

      for (ii=7; ii<9; ii++)
         if(isdigit(License[ii]))
            iReturn++;
      if(iReturn != 9)
            return false;

      return true;
   }
   return true;
}

//----------------------------------------------------------------------
static int iIsCorrectNumberOfDigits(char* License, int iCount)
{
   register i;
   if(strlen(License) > iCount)
      return false;
   for (i=0; i<iCount; i++)
      if ( ! isdigit(License[i]) )  // ##########
         return false;
   return true;
}
#endif


// This is the LICENCE.DAT table.
#if 0
CA    301         8       5       1      1     1     0     0        0
AL    112         7       7       0      0     0     0     0        0
AK    111         7       1       0      0     0     0     0        0
AZ    126         9       7       2      0     0     1     1        0
AR    118         9       5       0      0     0     1     0        0
CO    315        11       2       5      0     0     1     1        0
CT    320         9       9       0      0     0     0     1        0
DE    405         7       1       0      0     0     0     0        0
DC    403         9       1       0      0     0     1     0        0
FL    612        13      12       1      1     1     0     0        0
GA    701         9       1       0      0     0     1     0        0
HI    809         9       9       0      0     0     2     0        0
ID    904         9       9       3      0     0     1     1        0
IL    912        12      12       1      1     1     0     0        0
IN    914        10      10       0      0     0     0     0        0
IA    901         9       7       2      0     0     1     1        0
KS   1119         9       9       1      0     0     1     1        0
KY   1125        10       9       1      0     0     1     1        0
LA   1201         9       9       0      0     0     0     1        0
ME   1305         8       7       1      0     0     0     1        0
MD   1304        13      13       1      1     1     0     0        0
MA   1301         9       9       1      0     0     1     1        0
MI   1309        13      13       1      1     1     0     0        0
MN   1314        13      13       1      1     1     0     0        0
MS   1319         9       1       0      0     0     1     0        0
MO   1315        10       2       1      0     0     1     1        0
MT   1320         9       9       9      0     0     1     0        0
NE   1405         9       1       1      1     1     0     1        0
NV   1422        12       9       1      0     0     0     1        0
NH   1408        10      10       3      3     0     0     1        0
NJ   1410        15      15       1      1     1     0     0        0
NM   1413         9       8       0      0     0     1     0        0
NY   1425        22       9       1      1     0     1     1        0
NC   1403        12       1       0      0     0     0     0        0
ND   1404         9       1       0      0     0     1     0        0
OH   1508         9       8       2      0     0     1     1        0
OK   1511         9       9       0      0     0     2     0        0
OR   1518         7       1       0      0     0     0     0        0
PA   1601         8       8       0      0     0     0     0        0
RI   1809         7       7       0      0     0     0     0        0
SC   1903         9       9       0      0     0     2     0        0
SD   1904         9       8       0      0     0     1     0        0
TN   2014         8       8       0      0     0     0     0        0
TX   2024         8       8       0      0     0     0     0        0
UT   2120         9       4       0      0     0     0     0        0
VT   2220         8       8       1      0     0     0     0        0
VA   2201         9       9       1      0     0     1     1        0
WA   2301        12      12       7      0     1     0     0        0
WV   2322         7       7       1      0     0     0     0        0
WI   2309        14      14       1      1     1     0     0        0
WY   2325        10       9       0      0     0     0     0        0
AB    102         9       1       0      0     0     1     0        0
BC    203         7       7       0      0     0     0     0        0
MB   1302        12      12       9      7     0     0     0        0
NB   1402         7       1       7      0     0     0     0        0
NF   1406        10      10       1      1     1     0     0        0
NS   1419        14      12       5      5     0     0     0        0
ON   1514        15      15       1      1     1     0     0        0
PE   1605        12       1       0      0     0     0     0        0
PQ   1617        13      13       1      1     1     0     0        0
SK   1911         8       8       0      0     0     0     0        0

     int         MaxC    MinC    MaxA   MinA  Lead  SSN   More   Disable

NT - North West Territories
YT - Yukon Territory

    int  - literally, the 2 chars converted ascii wise to an int. A=01 B=02 Z=26
    MaxC - Maximum characters in Licence Number
    MinC - Minimum characters in Licence Number
    MaxA - Maximum Alpha characters in Licence Number
    MinA - Minimum Alpha characters in Licence Number
    Lead - Is there a leading Alpha character in Licence Number
    SSN  - 1 == it might be a SSN, 2 == it must be a SSN
    More - There is another test function after the table test
    Disable - All validation for the state is disabled.

    Setting SSN to 2 and More to 1 are mutually exclusive


#endif

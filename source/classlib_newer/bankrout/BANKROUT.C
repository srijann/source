#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mem.h>

#define FILE_OPEN_ERROR      1001
#define LENGTH_ERROR         1003
#define LEN_ACCT_BUF           32

#define DO_BANK_ACCOUNT_X_BANK_NUMBER     1
#define NUM_ROUTES_TO_READ_FROM_FILE    100
#define ROUTE_NOT_FOUND                1011

typedef struct _BANK_ROUTING_DATA
    {   long lBankRoute;
        int  iMaxChars;
        int  iMinChars;
    }BANK_ROUTING_DATA;

static int isValidTableBankRoute(char* pszBankRoute, char *pszBankAccount);
void logg1(int iLine, char *buf1, char *buf2, int i, long l);
void logg1(int iLine, char *buf1, char *buf2, int i, long l)
{
   FILE  *fout;
   fout = fopen("err.log", "a");
   fprintf(fout,"@%d %s - %s. .%ld. .%d.\n", iLine, buf1, buf2, i, l);
   fclose(fout);
}

//bool isValidBankAccountAndBankRoute(int fn)
void main()
{
 char  szBankRoute[32];
 char  szBankAccount[32];
 int   iReturn;
 int   iLenBankRoute;
 int   iLenBankAccount;

 while(1)
 {
 printf("\n\n   Enter Bank Route   : ");
 scanf("%s", szBankRoute);
 if(strlen(szBankRoute) < 5)
    exit(0);
 printf("   Enter Bank Account : ");
 scanf("%s", szBankAccount);
 printf("\n   Bank Num and Account - %s  %s", szBankRoute, szBankAccount);

  iReturn = isValidTableBankRoute(szBankRoute, szBankAccount);

  if(iReturn == LENGTH_ERROR)
     printf("\n   Invalid Bank Account");
  else
     printf("\n   Valid BankAccount");
  }

 //logg1(__LINE__, "ValidRouteAcct()-BankAccount is",szBankAccount,0,0);
}

//----------------------------------------------------------------------
static int isValidTableBankRoute(char* pszBankRoute, char *pszBankAccount )
{
   char szNumBuf[LEN_ACCT_BUF];
   char szbuf[LEN_ACCT_BUF];
   char cbuf1[16];
   char cbuf2[16];
   char cbuf3[16];
   FILE *fin;
   int   ii, jj, iii;
   int   iReturn;
   int   iCount;
   int   iLenBankAccount;
   long  lBankRoute;
   static int siRecordCount;
   static int siDataHasBeenReadFromFile;
   static BANK_ROUTING_DATA dat[NUM_ROUTES_TO_READ_FROM_FILE];

   logg1(__LINE__, "Start Validation","for",0,0);
   logg1(__LINE__, pszBankRoute, pszBankAccount, 0, 0);

   // get the data from file to memory once
   if(!( siDataHasBeenReadFromFile))  // file has not been read yet
   {

      logg1(__LINE__, "Start Validation","Read File",0,0);
      iReturn = 0;
      //if((fin = fopen("m:\\programs\\routing.dat", "r")) == NULL)  // couldn't open file
      if((fin = fopen("m:\\programs\\routing.dat", "r")) == NULL)  // couldn't open file

      {
         iReturn = FILE_OPEN_ERROR;
         logg1(__LINE__, "File open fail 1","",0,0);
      }
      if(iReturn)
      {
         iReturn = 0;
         logg1(__LINE__, "File open 2","",0,0);
         if((fin = fopen("c:\\sps\\programs\\routing.dat", "r")) == NULL)  // couldn't open file
         {
            iReturn = FILE_OPEN_ERROR;
            logg1(__LINE__, "File open fail 2","",0,0);
         }

      }
      if(iReturn)
      {
         iReturn = 0;
         logg1(__LINE__, "File open 3","",0,0);
         if((fin = fopen("routing.dat", "r")) == NULL)  // couldn't open file
         {
            iReturn = FILE_OPEN_ERROR;
            logg1(__LINE__, "File open fail 3","",0,0);
         }

      }

      if(iReturn)
         return iReturn;

      for(ii = 0; ii < NUM_ROUTES_TO_READ_FROM_FILE; ii++)
      {
         memset(szbuf,'\0',sizeof(szbuf));
         fgets(szbuf, LEN_ACCT_BUF, fin);
         logg1(__LINE__, "Buf is",szbuf,0,ii);

         if(feof(fin))
            break;

         sscanf(szbuf,"%ld %d %d ",
           &dat[ii].lBankRoute, &dat[ii].iMaxChars, &dat[ii].iMinChars);
/*
         iii = 0;
         jj = 0;
         while( (szbuf[iii] >= '0') && (szbuf[iii] <= '9'))
         {
            cbuf1[jj] = szbuf[iii];
            iii++; jj++;
         }
         cbuf1[jj] = '\0';

         while( szbuf[iii] == ' ')
            iii++;

         jj = 0;
         while( (szbuf[iii] >= '0') && (szbuf[iii] <= '9'))
         {
            cbuf2[jj] = szbuf[iii];
            iii++; jj++;
         }
         cbuf2[jj] = '\0';

         while( szbuf[iii] == ' ')
            iii++;

         jj = 0;
         while( (szbuf[iii] >= '0') && (szbuf[iii] <= '9'))
         {
            cbuf3[jj] = szbuf[iii];
            iii++; jj++;
         }
         cbuf3[jj] = '\0';

         dat[ii].lBankRoute = atol(cbuf1);
         dat[ii].iMaxChars  = atoi(cbuf2);
         dat[ii].iMinChars  = atoi(cbuf3);
*/
         logg1(__LINE__, "BankRout",cbuf1,dat[ii].lBankRoute,ii);
         logg1(__LINE__, cbuf2,cbuf3,(long)dat[ii].iMaxChars,dat[ii].iMinChars);
         siRecordCount++;
      }

      fclose(fin);
      siDataHasBeenReadFromFile = 1;
   }

   iLenBankAccount = strlen(pszBankAccount);
   lBankRoute = atol(pszBankRoute);

   logg1(__LINE__, "Validation","Past Read File",lBankRoute,iLenBankAccount);
   // find data for Bank Route
   for(iCount = 0; iCount < siRecordCount; iCount++)
   {

      logg1(__LINE__, "Test in ","",lBankRoute,iCount);
      logg1(__LINE__, "Test dat","",dat[iCount].lBankRoute,siRecordCount);
      logg1(__LINE__, "read","min max",(long)dat[iCount].iMinChars,dat[iCount].iMaxChars);
      if(dat[iCount].lBankRoute == lBankRoute)
         break;
   }

   if(iCount == siRecordCount)
   {
      logg1(__LINE__, "Route Not Found","",0,0);
      return ROUTE_NOT_FOUND;
   }

   logg1(__LINE__, "iCount","",0,iCount);
   logg1(__LINE__, "Length Error ","min max",(long)dat[iCount].iMinChars,dat[iCount].iMaxChars);
   // check length
   if ( (iLenBankAccount > dat[iCount].iMaxChars) ||
        (iLenBankAccount < dat[iCount].iMinChars) )
   {
      logg1(__LINE__, "Length Error ","min max",(long)dat[iCount].iMinChars,dat[iCount].iMaxChars);
      return LENGTH_ERROR;
   }
   return 0;
}





























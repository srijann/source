#include <stdio.h>
#include <ctype.h>

void logg(int iLine, char *buf1, char *buf2, int i, long l);

void logg(int iLine, char *buf1, char *buf2, int i, long l)
{
   FILE  *fout;
   fout = fopen("err.log", "a");
   fprintf(fout,"@%d %s - %s. .%ld. .%d.\n", iLine, buf1, buf2, i, l);
   fclose(fout);
}

static char StateCodeArray[] = { "AB" "AK" "AL" "AR" "AS" "AZ"};

void main()
{
    int ii;
    char ch;
    printf("\n\n\n\n   It's Howdy Doody Time!\n");
    for(ii = 0; ii<12; ii++)
    {  putchar('-');
       putchar(StateCodeArray[ii]);
    }

    /*for(ii = 0; ii<6; ii++)
    {
        printf("\n      Try .%s.",StateCodeArray[ii]);
        if((ii%10) == 0)
           ch = getchar();
    } */

    printf("\n\n   Bye Y'all!\n");
}
/*
static char StateCodeArray[] = {
  "AB",  "AK", "AL",  "AR", "AS",  "AZ",
  "BC",
  "CA", "CO", "CT", "CZ",
  "DC", "DE" "DS",
  "FL",
  "GA", "GU",
  "HI",
  "IA", "ID", "IL", "IN",
  "KS", "KY",
  "LA",
  "MA", "MB", "MD", "ME", "MI", "MN", "MO", "MS", "MT",
  "NB", "NC", "ND", "NE", "NF", "NH", "NJ", "NM", "NS", "NT", "NV", "NY",
  "OH", "OK", "ON", "OR",
  "PA", "PE", "PR",
  "QP",
  "RI",
  "SC", "SD", "SK",
  "TN", "TX",
  "UT",
  "VA", "VI", "VT",
  "WA", "WI", "WV", "WY",
  "YT" };
*/

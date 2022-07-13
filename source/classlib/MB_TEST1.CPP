#include<stdio.h>
#include<conio.h>
#include<string.h>
//#include<mem.h>


void main()
{
 FILE *f;
 char ch;
 char StringDBFname[80];

   strcpy(StringDBFname,"c:\\sps\\database\\voicemsg.dat");

   printf("\n\n\n\nWelcome to another fine Data Transmogrifier");
   printf("\nfrom those funny people of the EIC.\n\n");

   ch = getchar();

   if( (f=fopen(StringDBFname,"rb")) == NULL )
    printf("Unable to open File %s\n",StringDBFname);
   else
    printf("File %s open.\n",StringDBFname);

  ch =getchar();


#if 0


//logg1(__LINE__, "Yes, StringDB:Load() ate it.",StringDBFname, 0, 0);
      return 0;
      }
 // Empty File?
 if ( (FileBufLen=filelength(fileno(f))) != 0 )
  {
   if ( (FileBuf=(char*)malloc(FileBufLen)) == 0 )
    {
     MessageBox(0,"StringDB ALLOCATION ERROR",
                "OUT OF MEMORY", MB_ICONERROR|MB_OK);
     fclose(f);
     return 0;
    }

   if ( fread(FileBuf,1,FileBufLen,f) != (uint) FileBufLen )
    {
     MessageBoxf(0,"StringDB READ ERROR",MB_ICONERROR|MB_OK,
                 "Unable to read File %s\n%s",StringDBFname,strerror(errno));
     free ( FileBuf );
     FileBuf=0;
     fclose(f);
     return 0;
    }
  // Make sure the last character is a Null-Terminator
  FileBuf[FileBufLen-1] = 0;

  // Count the Number of Strings
  for (Bytes=0; Bytes<FileBufLen; Bytes += Len, ++StringCount)
    Len=strlen(&FileBuf[Bytes])+1;
  }
#endif

 fclose(f);
}
 

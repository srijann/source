void logg(int iLine, char *buf1, char *buf2, int i, long l);

void logg(int iLine, char *buf1, char *buf2, int i, long l)
{
   FILE  *fout;
   fout = fopen("err.log", "a");
   fprintf(fout,"@%d %s - %s. .%ld. .%d.\n", iLine, buf1, buf2, i, l);
   fclose(fout);
}


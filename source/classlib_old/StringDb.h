#ifndef __STRINGDB_H
#define __STRINGDB_H

class StringDB {
  bool  ModifiedFlag;
  char  StringDBFname[MAX_PATH+1];
  long  FileBufLen;
  char *FileBuf;
  struct IndexDef 
   {
    char* StrPtr;
    bool  New;
   } *Index;
  int   StringCount;
  int   IndexCount;
  char  UndefinedString[25];
  char  EmptyString[1];
  void  ChangeFileExtension(char*,char*);
  int   WriteStrings(char*);
  void  FreeIndex();

  public:
  StringDB(const char* Fname);
  ~StringDB();
  char* GetString(int);
  void  PutString(char*,int);
  int   Save(bool CreateBackup=true);
  int   Load();
  bool  IsModified() { return ModifiedFlag; };
 };
#endif 

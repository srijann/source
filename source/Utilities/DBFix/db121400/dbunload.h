#define STRICT
#include <windows.h>
#include <iostream.h>
#include <conio.h>
#include <stdio.h>
//#define NOFLDNAMES
#include "rds.h"
#include "DBPath.h"

extern RDM_SESS SessionHandle;
extern RDM_DB   DBHandle;
extern FILE* SaveFile;
extern char* SaveFileName;
extern int   RecordCount;

extern void quit(int ecode);
extern void Open(const char* dbName);
extern void Close();
extern void Init(const char* Name);
extern void CloseAll();
extern void CopyTemp(short Type,const char* Name,char* Buf,int Len);
        
extern void UnloadLogFile();





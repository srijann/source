#ifndef __DBMANMSG_H
#define __DBMANMSG_H

#include "dbdefs.h"
#include "CheckAuthFn.h"
#include "FormDialog.h"
#include "Dbmanrec.h"

class DBManMessage : public IPCMessage {
 public:
 DBManMessage();
 ~DBManMessage();
 bool DBPut(FormDialog* Win,char* TranType,FLDNUM RecFn,DBManRecord* Rec);
 int  DBGet(char* TranType,void* Key,int KeyLen,
            FLDNUM RecFn=0,DBManRecord* Rec=0,bool NotFoundOkay=true);
};
#endif

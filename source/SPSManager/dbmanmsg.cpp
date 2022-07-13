//----------------------------------------------------------------------------
//  Project spsmanager
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    spsmanager.apx Application
//  FILE:         DBManMsg.cpp
//  AUTHOR:
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of DBManMsg (IPCMessage)
//
//----------------------------------------------------------------------------

#include "win4me.h"
#pragma hdrstop
#include <time.h>
#include "dbmanmsg.h"
#include "DestName.h"
#include "splib.h"

#define NETTIMEOUT 10000

//----------------------------------------------------------------
// We're encapsulating the complexity of GETs and PUTs to the SPS
// Manager database here.
//
// This is a derived class of IPCMessage
//-----------------------------------------------------------------

DBManMessage::DBManMessage()
{
}

DBManMessage::~DBManMessage()
{
}

//------------------------------
// Put a record to the Database
//------------------------------
bool DBManMessage::DBPut(FormDialog* Win,char* TranType,FLDNUM RecFn,
              DBManRecord* Rec)
{
 DBRESULT Result;
 int Len;

 for (;;)
  {
   PutFld(FN_TRANTYPE,TranType);
   if ( RecFn )
    PutFld(RecFn,Rec->DataPtr(),Rec->DataLen());

   while ( ! SendRecv(MANAGERDB_DEST,NETTIMEOUT) )
    {
     Win->SetTop();
     if ( MessageBox(Win->GetHandle(),"","NO RESPONSE FROM DATABASE",
              MB_RETRYCANCEL|MB_ICONERROR) == IDCANCEL )
      return false;
    }

   Len=sizeof(Result);
   if ( ! GetFld(FN_DBRESULT,&Result,Len) )
    Result.ResponseCode=DB_ERROR;

   switch( Result.ResponseCode )
    {
     case DB_OKAY: Rec->PutTimeStamp(Result.TimeStamp);
                   return true;

     case DB_MODIFIED:
            Win->SetTop();
            if ( MessageBoxf(Win->GetHandle(),"",MB_ICONWARNING|MB_OKCANCEL,
                  "RECORD WAS MODIFIED\n%sDO YOU WANT TO REPLACE IT?\n",
                  asctime(localtime((const long*)&Result.TimeStamp)) ) == IDCANCEL )
               return false;
            Rec->PutTimeStamp(0);
            break;

     default:
       Win->SetTop();
       // If error text, display it and return
       if ( Result.Text[0] )
        {
         MessageBox(Win->GetHandle(),Result.Text,"",MB_OK);
         return false;
        }

       // Give option for retry
       if ( MessageBox(Win->GetHandle(),
             "A DATABASE I/O ERROR HAS OCCURRED","",
              MB_RETRYCANCEL|MB_ICONERROR) == IDCANCEL )
             return false;
       // If Retrying, switch Destinations
       break;
    }

   DelFld(FN_DBRESULT);
  }

}

//-------------------------------
// Get a record from the Database
//-------------------------------
int DBManMessage::DBGet(char* TranType,void* Key,int KeyLen,
                        FLDNUM RecFn,DBManRecord* Rec,bool NotFoundOkay)
{
 DBRESULT Result;
 int      Len;

 for (;;)
  {
   PutFld(FN_TRANTYPE,TranType);
   if ( Key )
     PutFld(FN_DBKEY,Key,KeyLen);

   if ( ! SendRecv(MANAGERDB_DEST,NETTIMEOUT,true) )
    return DB_ERROR;

   Len=sizeof(Result);
   if ( ! GetFld(FN_DBRESULT,&Result,Len) )
    Result.ResponseCode=DB_ERROR;

   if ( Result.ResponseCode != DB_ERROR &&
        RecFn  &&
        ! GetFld(RecFn) )
     Result.ResponseCode = DB_NOTFOUND;

   switch( Result.ResponseCode )
    {
     case DB_OKAY:        if ( Rec && RecFn )
                           {
                            Len=Rec->DataLen();
                            GetFld(RecFn,Rec->DataPtr(),Len);
                           }
                          return Result.ResponseCode;

     case DB_NOTFOUND:    if ( ! NotFoundOkay )
                            MessageBox(0,
                               "THE REQUESTED RECORD IS NOT ON FILE","",
                               MB_OK);
                          return Result.ResponseCode;

     case DB_ERROR:
     default:
       // No error message, give option for a retry to alternate dest
       if ( MessageBox(0,"A DATABASE I/O ERROR HAS OCCURRED","",
                       MB_RETRYCANCEL|MB_ICONERROR) == IDCANCEL )
         return Result.ResponseCode;

      // If retrying, switch destinations
      break;
    }

   DelFld(FN_DBRESULT);
  }

}

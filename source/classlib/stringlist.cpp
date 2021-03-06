//----------------------------------------------------------------------------
//  Project Classlib
//
//  Copyright ? 1996. All Rights Reserved.
//
//  FILE:         StringList.cpp
//  AUTHOR:       Jack Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of StringList (Base Class)
//
//  This is a general purpose List Handler class. It provides member
//  functions for fetching the list from a variety of sources, including
//  files and database servers. It also provides member functions for
//  managing the list.
//
//  List items are simply null-terminated Text Strings.
//----------------------------------------------------------------------------

#include "Win4me.h"
#pragma hdrstop
#include <time.h>
#include "StringList.h"
#include "CheckauthFn.h"
#include "splib.h"
#include "DestName.h"
#include "TranType.h"

#define NETTIMEOUT 10000

//-------------------------------------------------------------
// This Constructor fetches a list from the List Manager, using
// the List Name supplied. Specify some slack bytes if your
// list needs to grow (you don't have to but it saves re-allocating
// memory).
//-------------------------------------------------------------
StringList::StringList(char* ListName,int SlackBytes) :
 NumSlackBytes(SlackBytes),
 ShouldDelete(true),
 InitFail(false),
 TheList(0)
{
 IPCMessage* Msg = new IPCMessage();
 for (;;)
  {
   // Try primary dest
   if ( DbFetch(ListName,Msg) )
     break;

   // Ask user for retry. Throw an exception if NO
   if ( MessageBoxf(0,"",MB_ICONERROR|MB_RETRYCANCEL,
                    "UNABLE TO INTIALIZE LIST %s\n"
                     "NO RESPONSE FROM LIST MANAGER",ListName) == IDCANCEL )
    {
     TheList=new char[ListBufLen=sizeof(LISTHDR)];
     ListLen=ListBufLen;
     InitFail=true;
     break;
    }
  }

 delete Msg;
}

//--------------------------------------------------------
// This Constructor initializes the List from the callers'
// buffer.
//--------------------------------------------------------
StringList::StringList(char* List,int Length,int SlackBytes,bool Delete) :
 TheList(List),
 NumSlackBytes(SlackBytes),
 ShouldDelete(Delete),
 InitFail(false)
{
 ListBufLen=Length + SlackBytes;
 TheList = new char[ListBufLen];
 ListLen=Length;
 memcpy(TheList,List,Length);
}

//------------------------------------------------------
// This Constructor initializes a List from another List
//------------------------------------------------------
StringList::StringList(StringList* sList)
{
 ListLen=sList->ListLen;
 ListBufLen=sList->ListBufLen;
 ShouldDelete=sList->ShouldDelete;
 InitFail=sList->InitFail;
 NumSlackBytes=sList->NumSlackBytes;
 TheList=new char[ListBufLen];
 memcpy(TheList,sList->TheList,ListBufLen);
}

//-------------------------------------------------------
// Construct an empty list with InitialBuffer Size set as
// indicated
//-------------------------------------------------------
StringList::StringList(int InitialSize,int SlackBytes,char* ListName) :
 ShouldDelete(true),
 NumSlackBytes(SlackBytes),
 InitFail(false)
{
 LISTHDR* hdr;
 ListBufLen=max((int)sizeof(LISTHDR),InitialSize);
 TheList = new char[ListBufLen];
 ListLen=sizeof(LISTHDR);
 hdr=(LISTHDR*)TheList;
 memset(hdr,0,sizeof(hdr));
 memcpy(hdr->ListName,ListName,
           min(strlen(ListName),sizeof(hdr->ListName)-1));
}

//----------------------------
// Construct from data on file
//----------------------------
StringList::StringList(TFile* File,int Length,int SlackBytes) :
 NumSlackBytes(SlackBytes),
 ShouldDelete(true),
 InitFail(false)
{
 ListBufLen=max( (int)sizeof(LISTHDR), (int)(Length + SlackBytes) );
 ListLen=max(Length,ListBufLen);
 TheList = new char[ListBufLen];
 // If the Read fails, we set the list length to zero. The caller
 // can query the length after creating the object to check the
 // completion status
 if ( File->Read(TheList,Length) != Length )
  {
   ListLen=sizeof(LISTHDR);
   InitFail=true;
  }
}

//------------------------------------------
// Construct from contents of an IPC Message
//------------------------------------------
StringList::StringList(IPCMessage* Msg,int SlackBytes) :
 NumSlackBytes(SlackBytes),
 ShouldDelete(true),
 InitFail(false)
{
 ListLen = Msg->GetFldLen(FN_DBLIST);

 // NO LIST PRESENT?
 if ( ListLen < sizeof(LISTHDR) )
  {
   ListLen=sizeof(LISTHDR);
   ListBufLen=ListLen+SlackBytes;
   TheList=new char[ListBufLen];
   InitFail=true;
   return;
  }

 ListBufLen = ListLen + SlackBytes;
 TheList = new char[ListBufLen];
 Msg->GetFld(FN_DBLIST,TheList,ListLen);
}

//-----------
// Destructor
//-----------
StringList::~StringList()
{
 if ( ShouldDelete &&
      TheList != 0 )
  delete[] TheList;
}

//---------------------------------------------------------------
// Add an Item to the List. Optionally keeping it in sorted order
//---------------------------------------------------------------
void StringList::Add(char* Item,bool Sorted)
{
 int   Len=strlen(Item);
 int   i;
 char *ItemPtr;

 if ( Sorted )
   for (i=sizeof(LISTHDR), ItemPtr=&TheList[i]; i < ListLen;
        i += strlen(ItemPtr) )
    if ( stricmp(ItemPtr,Item) > 0 )
     {
      strcpy(MakeFit(Len+1,ItemPtr),Item);
      return;
     }

 strcpy(MakeFit(Len+1),Item);
}

//------------------------------------------------------------
// Insure room for an Item in the List, expanding if necessary
//------------------------------------------------------------
char* StringList::MakeFit(int Len,char *InsertPoint)
{
 char *NewList;
 int   Offset;

 if ( InsertPoint )
  {
   // Make sure it doesn't go beyond the end of the list
   if ( InsertPoint > &TheList[ListLen] )
    InsertPoint = &TheList[ListLen];
   Offset = InsertPoint - TheList;
  }
 else
   Offset = ListLen;

 // Expand the buffer if necessary
 if ( ListLen + Len > ListBufLen )
  {
   ListBufLen = ListBufLen+Len+NumSlackBytes;
   NewList = new char[ListBufLen];
   memcpy(NewList,TheList,ListLen);
   delete[] TheList;
   TheList=NewList;
  }

 if ( Offset < ListLen )
   memmove(&TheList[Offset+Len],&TheList[Offset],ListLen-Offset);

 ListLen += Len;  
 return &TheList[Offset];
}

//----------------------------------------------
// Empty the List. Clears all but reserved bytes
//----------------------------------------------
void StringList::Clear()
{
 ListLen=sizeof(LISTHDR);
}

//--------------------------------------------------------------------
// Iterator -- Passes String, Index, and args to callers' function for
// each member
//--------------------------------------------------------------------
void StringList::ForEach(void(*func)(char*,int,void*),void* Args)
{
 int i, n;
 for (i=sizeof(LISTHDR),n=0; i < ListLen;
      i += strlen(&TheList[i]) + 1, ++n )
   func(&TheList[i],n,Args);
}

//---------------------------------
// Fetch the list from the database
//---------------------------------
bool StringList::DbFetch(char* ListName,IPCMessage* Msg)
{
 DBLISTINFO Info;
 int Length;

 memset(&Info,0,sizeof(Info));
 for (;;)
  {
   // Put in the Tran type and the Info structure
   Msg->PutFld(FN_TRANTYPE,T_GETLIST);
   memcpy(Info.ListName,ListName,min(strlen(ListName),sizeof(Info.ListName)-1));
   Msg->PutFld(FN_DBLISTINFO,&Info,sizeof(Info));

   // Send request and Wait 7 seconds for a response. retry once
   if ( ! Msg->SendRecv(LISTMANAGER_DEST,NETTIMEOUT,true) )
     return false;

   // Pull the Info structure from the message. If not there, this
   // is an error
   Length=sizeof(Info);
   if ( ! Msg->GetFld(FN_DBLISTINFO,&Info,Length) )
    return false;

   // Is this the First Segment? -- Allocate buffer for the list
   if ( Info.SegmentOffset == 0 )
    {
     // If already had a List, delete it.
     if ( TheList != 0 )
       delete[] TheList;
     // Create new list buffer
     ListLen = max( Info.ListLen, sizeof(LISTHDR) );
     ListBufLen=ListLen+NumSlackBytes;
     TheList = new char[ListBufLen];
    }

   // Pull the list segment from the message buffer
   Length=Info.SegmentLen;
   Msg->GetFld(FN_DBLIST,&TheList[Info.SegmentOffset],Length);
   if ( Length != (int) Info.SegmentLen )
     return false;

   // Was this the final segment?
   if ( Info.SegmentLen + Info.SegmentOffset == Info.ListLen )
    {
     // Insure that there is a NULL terminator at the end
     if ( ListLen > sizeof(LISTHDR) )
      TheList[ListLen-1]=0;
     return true;
    }

   // Set up to fetch the next segment
   Msg->DelFld(FN_DBLIST);
   Info.SegmentOffset += Info.SegmentLen;
  }

}

//----------------------
// Answer a list request
//----------------------
bool StringList::DbAnswer(IPCMessage* Msg)
{
 DBLISTINFO Info;
 int Length = sizeof(Info);

 if ( ! Msg->GetFld(FN_DBLISTINFO,&Info,Length) )
  return false;

 // Clear the message of all but the Info structure
 Msg->ClearMsg();
 Info.TimeStamp=time(NULL);
 Info.ListLen=ListLen;
 Msg->PutFld(FN_DBLISTINFO,&Info,sizeof(Info));

 // Send 1st Segment if requestor's timestamp is zero
 if ( Info.TimeStamp == 0 )
  {
   Info.SegmentOffset=0;
   Info.SegmentLen = min( ListLen, Msg->GetMaxField() );
   Msg->PutFld(FN_DBLISTINFO,&Info,sizeof(Info));
   Msg->PutFld(FN_DBLIST,TheList,Info.SegmentLen);
   Msg->SendMsg("REPLY");
   return true;
  }

 // Handle Request for next segment of list

 // Protect against GPF
 if ( (int) Info.SegmentOffset >= ListLen )
   return false;

 // Return next segment
 Info.SegmentLen = min( (int)(ListLen - Info.SegmentOffset), Msg->GetMaxField() );
 Msg->PutFld(FN_DBLISTINFO,&Info,sizeof(Info));
 Msg->PutFld(FN_DBLIST,&TheList[Info.SegmentOffset],Info.SegmentLen);
 Msg->SendMsg("REPLY");
 return true;
}

//------------------------
// Save the List to a file
//------------------------
bool StringList::Save(TFile* File)
{
 return File->Write(TheList,ListLen) == ListLen;
}

//------------------------------
// Save the List to the Database
//------------------------------
bool StringList::Save()
{
 IPCMessage Msg;
 DBRESULT Result;
 int Len;

 for (;;)
  {
   Msg.ClearMsg();
   Msg.PutFld(FN_TRANTYPE,T_PUTLIST);
   if ( ! Msg.PutFld(FN_DBLIST,TheList,ListLen) )
    {
     MessageBox(0,"THE LIST IS TOO BIG","ERROR",MB_OK|MB_ICONERROR);
     return false;
    }

   if ( ! Msg.SendRecv(LISTMANAGER_DEST,NETTIMEOUT,true) )
     return false;

   Len=sizeof(Result);

   if ( ! Msg.GetFld(FN_DBRESULT,&Result,Len) )
    Result.ResponseCode=DB_ERROR;

   switch( Result.ResponseCode )
    {
     case DB_OKAY: memcpy(TheList,&Result.TimeStamp,sizeof(DBTIMESTAMP));
                   return true;

     case DB_MODIFIED:
            if ( MessageBoxf(0,"",MB_ICONWARNING|MB_OKCANCEL,
                  "RECORD WAS MODIFIED\n%sDO YOU WANT TO REPLACE IT?\n",
                  asctime(localtime((const long*)&Result.TimeStamp)) ) == IDCANCEL )
               return false;
            memset(TheList,0,sizeof(DBTIMESTAMP));
            break;

     default:
       if ( Result.Text[0] )
        {
         MessageBox(0,Result.Text,"",MB_OK);
         return false;
        }

       // Give option for retry
       if ( MessageBox(0,
             "A DATABASE I/O ERROR HAS OCCURRED","",
              MB_RETRYCANCEL|MB_ICONERROR) == IDCANCEL )
             return false;

       break;
    }

  }

}

//---------------
// Find by Number
//---------------
char* StringList::Find(int Num)
{
 int i, n;
 for (i=sizeof(LISTHDR),n=0; i < ListLen;
      i += strlen(&TheList[i]) + 1, ++n )
  if ( n==Num )
   return(&TheList[i]);

 return 0;
}

//---------------------------
// Replace a String by Number
//---------------------------
bool StringList::Replace(int Num,char* String)
{
 char* Ptr=Find(Num);
 uint oldLen, newLen;

 if ( Ptr==NULL )
  return false;

 // Same size strings, list stays the same
 if ( (oldLen=strlen(Ptr)) == (newLen=strlen(String)) )
  {
   strcpy(Ptr,String);
   return true;
  }

 // Shrink the List
 if ( oldLen > newLen )
  {
   memcpy(Ptr,&Ptr[oldLen-newLen],ListLen-(Ptr-TheList));
   ListLen -= (oldLen-newLen);
   TheList[ListLen]=0;
   strcpy(Ptr,String);
   return true;
  }

 // Expand the List
 strcpy(MakeFit(newLen-oldLen,Ptr),String);
 return true;
}

//---------------
// Find by String
//---------------
int StringList::Find(char* Str)
{
 int i, n;
 for (i=sizeof(LISTHDR),n=0; i < ListLen;
      i += strlen(&TheList[i]) + 1, ++n )
  if ( stricmp(Str,&TheList[i]) == 0 )
   return(n);

 return -1;  
}

//---------------------------------------------------
// Overloaded ++ Operator to iterate through strings.
// Resets to beginning when Num==0
//---------------------------------------------------
char* StringList::Next(bool Reset)
{
 char *Ptr;

 if ( Reset )
  {
   IteratorIndex = sizeof(LISTHDR);
   return 0;
  }

 if ( IteratorIndex >= ListLen )
   return 0;

 Ptr=&TheList[IteratorIndex];
 IteratorIndex += strlen(Ptr) + 1;
 return Ptr;
}

//---------------------------------------------------
// Iterate through strings.
// Resets to beginning when
//---------------------------------------------------
char* StringList::Next(int& Val,bool Reset)
{
 char *Ptr;
 char* commaPtr;
 int   i;

 if ( Reset )
  {
   IteratorIndex = sizeof(LISTHDR);
   return 0;
  }

 if ( IteratorIndex >= ListLen )
   return 0;

 Ptr=&TheList[IteratorIndex];
 strcpy(stringBuf,Ptr);
 Val=0;
 if ( (commaPtr=strchr(stringBuf,',')) != NULL )
  {
   *commaPtr=0;
   for (i=1; commaPtr[i] != 0 && ! isdigit(commaPtr[i]); ++i);
   Val=atoi(&commaPtr[i]);
  }

 IteratorIndex += strlen(Ptr) + 1;
 return stringBuf;
}


//-------------------------------------
// Initialize a Combo Box from the list
//-------------------------------------
void StringList::InitComboBox(TComboBox* cBox)
{
 char *strPtr;
 int   Val;

 // Add the string to the box, setting the item data appropriately
 for (Next(true); (strPtr=Next(Val)) != NULL; )
   cBox->SetItemData( cBox->AddString(strPtr), Val);
}

//----------------------------------
// Compare if two lists are the same
//----------------------------------
bool StringList::isSame(StringList* cList)
{
 if ( cList->ListLen != ListLen )
  return false;

 return memcmp(cList->TheList,TheList,ListLen) == 0;
}


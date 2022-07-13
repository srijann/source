//----------------------------------------------------------------------------
//  Project Classlib
//
//  Copyright © 1996. All Rights Reserved.
//
//  FILE:         StringTreeList.cpp
//  AUTHOR:       Jack Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of StringTreeList (StringList)
//
//  This is a Derived class of StringList to provide a tree structured
//  list. It is used by the SPS Manager database to handle the Auth
//  Rules List.
//
//  List items are preceded either by a P (Parent) or a C (Child)
//----------------------------------------------------------------------------

#include "win4me.h"
#pragma hdrstop
#include "StringTreeList.h"

StringTreeList::StringTreeList(char* ListName,int SlackBytes) :
  StringList(ListName,SlackBytes)
{
}

StringTreeList::StringTreeList(char* TheList,int ListLen,int SlackBytes,bool Delete):
 StringList(TheList,ListLen,SlackBytes,Delete)
{
}

StringTreeList::StringTreeList(int InitialSize,int SlackBytes,char* ListName):
  StringList(InitialSize,SlackBytes,ListName)
{
}

StringTreeList::StringTreeList(TFile* File,int Len,int SlackBytes):
  StringList(File,Len,SlackBytes)
{
}

StringTreeList::StringTreeList(IPCMessage* Msg,int SlackBytes):
  StringList(Msg,SlackBytes)
{
}

//------------------------------
// Add a Node
//   Assumption - Tree is sorted, does not allow duplicate names, and
//   allows only one level of nesting
//------------------------------
bool StringTreeList::AddNode(char* ParentName,char* ChildName)
{
 char* EndOfList;
 char* LoopPtr;
 char* InsertPtr=0;
 int   cc;

 //Loop through, looking for Parent match or insertion point
 for (EndOfList=&TheList[ListLen],LoopPtr=&TheList[sizeof(LISTHDR)];
       LoopPtr<EndOfList;
         LoopPtr += strlen(LoopPtr) + 1)
  if ( *LoopPtr == 'P' )
   {
    if ( (cc=strcmp(ParentName,&LoopPtr[1])) > 0 )
      continue;

    // Found Parent Insertion point?
    if ( cc<0 )
     {
      InsertPtr=LoopPtr;
      break;
     }

    // Parent Match. Must have child to insert
    if ( ! ChildName || ! ChildName[0] )
     return false;

    // Find Child Insertion point
    for (LoopPtr += strlen(LoopPtr)+1; LoopPtr<EndOfList;
              LoopPtr += strlen(LoopPtr) + 1 )
     {
      // End of Child List?
      if ( *LoopPtr != 'C' )
       break;
      // Child belongs here?
      if ( (cc=strcmp(ChildName,&LoopPtr[1])) < 0 )
       break;
      // Duplicate child? not allowed
      if ( cc==0 )
       return false;
     }

    // Insert Child here
    InsertPtr=MakeFit(strlen(ChildName)+2,LoopPtr);
    *InsertPtr='C';
    strcpy(&InsertPtr[1],ChildName);
    return true;
   }

 // Insert Parent and/or child at InsertPtr location
 InsertPtr=MakeFit(strlen(ParentName)+2,InsertPtr);
 *InsertPtr='P';
 strcpy(&InsertPtr[1],ParentName);
 if ( ChildName && ChildName[0] )
  {
   InsertPtr=&InsertPtr[strlen(InsertPtr)+1];
   InsertPtr=MakeFit(strlen(ChildName)+2,InsertPtr);
   *InsertPtr='C';
   strcpy(&InsertPtr[1],ChildName);
  }
 return true;
}

//---------------------------------------------------------------
// Find an Item by it's relative number. Return the Parent name
// and the Child name, if any.
//----------------------------------------------------------------
bool StringTreeList::FindNode(int Num,char* ParentName,char* ChildName)
{
 int   n;
 char* LoopPtr=&TheList[sizeof(LISTHDR)];
 char* EndOfList=&TheList[ListLen-1];
 char* ParentPtr="";

 // Loop through the strings until we hit the relative number
 for (n=0; LoopPtr < EndOfList; LoopPtr += strlen(LoopPtr) + 1, ++n)
  if ( n==Num )
   {
    switch(*LoopPtr)
     {
      default:
      case 'P':  strcpy(ParentName,&LoopPtr[1]);
                 if ( ChildName )
                   *ChildName=0;
                 return true;

      case 'C':  if ( ChildName )
                   strcpy(ChildName,&LoopPtr[1]);
                 strcpy(ParentName,ParentPtr);
                 return true;
     }
   }
  else
   // Remember the Parent
   if (*LoopPtr=='P' )
    ParentPtr=&LoopPtr[1];

 return false;
}

//--------------------------------------------------------------
// Find a Parent and/or Child and return it's relative number.
// return -1 if not found
//--------------------------------------------------------------
int StringTreeList::FindNode(char* ParentName,char *ChildName)
{
 int   n;
 char* LoopPtr=&TheList[sizeof(LISTHDR)];
 char* EndOfList=&TheList[ListLen-1];

 // Find the Parent by name
 for (n=0; LoopPtr < EndOfList; LoopPtr += strlen(LoopPtr) + 1, ++n)
  if ( *LoopPtr == 'P' &&
       strcmp(ParentName,&LoopPtr[1]) == 0 )
   break;

 if ( LoopPtr >= EndOfList )
   return -1;

 if ( ! ChildName )
   return n;

 // Find the Child
 for (LoopPtr += strlen(LoopPtr)+1; LoopPtr < EndOfList;
        LoopPtr += strlen(LoopPtr) + 1, ++n)
  if ( *LoopPtr == 'C' )
   {
    if ( strcmp(ChildName,&LoopPtr[1]) == 0 )
      return n;
   }
  else
    return -1;

 // no hit
 return -1;
}



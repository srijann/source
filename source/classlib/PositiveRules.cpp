//----------------------------------------------------------------------------
//  Project Classlib
//
//  Copyright © 1996. All Rights Reserved.
//
//  FILE:         PositiveRules.cpp
//  AUTHOR:       Jack Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of PositiveRules (DBManRecord)
//
//  This is a Derived class of DBManRecord to handle the Positive File
//  Rules Record.
//----------------------------------------------------------------------------

#include "Win4me.h"
#pragma hdrstop
#include <mem.h>
#include <string.h>
#include "PositiveRules.h"

//----------------------------
// Make a new (empty) rule set
//----------------------------
PositiveRules::PositiveRules()
{
 memset(&Data,0,sizeof(Data));
}

//-----------------------------------------------
// Make a rule set, copying from another rule set
//-----------------------------------------------
PositiveRules::PositiveRules(struct PositiveRulesRecord* Rec)
{
 memcpy(&Data,Rec,sizeof(Data));
}

//--------------------------------------------------
// Make a rule set, getting data from an IPC message
//--------------------------------------------------
PositiveRules::PositiveRules(FLDNUM fn,IPCMessage* Msg)
{
 int Len=sizeof(Data);

 if ( ! Msg->GetFld(fn,&Data,Len) )
  memset(&Data,0,sizeof(Data));
}

PositiveRules::~PositiveRules()
{
}

//-----------------------------------
// Format a New Positive Rules Record
//-----------------------------------
void PositiveRules::FormatNew(const char* ClassName)
{
 memset(&Data,0xff,sizeof(Data));
 memset(Data.PositiveRulesClassName,0,sizeof(Data.PositiveRulesClassName));
 strcpy((char*)Data.PositiveRulesClassName,ClassName);
 // These are constants. They are not input by the user.
 Data.AgeTbl[0].MaxAge = 21;
 Data.AgeTbl[1].MaxAge = 25;
 Data.AgeTbl[2].MaxAge = 30;
 Data.AgeTbl[3].MaxAge = 35;
 Data.AgeTbl[4].MaxAge = 40;
 Data.AgeTbl[5].MaxAge = 45;
 Data.AgeTbl[6].MaxAge = 50;
 Data.AgeTbl[7].MaxAge = 254;
}

//------------------------------------------------------
// Return the Key Type (this is for the Database Server)
//------------------------------------------------------
int PositiveRules::KeyType()
{
 return POSITIVERULESCLASSNAME;
}

//------------------------------------------------------
// Return the Rec Type (this is for the Database Server)
//------------------------------------------------------
int PositiveRules::RecType()
{
 return POSITIVERULESRECORD;
}



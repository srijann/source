//----------------------------------------------------------------------------
//  Project Classlib
//
//  Copyright © 1996. All Rights Reserved.
//
//  FILE:         PosResponse.cpp
//  AUTHOR:       Jack Ely 
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of PosResponse   (DBManRecord)
//
//  This is a Derived class of DBManRecord to handle the POS Response
//  Record.
//----------------------------------------------------------------------------

#include "Win4me.h"
#pragma hdrstop
#include <mem.h>
#include <string.h>
#include "PosResponse.h"

//----------------------------
// Make a new (empty) rule set
//----------------------------
PosResponse::PosResponse()
{
 memset(&Data,0,sizeof(Data));
}

//-----------------------------------------------
// Make a rule set, copying from another rule set
//-----------------------------------------------
PosResponse::PosResponse(struct PosResponseRecord* Rec)
{
 memcpy(&Data,Rec,sizeof(Data));
}

//--------------------------------------------------
// Make a rule set, getting data from an IPC message
//--------------------------------------------------
PosResponse::PosResponse(FLDNUM fn,IPCMessage* Msg)
{
 int Len=sizeof(Data);

 if ( ! Msg->GetFld(fn,&Data,Len) )
  memset(&Data,0,sizeof(Data));
}

PosResponse::~PosResponse()
{
}

//--------------------
// Format a New Record
//--------------------
void PosResponse::FormatNew(const char* ClassName)
{
 int i;
 memset(&Data,0,sizeof(Data));
 strcpy(Data.ResponseClassName,ClassName);
 for (i=0; i<80; ++i)
  strcpy(Data.ResponseText[i],"CALL %R");
 for (i=60; i<70; ++i)
  strcpy(Data.ResponseText[i],"OKAY %A");
}

int PosResponse::KeyType()
{
 return RESPONSECLASSNAME;
}

int PosResponse::RecType()
{
 return RESPONSERECORD;
}



//----------------------------------------------------------------------------
//  Project Classlib
//
//  Copyright © 1996. All Rights Reserved.
//
//  FILE:         edituns.cpp
//  AUTHOR:       Jack Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of TEditUns (PlainEdit).
//
//  This is a derived class of Tedit (PlainEdit) to support Unsigned
//  number conversions To/From Edit Controls (provides polymorphic
//  member functions for ease of use). See the UnsNum module
//  in the Lib directory for a description of Unsigned Numbers.
//
//  NOTE: See EditVar. It may be more useful than this. It does
//  the transferring for you automatically.
//----------------------------------------------------------------------------
#include "win4me.h"
#pragma hdrstop

#include "edituns.h"


//{{TEditUns Implementation}}


TEditUns::TEditUns(TWindow* parent, int id, uint textLimit, TModule* module) :
  PlainEdit(parent,id,textLimit,module)
{
 // The internal formatting buffer is sized big enough for the largest
 // possible integer. Make sure the text limit is not greater than this
 if ( textLimit > sizeof(FmtBuf) )
  SetTextLimit(sizeof(FmtBuf));
}


TEditUns::~TEditUns()
{
 Destroy(IDCANCEL);
}

// Get a plain long from the Control
void TEditUns::get(unsigned long& Val)
{
 Transfer(FmtBuf,tdGetData);
 atou(FmtBuf,Val);
}

// Get a plain int from the Control
void TEditUns::get(unsigned int& Val)
{
 Transfer(FmtBuf,tdGetData);
 atou(FmtBuf,Val);
}

// Get a a plain short from the control
void TEditUns::get(unsigned short& Val)
{
 Transfer(FmtBuf,tdGetData);
 atou(FmtBuf,Val);
}

// Get a plain char from the control
void TEditUns::get(unsigned char& Val)
{
 Transfer(FmtBuf,tdGetData);
 atou(FmtBuf,Val);
}

// Get a special long from the control
void TEditUns::Get(unsigned long& Val)
{
 Transfer(FmtBuf,tdGetData);
 AtoU(FmtBuf,Val);
}

// Get a special int from the control
void TEditUns::Get(unsigned int& Val)
{
 Transfer(FmtBuf,tdGetData);
 AtoU(FmtBuf,Val);
}

// Get a special short from the control
void TEditUns::Get(unsigned short& Val)
{
 Transfer(FmtBuf,tdGetData);
 AtoU(FmtBuf,Val);
}

// Get a special char from the control
void TEditUns::Get(unsigned char& Val)
{
 Transfer(FmtBuf,tdGetData);
 AtoU(FmtBuf,Val);
}

// Transfer a plain long to the control
void TEditUns::set(unsigned long Val,int width)
{
 utoa(FmtBuf,Val,width);
 Transfer(FmtBuf,tdSetData);
}

// Transfer a plain int to the control
void TEditUns::set(unsigned int Val,int width)
{
 utoa(FmtBuf,Val,width);
 Transfer(FmtBuf,tdSetData);
}

// Transfer a plain short to the control
void TEditUns::set(unsigned short Val,int width)
{
 utoa(FmtBuf,Val,width);
 Transfer(FmtBuf,tdSetData);
}

// Transfer a plain char to the control
void TEditUns::set(unsigned char Val,int width)
{
 utoa(FmtBuf,Val,width);
 Transfer(FmtBuf,tdSetData);
}

// Transfer a Special long to the control
void TEditUns::Set(unsigned long Val,int width)
{
 UtoA(FmtBuf,Val,width);
 Transfer(FmtBuf,tdSetData);
}

// Transfer a Special int to the control
void TEditUns::Set(unsigned int Val,int width)
{
 UtoA(FmtBuf,Val,width);
 Transfer(FmtBuf,tdSetData);
}

// Transfer a Special short to the control
void TEditUns::Set(unsigned short Val,int width)
{
 UtoA(FmtBuf,Val,width);
 Transfer(FmtBuf,tdSetData);
}

// Transfer a Special char to the control
void TEditUns::Set(unsigned char Val,int width)
{
 UtoA(FmtBuf,Val,width);
 Transfer(FmtBuf,tdSetData);
}



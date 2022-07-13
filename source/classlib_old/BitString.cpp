//----------------------------------------------------------------------------
//  Project Classlib
//
//  Copyright © 1996. All Rights Reserved.
//
//  FILE:         BitString.cpp
//  AUTHOR:       Jack Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of BitString (Base Class)
//
//  This is a general purpose Bit String Handler. I don't think it
//  was ever used in the SPS application.
//----------------------------------------------------------------------------

#include <mem.h>
#include <limits.h>
#include "BitString.h"

//------------------
// Class Constructor
//------------------
BitString::BitString(BITNUM  HowMany,char* String) :
 NumBits(HowMany),
 TheString(String)
{
 if ( TheString==0 )
  {
   NumBytes=(NumBits+7)/8;
   TheString=new char[NumBytes];
   ShouldDelete=true;
   memset(TheString,0,NumBytes);
  }
 else
  ShouldDelete=false;
}

//-----------------
// Class Destructor
//-----------------
BitString::~BitString()
{
 if ( ShouldDelete )
  delete[] TheString;
}

//-------------------
// Set a Bit to value
//-------------------
void BitString::Set(BITNUM  Bit,bool Value)
{
 if ( Bit >= NumBits )
  return;
 if ( Value==true )
   TheString[Bit/8] |= (unsigned char) (1 << (Bit%8));
 else
   TheString[Bit/8] &= (unsigned char) ~(1 << (Bit%8));
}

//-----------------------------
// Set a Range of bits to value
//-----------------------------
void BitString::Set(BITNUM  FirstBit,BITNUM  LastBit,bool Value)
{
 BITNUM  i;
 for (i=FirstBit; i<=LastBit; ++i)
  Set(i,Value);
}

//----------------------
// Set all bits to Value
//----------------------
void BitString::SetAll(bool Value)
{
 if ( Value==true )
  memset(TheString,0xff,NumBytes);
 else
  memset(TheString,0,NumBytes);
}

//-----------
// Test a Bit
//-----------
bool BitString::Test(BITNUM  Bit,bool Value)
{
 if ( Bit >= NumBits )
  return false;

 return ((TheString[Bit/8] >> (Bit%8)) & 1 ) == Value;
}

//----------------------------------
// Test if Any bits in a range=value
//----------------------------------
bool BitString::TestAny(BITNUM  FirstBit,BITNUM  LastBit,bool Value)
{
 BITNUM  Bit;
 for (Bit=FirstBit; Bit<=LastBit; ++Bit)
  if ( Test(Bit,Value) )
    return true;

 return false;
}

//----------------------------------------------
// Test if Any bits in the entire string = Value
//----------------------------------------------
bool BitString::TestAny(bool Value)
{
 return TestAny(0,NumBits-1,Value);
}

//------------------------------------------
// Test if All the Bits in the range = Value
//------------------------------------------
bool BitString::TestAll(BITNUM  FirstBit,BITNUM  LastBit,bool Value)
{
 BITNUM  i;
 for (i=FirstBit; i<=LastBit; ++i)
  if ( ! Test(i,Value) )
    return false;

 return true;
}

//-------------------------------------------
// Test if ALL the Bits in the String = Value
//-------------------------------------------
bool BitString::TestAll(bool Value)
{
 return TestAll(0,NumBits-1,Value);
}

//-----------------------------------------
// Count how many bits in the range = Value
//-----------------------------------------
BITNUM  BitString::HowMany(BITNUM  FirstBit,BITNUM  LastBit,bool Value)
{
 BITNUM  Count, i;

 for (i=FirstBit, Count=0; i<=LastBit; ++i)
  if ( Test(i,Value) )
   ++Count;

 return Count;
}

//------------------------------------------
// Count how many bits in the String = Value
//------------------------------------------
BITNUM  BitString::HowMany(bool Value)
{
 return HowMany(0,NumBits,Value);
}

//---------------------------------------------------------
// Format callers' string with a representation of the bits
//---------------------------------------------------------
void BitString::FormatString(char* Str)
{
 BITNUM  i;
 for (i=0; i<NumBits; ++i)
  Str[i]=Test(i) ? '1' : '0';

 Str[NumBits]=0;
}

//-------------------------------------------------------------------
// Create a new string with a representation of the bits (caller must
// delete.
//-------------------------------------------------------------------
char* BitString::FormatString()
{
 char *NewBuf = new char[NumBytes+1];
 FormatString(NewBuf);
 return NewBuf;
}

//------------------------------------------------------------------
// An Interator function to invoke caller's function for each bit =
// to value.
//------------------------------------------------------------------
void BitString::ForEach(void (*Func)(BITNUM ,void*), void* args,
                        bool Value)
{
 BITNUM  i;
 for (i=0; i<NumBits; ++i)
  if ( Test(i,Value) )
   Func(i,args);
}

//--------------------------------------------------------------------
// Scan string from the beginning and invoke caller's function for each
// bit that matches value. Caller's function should return true if it
// Satisfies the criteria, false otherwise.
// Returns UINT_MAX if no hit.
//-------------------------------------------------------------------
BITNUM  BitString::FirstThat(bool (*Func)(BITNUM ,void*), void* args,
                                  bool Value)
{
 BITNUM i;
 for (i=0; i<NumBits; ++i)
  if ( Test(i,Value) &&
       Func(i,args) )
   return i;

 return UINT_MAX;
}

//--------------------------------------------------------------------
// Scan string from the end and invoke caller's function for each
// bit that matches value. Caller's function should return true if it
// Satisfies the criteria, false otherwise.
// Returns UINT_MAX if no hit.
//-------------------------------------------------------------------
BITNUM  BitString::LastThat(bool (*Func)(BITNUM ,void*), void* args,
                                 bool Value)
{
 BITNUM i;
 for (i=NumBits-1; i != UINT_MAX; --i)
  if ( Test(i,Value) &&
       Func(i,args) )
   return i;

 return UINT_MAX;
}

BITNUM BitString::operator==(bool Value)
{
 BITNUM i;
 for (i=0; i<NumBits; ++i)
  if ( Test(i,Value) )
   return i;

 return UINT_MAX;
}

//#define TEST
#ifdef TEST
#include <iostream.h>
#include <conio.h>

void pt(BITNUM  Bit,void*)
{
 cout << Bit << " ";
}

bool ft(BITNUM Bit,void *)
{
 return true;
}

void main(void)
{
 BITNUM  i, x;
 char Buf[17];

 BitString B(16);
 for (i=0; i<16; i+=2 )
  B.Set(i);
 B.FormatString(Buf);
 cout << Buf << "\n";
 cout << "ForEach(true): ";
 B.ForEach(pt,NULL);
 cout << "\nForEach(false): ";
 B.ForEach(pt,NULL,false);
 cout << "\n";
 cout << "FirstThat(true): " << B.FirstThat(ft) << "\n";
 cout << "FirstThat(false): " << B.FirstThat(ft,0,false) << "\n";
 cout << "LastThat(true): " << B.LastThat(ft) << "\n";
 cout << "LastThat(false): " << B.LastThat(ft,0,false) << "\n";
 cout << "Overload[]: ";
 for (i=0; i<B.GetNumBits(); ++i)
  if (B[i] )
   cout << '1';
  else
   cout << '0';
 cout << "\n";
 cout << "Overload+=: ";
 for (i=0; i<B.GetNumBits(); ++i)
  B+=i;
 B.FormatString(Buf);
 cout << Buf << "\n";
 cout << "Overload-=: ";
 for (i=0; i<B.GetNumBits(); ++i)
  B-=i;
 B.FormatString(Buf);
 cout << Buf << "\n";
 getch();
 cout << "Overload==:\n";
 B.SetAll(true);
 while ( (i=(B==true)) != UINT_MAX )
  {
   B-=i;
   B.FormatString(Buf);
   cout << Buf << "\n";
  }

 getch();
}
#endif

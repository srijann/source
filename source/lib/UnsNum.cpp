//-----------------------------------------------------------------------
// UnsNum.cpp
//
// Conversion routines for Unsigned Binary Integers
// Converts Ascii to Integer and Integer to Ascii for all of the unsigned
//    binary data types, ie.,
//       unsigned long
//       unsigned int
//       unsigned short
//       unsigned char
//
// Supports two types of Unsigned integers:
//
//   Plain Numbers --  The number can be 0 to the MAX for the data type
//                     as defined in Limits.h. When converting ascii
//                     to Integer, empty or invalid strings are converted
//                     to zero. Integers greater than MAX are set to MAX.
//                     When converting Integer to Ascii the number is
//                     converted as is, from 0 to MAX.
//
//  Special Numbers -- The number can be 0 to MAX-1 with MAX reserved
//                     as an EMPTY indicator. When converting ascii
//                     to Integer, empty or invalid strings are converted
//                     to MAX. Integers greater than or equal to MAX are
//                     set equal to MAX-1. When converting Integer to
//                     Ascii, the value MAX will result in an empty
//                     string, otherwise the number is converted
//                     normally.
//
// The Plain number functions are the lower case ones.
// The Special number functions are the upper case ones.
//
// Note: developed for 32 bit environment only. 16 bit ignored. Change
//       unsigned int functions to call unsigned short instead of
//       long if porting to 16 bits.
//-----------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

//----------------------------------------
// Ascii to Long Integer Internal function
//----------------------------------------
static void _atou(char* String,unsigned long& Val,
                  unsigned long EmptyVal,
                  unsigned long MaxRealVal)
{
 unsigned long I;

 if ( sscanf(String,"%U",&I) != 1 )
  {
   Val=EmptyVal;
   return;
  }

 Val= (I==ULONG_MAX) ? MaxRealVal : I;
}

//-----------------------------------------
// Ascii to Short Integer Internal function
//-----------------------------------------
static void _atou(char* String,unsigned short& Val,unsigned short EmptyVal,
                  unsigned short MaxRealVal)
{
 unsigned long I;

 if ( sscanf(String,"%U",&I) != 1)
  {
   Val=EmptyVal;
   return;
  }

 Val = ( I >= USHRT_MAX ) ? MaxRealVal : (unsigned short)I;
}

//--------------------------------
// Ascii to char Internal function
//--------------------------------
static void _atou(char* String,unsigned char& Val,unsigned char EmptyVal,
                  unsigned char MaxRealVal)
{
 unsigned long I;

 if ( sscanf(String,"%U",&I) != 1)
  {
   Val=EmptyVal;
   return;
  }

 Val = ( I >= UCHAR_MAX ) ? MaxRealVal : (unsigned char)I;
}


//----------------------
// Ascii to long Special
//----------------------
void AtoU(char* String,unsigned long& Val)
{
 _atou(String,Val,ULONG_MAX,ULONG_MAX-1);
}

//----------------------------------------------------------
// Ascii to int Special (same as long in 32 bit environment)
//----------------------------------------------------------
void AtoU(char* String,unsigned int& Val)
{
 _atou(String,(unsigned long&)Val,ULONG_MAX,ULONG_MAX-1);
}

//-----------------------
// Ascii to short Special
//-----------------------
void AtoU(char* String,unsigned short& Val)
{
 _atou(String,Val,USHRT_MAX,USHRT_MAX-1);
}

//----------------------
// Ascii to char Special
//----------------------
void AtoU(char* String,unsigned char& Val)
{
 _atou(String,Val,UCHAR_MAX,UCHAR_MAX-1);
}

//--------------------
// Ascii to long Plain
//--------------------
void atou(char* String,unsigned long& Val)
{
 _atou(String,Val,0,ULONG_MAX);
}

//-------------------------------------------------
// Ascii to int Plain (same as long in 32 bit land)
//-------------------------------------------------
void atou(char* String,unsigned int& Val)
{
 _atou(String,(unsigned long&)Val,0,ULONG_MAX);
}

//---------------------
// Ascii to short Plain
//---------------------
void atou(char* String,unsigned short& Val)
{
 _atou(String,Val,0,USHRT_MAX);
}

//--------------------
// Ascii to char Plain
//--------------------
void atou(char* String,unsigned char& Val)
{
 _atou(String,Val,0,UCHAR_MAX);
}

//-------------------------------------
// Internal use function for utoa functions
//-------------------------------------
void _utoa(char* String,unsigned long Val,int Width)
{
 if ( Width )
  sprintf(String,"%0*.*u",Width,Width,Val);
 else
  sprintf(String,"%u",Val);
}

//----------------------
// long to Ascii Special
//----------------------
void UtoA(char* String,unsigned long Val,int Width)
{
 if ( Val==ULONG_MAX )
  String[0] = 0;
 else
  _utoa(String,Val,Width);
}

//-----------------------
// short to Ascii Special
//-----------------------
void UtoA(char* String,unsigned short Val,int Width)
{
 if ( Val==USHRT_MAX )
  String[0] = 0;
 else
  _utoa(String,Val,Width);
}

//---------------------
// int to Ascii Special
//---------------------
void UtoA(char* String,unsigned int Val,int Width)
{
 if ( Val==UINT_MAX )
  String[0] = 0;
 else
  _utoa(String,Val,Width);
}

//----------------------
// char to Ascii Special
//----------------------
void UtoA(char* String,unsigned char Val,int Width)
{
 if ( Val==UCHAR_MAX )
  String[0] = 0;
 else
  _utoa(String,Val,Width);
}

//--------------------
// long to Ascii Plain
//--------------------
void utoa(char* String,unsigned long Val,int Width)
{
 _utoa(String,Val,Width);
}

//---------------------
// short to Ascii Plain
//---------------------
void utoa(char* String,unsigned short Val,int Width)
{
 _utoa(String,Val,Width);
}

//-------------------
// int to Ascii Plain
//-------------------
void utoa(char* String,unsigned int Val,int Width)
{
 _utoa(String,Val,Width);
}

//--------------------
// char to Ascii Plain
//--------------------
void utoa(char* String,unsigned char Val,int Width)
{
 _utoa(String,Val,Width);
}

//----------------------------
// Test for Empty Special char
//----------------------------
bool isEmpty(unsigned char Val)
{
 return Val == UCHAR_MAX;
}

//-----------------------------
// Test for Empty Special short
//-----------------------------
bool isEmpty(unsigned short Val)
{
 return Val==USHRT_MAX;
}

//---------------------------
// Test for Empty Special int
//---------------------------
bool isEmpty(unsigned int Val)
{
 return Val==UINT_MAX;
}

//----------------------------
// Test for Empty Special long
//----------------------------
bool isEmpty(unsigned long Val)
{
 return Val==ULONG_MAX;
}

void Empty(unsigned long& Val)
{
 Val=ULONG_MAX;
}

void Empty(unsigned int& Val)
{
 Val=UINT_MAX;
}

void Empty(unsigned short& Val)
{
 Val=USHRT_MAX;
}

void Empty(unsigned char& Val)
{
 Val=UCHAR_MAX;
}

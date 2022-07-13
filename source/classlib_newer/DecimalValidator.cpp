//----------------------------------------------------------------------------
//  Project Classlib
//
//  Copyright © 1996. All Rights Reserved.
//
//  FILE:         DecimalValidator.cpp
//  AUTHOR:       Jack Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of DecimalValidator (TValidator)
//
//  A validator for decimal entry. Requires all numerics and allows
//  only one decimal point. Insures no more than max number of characters
//  provided by caller to the right and left of the decimal points.
//----------------------------------------------------------------------------

#include "win4me.h"
#pragma hdrstop
#include <owl/validate.h>
#include <ctype.h>
#include <string.h>
#define  SPLIB
#include "splib.h"
#include "DecimalValidator.h"

//------------------------------------------------------------------
// Indicate how many digits to the right and to the left are allowed
//------------------------------------------------------------------
TDecimalValidator::TDecimalValidator(int max_left,
                                     int max_right)
{
 MaxLeft=max_left;
 MaxRight=max_right;
}

//-----------------------------------------------------------------
// We get each keystroke and we get to say whether or not it's okay
//-----------------------------------------------------------------
bool TDecimalValidator::IsValidInput(char far* str,bool)
{
 register i, cnt, dp;

 for (i=dp=cnt=0; str[i] != 0; ++i)
  if ( isdigit(str[i]) )
   {
    ++cnt;
    if ( ! dp )
     {
      if ( cnt > MaxLeft )
        return false;
     }
    else
     if ( cnt > MaxRight )
       return false;
   }

  else
   if ( str[i] == '.' )
    {
     if ( dp )
       return false;
     else
      {
       dp=true;
       cnt=0;
      }
    }

   else
     return false;

 return true;
}

//----------------------------------------------------------------------------
//  Project Classlib
//
//  Copyright © 1996. All Rights Reserved.
//
//  FILE:         editvar.cpp
//  AUTHOR:       Jack Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of TEditVar::TEditVar (PlainEdit).
//
//  Makes a direct connection between a TEdit and a Variable, moving
//  it automatically from the Control to the Variable when the Control
//  value is changed.
//
//  Displays the Variable value when the TEdit is created. After that
//  you have to call the Display function to re-display the variable.
//
//  Call the transfer function to cause the control value to be moved
//  immediately to the variable.
//
//  Supports all standard data types and includes these Options:
//
//
//   EVOPT_ZEROISNULL  - Treat Zero as the NULL Value. Display a
//                       blank field instead of zero. If a field is
//                       blank, set it's variable to zero.
//
//   EVOPT_MAXISNULL   - Treat Max value for the unsigned type as NULL,
//                       (UINT_MAX,USHRT_MAX,UCHAR_MAX).
//
//   EVOPT_ZEROFILL    - Zero fill values to TextLimit when displayed.
//
//----------------------------------------------------------------------------
#include "win4me.h"

#include <stdio.h>
#include "editvar.h"

// Maximum length of a string representing a signed 32 bit number

#define MAXNUMBER (uint)(9+2)

//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(TEditVar, PlainEdit)
//{{TEditVarRSP_TBL_BEGIN}}
EV_WM_KILLFOCUS,
EV_WM_CHAR,
EV_WM_KEYDOWN,
//{{TEditVarRSP_TBL_END}}
END_RESPONSE_TABLE;

//{{TEditVar::TEditVar Implementation}}

TEditVar::TEditVar(TWindow* parent, unsigned char& Var,int id, uint textLimit,uint options)
 : PlainEdit(parent,id,textLimit),
   VarPtr(&Var),
   Options(options),
   Type(TYPE_UNSCHAR),
   setupComplete(false)
{
 transferBuf = new char[max(MAXNUMBER,textLimit)];
}

TEditVar::TEditVar(TWindow* parent, unsigned short& Var,int id, uint textLimit,uint options)
 : PlainEdit(parent,id,textLimit),
   VarPtr((unsigned char*)&Var),
   Options(options),
   Type(TYPE_UNSSHORT),
   setupComplete(false)
{
 transferBuf = new char[max(MAXNUMBER,textLimit)];
}

//---------------------------------------------------
// Transfer the Data from the Control to the Variable
//---------------------------------------------------
void TEditVar::transfer()
{
 Transfer(transferBuf,tdGetData);
 switch( Type )
  {
   case TYPE_UNSCHAR:
     if ( transferBuf[0] == 0 )
      {
       if ( Options & EVOPT_MAXISNULL )
        *((char*)VarPtr) = UCHAR_MAX;
       else
        *((char*)VarPtr) = 0;
      }
     else
      *((char*)VarPtr) = (char)atol(transferBuf);
     break;

   case TYPE_UNSSHORT:
     if ( transferBuf[0] == 0 )
      {
       if ( Options & EVOPT_MAXISNULL )
        *((unsigned short*)VarPtr) = USHRT_MAX;
       else
        *((unsigned short*)VarPtr) = 0;
      }
     else
      *((unsigned short*)VarPtr) = (unsigned short)atol(transferBuf);
     break;

   case TYPE_UNSINT:
     if ( transferBuf[0] == 0 )
      {
       if ( Options & EVOPT_MAXISNULL )
        *((unsigned int*)VarPtr) = USHRT_MAX;
       else
        *((unsigned int*)VarPtr) = 0;
      }
     else
      *((unsigned int*)VarPtr) = (unsigned int)atol(transferBuf);
     break;

   case TYPE_UNSLONG:
     if ( transferBuf[0] == 0 )
      {
       if ( Options & EVOPT_MAXISNULL )
        *((unsigned long*)VarPtr) = ULONG_MAX;
       else
        *((unsigned long*)VarPtr) = 0;
      }
     else
      *((unsigned long*)VarPtr) = (unsigned long)atol(transferBuf);
     break;

   case TYPE_CHAR:
     if ( transferBuf[0] == 0 )
      *((char*)VarPtr) = 0;
     else
      *((char*)VarPtr) = (char)atol(transferBuf);
     break;

   case TYPE_SHORT:
     if ( transferBuf[0] == 0 )
      *((short*)VarPtr) = 0;
     else
      *((short*)VarPtr) = (short)atol(transferBuf);
     break;

   case TYPE_INT:
     if ( transferBuf[0] == 0 )
      *((int*)VarPtr) = 0;
     else
      *((int*)VarPtr) = (int)atol(transferBuf);
     break;

   case TYPE_LONG:
     if ( transferBuf[0] == 0 )
      *((long*)VarPtr) = 0;
     else
      *((long*)VarPtr) = (long)atol(transferBuf);
    break;

   case TYPE_STRING:
     strcpy((char*)VarPtr,(const char*)transferBuf);
     break;
  }

}

TEditVar::TEditVar(TWindow* parent, unsigned int& Var,int id, uint textLimit,uint options)
 : PlainEdit(parent,id,textLimit),
   VarPtr((unsigned char*)&Var),
   Options(options),
   Type(TYPE_UNSINT),
   setupComplete(false)
{
 transferBuf = new char[max(MAXNUMBER,textLimit)];
}

TEditVar::TEditVar(TWindow* parent, unsigned long& Var,int id, uint textLimit,uint options)
 : PlainEdit(parent,id,textLimit),
   VarPtr((unsigned char*)&Var),
   Options(options),
   Type(TYPE_UNSLONG),
   setupComplete(false)
{
 transferBuf = new char[max(MAXNUMBER,textLimit)];
}

TEditVar::TEditVar(TWindow* parent, signed char& Var,int id, uint textLimit,uint options)
 : PlainEdit(parent,id,textLimit),
   VarPtr((unsigned char*)&Var),
   Options(options),
   Type(TYPE_CHAR),
   setupComplete(false)
{
 transferBuf = new char[max(MAXNUMBER,textLimit)];
}

TEditVar::TEditVar(TWindow* parent, short& Var,int id, uint textLimit,uint options)
 : PlainEdit(parent,id,textLimit),
   VarPtr((unsigned char*)&Var),
   Options(options),
   Type(TYPE_SHORT),
   setupComplete(false)
{
 transferBuf = new char[max(MAXNUMBER,textLimit)];
}

TEditVar::TEditVar(TWindow* parent, int& Var,int id, uint textLimit,uint options)
 : PlainEdit(parent,id,textLimit),
   VarPtr((unsigned char*)&Var),
   Options(options),
   Type(TYPE_INT),
   setupComplete(false)
{
 transferBuf = new char[max(MAXNUMBER,textLimit)];
}

TEditVar::TEditVar(TWindow* parent, long& Var,int id, uint textLimit,uint options)
 : PlainEdit(parent,id,textLimit),
   VarPtr((unsigned char*)&Var),
   Options(options),
   Type(TYPE_LONG),
   setupComplete(false)
{
 transferBuf = new char[max(MAXNUMBER,textLimit)];
}


TEditVar::TEditVar(TWindow* parent, char* Var,int id, uint textLimit)
 : PlainEdit(parent,id,textLimit),
   VarPtr((unsigned char*)Var),
   Type(TYPE_STRING),
   setupComplete(false)
{
 transferBuf = new char[textLimit];
}


TEditVar::~TEditVar()
{
  Destroy(IDCANCEL);
  delete[] transferBuf;
}


void TEditVar::Display()
{
 transferBuf[0] = 0;

 switch( Type )
  {
   case TYPE_UNSCHAR:
    if ( ! (((Options & EVOPT_MAXISNULL) && *VarPtr == UCHAR_MAX) ||
           ((Options & EVOPT_ZEROISNULL) && *VarPtr == 0)) )
     if ( Options & EVOPT_ZEROFILL )
      sprintf(transferBuf,"%0*u",TextLimit-1,*VarPtr);
     else
      sprintf(transferBuf,"%u",*VarPtr);
    break;

   case TYPE_UNSSHORT:
    if ( ! (((Options & EVOPT_MAXISNULL) &&
               *((unsigned short*)VarPtr) == USHRT_MAX) ||
           ((Options & EVOPT_ZEROISNULL) &&
               *((unsigned short*)VarPtr) == 0)) )
     if ( Options & EVOPT_ZEROFILL )
      sprintf(transferBuf,"%0*u",TextLimit-1,*((unsigned short*)VarPtr));
     else
      sprintf(transferBuf,"%u",*((unsigned short*)VarPtr));
    break;

   case TYPE_UNSINT:
    if ( ! (((Options & EVOPT_MAXISNULL) &&
               *((unsigned int*)VarPtr) == UINT_MAX) ||
           ((Options & EVOPT_ZEROISNULL) &&
               *((unsigned int*)VarPtr) == 0)) )
     if ( Options & EVOPT_ZEROFILL )
      sprintf(transferBuf,"%0*u",TextLimit-1,*((unsigned int*)VarPtr));
     else
      sprintf(transferBuf,"%u",*((unsigned int*)VarPtr));
    break;

   case TYPE_UNSLONG:
    if ( ! (((Options & EVOPT_MAXISNULL) &&
               *((unsigned int*)VarPtr) == ULONG_MAX) ||
           ((Options & EVOPT_ZEROISNULL) &&
               *((unsigned int*)VarPtr) == 0)) )
     if ( Options & EVOPT_ZEROFILL )
      sprintf(transferBuf,"%0*u",TextLimit-1,*((unsigned long*)VarPtr));
     else
     sprintf(transferBuf,"%u",*((unsigned long*)VarPtr));
    break;

   case TYPE_CHAR:
     if   ( ! ((Options & EVOPT_ZEROISNULL) &&
               *((signed char*)VarPtr) == 0)  )
     if ( Options & EVOPT_ZEROFILL )
      sprintf(transferBuf,"%0*d",TextLimit-1,*((signed char*)VarPtr));
     else
      sprintf(transferBuf,"%d",*((signed char*)VarPtr));
    break;

   case TYPE_SHORT:
     if   ( ! ((Options & EVOPT_ZEROISNULL) &&
               *((short*)VarPtr) == 0)  )
     if ( Options & EVOPT_ZEROFILL )
      sprintf(transferBuf,"%0*d",TextLimit-1,*((short*)VarPtr));
     else
      sprintf(transferBuf,"%d",*((short*)VarPtr));
     break;

   case TYPE_INT:
     if   ( ! ((Options & EVOPT_ZEROISNULL) &&
               *((int*)VarPtr) == 0)  )
     if ( Options & EVOPT_ZEROFILL )
      sprintf(transferBuf,"%0*d",TextLimit-1,*((int*)VarPtr));
     else
      sprintf(transferBuf,"%d",*((int*)VarPtr));
     break;

   case TYPE_LONG:
     if   ( ! ((Options & EVOPT_ZEROISNULL) &&
               *((long*)VarPtr) == 0)  )
     if ( Options & EVOPT_ZEROFILL )
      sprintf(transferBuf,"%0*d",TextLimit-1,*((long*)VarPtr));
     else
      sprintf(transferBuf,"%d",*((long*)VarPtr));
     break;

   case TYPE_STRING:
     strcpy(transferBuf,(const char*)VarPtr);
     break;
  }

 Transfer(transferBuf,tdSetData);
 setupComplete=true;
}


void TEditVar::SetupWindow()
{
 PlainEdit::SetupWindow();
 Display();
 ClearModify();
}

uint TEditVar::Sizeof()
{
 switch(Type)
  {
   case TYPE_UNSCHAR:
   case TYPE_CHAR:   return sizeof(char);

   case TYPE_UNSSHORT:
   case TYPE_SHORT:  return sizeof(short);

   case TYPE_UNSINT:
   case TYPE_INT:    return sizeof(int);

   case TYPE_UNSLONG:
   case TYPE_LONG:   return sizeof(long);

   case TYPE_STRING: return TextLimit;
  }

 return 0; 
}

void TEditVar::EvKillFocus(HWND hWndGetFocus)
{
 PlainEdit::EvKillFocus(hWndGetFocus);
 transfer();
}

void TEditVar::EvChar(uint key, uint repeatCount, uint flags)
{
 PlainEdit::EvChar(key,repeatCount,flags);
 transfer();
}

void TEditVar::EvKeyDown(uint key, uint repeatCount, uint flags)
{
 PlainEdit::EvKeyDown(key,repeatCount,flags);
 transfer();
}



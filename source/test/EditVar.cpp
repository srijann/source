//----------------------------------------------------------------------------
//  Project SpsManager
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    SpsManager.apx Application
//  FILE:         editvar.cpp
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of TEditVar::TEditVar (TEdit).
//
//----------------------------------------------------------------------------
#include <owl/pch.h>
//#include "win4me.h"

#include <stdio.h>
#include "editvar.h"

// Maximum length of a string representing a signed 32 bit number

#define MAXNUMBER 9+2

//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(TEditVar, TEdit)
//{{TEditVarRSP_TBL_BEGIN}}
  EV_WM_CHAR,
//{{TEditVarRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{TEditVar::TEditVar Implementation}}

TEditVar::TEditVar(TWindow* parent, unsigned char& Var,int id, uint textLimit,uint options)
 : TEdit(parent,id,textLimit),
   VarPtr(&Var),
   Options(options),
   Type(TYPE_UNSCHAR),
   setupComplete(false)
{
 transferBuf = new char[max(MAXNUMBER,textLimit)];
}

TEditVar::TEditVar(TWindow* parent, unsigned short& Var,int id, uint textLimit,uint options)
 : TEdit(parent,id,textLimit),
   VarPtr((unsigned char*)&Var),
   Options(options),
   Type(TYPE_UNSSHORT),
   setupComplete(false)
{
 transferBuf = new char[max(MAXNUMBER,textLimit)];
}


void TEditVar::EvChar(uint key, uint repeatCount, uint flags)
{
 TEdit::EvChar(key,repeatCount,flags);

 Transfer(transferBuf,tdGetData);
 switch( Type )
  {
   case TYPE_UNSCHAR:
     if ( transferBuf[0] == 0 )
      {
       if ( Options & EVAR_MAXISNULL )
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
       if ( Options & EVAR_MAXISNULL )
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
       if ( Options & EVAR_MAXISNULL )
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
       if ( Options & EVAR_MAXISNULL )
        *((unsigned long*)VarPtr) = USHRT_MAX;
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
 : TEdit(parent,id,textLimit),
   VarPtr((unsigned char*)&Var),
   Options(options),
   Type(TYPE_UNSINT),
   setupComplete(false)
{
 transferBuf = new char[max(MAXNUMBER,textLimit)];
}

TEditVar::TEditVar(TWindow* parent, unsigned long& Var,int id, uint textLimit,uint options)
 : TEdit(parent,id,textLimit),
   VarPtr((unsigned char*)&Var),
   Options(options),
   Type(TYPE_UNSLONG),
   setupComplete(false)
{
 transferBuf = new char[max(MAXNUMBER,textLimit)];
}

TEditVar::TEditVar(TWindow* parent, signed char& Var,int id, uint textLimit,uint options)
 : TEdit(parent,id,textLimit),
   VarPtr((unsigned char*)&Var),
   Options(options),
   Type(TYPE_CHAR),
   setupComplete(false)
{
 transferBuf = new char[max(MAXNUMBER,textLimit)];
}

TEditVar::TEditVar(TWindow* parent, short& Var,int id, uint textLimit,uint options)
 : TEdit(parent,id,textLimit),
   VarPtr((unsigned char*)&Var),
   Options(options),
   Type(TYPE_SHORT),
   setupComplete(false)
{
 transferBuf = new char[max(MAXNUMBER,textLimit)];
}

TEditVar::TEditVar(TWindow* parent, int& Var,int id, uint textLimit,uint options)
 : TEdit(parent,id,textLimit),
   VarPtr((unsigned char*)&Var),
   Options(options),
   Type(TYPE_INT),
   setupComplete(false)
{
 transferBuf = new char[max(MAXNUMBER,textLimit)];
}

TEditVar::TEditVar(TWindow* parent, long& Var,int id, uint textLimit,uint options)
 : TEdit(parent,id,textLimit),
   VarPtr((unsigned char*)&Var),
   Options(options),
   Type(TYPE_LONG),
   setupComplete(false)
{
 transferBuf = new char[max(MAXNUMBER,textLimit)];
}


TEditVar::TEditVar(TWindow* parent, char* Var,int id, uint textLimit)
 : TEdit(parent,id,textLimit),
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


void TEditVar::SetupWindow()
{
 TEdit::SetupWindow();

 transferBuf[0] = 0;

 switch( Type )
  {
   case TYPE_UNSCHAR:
    if ( ! (((Options & EVAR_MAXISNULL) && *VarPtr == UCHAR_MAX) ||
           ((Options & EVAR_ZEROISNULL) && *VarPtr == 0)) )
      sprintf(transferBuf,"%u",*VarPtr);
    break;

   case TYPE_UNSSHORT:
    if ( ! (((Options & EVAR_MAXISNULL) &&
               *((unsigned short*)VarPtr) == USHRT_MAX) ||
           ((Options & EVAR_ZEROISNULL) &&
               *((unsigned short*)VarPtr) == 0)) )
     sprintf(transferBuf,"%u",*((unsigned short*)VarPtr));
    break;

   case TYPE_UNSINT:
    if ( ! (((Options & EVAR_MAXISNULL) &&
               *((unsigned int*)VarPtr) == UINT_MAX) ||
           ((Options & EVAR_ZEROISNULL) &&
               *((unsigned int*)VarPtr) == 0)) )
     sprintf(transferBuf,"%u",*((unsigned int*)VarPtr));
    break;

   case TYPE_UNSLONG:
    if ( ! (((Options & EVAR_MAXISNULL) &&
               *((unsigned int*)VarPtr) == ULONG_MAX) ||
           ((Options & EVAR_ZEROISNULL) &&
               *((unsigned int*)VarPtr) == 0)) )
     sprintf(transferBuf,"%u",*((unsigned long*)VarPtr));
    break;

   case TYPE_CHAR:
     if   ( ! ((Options & EVAR_ZEROISNULL) &&
               *((signed char*)VarPtr) == 0)  )
      sprintf(transferBuf,"%d",*((signed char*)VarPtr));
    break;

   case TYPE_SHORT:
     if   ( ! ((Options & EVAR_ZEROISNULL) &&
               *((short*)VarPtr) == 0)  )
      sprintf(transferBuf,"%d",*((short*)VarPtr));
     break;

   case TYPE_INT:
     if   ( ! ((Options & EVAR_ZEROISNULL) &&
               *((int*)VarPtr) == 0)  )
      sprintf(transferBuf,"%d",*((int*)VarPtr));
     break;

   case TYPE_LONG:
     if   ( ! ((Options & EVAR_ZEROISNULL) &&
               *((long*)VarPtr) == 0)  )
      sprintf(transferBuf,"%d",*((long*)VarPtr));
     break;

   case TYPE_STRING:
     strcpy(transferBuf,(const char*)VarPtr);
     break;
  }

 Transfer(transferBuf,tdSetData);
 setupComplete=true;
}




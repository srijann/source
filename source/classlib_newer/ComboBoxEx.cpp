//----------------------------------------------------------------------------
//  Project combotest
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    combotest.apx Application
//  FILE:         comboboxex.cpp
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of TComboBoxEx (TComboBox).
//
//  Provides the following enhancements to the TComboBox Class
//
//    1. Causes the list to drop automatically whenever the Control gets
//       the focus.
//    2. SpaceBar toggles the LIST UP and DOWN.
//    2. Treats first character of each String as a selection Hot Key
//    3. Supports Clear function (Windows doesn't allow it for Drop
//         Down List Boxes for some unknown reason).
//    4. Supports selection by Associated Data Value.
//----------------------------------------------------------------------------
#include "win4me.h"
#pragma hdrstop

#include <string.h>
#include <ctype.h>
#include "ComboBoxEx.h"

//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(TComboBoxEx, TComboBox)
//{{TComboBoxExRSP_TBL_BEGIN}}
  EV_WM_SETFOCUS,
  EV_WM_CHAR,
//{{TComboBoxExRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{TComboBoxEx Implementation}}


//------------
// Constructor
//------------
TComboBoxEx::TComboBoxEx(TWindow* parent, int id,TModule* Module)
:
 TComboBox(parent,id,0,Module),
 DefaultValue(-1),
 GetStringBufSize(80)
{
 GetStringBuf = new char[GetStringBufSize+1];
}

//-----------
// Destructor
//-----------
TComboBoxEx::~TComboBoxEx()
{
 delete[] GetStringBuf;
 Destroy();
}

//-----------------------------------------
// Add a String AND a DATA item to the list
//-----------------------------------------
uint TComboBoxEx::AddItem(char* String,int Data)
{
 int i=AddString(String);
 SetItemData(i,Data);
 return i;
}

//---------------------------------------------
// Drop down the list whenever we get the FOCUS
//---------------------------------------------
void TComboBoxEx::EvSetFocus(THandle hWndLostFocus )
{
 //ShowList(true);
 TComboBox::EvSetFocus(hWndLostFocus );
}

//----------------------------------------------------
// Select an ITEM by the First character in the String
//----------------------------------------------------
bool TComboBoxEx::Select(char FirstChar)
{
 int Num=GetCount();
 int i;

 // Unfortunately we have to get the whole string because the
 // class gives us no access to the strings once they are stored.
 for (i=0; i<Num; ++i)
  {
   if ( GetStringLen(i) > GetStringBufSize )
    {
     delete[] GetStringBuf;
     GetStringBuf=new char[ (GetStringBufSize=GetStringLen(i))+1 ];
    }
   GetString(GetStringBuf,i);
   if ( FirstChar == toupper(GetStringBuf[0]) )
    {
     SetSelIndex(i);
     return true;
    }
  }

 return false;
}

//------------------------------------
// Select an ITEM using the DATA value
//------------------------------------
bool TComboBoxEx::Select(int Data)
{
 int Num=GetCount();
 int i;

 for (i=0; i<Num; ++i)
  if ( GetItemData(i) == (uint) Data )
   {
    SetSelIndex(i);
    return true;
   }

 return false;
}

//------------------------------------------------------------
// Get DATA Value of Selected Item. Return DEFAULT value if no
// Selection.
//------------------------------------------------------------
bool TComboBoxEx::GetSelectedValue(int& Data)
{
 int i=GetSelIndex();

 if ( i<0 )
  {
   Data=DefaultValue;
   return false;
  }
 else
  {
   Data=GetItemData(i);
   return true;
  }
}

//------------------------------------
// Handle potential HOTKEY or SPACEBAR
//------------------------------------
void TComboBoxEx::EvChar(uint key, uint repeatCount, uint flags)
{
 // Any Character Keyed into this field will drop the list unless
 // it is one of the selection characters.
 /*
 char ch = (char)toupper((char)key);

 if ( key != VK_ESCAPE )
   if ( Select(ch) )
     ShowList(false);
   else
     ShowList(true);
 */      

 TComboBox::EvChar(key,repeatCount,flags);
}

//----------------------------------------------------------------
// Set the DEFAULT value to be returned when there is no selection
// Defaults to Zero unless this is called.
//----------------------------------------------------------------
void TComboBoxEx::SetDefaultValue(int Value)
{
 DefaultValue=Value;
}

//----------------------------------------------------------------------
// Tacky WINDOWS Horse-shit. For some reason Windows doesn't allow you to
// clear a Combo Box when it's a DROP DOWN LIST BOX.
//
// Why?
//
// Apparently everyone on the planet knows that you should never clear
// the EDIT field on a DROP DOWN LIST BOX but I've been
// programming for 20 years and I'm not as smart as I used to be.
//
// It took me about 6 hours to figure this out but here I let you
// people that are as dumb as me actually do the Clear function on MY
// Combo Box. It REALLY works too.
//
// This overrides the normal Clear function which doesn't work for drop
// down list boxes.
//----------------------------------------------------------------------
void TComboBoxEx::Clear()
{
 SetSelIndex(-1);
}



//----------------------------------------------------------------------------
//  Project Classlib
//
//  Copyright © 1996. All Rights Reserved.
//
//  FILE:         levelselect.cpp
//  AUTHOR:       Jack Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of TLevelSelect (TEdit).
//
//  This is an Edit control that acts like a Button. When you click it,
//  it changes values. It's used by SPSMANAGER to handle the Rule
//  Enablers.
//----------------------------------------------------------------------------
#include "win4me.h"
#pragma hdrstop
#include "levelselect.h"
//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(TLevelSelect, TEdit)
//{{TLevelSelectRSP_TBL_BEGIN}}
  EV_WM_LBUTTONDBLCLK,
  EV_WM_LBUTTONDOWN,
  EV_WM_RBUTTONDOWN,
  EV_WM_RBUTTONDBLCLK,
  EV_WM_CHAR,
//{{TLevelSelectRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{TLevelSelect Implementation}}


//---------------------------------------------------------
// Provide an array of selections when creating the control
//---------------------------------------------------------
TLevelSelect::TLevelSelect(TWindow* parent, int id,char* Selections,
                           TModule* module)
:
 TEditUns(parent, id, 2, module),
 Selection(0)
{
 SetCursor(0,IDC_ARROW);
 SelectionString=new char[strlen(Selections)+1];
 strcpy(SelectionString,Selections);
 MaxLevel=strlen(SelectionString)-1;
}


TLevelSelect::~TLevelSelect()
{
  Destroy(IDCANCEL);
  delete[] SelectionString;
}

//-----------------------------------------
// Handle Double Click Same as single Click
//------------------------------------------
void TLevelSelect::EvLButtonDblClk(uint modKeys,TPoint& point)
{
 EvLButtonDown(modKeys, point);
}

//-------------------------------------------------
// Handle click by iterating through the selections
//-------------------------------------------------
void TLevelSelect::EvLButtonDown(uint,TPoint&)
{
 SetSel();
 if ( ++Selection > MaxLevel )
  Selection=0;
 SetLevel(Selection);
}

//----------------------------------
// Set Level using zero-based offset
//----------------------------------
void TLevelSelect::SetLevel(int Level)
{
 char TransBuf[2];

 Selection=min(Level,MaxLevel);
 TransBuf[0]=SelectionString[Selection];
 TransBuf[1]=0;
 Transfer(TransBuf,tdSetData);
}

//--------------------------
// Set Level using Character
//--------------------------
void TLevelSelect::SetLevel(unsigned char Level)
{
 char *Ptr=strchr(SelectionString,Level);

 if ( Ptr )
  Selection=Ptr-SelectionString;
 else
  Selection=0;
 SetLevel(Selection);
}


//-----------------------------------------------
// Handle the right button by iterating backwards
//-----------------------------------------------
void TLevelSelect::EvRButtonDown(uint,TPoint&)
{
 SetSel();
 if ( --Selection <0 )
  Selection=MaxLevel;
 SetLevel(Selection);
}


//---------------------------------------------------
// Handle double right click the same as single click
//---------------------------------------------------
void TLevelSelect::EvRButtonDblClk(uint modKeys, TPoint& point)
{
 EvRButtonDown(modKeys, point);
}

//------------------------
// Set the Selection Value
//------------------------
void TLevelSelect::SetSel()
{
 char Buf[2];
 char *Ptr;
 Transfer(Buf,tdGetData);

 if ( (Ptr=strchr(SelectionString,Buf[0])) == NULL )
  Selection=0;
 else
  Selection=Ptr-SelectionString;
}


//--------------------------------------------------------
// Ignore any key strokes that aren't in the selection set
//--------------------------------------------------------
void TLevelSelect::EvChar(uint key, uint repeatCount, uint flags)
{
 if ( strchr(SelectionString,key) != NULL )
   TEdit::EvChar(key, repeatCount, flags);
}

//---------------------------------------
// Return the Current Level to the caller
//---------------------------------------
int TLevelSelect::GetLevel(unsigned char* Value)
{
 if ( Value )
  *Value=SelectionString[Selection];
 return Selection;
}


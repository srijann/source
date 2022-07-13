//----------------------------------------------------------------------------
//  Project Classlib
//
//  Copyright © 1996. All Rights Reserved.
//
//  FILE:         CalcMenuPos.cpp
//  AUTHOR:       Jack Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of CalcMenuPos
//
//  Note: This is not an object. It's just a plain old function.
//  Calculates where to pop up a menu in relation to a window.
//
//  It's used by the TEditStateCode class to figure out where to
//  pop up the state code list.
//----------------------------------------------------------------------------

#include "win4me.h"

void CalcMenuPos(TPoint& Point,TWindow* Win,TMenu* Menu,bool Over)
{
 static int CursorHeight = 0;
 static int MenuItemHeight;
 int Count = Menu->GetMenuItemCount();

 // Get these Metrics the first time through only
 if ( CursorHeight==0 )
  {
   CursorHeight = GetSystemMetrics(SM_CYCURSOR);
   MenuItemHeight = GetSystemMetrics(SM_CYMENU);
  }

 // We know the cursor is in the EDIT field so we calculate where to pop
 // up the menu based on the cursor position
 GetCaretPos(&Point);
 ClientToScreen(Win->HWindow,&Point);

 // If it's Under just adjust down by the cursor height
 if ( ! Over )
   Point.y += CursorHeight / 2;

 // If it's over adjust up according to how many items and the
 // height of the cursor (The Count/2 makes some kind of adjustment
 // I don't understand. When there's a lot of items, the menu can
 // be positioned down too far. Will have to figure this out later.)
 else
   Point.y -= ( CursorHeight / 2 +
                (MenuItemHeight) * Count + Count /2);

}

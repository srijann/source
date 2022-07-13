#include <owl/pch.h>

static void set(TWindow* Win,void* CurPtr)
{
 Win->SetCursor(0,*(TResId *)CurPtr);
}

void SetCursors(TWindow* Window,TResId Cursor)
{
 Window->ForEach(set,&Cursor);
 Window->SetCursor(0,Cursor);
}
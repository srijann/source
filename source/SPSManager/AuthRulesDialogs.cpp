//----------------------------------------------------------------------------
//  Project spsmanager
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    spsmanager.apx Application
//  FILE:         authrulesdialogs.cpp
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of AuthRulesDialogs (TWindow).
//
//----------------------------------------------------------------------------
#include "Win4me.h"
#include "authrulesdialogs.h"


//{{AuthRulesDialogs Implementation}}


AuthRulesDialogs::AuthRulesDialogs(TWindow* parent, const char far* title, TModule* module)
:
    TWindow(parent, title, module)
{
  // Override the default window style for TWindow.
  //
      Attr.Style |= WS_CHILD;
    Attr.Style &= ~(WS_VISIBLE);

 Manager = new DialogManager();
}


AuthRulesDialogs::~AuthRulesDialogs()
{
 Destroy(IDCANCEL);
 delete Manager;
}

void DialogManager::Tab(int Sel)
{
 CurrentDialog->Show(SW_HIDE);
 switch( Sel )
 {
  case 0: CurrentDialog=RulesWin; break;
  case 1: CurrentDialog=DateTimeWin; break;
  case 2: CurrentDialog=ProductWin; break;
 }

 CurrentDialog->Show(SW_SHOW);
 CurrentDialog->SetFocus();
}


//----------------------------------------------------------------------------
//  Project Test
//  
//  Copyright © 1997. All Rights Reserved.
//
//  SUBSYSTEM:    Test Application
//  FILE:         tstdlgcl.cpp
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of TTestDlgClient (TDialog).
//
//----------------------------------------------------------------------------

#include <owl/pch.h>

#include "testapp.h"
#include "tstdlgcl.h"
#include "EditVar.h"


//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(TTestDlgClient, TDialog)
//{{TTestDlgClientRSP_TBL_BEGIN}}
  EV_BN_CLICKED(IDOK, BNClicked),
//{{TTestDlgClientRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{TTestDlgClient Implementation}}

TEditVar* T;

signed char sc;
unsigned char uc;
short ss;
unsigned short us;
int si;
unsigned int ui;
long sl;
unsigned long ul;
char str[50];

void TTestDlgClient::BNClicked()
{
}

TTestDlgClient::TTestDlgClient(TWindow* parent, TResId resId, TModule* module)
:
  TDialog(parent, resId, module)
{
 ui=0;
 T=new TEditVar(this,ui,101,20,EVAR_MAXISNULL);
}


TTestDlgClient::~TTestDlgClient()
{
  Destroy();

  // INSERT>> Your destructor code here.
}



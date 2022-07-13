//----------------------------------------------------------------------------
//  Project LogSearch
//  Secure Payment Systems
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    LogSearch Application
//  FILE:         logsearchdlgclient.cpp
//  AUTHOR:       Jack Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of TLogSearchDlgClient (TDialog).
//
//----------------------------------------------------------------------------

#include "win4me.h"

#include "logsearchapp.h"
#include "logsearchdlgclient.h"


//{{TLogSearchDlgClient Implementation}}


//--------------------------------------------------------
// TLogSearchDlgClient
// ~~~~~~~~~~
// Construction/Destruction handling.
//
TLogSearchDlgClient::TLogSearchDlgClient(TWindow* parent, TResId resId, TModule* module)
:
  TDialog(parent, resId, module)
{
}


TLogSearchDlgClient::~TLogSearchDlgClient()
{
  Destroy();

  // INSERT>> Your destructor code here.
}

void TLogSearchDlgClient::SetupWindow()
{
 TDialog::SetupWindow();
}


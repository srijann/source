//----------------------------------------------------------------------------
//  Project KDSTest
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    KDSTest Application
//  FILE:         kdstestdlgclient.cpp
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of TKDSTestDlgClient (TDialog).
//
//----------------------------------------------------------------------------

#include "win4me.h"

#include "kdstestapp.h"
#include "kdstestdlgclient.h"


//{{TKDSTestDlgClient Implementation}}


//--------------------------------------------------------
// TKDSTestDlgClient
// ~~~~~~~~~~
// Construction/Destruction handling.
//
TKDSTestDlgClient::TKDSTestDlgClient(TWindow* parent, TResId resId, TModule* module)
:
  TDialog(parent, resId, module)
{
  // INSERT>> Your constructor code here.
}


TKDSTestDlgClient::~TKDSTestDlgClient()
{
  Destroy();

  // INSERT>> Your destructor code here.
}

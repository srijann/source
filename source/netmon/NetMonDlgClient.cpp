//----------------------------------------------------------------------------
//  Project NetMon
//  Secure Payment Systems
//  Copyright © 1997. All Rights Reserved.
//
//  SUBSYSTEM:    NetMon Application
//  FILE:         netmondlgclient.cpp
//  AUTHOR:       Jack H. Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of TNetMonDlgClient (TDialog).
//
//----------------------------------------------------------------------------

#include "win4me.h"

#include "netmonapp.h"
#include "netmondlgclient.h"


//{{TNetMonDlgClient Implementation}}


//--------------------------------------------------------
// TNetMonDlgClient
// ~~~~~~~~~~
// Construction/Destruction handling.
//
TNetMonDlgClient::TNetMonDlgClient(TWindow* parent, TResId resId, TModule* module)
:
  TDialog(parent, resId, module)
{
  // INSERT>> Your constructor code here.
}


TNetMonDlgClient::~TNetMonDlgClient()
{
  Destroy();

  // INSERT>> Your destructor code here.
}

//------------------------------------------
// Allow No Means of Escape from this Dialog
//------------------------------------------
TResult TNetMonDlgClient::EvCommand(uint id, THandle hWndCtl, uint notifyCode)
{
 TResult result;
  switch( notifyCode )
   {
    case 0:
       if ( id==IDCANCEL || id==IDOK )
         return 0;
      break;
    default:
      break;
   }

  result = TDialog::EvCommand(id, hWndCtl, notifyCode);
  return result;
}


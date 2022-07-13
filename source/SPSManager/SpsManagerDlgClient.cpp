//----------------------------------------------------------------------------
//  Project SpsManager
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    SpsManager Application
//  FILE:         spsmanagerdlgclient.cpp
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of TSpsManagerDlgClient (TDialog).
//
//  This is a DO-nothing Dialog. We need it for OWL.
//----------------------------------------------------------------------------

#include "win4me.h"
#pragma hdrstop
#include "spsmanagerapp.h"
#include "spsmanagerdlgclient.h"

// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(TSpsManagerDlgClient, TDialog)
//{{TSpsManagerDlgClientRSP_TBL_BEGIN}}
//{{TSpsManagerDlgClientRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{TSpsManagerDlgClient Implementation}}


//--------------------------------------------------------
// TSpsManagerDlgClient
// ~~~~~~~~~~
// Construction/Destruction handling.
//
TSpsManagerDlgClient::TSpsManagerDlgClient(TWindow* parent, TResId resId, TModule* module)
:
  TDialog(parent, resId, module)
{
}


TSpsManagerDlgClient::~TSpsManagerDlgClient()
{
 Destroy();
}

//----------------------------------------------------------
// Override EvCommand to prevent ESCape from the main dialog
//----------------------------------------------------------
TResult TSpsManagerDlgClient::EvCommand(uint id, THandle hWndCtl, uint notifyCode)
{
  TResult result;
  switch( notifyCode )
   {
    case 0:
      if ( id==IDCANCEL || id==IDOK )
       // Don't Allow ESCAPE. Make'em go through the menu.
        return 0;
      break;
    default:
      break;
   }

  result = TDialog::EvCommand(id, hWndCtl, notifyCode);
  return result;
}



void TSpsManagerDlgClient::SetupWindow()
{
 TDialog::SetupWindow();
}




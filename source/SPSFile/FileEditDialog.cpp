//----------------------------------------------------------------------------
//  Project PosFileMan
//  Secure Payment Systems
//  Copyright © 1997. All Rights Reserved.
//
//  SUBSYSTEM:    PosFileMan Application
//  FILE:         posfilemandlgclient.cpp
//  AUTHOR:       Jack Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of FileEditDialog (TDialog).
//
//----------------------------------------------------------------------------

#include "win4me.h"

#include <stdio.h>
#include <time.h>
#include "FileEditDialog.h"
#include "PosFileManApp.rh"

//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(FileEditDialog, TDialog)
//{{FileEditDialogRSP_TBL_BEGIN}}
  EV_COMMAND_ENABLE(CM_GETRECORD, CeGetrecord),
  EV_COMMAND_ENABLE(CM_FILEDELETE, CeFiledelete),
  EV_COMMAND_ENABLE(CM_FILECHANGE, CeFilechange),
  EV_COMMAND_ENABLE(CM_FILEADD, CeFileadd),
  EV_COMMAND(CM_EDITCLEARALL, CmEditclearall),
  EV_COMMAND_ENABLE(CM_EDITCLEARALL, CeEditclearall),
  EV_COMMAND(CM_GETRECORD, CmGetrecord),
  EV_COMMAND(CM_FILEADD, CmFileadd),
  EV_COMMAND(CM_FILECHANGE, CmFilechange),
  EV_COMMAND(CM_FILEDELETE, CmFiledelete),
//{{FileEditDialogRSP_TBL_END}}
END_RESPONSE_TABLE;

//--------------------------------------------------------
// FileEditDialog
// ~~~~~~~~~~
// Construction/Destruction handling.
//
FileEditDialog::FileEditDialog(TWindow* parent, TResId resId)
:
  TDialog(parent, resId)
{
}

//-----------------
// Class Destructor
//-----------------
FileEditDialog::~FileEditDialog()
{
  Destroy();
}


//-----------------
// Clear all fields
//-----------------
void FileEditDialog::CmEditclearall()
{
}

//------------------
// Clear All enabler
//------------------
void FileEditDialog::CeEditclearall(TCommandEnabler &tce)
{
 tce.Enable(false);
}

//----------------
// Command Handler
//----------------
TResult FileEditDialog::EvCommand(uint id, THandle hWndCtl, uint notifyCode)
{
  TResult result;

  switch ( notifyCode )
   {
    case 0:
       if ( id==IDCANCEL || id==IDOK )
         return 0;
      break;

    case EN_CHANGE:  ChangeHandler(id);
                     break;
   }

  result = TDialog::EvCommand(id, hWndCtl, notifyCode);
  return result;
}

//------------------------
// Handle change to fields
//------------------------
void FileEditDialog::ChangeHandler(uint)
{
}


//---------------
// Add the record
//---------------
void FileEditDialog::CmFileadd()
{
}

//-------------------
// Get record handler
//-------------------
void FileEditDialog::CmGetrecord()
{
}

//-------------------------
// Change (Update) a record
//-------------------------
void FileEditDialog::CmFilechange()
{
}

//----------------
// Delete a record
//----------------
void FileEditDialog::CmFiledelete()
{
}


//-----------------
// Get a New record
//-----------------
void FileEditDialog::CeGetrecord(TCommandEnabler &tce)
{
 tce.Enable( false );
}

//----------------------
// Delete record enabler
//----------------------
void FileEditDialog::CeFiledelete(TCommandEnabler &tce)
{
 tce.Enable( false );
}

//----------------------
// Change record enabler
//----------------------
void FileEditDialog::CeFilechange(TCommandEnabler &tce)
{
 tce.Enable( false );
}

//-------------------
// Add record enabler
//-------------------
void FileEditDialog::CeFileadd(TCommandEnabler &tce)
{
 tce.Enable( false );
}


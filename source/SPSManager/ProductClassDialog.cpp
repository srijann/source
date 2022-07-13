//----------------------------------------------------------------------------
//  Project SpsManager
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    SpsManager.apx Application
//  FILE:         productclassdialog.cpp
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of ProductClassDialog (TDialog).
//
//----------------------------------------------------------------------------
#include "win4me.h"

#include "productclassdialog.h"
#include "SaveFile.h"

#define NUMCLASSES 12
#define NUMSUBCLASSES 12
#define TEXTMAX 20

//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(ProductClassDialog, TDialog)
//{{ProductClassDialogRSP_TBL_BEGIN}}
  EV_TVN_ENDLABELEDIT(IDC_TREEVIEW1, TVNEndlabeledit),
  EV_TVN_BEGINLABELEDIT(IDC_TREEVIEW1, TVNBeginlabeledit),
//{{ProductClassDialogRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{ProductClassDialog Implementation}}


ProductClassDialog::ProductClassDialog(TWindow* parent, ProductClassList* origList)
:
  FormDialog(parent, IDD_PRODCLASS, RecTypeProductClasses),
  OrigList(origList),
  SetupComplete(false),
  editInProgress(false)
{
 TreeWin = new TTreeWindow(this,IDC_TREEVIEW1);
 newList = new ProductClassList(origList);
}

ProductClassDialog::~ProductClassDialog()
{
  Destroy(IDCANCEL);
  delete OrigList;
}


void ProductClassDialog::SetupWindow()
{
 TDialog::SetupWindow();
 newList->InitMaintView(TreeWin);
}


void ProductClassDialog::TVNEndlabeledit(TTwDispInfoNotify& twdin)
{
 if ( ! Escape )
   newList->TreeTextChange(TreeWin,twdin.item.pszText);
 editInProgress=false;
}

TResult ProductClassDialog::EvCommand(uint id, THandle hWndCtl, uint notifyCode)
{
 TResult result;
  switch( notifyCode )
   {
    case 0:
      // We provide Enter and Escape key functionality during
      // Label Editing here
       switch(id)
        {
         case IDCANCEL:
           if ( editInProgress )
            {
             Escape=true;
             TreeWin->PostMessage(TVM_ENDEDITLABELNOW,true,0);
            }
          return 0;

         case IDOK:
           if ( editInProgress )
            TreeWin->PostMessage(TVM_ENDEDITLABELNOW,false,0);
           else
            TreeWin->PostMessage(TVM_EDITLABEL,0,
                        (LPARAM)TreeWin->GetSelection().GetHTreeItem());
          return 0;

         default:
          break;
        }
      break;

    default:
      break;
   }

  result = TDialog::EvCommand(id, hWndCtl, notifyCode);
  return result;
}


bool ProductClassDialog::TVNBeginlabeledit(TTwDispInfoNotify&)
{
 Escape=false;
 editInProgress=true;
 return false;
}

//----------------------
// Query modified status
//----------------------
bool ProductClassDialog::IsModified()
{
 return newList->isSame(OrigList) == false;
}

//-------------------------
// Save Changes to DataBase
//-------------------------
bool ProductClassDialog::Save()
{
 if ( newList->Save() )
  {
   ResetModified();
   return true;
  }
 else
  return false;  
}

//--------------------------
// Save changes to temp file
//--------------------------
bool ProductClassDialog::SaveTemp(TFile* File)
{
 SAVERECORD_SENTINEL Sentinel;

 Sentinel.Type = GetType();
 Sentinel.Version = SaveRecordVersion[Sentinel.Type];
 Sentinel.Len =newList->GetLength();
 
 if ( File->Write(&Sentinel,sizeof(Sentinel)) != sizeof(Sentinel) ||
      ! newList->Save(File) )
  return false;

 ResetModified();
 return true;
}

//------------------------
// Reset the modified flag
//------------------------
void ProductClassDialog::ResetModified()
{
 delete OrigList;
 OrigList = newList;
 newList = new ProductClassList(OrigList);
 FormDialog::ResetModified();
}




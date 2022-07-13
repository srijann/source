//----------------------------------------------------------------------------
//  Project spsmanager
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    spsmanager.apx Application
//  FILE:         openclassdialog.cpp
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of OpenClassDialog (TDialog).
//
//----------------------------------------------------------------------------
#include "win4me.h"
#pragma hdrstop
#include "openclassdialog.h"

//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(OpenClassDialog, TDialog)
//{{OpenClassDialogRSP_TBL_BEGIN}}
  EV_BN_CLICKED(IDC_OPENCLASSBUTTON, BNClicked),
  EV_LBN_DBLCLK(IDC_OPENCLASSLIST, LBNDblclk),
  EV_BN_CLICKED(IDC_OPENTEMPLATEBUTTON, BNTemplateClicked),
//{{OpenClassDialogRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{OpenClassDialog Implementation}}


OpenClassDialog::OpenClassDialog(TWindow* parent,char*Title,
     OpenClassInfo* Info, char* ListName,TResId resId, TModule* module)
:
  TDialog(parent, resId, module),
  InfoPtr(Info),
  listName(ListName)
{
 SetCaption(Title);
 OPENCLASSNAME = new TEdit(this,IDC_OPENCLASSNAME,MAXCLASSNAME+1);
 OPENCLASSTEMPLATE = new TEdit(this,IDC_OPENCLASSTEMPLATE,MAXCLASSNAME+1);
 OPENCLASSBUTTON = new TButton(this,IDC_OPENCLASSBUTTON);
 OPENCLASSLIST = new TListBox(this,IDC_OPENCLASSLIST);
 OPENTEMPLATEBUTTON = new TButton(this,IDC_OPENTEMPLATEBUTTON);
}


OpenClassDialog::~OpenClassDialog()
{
 Destroy(IDCANCEL);
 delete List;
}


void OpenClassDialog::SetupWindow()
{
 char *ItemPtr;
 TDialog::SetupWindow();
 // Initialize the List Box
 List->Next(true);
 while( (ItemPtr=List->Next()) != 0 )
  OPENCLASSLIST->AddString(ItemPtr);
 OPENCLASSLIST->SetSelIndex(0);
 OPENCLASSLIST->SetFocus();
}

//-------------------------------------------------------------
// Override the Execute function to provide List initialization
//-------------------------------------------------------------
int OpenClassDialog::Execute()
{
  int result;

  List = new StringList(listName);
  if ( List->GetInitFail() )
    return IDCANCEL;

  result = TDialog::Execute();
  return result;
}


void OpenClassDialog::BNClicked()
{
 CmOk();
}


void OpenClassDialog::LBNDblclk()
{
 OPENCLASSNAME->Transfer("",tdSetData);
 OPENCLASSTEMPLATE->Transfer("",tdSetData);
 CmOk();
}


bool OpenClassDialog::CanClose()
{
 bool result;

 memset(InfoPtr,0,sizeof(*InfoPtr));
 OPENCLASSNAME->Transfer(InfoPtr->ClassName,tdGetData);
 if ( InfoPtr->ClassName[0] )
  {
   if ( List->Find(InfoPtr->ClassName) != -1 )
    {
     MessageBox("This Class name already exists","Edit Error",
                 MB_OK|MB_ICONERROR);
     OPENCLASSNAME->SetFocus();            
     return false;
    }
   OPENCLASSTEMPLATE->Transfer(InfoPtr->TemplateClassName,tdGetData);
   InfoPtr->IsNew=true;
  }
 else
  {
   OPENCLASSLIST->GetString(InfoPtr->ClassName,OPENCLASSLIST->GetSelIndex());
   InfoPtr->IsNew=false;
  }

 result = TDialog::CanClose();
 return result;
}


void OpenClassDialog::BNTemplateClicked()
{
 int i=OPENCLASSLIST->GetSelIndex();
 char Buf[MAXCLASSNAME+1];

 if ( i==-1 )
  return;
 OPENCLASSLIST->GetString(Buf,i);
 OPENCLASSTEMPLATE->Transfer(Buf,tdSetData);
}


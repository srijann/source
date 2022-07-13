//----------------------------------------------------------------------------
//  Project spsmanager
//
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    spsmanager.apx Application
//  FILE:         opentreedialog.cpp
//  AUTHOR:
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of OpenTreeDialog (TDialog).
//
//----------------------------------------------------------------------------
#include "win4me.h"
#pragma hdrstop

#include "OpenTreeDialog.h"
#include "splib.h"

#define MYFOCUSMSG "CLASSDIALOGFOCUSMSG"

//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(OpenTreeDialog, TDialog)
//{{OpenTreeDialogRSP_TBL_BEGIN}}
  EV_BN_CLICKED(IDC_OPENBUTTON, BNOpenClicked),
  EV_TVN_SELCHANGED(IDC_RULESCLASSTREE, TVNSelchanged),
  EV_BN_CLICKED(IDC_TEMPLATEBUTTON, BNTemplateClicked),
  EV_REGISTERED(MYFOCUSMSG, DialogSetFocus),
//{{OpenTreeDialogRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{OpenTreeDialog Implementation}}

#define MAXITEMS 10000

//------------
// Constructor
//------------
OpenTreeDialog::OpenTreeDialog(TWindow* parent, OpenTreeInfo* Info,
  TResId resId, TModule* module)
:
  TDialog(parent, resId, module),
  InfoPtr(Info)
{
 TEMPLATECLASS = new TEdit(this,IDC_TEMPLATECLASS,MAXCLASSNAME+1);
 TEMPLATESUBCLASS = new TEdit(this,IDC_TEMPLATESUBCLASS,MAXCLASSNAME+1);
 RULESCLASSNAME = new TEdit(this,IDC_RULESCLASSNAME,MAXCLASSNAME+1);
 RULESSUBCLASSNAME = new TEdit(this,IDC_RULESSUBCLASSNAME,MAXCLASSNAME+1);
 TEMPLATEBUTTON = new TButton(this,IDC_TEMPLATEBUTTON);
 OPENBUTTON = new TButton(this,IDC_OPENBUTTON);
 TreeWind=new TTreeWindow(this,IDC_RULESCLASSTREE);
}


//-----------
// Destructor
//-----------
OpenTreeDialog::~OpenTreeDialog()
{
 Destroy(IDCANCEL);
 delete ClassList;
}

//------------------------------------------------------------
// Override of virtual function to set up the Tree Window with
// the class list
//------------------------------------------------------------
void OpenTreeDialog::SetupWindow()
{
 TDialog::SetupWindow();
 BuildTree();
 TreeWind->SetFocus();
}

//--------------------------------
// Handle click of the open button
//--------------------------------
void OpenTreeDialog::BNOpenClicked()
{
 CmOk();
}

//----------------------------------------------
// Handle a Selection Change in the Tree Window
//----------------------------------------------
void OpenTreeDialog::TVNSelchanged(TTwNotify& twn)
{
 Selection=twn.itemNew.lParam;
 char ClassName[MAXCLASSNAME+1];
 char SubClassName[MAXCLASSNAME+1];

 // Enable the Template button only if on a selectable item (ie.,
 // not a NEW item).
 TEMPLATEBUTTON->EnableWindow( Selection < MAXITEMS );

 // New Item?
 if ( Selection >= MAXITEMS )
  {
   RULESSUBCLASSNAME->EnableWindow(true);
   RULESCLASSNAME->EnableWindow(true);
   RULESSUBCLASSNAME->Transfer("",tdSetData);
   RULESCLASSNAME->Transfer("",tdSetData);
   // New Class Selection
   if ( Selection==MAXITEMS )
    {
     RULESCLASSNAME->SetReadOnly(false);
     RULESSUBCLASSNAME->SetReadOnly(true);
     // We want to set the focus on the class field but we can't do
     // it from here. We have to send a message to do it.
     // RULESCLASSNAME->SetFocus();
     PostMessage(RegisterWindowMessage(MYFOCUSMSG),0,(LPARAM)RULESCLASSNAME);
    }
   // New SubClass Selection
   else
    {
     ClassList->FindNode(Selection-MAXITEMS,ClassName,SubClassName);
     RULESCLASSNAME->Transfer(ClassName,tdSetData);
     RULESSUBCLASSNAME->SetReadOnly(false);
     RULESCLASSNAME->SetReadOnly(true);
     // Ditto above
     //RULESSUBCLASSNAME->SetFocus();
     PostMessage(RegisterWindowMessage(MYFOCUSMSG),0,(LPARAM)RULESSUBCLASSNAME);
    }
  }
 else
  {
   RULESCLASSNAME->SetReadOnly(true);
   RULESSUBCLASSNAME->SetReadOnly(true);
  }
}

//-------------------------------------
// Handle the Set Template Button Click
//-------------------------------------
void OpenTreeDialog::BNTemplateClicked()
{
 char ClassName[MAXCLASSNAME+1];
 char SubClassName[MAXCLASSNAME+1];
 if ( ! ClassList->FindNode(Selection,ClassName,SubClassName) )
  ClassName[0] = SubClassName[0] = 0;
 TEMPLATECLASS->Transfer(ClassName,tdSetData);
 TEMPLATESUBCLASS->Transfer(SubClassName,tdSetData);
}

//--------------------------------------------------------
// Can Close function. Insure valid names if new selection
//--------------------------------------------------------
bool OpenTreeDialog::CanClose()
{
 bool result;

 memset(InfoPtr,0,sizeof(*InfoPtr));

 // Get CLASS/SUBCLASS from New fields
 RULESCLASSNAME->Transfer(InfoPtr->ClassName,tdGetData);
 RULESSUBCLASSNAME->Transfer(InfoPtr->SubClassName,tdGetData);

 // Creating a new Class?
 if ( InfoPtr->ClassName[0] && ! InfoPtr->SubClassName[0] )
  {
   InfoPtr->IsNew = true;

   // Insure that the name is unique
   if ( strcmp(InfoPtr->ClassName,"DEFAULT") == 0 ||
        ClassList->FindNode(InfoPtr->ClassName) != -1 )
    {
     MessageBox("The Class already exists!","Edit Error",
                MB_OK|MB_ICONERROR);
     RULESCLASSNAME->SetFocus();
     return false;
    }
  }

 else

 //Creating a new SubClass?
 if ( InfoPtr->SubClassName[0] )
  {
   InfoPtr->IsNew=true;

   // Insure SubClass Unique
   if ( ClassList->FindNode(InfoPtr->ClassName,InfoPtr->SubClassName) != -1 )
    {
     MessageBoxf(HWindow,"Edit Error",MB_OK|MB_ICONERROR,
                "The Subclass already exists for class %s!",
                InfoPtr->ClassName);
     RULESSUBCLASSNAME->SetFocus();
     return false;
    }
  }

  else
   {
    // If Current Selection is New, require New Class/SubClass Name
    if ( Selection>=MAXITEMS )
     {
      MessageBox("A new Class/Subclass Name is required","",MB_OK);
      return false;
     }
    InfoPtr->IsNew=false;
    ClassList->FindNode(Selection,InfoPtr->ClassName,
                             InfoPtr->SubClassName);
   }

  // Get Template Class if present and new
  if ( InfoPtr->IsNew )
   {
    TEMPLATECLASS->Transfer(InfoPtr->TemplateClassName,tdGetData);
    TEMPLATESUBCLASS->Transfer(InfoPtr->TemplateSubClassName,tdGetData);
   }

  result = TDialog::CanClose();

  return result;
}

//------------------------------------------
// Build the Tree Window from the Class List
//------------------------------------------
void OpenTreeDialog::BuildTree()
{
 TTreeNode root = TreeWind->GetRoot();
 char *Ptr;
 TTreeNode* Global=0;
 TTreeNode* Class=0;
 TTreeNode* NewItem;
 int ItemNum;

 ClassList->Next(true);

 for (ItemNum=0; (Ptr=ClassList->Next()) != 0 &&
                 ItemNum < MAXITEMS; ++ItemNum)
   switch( Ptr[0] )
    {
     case 'G':  Global = &root.AddChild(TTreeNode(*TreeWind,&Ptr[1]));
                Global->SetItemData(ItemNum);
                NewItem=&Global->AddChild(TTreeNode(*TreeWind,"(NEW CLASS)"));
                NewItem->SetItemData(MAXITEMS);
                break;

     case 'P':  if ( Global )
                {
                 Class = &Global->AddChild(TTreeNode(*TreeWind,&Ptr[1]));
                 Class->SetItemData(ItemNum);
                 NewItem = &Class->AddChild(TTreeNode(*TreeWind,"(NEW SUBCLASS)"));
                 NewItem->SetItemData(MAXITEMS+ItemNum);
                }
              break;

    case 'C': if ( Class )
               {
                NewItem=&Class->AddChild(TTreeNode(*TreeWind,&Ptr[1]));
                NewItem->SetItemData(ItemNum);
               }
              break;
   }

 if ( Global )
  {
   Global->ExpandItem(TVE_EXPAND);
   TreeWind->SelectItem(TVGN_ROOT,Global->GetHTreeItem());
  }

 Selection=0;
}

//-------------------------------------------------------------------
// Handle application specific message to set the focus. We need this
// to change the focus while we're in the Tree Window
//-------------------------------------------------------------------
#pragma argsused
LRESULT OpenTreeDialog::DialogSetFocus(WPARAM MsgType, LPARAM Win)
{
 ((TEdit*)Win)->SetFocus();
 return 1;
}


int OpenTreeDialog::Execute()
{
 ClassList=new StringTreeList(AUTHRULES_LIST);

 if ( ClassList->GetInitFail() )
  return IDCANCEL;
 else
  return TDialog::Execute();
}

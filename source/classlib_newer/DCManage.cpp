//----------------------------------------------------------------------------
//  Project Classlib
//
//  Copyright © 1996. All Rights Reserved.
//
//  FILE:         DCManage.cpp
//  AUTHOR:       Jack Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of DialogControlManager (IPCTransfer)
//
//  This is a derived class of IPCTranser. It's a container/handler
//  for Dialog Controls. It provides a way
//  of linking fields directly to controls via IPCMessage Field numbers.
//  It also provides the ability to associate Message Numbers with
//  the fields.
//
//  This object is used primarily by the Voice Authorization program
//  to move data back and forth from the IPCMessage to a Window and
//  it also updates the Message Window as control focus changes.
//----------------------------------------------------------------------------


#include "win4me.h"
#include "DCManage.h"

static int FindId(const CTLITEM& Item,void* Id);
static int FindFn(const CTLITEM& Item,void* IPCFn);

//----------------------------------------------------------------------
// Constructor
//
// Specify the maximum number of controls that need to be handled and
// optionally provide a default IPCMessage pointer and a Message Window.
//-----------------------------------------------------------------------
DialogControlManager::DialogControlManager(TWindow* Parent,int NumControls,
              IPCMessage* IPCMsgPtr,TMsgWin* MsgWindow,TModule* ModulePtr) :
 IPCTransfer(IPCMsgPtr),
 ParentWin(Parent),
 Vector(NumControls),
 IPCMsg(IPCMsgPtr),
 MsgWin(MsgWindow),
 Module(ModulePtr),
 IdWithFocus(-1)
{
}

//-----------------
// Class Destructor
//-----------------
DialogControlManager::~DialogControlManager()
{
 // No Cleanup. OWL will clean up the controls for us and we
 // have no internal buffers.
}

//--------------------------------------------
// Add a TEdit control that is already created
//--------------------------------------------
TEdit* DialogControlManager::Add(int Id,TEdit* EditCtl,FLDNUM Fn,
                                 int MsgNum,BYTE GroupNum,FLDTYPE FldType)
{
 CTLITEM Item;
 Item.Id=Id;
 Item.IPCFn=Fn;
 Item.MsgNum=MsgNum;
 Item.CtlPtr.Edit=EditCtl;
 Item.CtlType=DCTYPE_EDIT;
 Item.FldType=(BYTE)FldType;
 Item.Text=0;
 Item.GroupNum=GroupNum;
 Vector.Add(Item);
 return EditCtl;
}

//-----------------------------
// Same as above with no MSGNUM
//-----------------------------
TEdit* DialogControlManager::Add(int Id,TEdit* EditCtl,FLDNUM Fn,
                                 BYTE GroupNum,FLDTYPE FldType)
{
 return Add(Id,EditCtl,Fn,-1,GroupNum,FldType);
}

//-------------------------------
// Create and Add a TEDIT control
//-------------------------------
TEdit* DialogControlManager::Add(int Id,int Len,FLDNUM Fn,int MsgNum,
                                 BYTE GroupNum,FLDTYPE FldType)

{
 TEdit* Edit=new TEdit(ParentWin,Id,Len+1,Module);
 return Add(Id,Edit,Fn,MsgNum,GroupNum,FldType);
}

//-----------------------------
// Same as above with no MSGNUM
//-----------------------------
TEdit* DialogControlManager::Add(int Id,int Len,FLDNUM Fn,BYTE GroupNum,
                                 FLDTYPE FldType)

{
 return Add(Id,Len,Fn,-1,GroupNum,FldType);
}

//-------------------------------------------------------
// Add a Combo Box Control with Combo Box already created
//-------------------------------------------------------
TComboBoxEx* DialogControlManager::AddComboBox(int Id,TComboBoxEx* ComboBox,
                                  FLDNUM Fn,int MsgNum,BYTE GroupNum,
                                  FLDTYPE FldType)
{
 CTLITEM Item;
 Item.Id=Id;
 Item.IPCFn=Fn;
 Item.MsgNum=MsgNum;
 Item.CtlPtr.ComboBox=ComboBox;
 Item.CtlType=DCTYPE_COMBOBOX;
 Item.FldType=(BYTE)FldType;
 Item.Text=0;
 Item.GroupNum=GroupNum;
 Item.rdOnly=false;
 Vector.Add(Item);
 return ComboBox;
}

//-----------------------------
// Same as above with no MSGNUM
//-----------------------------
TComboBoxEx* DialogControlManager::AddComboBox(int Id,TComboBoxEx* ComboBox,
                                  FLDNUM Fn,BYTE GroupNum,FLDTYPE FldType)
{
 return AddComboBox(Id,ComboBox,-1,Fn,GroupNum,FldType);
}


//----------------------------------
// Create and Add a ComboBox control
//----------------------------------
TComboBoxEx* DialogControlManager::AddComboBox(int Id,FLDNUM Fn,int MsgNum,
                                       BYTE GroupNum,FLDTYPE FldType)
{
 return AddComboBox(Id,new TComboBoxEx(ParentWin,Id,Module),Fn,MsgNum,
                                  GroupNum,FldType);
}

//-----------------------------
// Same as above with no MSGNUM
//-----------------------------
TComboBoxEx* DialogControlManager::AddComboBox(int Id,FLDNUM Fn,
                                          BYTE GroupNum,FLDTYPE FldType)
{
 return AddComboBox(Id,Fn,-1,GroupNum,FldType);
}

//-----------------
// Enable a Control
//-----------------
static void enableitem(CTLITEM& Item,void* groupNum)
{
 if ( groupNum !=NULL )
  {
   BYTE Group=*((BYTE*)groupNum);
   if ( Group != 0 &&
        Group != Item.GroupNum )
    return;
  }

 switch(Item.CtlType)
  {
   case DCTYPE_EDIT:     Item.CtlPtr.Edit->EnableWindow(true); break;
   case DCTYPE_COMBOBOX:
     if ( ! Item.rdOnly )
       Item.CtlPtr.ComboBox->EnableWindow(true);
     break;
   case DCTYPE_STATIC:   Item.CtlPtr.Static->EnableWindow(true); break;
   case DCTYPE_BUTTON:   Item.CtlPtr.Button->EnableWindow(true); break;
  }
 if ( Item.Text )
  Item.Text->EnableWindow(true);
}

//--------------------
// Enable ALL Controls
//--------------------
void DialogControlManager::EnableAll(BYTE GroupNum)
{
 Vector.ForEach(enableitem,&GroupNum);
}

//-----------------------------------
// Enable Control by IPC Field number
//-----------------------------------
void DialogControlManager::Enable(FLDNUM Fn)
{
 CTLITEM* Item=Vector.FirstThat(FindFn,(void*)&Fn);
 if ( Item )
  enableitem(*Item,NULL);
}

//------------------------------
// Enable a Control by ID Number
//------------------------------
void DialogControlManager::Enable(int Id)
{
 CTLITEM* Item=Vector.FirstThat(FindId,(void*)&Id);
 if ( Item )
  enableitem(*Item,NULL);
}


//------------------
// Disable a Control
//------------------
static void disableitem(CTLITEM& Item,void* groupNum)
{
 if ( groupNum !=NULL )
  {
   BYTE Group=*((BYTE*)groupNum);
   if ( Group != 0 &&
        Group != Item.GroupNum )
    return;
  }

 switch(Item.CtlType)
  {
   case DCTYPE_EDIT:     Item.CtlPtr.Edit->EnableWindow(false); break;
   case DCTYPE_COMBOBOX: Item.CtlPtr.ComboBox->EnableWindow(false); break;
   case DCTYPE_STATIC:   Item.CtlPtr.Static->EnableWindow(false); break;
   case DCTYPE_BUTTON:   Item.CtlPtr.Button->EnableWindow(false); break;
  }

 if ( Item.Text )
  Item.Text->EnableWindow(false);
}

//------------------------------------
// Disable Control by IPC Field Number
//------------------------------------
void DialogControlManager::Disable(FLDNUM Fn)
{
 CTLITEM* Item=Vector.FirstThat(FindFn,(void*)&Fn);
 if ( Item )
  disableitem(*Item,NULL);
}

//-----------------------------
// Disable Control by ID Nunber
//-----------------------------
void DialogControlManager::Disable(int Id)
{
 CTLITEM* Item=Vector.FirstThat(FindId,(void*)&Id);
 if ( Item )
  disableitem(*Item,NULL);
}

//---------------------
// Disable all Controls
//---------------------
void DialogControlManager::DisableAll(BYTE GroupNum)
{
 Vector.ForEach(disableitem,&GroupNum);
}

//--------------------------------
// Invoke Control's clear function
//--------------------------------
static void clearitem(CTLITEM& Item,void* groupNum)
{
 if ( groupNum !=NULL )
  {
   BYTE Group=*((BYTE*)groupNum);
   if ( Group != 0 &&
        Group != Item.GroupNum )
    return;
  }

 switch(Item.CtlType)
  {
   case DCTYPE_EDIT:      Item.CtlPtr.Edit->Clear(); break;
   case DCTYPE_COMBOBOX:  Item.CtlPtr.ComboBox->Clear(); break;
   default: break;
  }
}

//--------------------------------------
// Clear ALL Edit and Combo Box controls
//--------------------------------------
void DialogControlManager::ClearAll(BYTE GroupNum)
{
 Vector.ForEach(clearitem,&GroupNum);
}

//------------------------------------
// Clear Control by IPC Field Number
//------------------------------------
void DialogControlManager::Clear(FLDNUM Fn)
{
 CTLITEM* Item=Vector.FirstThat(FindFn,(void*)&Fn);
 if ( Item )
  clearitem(*Item,NULL);
}

//-----------------------------
// Clear Control by ID Nunber
//-----------------------------
void DialogControlManager::Clear(int Id)
{
 CTLITEM* Item=Vector.FirstThat(FindId,(void*)&Id);
 if ( Item )
  clearitem(*Item,NULL);
}


static int FindId(const CTLITEM& Item,void* Id)
{
 return (Item.Id == *(int*)Id ) ? 1 : 0;
}

static int FindFn(const CTLITEM& Item,void* IPCFn)
{
 // Only EDIT and COMBO box controls can have IPC field numbers
 if ( Item.CtlType != DCTYPE_EDIT &&
      Item.CtlType != DCTYPE_COMBOBOX )
  return 0;

 return (Item.IPCFn == *(FLDNUM*)IPCFn) ? 1 : 0;
}

//---------------------------------------------
// Private function to set the focus to an Item
//---------------------------------------------
void DialogControlManager::set_focus(CTLITEM* Item,int MsgNum)
{
 switch( Item->CtlType )
  {
   case DCTYPE_EDIT:
     Item->CtlPtr.Edit->EnableWindow(true);
     Item->CtlPtr.Edit->SetFocus();
     break;
   case DCTYPE_COMBOBOX:
     Item->CtlPtr.ComboBox->EnableWindow(true);
     Item->CtlPtr.ComboBox->SetFocus();
     if ( Item->CtlPtr.ComboBox->GetSelIndex() == -1 )
      {
       Item->CtlPtr.ComboBox->ShowList();
       Item->CtlPtr.ComboBox->SetSelIndex(0);
      }
     break;
   case DCTYPE_BUTTON:
     Item->CtlPtr.Button->EnableWindow(true);
     Item->CtlPtr.Button->SetFocus();
   default:
     return;
  }

 if ( Item->Text )
  Item->Text->EnableWindow(true);
 IdWithFocus=Item->Id;
 if ( MsgWin != 0 )
  {
   MsgNum = (MsgNum!=-1) ? MsgNum : Item->MsgNum;
   MsgWin->Say(MsgNum);
  }
}

//----------------------------------------
// Set the Focus on a control using the ID
//----------------------------------------
void DialogControlManager::SetFocus(int Id,int MsgNum)
{
 CTLITEM* Item=Vector.FirstThat(FindId,(void*)&Id);
 if ( Item==0 )
  IdWithFocus = -1;
 else
  set_focus(Item,MsgNum);
}

//---------------------------------------
// Set Focus by using an IPC Field number
//---------------------------------------
void DialogControlManager::SetFocus(FLDNUM Fn,int MsgNum)
{
 CTLITEM* Item=Vector.FirstThat(FindFn,(void*)&Fn);
 if ( Item==0 )
  IdWithFocus = -1;
 else
  set_focus(Item,MsgNum);
}

//-----------------------------------
// Return which control has the focus
//-----------------------------------
int DialogControlManager::HasFocus()
{
 return IdWithFocus;
}

//-------------------------------------
// Get pointer to an EDIT control by ID
//-------------------------------------
TEdit* DialogControlManager::EditPtr(int Id)
{
 CTLITEM* Item=Vector.FirstThat(FindId,(void*)&Id);
 return (Item==0) ? 0 : Item->CtlPtr.Edit;
}

//----------------------------------------
// Get pointer to a ComboBox control by ID
//----------------------------------------
TComboBoxEx* DialogControlManager::ComboBoxPtr(int Id)
{
 CTLITEM* Item=Vector.FirstThat(FindId,(void*)&Id);
 return (Item==0) ? 0 : Item->CtlPtr.ComboBox;
}

//---------------------------------------------------
// Get pointer to an EDIT control by IPC Field Number
//---------------------------------------------------
TEdit* DialogControlManager::EditPtr(FLDNUM Fn)
{
 CTLITEM* Item=Vector.FirstThat(FindFn,(void*)&Fn);
 return (Item==0) ? 0 : Item->CtlPtr.Edit;
}

//------------------------------------------------------
// Get pointer to a ComboBox control by IPC Field Number
//------------------------------------------------------
TComboBoxEx* DialogControlManager::ComboBoxPtr(FLDNUM Fn)
{
 CTLITEM* Item=Vector.FirstThat(FindFn,(void*)&Fn);
 return (Item==0) ? 0 : Item->CtlPtr.ComboBox;
}


//--------------------------------
// Get pointer to a STATIC control
//--------------------------------
TStatic* DialogControlManager::StaticPtr(int Id)
{
 CTLITEM* Item=Vector.FirstThat(FindId,(void*)&Id);
 return (Item==0) ? 0 : Item->CtlPtr.Static;
}

//--------------------------------
// Get pointer to a BUTTON control
//--------------------------------
TButton* DialogControlManager::ButtonPtr(int Id)
{
 CTLITEM* Item=Vector.FirstThat(FindId,(void*)&Id);
 return (Item==0) ? 0 : Item->CtlPtr.Button;
}


//-----------------------------------------------------
// Private Function to copy from a message to a control
//-----------------------------------------------------
bool DialogControlManager::copy_from_msg(CTLITEM* Item,BYTE GroupNum)
{
 if ( GroupNum &&
      GroupNum != Item->GroupNum )
  return false;

 switch(Item->CtlType)
  {
   case DCTYPE_EDIT:     return ToCtl(Item->IPCFn,Item->CtlPtr.Edit);
   case DCTYPE_COMBOBOX: return ToCtl(Item->IPCFn,Item->CtlPtr.ComboBox);
   default: return false;
  }
}

//-----------------------------------------------------
// Private Function to copy from a message to a control
//-----------------------------------------------------
bool DialogControlManager::copy_to_msg(CTLITEM* Item,BYTE GroupNum)
{
 if ( GroupNum &&
      GroupNum != Item->GroupNum )
  return false;

 switch(Item->CtlType)
  {
   case DCTYPE_EDIT:     return ToMsg(Item->IPCFn,Item->CtlPtr.Edit,
                                      Item->FldType);
   case DCTYPE_COMBOBOX: return ToMsg(Item->IPCFn,Item->CtlPtr.ComboBox,
                                      Item->FldType);

   default: return false;
  }
}

//----------------------------------------------------
// Copy a field from the Message to a control using ID
//----------------------------------------------------
bool DialogControlManager::CopyFromMsg(int Id,IPCMessage* Msg)
{
 bool ret;
 CTLITEM* Item=Vector.FirstThat(FindId,(void*)&Id);

 if ( Msg )
  {
   SetIPCMsg(Msg);
   ret=copy_from_msg(Item);
   SetIPCMsg(IPCMsg);
  }
 else
  ret=copy_from_msg(Item);

 return ret;
}

//----------------------------------------------------
// Copy a field from a Control to the Message using ID
//----------------------------------------------------
bool DialogControlManager::CopyToMsg(int Id,IPCMessage* Msg)
{
 bool ret;
 CTLITEM* Item=Vector.FirstThat(FindId,(void*)&Id);

 if ( Msg )
  {
   SetIPCMsg(Msg);
   ret=copy_to_msg(Item);
   SetIPCMsg(IPCMsg);
  }
 else
  ret=copy_to_msg(Item);

 return ret;
}

//--------------------------------------------------------------
// Copy a field from the Message to a control using Field Number
//--------------------------------------------------------------
bool DialogControlManager::CopyFromMsg(FLDNUM Fn,IPCMessage* Msg)
{
 bool ret;
 CTLITEM* Item=Vector.FirstThat(FindFn,(void*)&Fn);

 if ( Msg )
  {
   SetIPCMsg(Msg);
   ret=copy_from_msg(Item);
   SetIPCMsg(IPCMsg);
  }
 else
  ret=copy_from_msg(Item);

 return ret;
}

//--------------------------------------------------------------
// Copy a field from a Control to the Message using Field Number
//--------------------------------------------------------------
bool DialogControlManager::CopyToMsg(FLDNUM Fn,IPCMessage* Msg)
{
 bool ret;
 CTLITEM* Item=Vector.FirstThat(FindFn,(void*)&Fn);

 if ( Msg )
  {
   SetIPCMsg(Msg);
   ret=copy_to_msg(Item);
   SetIPCMsg(IPCMsg);
  }
 else
  ret=copy_to_msg(Item);

 return ret;
}

//------------------------------------------------
// Copy ALL fields from the Message to the Control
//------------------------------------------------
void DialogControlManager::CopyAllFromMsg(IPCMessage* Msg,BYTE GroupNum)
{
 int i, Num;

 if ( Msg )
   SetIPCMsg(Msg);
 for (i=0, Num=Vector.GetItemsInContainer(); i<Num; ++i)
  copy_from_msg(&Vector[i],GroupNum);
 if ( Msg )
  SetIPCMsg(IPCMsg);
}

//------------------------------------------------
// Copy ALL Fields from the control to the Message
//------------------------------------------------
void DialogControlManager::CopyAllToMsg(IPCMessage* Msg,BYTE GroupNum)
{
 int i, Num;

 if ( Msg )
   SetIPCMsg(Msg);

 for (i=0, Num=Vector.GetItemsInContainer(); i<Num; ++i)
  copy_to_msg(&Vector[i],GroupNum);
 if ( Msg )
  SetIPCMsg(IPCMsg);
}

//------------------------------------
// Set the Default IPC Message Pointer
//------------------------------------
void DialogControlManager::SetIPCMessage(IPCMessage* Msg)
{
 IPCMsg=Msg;
 SetIPCMsg(IPCMsg);
}

//---------------------------------------
// Fetch value of a control as an integer
//---------------------------------------
bool DialogControlManager::FetchCtl(int Id,int& Value)
{
 CTLITEM* Item=Vector.FirstThat(FindId,(void*)&Id);
 char* Buf;
 bool  ret;

 if ( Item==0 )
  return false;
 switch( Item->CtlType )
  {
   case DCTYPE_EDIT:
     Buf = new char[Item->CtlPtr.Edit->TextLimit+1];
     Item->CtlPtr.Edit->Transfer(Buf,tdGetData);
     Value=atoi(Buf);
     ret =  Buf[0] ? true : false;
     delete[] Buf;
     return ret;

   case DCTYPE_COMBOBOX:
     return  Item->CtlPtr.ComboBox->GetSelectedValue(Value)? true : false;

   default: return false;
  }

}

//-----------------------------------
// Fetch value of control as a string
//-----------------------------------
bool DialogControlManager::FetchCtl(int Id,char* String)
{
 CTLITEM* Item=Vector.FirstThat(FindId,(void*)&Id);
 if ( Item==0 ||
      Item->CtlType != DCTYPE_EDIT )
  return false;

 Item->CtlPtr.Edit->Transfer(String,tdGetData);
 return String[0] ? true : false;
}

//---------------------
// Add a Static Control
//---------------------
TStatic* DialogControlManager::AddStatic(int Id,int AssociateId,BYTE GroupNum)
{
 if ( AssociateId )
  {
   CTLITEM* AssociateItem=Vector.FirstThat(FindId,(void*)&AssociateId);
   if ( AssociateItem == NULL )
     return 0;

   AssociateItem->Text=new TStatic(ParentWin,Id,0,Module);
   return AssociateItem->Text;
  }
 else
  {
   CTLITEM Item;
   Item.Id=Id;
   Item.IPCFn=0;
   Item.MsgNum=-1;
   Item.CtlPtr.Static=new TStatic(ParentWin,Id,0,Module);
   Item.CtlType=DCTYPE_STATIC;
   Item.Text=0;
   Item.GroupNum=GroupNum;
   Vector.Add(Item);
   return Item.CtlPtr.Static;
  }
}

//-------------
// Add a Button
//-------------
TButton* DialogControlManager::AddButton(int Id,int MsgNum,TButton* Button)
{
 CTLITEM Item;
 Item.Id=Id;
 Item.IPCFn=0;
 Item.MsgNum=MsgNum;
 if ( Button==0 )
   Item.CtlPtr.Button=new TButton(ParentWin,Id);
 else
   Item.CtlPtr.Button=Button;
 Item.CtlType=DCTYPE_BUTTON;
 Item.Text=0;
 Item.GroupNum=0;
 Vector.Add(Item);
 return Item.CtlPtr.Button;
}

static void setReadOnly(CTLITEM& Item,void* MsgPt)
{
 switch(Item.CtlType)
  {
   case DCTYPE_EDIT:
     if ( ((IPCMessage*)MsgPt)->GetFld(Item.IPCFn) )
      Item.CtlPtr.Edit->SetReadOnly(true);
     break;
   case DCTYPE_COMBOBOX:
     if ( ((IPCMessage*)MsgPt)->GetFld(Item.IPCFn) )
      {
       Item.CtlPtr.ComboBox->EnableWindow(false);
       Item.rdOnly=true;
      }
    break;
  }
}

static void setreadonly(CTLITEM& Item,void* groupNum)
{
 if ( groupNum !=NULL )
  {
   BYTE Group=*((BYTE*)groupNum);
   if ( Group != 0 &&
        Group != Item.GroupNum )
    return;
  }

 switch(Item.CtlType)
  {
   case DCTYPE_EDIT:
     Item.CtlPtr.Edit->SetReadOnly(true);
     break;
   case DCTYPE_COMBOBOX:
     Item.CtlPtr.ComboBox->EnableWindow(false);
     Item.rdOnly=true;
    break;
  }
}


//-------------------------------------------
// Set all fields that have data to Read Only
//-------------------------------------------
void DialogControlManager::SetAllPresentToReadOnly(IPCMessage* Msg)
{
 if ( Msg==0 )
  {
   if ( IPCMsg==0 )
    return;
   Msg=IPCMsg;
  }

 Vector.ForEach(setReadOnly,Msg);
}

//-------------------------------------------
// Set all fields that have data to Read Only
//-------------------------------------------
void DialogControlManager::SetAllReadOnly(BYTE GroupNum)
{
 Vector.ForEach(setreadonly,&GroupNum);
}

static void resetReadOnly(CTLITEM& Item,void* groupNum)
{
 if ( groupNum !=NULL )
  {
   BYTE Group=*((BYTE*)groupNum);
   if ( Group != 0 &&
        Group != Item.GroupNum )
    return;
  }

 switch( Item.CtlType )
  {
   case DCTYPE_EDIT:
    Item.CtlPtr.Edit->SetReadOnly(false);
    break;
   case DCTYPE_COMBOBOX:
    Item.CtlPtr.Edit->EnableWindow(true);
    Item.rdOnly=false;
    break;
  }
}

//----------------------------------------------
// Clear all the Readonly flags from edit fields
//----------------------------------------------
void DialogControlManager::ClearAllReadOnly(BYTE GroupNum)
{
 Vector.ForEach(resetReadOnly,&GroupNum);
}



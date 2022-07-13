//----------------------------------------------------------------------------
//  Project Classlib
//
//  Copyright © 1996. All Rights Reserved.
//
//  FILE:         ProductClassList.cpp
//  AUTHOR:       Jack Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of ProductClassList (StringList)
//
//  This is a Derived class of StringList to handle the Product Code
//  List.
//----------------------------------------------------------------------------

#include "win4me.h"
#pragma hdrstop
#include <time.h>
#include "ProductClassList.h"
#include "ipc.h"
#include "CheckauthFn.h"
#include "TranType.h"
#include "dbdefs.h"
#include "splib.h"

// Added 12/02/02 by MAB
#if 1
#include <treewind.h>
#endif

//---------------------------
// Class Constructor
//   Fetch and store the list
//---------------------------
ProductClassList::ProductClassList(char *List,int Length) :
  StringList(List,Length,0)
{
}

ProductClassList::ProductClassList() :
  StringList(PRODUCT_LIST,
       (NUMPRODUCTCLASSES*(NUMPRODUCTSUBCLASSES+1))*PRODUCTTEXTMAX+1)
{
 int  Class, SubClass;
 Next(true);

 // If the List is empty, initialize it with Empty Strings
 if ( Next() == 0 )
  for (Class=1; Class<=NUMPRODUCTCLASSES; ++Class)
   {
    char Buf[25];
    sprintf(Buf,"Group %d",Class);
    Add(Buf);
    for (SubClass=1; SubClass<=NUMPRODUCTSUBCLASSES; ++SubClass)
      Add("");
   }
}

ProductClassList::ProductClassList(TFile* File,int ListLen) :
 StringList(File,ListLen)
{
}

ProductClassList::ProductClassList(IPCMessage* Msg) :
 StringList(Msg)
{
}

ProductClassList::ProductClassList(ProductClassList* PCList) :
 StringList(PCList)
{
}

//--------------------------------------------
// Init a Tree view for all classes/subclasses
//--------------------------------------------
void ProductClassList::InitMaintView(TTreeWindow* TreeWind)
{
 TTreeNode root = TreeWind->GetRoot();
 char *Ptr;
 TTreeNode* Class;
 TTreeNode* NewItem;
 int ClassNum;
 int SubClassNum;
 char Buf[50];

 TreeWind->DeleteAllItems();
 Next(true);

 for (ClassNum=1; ClassNum <= NUMPRODUCTCLASSES; ++ClassNum)
  {
   if ( (Ptr=Next()) == NULL ||
        *Ptr==0 )
    {
     sprintf(Buf,"%d. ",ClassNum);
     strcat(Buf,"__________");
    }
   else
    strcpy(Buf,Ptr);
   Class = &root.AddChild(TTreeNode(*TreeWind,Buf));
   Class->SetItemData(ClassNum*100);
   for ( SubClassNum=1; SubClassNum <= NUMPRODUCTSUBCLASSES; ++SubClassNum )
    {
     if ( (Ptr=Next()) == NULL ||
          *Ptr==0 )
      {
       sprintf(Buf,"%d. ",SubClassNum);
       strcat(Buf,"__________");
      }
     else
      strcpy(Buf,Ptr);
     NewItem=&Class->AddChild(TTreeNode(*TreeWind,Buf));
     NewItem->SetItemData(ClassNum*100+SubClassNum);
    }
  }

}

//-------------------------------
// Handle Change to the Tree View
//--------------------------------
void ProductClassList::TreeTextChange(TTreeWindow* TreeWin,char* newText)
{
 char Buf[PRODUCTTEXTMAX+10];
 char newTextBuf[PRODUCTTEXTMAX+1];
 uint32 ItemNum;
 TTreeNode Node = TreeWin->GetSelection();
 Node.GetItemData(ItemNum);

 if ( newText==NULL )
  return;
  
 memset(newTextBuf,0,sizeof(newTextBuf));
 memcpy(newTextBuf,newText,min(strlen(newText),(size_t)PRODUCTTEXTMAX));
 if ( newTextBuf[0] == 0 )
 {
  if ( ItemNum %100 == 0 )
   sprintf(Buf,"%d. ",ItemNum / 100 );
  else
  sprintf(Buf,"%d. ",ItemNum % 100 );
  strcat(Buf,"__________");
  }
 else
  strcpy(Buf,newTextBuf);

 Replace( (ItemNum / 100 - 1) * (NUMPRODUCTCLASSES+1) + ItemNum % 100, newTextBuf );
 Node.SetText(Buf);
}

//--------------------------------------------
// Init a Combox Box for Subclasses of a Class
//--------------------------------------------
void ProductClassList::InitComboBoxClass(TComboBox* cBox,uint Class,uint Select)
{
 uint i, Count, n;
 int SelectI;
 char* Ptr;

 // WINDOWS/OWL Bug here. 
 cBox->ClearList();
 //cBox->SendMessage(CB_RESETCONTENT,0,0);

 if ( Class > NUMPRODUCTCLASSES ||
      Class == 0 )
  return;

 for ( SelectI=-1, n = (Class-1) * (NUMPRODUCTSUBCLASSES + 1) + 1, Count=0;
           Count < NUMPRODUCTSUBCLASSES; ++n, ++Count )
  {
   if ( (Ptr=Find(n)) != NULL &&
        Ptr[0] != 0 )
    {
     i = cBox->AddString(Ptr);
     cBox->SetItemData(i,Count+1);
     if ( Count+1 == Select )
      SelectI = i;
    }
  }

 if ( SelectI != -1 )
  cBox->SetSelIndex(SelectI);
}

//----------------------------------
// Init a Combox Box for all Classes
//----------------------------------
void ProductClassList::InitComboBoxClasses(TComboBox* cBox,
          bool emptyGroup,uint Select)
{
 uint i, n, Count;
 int SelectI;
 char* Ptr;

 cBox->ClearList();

 // Want an empty group?
 if ( emptyGroup )
  {
   cBox->AddString(" ");
   cBox->SetItemData(0,0);
  }

 for ( SelectI = -1, n = 0, Count=0;
          Count < NUMPRODUCTCLASSES;
       n += NUMPRODUCTSUBCLASSES+1, ++Count )
  {
   if ( (Ptr=Find(n)) != NULL &&
        Ptr[0] != 0 )
    {
     i=cBox->AddString(Ptr);
     cBox->SetItemData(i,Count+1);
     if ( Count+1 == Select )
      SelectI = i;
    }
  }

 if ( SelectI != -1 )
  cBox->SetSelIndex(SelectI);
}

//---------------------------------------------------------
// Init an array of Static Controls for all subclasses of a
// Class
//----------------------------------------------------------
void ProductClassList::InitText(TStatic** TextCtl,uint Class)
{
 uint n, Count;
 char* Ptr;

 if ( Class > NUMPRODUCTCLASSES ||
      Class == 0 )
  return;

 for ( n = (Class-1) * (NUMPRODUCTSUBCLASSES + 1) + 1, Count=0;
           Count < NUMPRODUCTSUBCLASSES; ++n, ++Count )
   if ( (Ptr=Find(n)) == NULL )
    TextCtl[Count]->Transfer("",tdSetData);
   else
    TextCtl[Count]->Transfer(Ptr,tdSetData);

}

//--------------------------------------------------------------------
// Iterator -- Passes String, Index, and args to callers' function for
// each member
//--------------------------------------------------------------------
void ProductClassList::ForEachClass(void(*func)(char*,int,void*),
                                         void* Args)
{
 int n, count;

 for( n=count=0; count < NUMPRODUCTCLASSES; ++count,
          n+=NUMPRODUCTSUBCLASSES+1)
  func(Find(n), count+1, Args);
}

//--------------------------------------------------------------------
// Iterator -- Passes String, Index, and args to callers' function for
// each member
//--------------------------------------------------------------------
void ProductClassList::ForEachSubClass(uint Class,
        void(*func)(char*,int,void*),void* Args)
{
 int n, count;

 if ( Class > NUMPRODUCTCLASSES ||
      Class == 0 )
  return;

 for( n= (Class-1) * (NUMPRODUCTSUBCLASSES + 1) + 1, count=0;
        count < NUMPRODUCTSUBCLASSES; ++count, n++ )
  func(Find(n), count+1, Args);
}

void ProductClassList::SetComboBoxClass(TComboBox* cBox,uint Class)
{
 char *Ptr;

 if ( Class == 0 ||
      Class > NUMPRODUCTCLASSES )
 {
  cBox->SetSelIndex(-1);
  return;
 }

 if ( (Ptr=Find( (Class-1) * (NUMPRODUCTSUBCLASSES+1))) == NULL ||
      Ptr[0] == 0 )
  return;

 cBox->SetSelString(Ptr,0);
}

//--------------------------------
// Test if a group number is valid
//--------------------------------
bool ProductClassList::isValidGroupNumber(uint GroupNum)
{
 uint n, count;
 char *ptr;

 if ( GroupNum == 0 ||
      GroupNum > NUMPRODUCTCLASSES )
  return false;

 for( n= (GroupNum-1) * (NUMPRODUCTSUBCLASSES + 1) + 1, count=0;
        count < NUMPRODUCTSUBCLASSES; ++count, n++ )
  if ( (ptr=Find(n)) != NULL &&
       ptr[0] != 0 )
    return true;

 return false;
}

//--------------------------------
// Find an Item for Class/Subclass
//--------------------------------
char* ProductClassList::FindName(uint Class,uint SubClass)
{
 uint n;

 if ( Class==0 ||
      Class > NUMPRODUCTCLASSES ||
      SubClass > NUMPRODUCTSUBCLASSES )
  return 0;

 n=(Class-1)*(NUMPRODUCTSUBCLASSES+1);

 if ( SubClass != 0 )
  return Find(n+SubClass);
 else
  return Find(n); 
}

#ifndef __PCLIST_H
#define __PCLIST_H
#include "ListDefs.h"
#include <owl/ComboBox.h>
#include <owl/treewind.h>
#include <owl/static.h>

#define NUMPRODUCTCLASSES 12
#define NUMPRODUCTSUBCLASSES 12
#define PRODUCTTEXTMAX 25

class ProductClassList : public StringList {
  public:
  ProductClassList();
  ProductClassList(char* TheList,int ListLen);
  ProductClassList(TFile* File,int ListLen);
  ProductClassList(IPCMessage* Msg);
  ProductClassList(ProductClassList*);
  // Init a Combo Box for all subclasses of a class
  void InitComboBoxClass(TComboBox* cBox,uint Class,uint Select=0);
  // Init a Combox Box for all the classes
  void InitComboBoxClasses(TComboBox* cBox,bool emptyGroup=false,
                                uint Select=0);
  void SetComboBoxClass(TComboBox* cBox,uint Class);
  // Init a Tree view for all classes/subclasses
  void InitMaintView(TTreeWindow* treeWin);
  // Handle a change to the Tree Window
  void TreeTextChange(TTreeWindow* TreeWind,char* newText);
  // Init an array of Text boxes for a Class
  void InitText(TStatic** Ctls,uint Class);
  void ForEachClass(void(*)(char* String,int Index,void* args),
                        void* args=0);
  void ForEachSubClass(uint SubClass,
        void(*)(char* String,int Index,void* args),void* args=0);
  bool isValidGroupNumber(uint GroupNum);
  char* FindName(uint Class,uint SubClass=0);
 };
#endif
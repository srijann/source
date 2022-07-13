#ifndef __DIALOGMANAGER_H
#define __DIALOGMANAGER_H

#include "IPCTransfer.h"
#include <owl/validate.h>
#include <owl/button.h>
#include <classlib/arrays.h>
#include "MsgWin.h"

enum {
  DCTYPE_EDIT,
  DCTYPE_STATIC,
  DCTYPE_COMBOBOX,
  DCTYPE_BUTTON
 };

typedef struct ctlitem {
  int    Id;       // The Control ID
  FLDNUM IPCFn;    // The IPC Field Number
  int    MsgNum;   // The Default Message Number for the Message Window
  BYTE   CtlType;  // Tells what type of control it is
  BYTE   FldType;  // Tells what type of IPC field it is
  BYTE   rdOnly;   // Read only flag for Combo Boxes
  BYTE   GroupNum; // Group Number
  TStatic* Text;   // The Associated Text Buddy Window, if any
  union
   {
    TEdit* Edit;
    TComboBoxEx* ComboBox;
    TStatic* Static;
    TButton* Button;
   } CtlPtr;
  int operator==(const ctlitem& r) const
   {
    return (Id==r.Id) ? 1 : 0;
   }
  int operator<(const ctlitem& r) const
   {
    return (Id<r.Id) ? 1 : 0;
   }
 } CTLITEM;

class DialogControlManager : public IPCTransfer {
  TWindow* ParentWin;
  IPCMessage* IPCMsg;
  TMsgWin* MsgWin;
  TSArrayAsVector<CTLITEM> Vector;
  TModule* Module;
  int IdWithFocus;
  bool copy_from_msg(CTLITEM* Item,BYTE GroupNum=0);
  bool copy_to_msg(CTLITEM* Item,BYTE GroupNum=0);
  void set_focus(CTLITEM* Item,int MsgNum);
  public:
  DialogControlManager(TWindow* Parent,int NumControls,
                        IPCMessage* IPCMsg=0,TMsgWin* MsgWindow=0,
                        TModule* ModulePtr=0);
  ~DialogControlManager();
  // TEdit control created for you
  TEdit* Add(int Id,int Len,FLDNUM Fn,int MsgNum,
             BYTE GroupNum=1,FLDTYPE Type=FLDTYPE_STRING);
  TEdit* Add(int Id,int Len,FLDNUM Fn,
             BYTE GroupNum=1,FLDTYPE Type=FLDTYPE_STRING);
  TEdit* Add(int Id,TEdit* EditCtl,FLDNUM Fn,int MsgNum,
             BYTE GroupNum=1,FLDTYPE Type=FLDTYPE_STRING);
  TEdit* Add(int Id,TEdit* EditCtl,FLDNUM Fn,
             BYTE GroupNum=1,FLDTYPE Type=FLDTYPE_STRING);

  // Combo Box created for you

  TComboBoxEx* AddComboBox(int Id,FLDNUM Fn,int MsgNum,
                           BYTE GroupNum=1,FLDTYPE Type=FLDTYPE_BYTE);
  TComboBoxEx* AddComboBox(int Id,FLDNUM Fn,BYTE GroupNum=1,
                           FLDTYPE Type=FLDTYPE_BYTE);
  TComboBoxEx* AddComboBox(int Id,TComboBoxEx* ComboBox,FLDNUM Fn,
                   int MsgNum,BYTE GroupNum=1,FLDTYPE Type=FLDTYPE_BYTE);
  TComboBoxEx* AddComboBox(int Id,TComboBoxEx* ComboBox,FLDNUM Fn,
                           BYTE GroupNum=1,FLDTYPE Type=FLDTYPE_BYTE);

  TStatic* AddStatic(int Id,int AssociateId=0,BYTE GroupNum=1);

  TButton* AddButton(int Id,int MsgNum,TButton* Button=0);

  // Set Focus by ID or FIeld Number. Optionally, display Message in
  // Message window.
  void SetFocus(int Id,int MsgNum=-1);
  void SetFocus(FLDNUM Fn,int MsgNum=-1);

  // Enable Fields
  void EnableAll(BYTE GroupNum=0);
  void Enable(FLDNUM Fn);
  void Enable(int Id);

  // Disable Fields
  void DisableAll(BYTE GroupNum=0);
  void Disable(FLDNUM Fn);
  void Disable(int Id);

  // Clear ALL Fields
  void ClearAll(BYTE GroupNum=0);
  void Clear(FLDNUM Fn);
  void Clear(int Id);

  // Copy a Field from the Message to a Window
  bool CopyFromMsg(int Id,IPCMessage* Msg=0);
  // Copy a Field To the message from a window
  bool CopyToMsg(int Id,IPCMessage* Msg=0);
  // Copy a Field from the Message to a Window
  bool CopyFromMsg(FLDNUM Fn,IPCMessage* Msg=0);
  // Copy a Field To the message from a window
  bool CopyToMsg(FLDNUM Fn,IPCMessage* Msg=0);


  // Copy ALL fields from the Message to the window
  void CopyAllFromMsg(IPCMessage* Msg=0,BYTE GroupNum=0);

  // Copy ALL fields to the message from the window
  void CopyAllToMsg(IPCMessage* Msg=0,BYTE GroupNum=0);

  // Set the IPC Message
  void SetIPCMessage(IPCMessage* Msg);

  // Set all fields that have data in a message to Read Only
  void SetAllPresentToReadOnly(IPCMessage* Msg=0);

  // Set all fields to Read Only
  void SetAllReadOnly(BYTE GroupNum=0);
  void ClearAllReadOnly(BYTE GroupNum=0);
  int  HasFocus();

  TEdit* EditPtr(int Id);
  TComboBoxEx* ComboBoxPtr(int Id);
  TEdit* EditPtr(FLDNUM Fn);
  TComboBoxEx* ComboBoxPtr(FLDNUM Fn);
  TStatic* StaticPtr(int Id);
  TButton* ButtonPtr(int Id);

  bool FetchCtl(int Id,int&);
  bool FetchCtl(int Id,char*);
 };
#endif

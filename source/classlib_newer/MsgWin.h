//----------------------------------------------------------------------------
//  Project ClassLib
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    ClassLib.apx Application
//  FILE:         msgwin.h
//  AUTHOR:
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for TMsgWin (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(MsgWin_h)              // Sentry, use file only if it's not already included.
#define MsgWin_h

#include <owl/edit.h>

#include "StringDB.h"

//{{TWindow = TMsgWin}}
class TMsgWin : public TDialog {

  TEdit* TextWin;
  StringDB* StringHandler;
  bool  ShouldDeleteStringHandler;
  TPopupMenu* PopUpMenu;

  char  MsgFileName[MAX_PATH];

  void  CmReloadMessages();
  void  CmEditMessages();
  void  CmSaveMessages();

  bool  MessageChangeFlag;

  UINT  CurrentMsgNum;
  int   MoveIncrement;

  struct
   {
    int curPosition;
    int increment;
    int origY;
    int origX;
   } MoveInfo;

  bool isBuddy; 
  TMsgWin* Buddy; 

  public:
    TMsgWin(TWindow* parent,int Id,char* MsgFile,TModule* Module=0);
    TMsgWin(TMsgWin*,int Id,int MoveIncrement=0);
    void Move(int How=Toggle);
    virtual ~TMsgWin();
    virtual void Say(int);
    virtual void Say(char*);

        // Literals for Reposition 'How' parm
    enum
     {
      Toggle=1,
      Down=2,
      Up=3
     };

//{{TMsgWinVIRTUAL_BEGIN}}
  public:
    virtual bool CanClose();
    virtual TResult EvCommand(uint id, THandle hWndCtl, uint notifyCode);
    virtual void SetupWindow();
//{{TMsgWinVIRTUAL_END}}

//{{TMsgWinRSP_TBL_BEGIN}}
  protected:
  void EvRButtonDown(uint modKeys, TPoint& point);
  void EvLButtonDown(uint modKeys, TPoint& point);
//{{TMsgWinRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(TMsgWin);
};    //{{TMsgWin}}


#endif  // MsgWin_h sentry.



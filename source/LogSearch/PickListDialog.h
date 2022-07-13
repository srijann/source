//----------------------------------------------------------------------------
//  Project LogSearch
//  Secure Payment Systems
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    LogSearch.apx Application
//  FILE:         picklistdialog.h
//  AUTHOR:       Jack Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for PickListDialog (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(picklistdialog_h)              // Sentry, use file only if it's not already included.
#define picklistdialog_h

#include <owl/dialog.h>
#include <owl/listbox.h>

#include <owl/commctrl.h>
#include <owl/button.h>

#include "logsearchapp.rh"            // Definition of all resources.
#include "SearchInfo.h"
#include "RcodeInfo.h"


//{{TDialog = PickListDialog}}
class PickListDialog : public TDialog {
  TListBox* LISTBOX;
  TButton* QUITBTN;
  TButton* SELECTBTN;
  TButton* CANCELBTN;

  RcodeInfo rcInfo;
  bool quitSearch;
  bool continueBTN;
  bool eof;
  IPCMessage Msg;
  IPCMessage* msgStack;
  uint maxMsg;
  uint* msgCount;
  SEARCHINFO* searchInfo;

  int retryCount;

  void setupSearchKeys();
  void setupSearchDate();
  void addToList(IPCMessage*);
  void endOfSearch();
  bool isMatch();
  bool isKeyMatch();
  bool isDateMatch();
  void requestNextMsg();

  public:
    PickListDialog(TWindow* parent);
    virtual ~PickListDialog();
    void Build(SEARCHINFO* SearchInfo,IPCMessage* MsgStack,
               uint* MsgCount,uint MaxMsg);
    void SetSelection(int Index);           

//{{PickListDialogVIRTUAL_BEGIN}}
  public:
    virtual void SetupWindow();
    virtual TResult EvCommand(uint id, THandle hWndCtl, uint notifyCode);
//{{PickListDialogVIRTUAL_END}}

//{{PickListDialogRSP_TBL_BEGIN}}
  protected:
    void LBNDblclk();
    void BNQuitClicked();
    void BNCancelClicked();
    void BNSelectClicked();
    LRESULT ProcessMessage(WPARAM MsgType, LPARAM Id);
//{{PickListDialogRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(PickListDialog);
};    //{{PickListDialog}}


#endif  // picklistdialog_h sentry.


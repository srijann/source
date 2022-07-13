//----------------------------------------------------------------------------
//  Project combotest
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    combotest.apx Application
//  FILE:         comboboxex.h
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for TComboBoxEx (TComboBox).
//
//----------------------------------------------------------------------------
#if !defined(comboboxex_h)              // Sentry, use file only if it's not already included.
#define comboboxex_h

#include <classlib/arrays.h>
#include <owl/combobox.h>

//{{TComboBox = TComboBoxEx}}
class TComboBoxEx : public TComboBox {

  int   DefaultValue;
  char *GetStringBuf;
  int   GetStringBufSize;

  UINT    NotifyMsgNum;
  WPARAM  NotifyWP;
  LPARAM  NotifyLP;

  public:
    TComboBoxEx(TWindow* parent, int id,TModule* Module);
    virtual ~TComboBoxEx();
    void Clear();
    void SetDefaultValue(int);
    uint AddItem(char*,int Data=0);
    bool Select(int);
    bool Select(char);
    bool GetSelectedValue(int& Data);

//{{TComboBoxExVIRTUAL_BEGIN}}
//{{TComboBoxExVIRTUAL_END}}

//{{TComboBoxExRSP_TBL_BEGIN}}
  protected:
  void EvSetFocus(THandle);
  void EvChar(uint,uint,uint);
//{{TComboBoxExRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(TComboBoxEx);
};    //{{TComboBoxEx}}


#endif  // comboboxex_h sentry.


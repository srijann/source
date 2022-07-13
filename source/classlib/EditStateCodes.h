//----------------------------------------------------------------------------
//  Project Classlib
//
//  Copyright © 1996. All Rights Reserved.
//
//  FILE:         EditStateCodes.h
//  AUTHOR:
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for TEditStateCodes (TEdit)
//
//----------------------------------------------------------------------------
#if !defined(EditStateCodes_h)              // Sentry, use file only if it's not already included.
#define EditStateCodes_h

#include <owl/edit.h>
#include <owl/menu.h>

#define POS_UNDER 0
#define POS_OVER  1

//{{TEdit = TEditStateCodes}}
class TEditStateCodes : public TEdit {
  bool CanadianFlag;
  int  PosType;
  bool IsCanadian(char* State);
  TPopupMenu* Menu;
  public:
    TEditStateCodes(TWindow* parent, int ResourceId,int Pos=POS_UNDER);
    void SetCanadian(bool);
    bool GetCanadian();
    void SetPosType(int);
    int  GetPosType();
    virtual TResult EvCommand(uint id, THandle hWndCtl, uint notifyCode);
    virtual ~TEditStateCodes();
  protected:
    void EvChar(uint key, uint repeatCount, uint flags);
DECLARE_RESPONSE_TABLE(TEditStateCodes);
};    //{{TEditStateCodes}}


#endif  // EditStateCodes_h sentry.


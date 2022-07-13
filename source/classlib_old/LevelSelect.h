//----------------------------------------------------------------------------
//  Project spsmanager
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    spsmanager.apx Application
//  FILE:         levelselect.h
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for TLevelSelect (TEdit).
//
//----------------------------------------------------------------------------
#if !defined(levelselect_h)              // Sentry, use file only if it's not already included.
#define levelselect_h
#include "EditUns.h"

//{{TEdit = TLevelSelect}}
class TLevelSelect : public TEditUns {
  int Selection;
  int MaxLevel;
  char* SelectionString;
  void SetSel();
  public:
    TLevelSelect(TWindow* parent, int id, char* Selections, TModule* module = 0);
    virtual ~TLevelSelect();
    void SetLevel(int Level);
    void SetLevel(unsigned char Value);
    int GetLevel(unsigned char* Value=0);

//{{TLevelSelectRSP_TBL_BEGIN}}
  protected:
    void EvLButtonDblClk(uint modKeys, TPoint& point);
    void EvLButtonDown(uint modKeys, TPoint& point);
    void EvRButtonDown(uint modKeys, TPoint& point);
    void EvRButtonDblClk(uint modKeys, TPoint& point);
    void EvChar(uint key,uint,uint);
//{{TLevelSelectRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(TLevelSelect);
};    //{{TLevelSelect}}
#endif


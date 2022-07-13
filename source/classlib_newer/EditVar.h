//----------------------------------------------------------------------------
//  Project SpsManager
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    SpsManager.apx Application
//  FILE:         editvar.h
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for TEditVar (PlainEdit).
//
//----------------------------------------------------------------------------
#ifndef __EDITVAR_H
#define __EDITVAR_H

#include "PlainEdit.h"

// Defines what the NULL Values (if any) are
#define EVOPT_ZEROISNULL        0x1
#define EVOPT_MAXISNULL         0x2
#define EVOPT_ZEROFILL          0x4 

//{{PlainEdit = TEditVar}}
class TEditVar : public PlainEdit {

  unsigned char* VarPtr;
  unsigned short Type;
  bool setupComplete;
  char* transferBuf;
  uint  Options;

  public:
    TEditVar(TWindow* parent, unsigned char& Var,int id, uint textLimit,uint options=0);
    TEditVar(TWindow* parent, unsigned short& Var,int id, uint textLimit,uint options=0);
    TEditVar(TWindow* parent, unsigned int& Var,int id, uint textLimit,uint options=0);
    TEditVar(TWindow* parent, unsigned long& Var,int id, uint textLimit,uint options=0);
    TEditVar(TWindow* parent, signed char& Var,int id, uint textLimit,uint options=0);
    TEditVar(TWindow* parent, short& Var,int id, uint textLimit,uint options=0);
    TEditVar(TWindow* parent, int& Var,int id, uint textLimit,uint options=0);
    TEditVar(TWindow* parent, long& Var,int id, uint textLimit,uint options=0);
    TEditVar(TWindow* parent, char* Var,int id, uint textLimit);
    virtual ~TEditVar();

    enum
     {
      TYPE_UNSCHAR,
      TYPE_UNSSHORT,
      TYPE_UNSINT,
      TYPE_UNSLONG,
      TYPE_CHAR,
      TYPE_SHORT,
      TYPE_INT,
      TYPE_LONG,
      TYPE_STRING
     };

    void Display();
    void transfer();
    uint GetType() { return Type; };
    uint Sizeof();

//{{TEditVarVIRTUAL_BEGIN}}
  public:
    void EvKillFocus(HWND hWndGetFocus);
    void EvChar(uint key, uint repeatCount, uint flags);
    void EvKeyDown(uint key, uint repeatCount, uint flags);
    void EvRButtonDown(uint,TPoint&);
    virtual void SetupWindow();
//{{TEditVarVIRTUAL_END}}
DECLARE_RESPONSE_TABLE(TEditVar);
};    //{{TEditVar}}


#endif  // editvar_h sentry.


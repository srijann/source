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
//  Class definition for TEditVar (TEdit).
//
//----------------------------------------------------------------------------
#if !defined(editvar_h)              // Sentry, use file only if it's not already included.
#define editvar_h

#include <owl/edit.h>

// Defines what the NULL Values (if any) are
#define EVAR_ZEROISNULL        0x1
#define EVAR_MAXISNULL         0x2

//{{TEdit = TEditVar}}
class TEditVar : public TEdit {
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

//{{TEditVarVIRTUAL_BEGIN}}
  public:
    void EvChar(uint key, uint repeatCount, uint flags);
    virtual void SetupWindow();
//{{TEditVarVIRTUAL_END}}
DECLARE_RESPONSE_TABLE(TEditVar);
};    //{{TEditVar}}


#endif  // editvar_h sentry.


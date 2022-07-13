//----------------------------------------------------------------------------
//  Project spsmanager
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    spsmanager.apx Application
//  FILE:         edituns.h
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for TEditUns (TEdit).
//
//----------------------------------------------------------------------------
#if !defined(edituns_h)              // Sentry, use file only if it's not already included.
#define edituns_h

#include "PlainEdit.h"
#include "UnsNum.h"

//{{TEdit = TEditUns}}
class TEditUns : public PlainEdit {
  char FmtBuf[11];
  public:
    TEditUns(TWindow* parent, int resourceId, uint textLimit,
             TModule* module = 0);
    virtual ~TEditUns();
    void get(unsigned long&);
    void get(unsigned int&);
    void get(unsigned short&);
    void get(unsigned char&);
    void Get(unsigned long&);
    void Get(unsigned int&);
    void Get(unsigned short&);
    void Get(unsigned char&);
    void set(unsigned long,int width=0);
    void set(unsigned int,int width=0);
    void set(unsigned short,int width=0);
    void set(unsigned char,int width=0);
    void Set(unsigned long,int width=0);
    void Set(unsigned int,int width=0);
    void Set(unsigned short,int width=0);
    void Set(unsigned char,int width=0);
};    //{{TEditUns}}


#endif  // edituns_h sentry.


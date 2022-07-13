//----------------------------------------------------------------------------
//  Project LogSearch
//  Secure Payment Systems
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    LogSearch.apx Application
//  FILE:         posrulesdialog.h
//  AUTHOR:       Jack Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for PosRulesDialog (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(posrulesdialog_h)              // Sentry, use file only if it's not already included.
#define posrulesdialog_h

#include <owl/dialog.h>
#include <owl/static.h>

#include "posrulesdialog.rh"            // Definition of all resources.

#define NUMROWS 9
#define NUMCOLS 6

//{{TDialog = PosRulesDialog}}
class PosRulesDialog : public TDialog {
  TStatic* POSMAXOVERRIDES;
  TStatic* POSPERIOD;
  TStatic* OVERRIDEACCUM;
  TStatic* WAITBEFOREPOSITIVE;
  TStatic* MULTIPLYNUMAPPROVED;
  TStatic* MULTIPLYDOLLARSAPPROVED;
  TStatic* MULTIPLYMONTHS;

  struct
   {
    TStatic* AMT;
    TStatic* MULTI[NUMCOLS];
   } Row[NUMROWS];

  void multiplierOut(TStatic*,unsigned char);
  void NumOut(TStatic*,unsigned long);

  public:
    PosRulesDialog(TWindow* parent);
    virtual ~PosRulesDialog();
    void Display(IPCMessage*);
};    //{{PosRulesDialog}}


#endif  // posrulesdialog_h sentry.


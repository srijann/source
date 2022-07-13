//----------------------------------------------------------------------------
//  Project spsmanager
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    spsmanager.apx Application
//  FILE:         opentreedialog.h
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for opentreedialog (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(opentreedialog_h)              // Sentry, use file only if it's not already included.
#define opentreedialog_h

#include <owl/dialog.h>
#include <owl/treewind.h>
#include <owl/edit.h>
#include <owl/button.h>
#include <owl/commctrl.h>
#include "ListDefs.h"
#include "opentreedialog.rh"            // Definition of all resources.
#include "OpenInfo.h"

//{{TDialog = opentreedialog}}
class OpenTreeDialog : public TDialog {
  TTreeWindow* TreeWind;
  TEdit*   RULESCLASSTEMPLATE;
  TEdit*   RULESCLASSNAME;
  TEdit*   RULESSUBCLASSNAME;
  TEdit*   TEMPLATECLASS;
  TEdit*   TEMPLATESUBCLASS;
  TButton* TEMPLATEBUTTON;
  TButton* OPENBUTTON;
  OpenTreeInfo* InfoPtr;
  int Selection;
  StringTreeList* ClassList;
  void BuildTree();
  public:
    OpenTreeDialog(TWindow* parent, OpenTreeInfo* Info,
                    TResId resId = IDD_OPENTREE, TModule* module = 0);
    virtual ~OpenTreeDialog();

//{{opentreedialogVIRTUAL_BEGIN}}
  public:
    virtual void SetupWindow();
    virtual bool CanClose();
    virtual int Execute();
//{{opentreedialogVIRTUAL_END}}

//{{opentreedialogRSP_TBL_BEGIN}}
  protected:
    void BNOpenClicked();
    void TVNSelchanged(TTwNotify& twn);
    void BNTemplateClicked();
    LRESULT DialogSetFocus(WPARAM MsgType, LPARAM Id);
//{{opentreedialogRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(OpenTreeDialog);
};    //{{opentreedialog}}


#endif  // opentreedialog_h sentry.


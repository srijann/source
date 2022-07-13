//----------------------------------------------------------------------------
//  Project spsmanager
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    spsmanager.apx Application
//  FILE:         openclassdialog.h
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for OpenClassDialog (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(openclassdialog_h)              // Sentry, use file only if it's not already included.
#define openclassdialog_h

#include <owl/dialog.h>
#include <owl/edit.h>

#include <owl/commctrl.h>
#include <owl/button.h>
#include <owl/listbox.h>

#include "OpenClassDialog.rh"            // Definition of all resources.
#include "OpenInfo.h"
#include "ListDefs.h"

//{{TDialog = OpenClassDialog}}
class OpenClassDialog : public TDialog {
  OpenClassInfo* InfoPtr;
  StringList* List;
  char* listName;
  TEdit* OPENCLASSNAME;
  TEdit* OPENCLASSTEMPLATE;
  TButton* OPENCLASSBUTTON;
  TButton* OPENTEMPLATEBUTTON;
  TListBox* OPENCLASSLIST;
  public:
    OpenClassDialog(TWindow* parent, char*Title,
       OpenClassInfo* Info, char* ListName,
       TResId resId = IDD_OPENCLASS, TModule* module = 0);
    virtual ~OpenClassDialog();

//{{OpenClassDialogVIRTUAL_BEGIN}}
  public:
    virtual void SetupWindow();
    virtual int Execute();
    virtual bool CanClose();
//{{OpenClassDialogVIRTUAL_END}}

//{{OpenClassDialogRSP_TBL_BEGIN}}
  protected:
    void BNClicked();
    void LBNDblclk();
    void BNTemplateClicked();
//{{OpenClassDialogRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(OpenClassDialog);
};    //{{OpenClassDialog}}


#endif  // openclassdialog_h sentry.


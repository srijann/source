//----------------------------------------------------------------------------
//  Project SpsManager
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    SpsManager.apx Application
//  FILE:         productclassdialog.h
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for ProductClassDialog (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(productclassdialog_h)              // Sentry, use file only if it's not already included.
#define productclassdialog_h

#include <owl/dialog.h>
#include <owl/treewind.h>

#include <owl/commctrl.h>

#include "ProdClassDialog.rh"            // Definition of all resources.
#include "FormDialog.h"
#include "ProductClassList.h"

//{{TDialog = ProductClassDialog}}
class ProductClassDialog : public FormDialog {
  bool SetupComplete;
  ProductClassList* OrigList;
  ProductClassList* newList;

  TTreeWindow *TreeWin;
  bool Escape;
  bool editInProgress;

  public:
    ProductClassDialog(TWindow* parent, ProductClassList* origList);
    virtual ~ProductClassDialog();
    virtual bool IsModified();
    virtual bool Save();
    virtual bool SaveTemp(TFile* File);
    virtual void ResetModified();

//{{ProductClassDialogVIRTUAL_BEGIN}}
  public:
    virtual void SetupWindow();
    virtual TResult EvCommand(uint id, THandle hWndCtl, uint notifyCode);
//{{ProductClassDialogVIRTUAL_END}}

//{{ProductClassDialogRSP_TBL_BEGIN}}
  protected:
    void TVNEndlabeledit(TTwDispInfoNotify& twdin);
    bool TVNBeginlabeledit(TTwDispInfoNotify& twdin);
//{{ProductClassDialogRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(ProductClassDialog);
};    //{{ProductClassDialog}}


#endif  // productclassdialog_h sentry.


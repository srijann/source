//----------------------------------------------------------------------------
//  Project spsmanager
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    spsmanager.apx Application
//  FILE:         productadjustdialog.h
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for ProductAdjustDialog (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(productadjustdialog_h)              // Sentry, use file only if it's not already included.
#define productadjustdialog_h

#include <owl/dialog.h>
#include <owl/edit.h>
#include <owl/updown.h>
#include <owl/tabctrl.h>

#include "AuthRulesDialog.rh"            // Definition of all resources.
#include "AuthRules.h"
#include "FormDialog.h"
#include "ProductClassList.h"

//{{TDialog = ProductAdjustDialog}}
class ProductAdjustDialog : public FormDialog {
   AuthRules* Rules;

   TEdit*   AdjValue[NUMPRODUCTSUBCLASSES];
   TStatic* ClassName[NUMPRODUCTSUBCLASSES];
   TUpDown* AdjUPDN[NUMPRODUCTCLASSES];

   TTabControl* ProductGroupTab;

   uint CurrentGroup;

   ProductClassList PCList;

   void ClickAdjustment(BYTE& Parm,TEdit* Ctl,bool Up);
   void OutPutAdjustment(BYTE Value,TEdit* Ctl);
   void OutputGroupValues();

  public:
    ProductAdjustDialog(TWindow* parent,AuthRules* TheRules,
           TResId resId = IDD_PRODUCTADJUST);
    virtual ~ProductAdjustDialog();

//{{ProductAdjustDialogVIRTUAL_BEGIN}}
  public:
    virtual void SetupWindow();
//{{ProductAdjustDialogVIRTUAL_END}}

//{{ProductAdjustDialogRSP_TBL_BEGIN}}
  protected:
  void TCNSelchange(TNotify& tcn);
  bool UDNDeltapos(TNmUpDown& udn);  
//{{ProductAdjustDialogRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(ProductAdjustDialog);
};    //{{ProductAdjustDialog}}


#endif  // productadjustdialog_h sentry.


//----------------------------------------------------------------------------
//  Project spsmanager
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    spsmanager.apx Application
//  FILE:         merchnofdialog.h
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for MerchNofDialog (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(merchnofdialog_h)              // Sentry, use file only if it's not already included.
#define merchnofdialog_h

#include <owl/dialog.h>
#include <owl/static.h>

#include "spsmanagerapp.rh"            // Definition of all resources.


//{{TDialog = MerchNofDialog}}
class MerchNofDialog : public TDialog {
  TStatic* Line1;
  TStatic* Line2;
  char* key;
  public:
    MerchNofDialog(TWindow* parent, char* Key, TResId resId = IDD_NEWMERCH, TModule* module = 0);
    virtual ~MerchNofDialog();

//{{MerchNofDialogVIRTUAL_BEGIN}}
  public:
    virtual void SetupWindow();
//{{MerchNofDialogVIRTUAL_END}}
};    //{{MerchNofDialog}}


#endif  // merchnofdialog_h sentry.


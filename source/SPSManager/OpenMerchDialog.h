//----------------------------------------------------------------------------
//  Project spsmanager
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    spsmanager.apx Application
//  FILE:         openmerchdialog.h
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for OpenMerchDialog (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(openmerchdialog_h)              // Sentry, use file only if it's not already included.
#define openmerchdialog_h

#include <owl/dialog.h>
#include <owl/edit.h>

#include "spsmanagerapp.rh"            // Definition of all resources.


//{{TDialog = OpenMerchDialog}}
class OpenMerchDialog : public TDialog {
  TEdit* MerchId;
  char* BufPtr;
  public:
    OpenMerchDialog(TWindow* parent, char* Buf,TResId resId = IDD_OPENMERCH, TModule* module = 0);
    virtual ~OpenMerchDialog();

//{{OpenMerchDialogVIRTUAL_BEGIN}}
  public:
    virtual bool CanClose();
//{{OpenMerchDialogVIRTUAL_END}}
};    //{{OpenMerchDialog}}


#endif  // openmerchdialog_h sentry.


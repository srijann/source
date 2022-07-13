//----------------------------------------------------------------------------
//  Project spsmanager
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    spsmanager.apx Application
//  FILE:         accountdialog.h
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for AccountDialog (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(accountdialog_h)              // Sentry, use file only if it's not already included.
#define accountdialog_h

#include <owl/dialog.h>
#include <owl/edit.h>
#include <owl/combobox.h>
#include <owl/checkbox.h>

#include "StationDialog.rh"

//{{TDialog = AccountDialog}}
class AccountDialog : public TDialog {
  TEdit* ACCOUNTNUMBER;
  TEdit* ACCOUNTNAME;
  TEdit* PRIVATELABELNAME;
  TEdit* STATE;
  TEdit* POSTALREGION;
  TEdit* SIC;

  TComboBox* DATAREQUIRED;
  TComboBox* BASICAUTH;
  TComboBox* SECURITYAUTH;
  TComboBox* PRODUCTCODES;
  TComboBox* CHECKTYPE;
  TComboBox* PRODUCTCLASS;

  TCheckBox* FEDREG1;
  TCheckBox* FEDREG2;
  TCheckBox* FEDREG3;
  TCheckBox* FEDREG4;
  TCheckBox* FEDREG5;
  TCheckBox* FEDREG6;
  TCheckBox* FEDREG7;
  TCheckBox* FEDREG8;
  TCheckBox* FEDREG9;
  TCheckBox* FEDREG10;
  TCheckBox* FEDREG11;
  TCheckBox* FEDREG12;
  TCheckBox* CAIDONLY;
  TCheckBox* USIDONLY;
  TCheckBox* NOIDTENDERED;
  TCheckBox* HOLDCHECK;
  TCheckBox* PREAUTH;




  public:
    AccountDialog(TWindow* parent, TResId resId = IDD_STATION, TModule* module = 0);
    virtual ~AccountDialog();

//{{AccountDialogVIRTUAL_BEGIN}}
  public:
    virtual void SetupWindow();
//{{AccountDialogVIRTUAL_END}}
};    //{{AccountDialog}}


#endif  // accountdialog_h sentry.


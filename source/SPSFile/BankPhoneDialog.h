//----------------------------------------------------------------------------
//  Project SPSFileEdit
//  Secure Payment Systems
//  Copyright © 1997. All Rights Reserved.
//
//  SUBSYSTEM:    SPSFileEdit.apx Application
//  FILE:         bankphonedialog.h
//  AUTHOR:       Jack Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for BankPhoneDialog (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(bankphonedialog_h)              // Sentry, use file only if it's not already included.
#define bankphonedialog_h

#include <owl/dialog.h>
#include <owl/edit.h>

#define NOFLDNAMES
#include "BankPhoneDB.h"
#include "posfilemanapp.rh"            // Definition of all resources.
#include "FileEditDialog.h"


//{{TDialog = BankPhoneDialog}}
class BankPhoneDialog : public FileEditDialog {
  struct BankPhoneRecord newRec;
  struct BankPhoneRecord currentRec;

  TEdit* BANKNUMBER;
  TEdit* BANKNAME;
  TEdit* PHONENUMBER;
  TEdit* COMMENT;

  int hasData(TEdit* Ctl);
  void Display();
  bool editData();
  bool haveCurrentRec;
  bool newRecIsCurrentRec;
  bool hasBankNumber;


  public:
    BankPhoneDialog(TWindow* parent, TResId resId = IDD_BANKPHONE);
    virtual ~BankPhoneDialog();

//{{BankPhoneDialogVIRTUAL_BEGIN}}
  public:
    virtual TResult EvCommand(uint id, THandle hWndCtl, uint notifyCode);
    virtual void ChangeHandler(uint);

//{{BankPhoneDialogVIRTUAL_END}}

//{{BankPhoneDialogRSP_TBL_BEGIN}}
  public:
    void CmEditclearall();
    void CeEditclearall(TCommandEnabler &tce);
    void CmFileadd();
    void CeFileadd(TCommandEnabler &tce);
    void CmFilechange();
    void CeFilechange(TCommandEnabler &tce);
    void CmFiledelete();
    void CeFiledelete(TCommandEnabler &tce);
    void CmGetrecord();
    void CeGetrecord(TCommandEnabler &tce);
//{{BankPhoneDialogRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(BankPhoneDialog);
};    //{{BankPhoneDialog}}


#endif  // bankphonedialog_h sentry.


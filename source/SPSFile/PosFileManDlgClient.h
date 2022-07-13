//----------------------------------------------------------------------------
//  Project PosFileMan
//  Secure Payment Systems
//  Copyright © 1997. All Rights Reserved.
//
//  SUBSYSTEM:    PosFileMan Application
//  FILE:         posfilemandlgclient.h
//  AUTHOR:       Jack Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for TPosFileManDlgClient (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(posfilemandlgclient_h)              // Sentry, use file only if it's not already included.
#define posfilemandlgclient_h

#include <owl/edit.h>
#include "posfilemanapp.rh"                  // Definition of all resources.
#define NOFLDNAMES
#include "PosFile.h"
#include "FileEditDialog.h"


//{{TDialog = TPosFileManDlgClient}}
class TPosFileManDlgClient : public FileEditDialog {
  TEdit*  BANKNUMBER;
  TEdit*  BANKACCOUNT;
  TEdit*  LICENSESTATE;
  TEdit*  LICENSE;
  TEdit*  CONSUMERNAME;
  TEdit*  BUSINESSNAME;
  TEdit*  OVERRIDESALLOWED;
  TEdit*  OVERRIDEDAYS;
  TEdit*  MAXOVERRIDEAMOUNT;
  TEdit*  MAXOVERRIDEACCUM;
  TEdit*  ACCOUNT1;
  TEdit*  ACCOUNT2;
  TEdit*  ACCOUNT3;
  TEdit*  SIC1;
  TEdit*  SIC2;
  TEdit*  SIC3;
  TEdit*  DATEADDED;
  TEdit*  DATELASTAPPROVAL;
  TEdit*  APPROVEDTRANSACTIONS;
  TEdit*  APPROVEDDOLLARS;
  TEdit*  DOB;
  TEdit*  PHONE;

  struct PosfileRecord newRec;
  struct PosfileRecord currentRec;
  void ChangeHandler(uint);
  void EditToNum(TEdit*,unsigned long&);
  void EditToNum(TEdit*,unsigned short&);
  void NumToEdit(TEdit*,unsigned long,int Len=0);
  int  hasData(TEdit*);
  void Display();
  void clearAll();
  bool editData();
  void DateToEdit(TEdit*,unsigned long);
  bool isValidDate(unsigned long);

  bool haveCurrentRec;
  bool hasIds;
  bool newRecIsCurrentRec;

  public:
    TPosFileManDlgClient(TWindow* parent, TResId resId = IDD_POSFILE, TModule* module = 0);
    virtual ~TPosFileManDlgClient();


//{{TPosFileManDlgClientVIRTUAL_BEGIN}}
  public:
    virtual TResult EvCommand(uint id, THandle hWndCtl, uint notifyCode);
//{{TPosFileManDlgClientVIRTUAL_END}}

//{{TPosFileManDlgClientRSP_TBL_BEGIN}}
  public:   
    void CeGetrecord(TCommandEnabler &tce);
    void CeFiledelete(TCommandEnabler &tce);
    void CeFilechange(TCommandEnabler &tce);
    void CeFileadd(TCommandEnabler &tce);
    void CmEditclearall();
    void CeEditclearall(TCommandEnabler &tce);
    void CmGetrecord();
    void CmFileadd();
    void CmFilechange();
    void CmFiledelete();
//{{TPosFileManDlgClientRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(TPosFileManDlgClient);
};    //{{TPosFileManDlgClient}}


#endif  // posfilemandlgclient_h sentry.

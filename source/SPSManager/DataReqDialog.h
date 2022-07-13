//----------------------------------------------------------------------------
//  Project spsmanager
//
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    spsmanager.apx Application
//  FILE:         datareqdialog.h
//  AUTHOR:
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for DataReqDialog (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(datareqdialog_h)              // Sentry, use file only if it's not already included.
#define datareqdialog_h

#include <owl/dialog.h>
#include <owl/tabctrl.h>
#include <owl/checkbox.h>

#include "DataReqDialog.rh"
#include "DataReq.h"
#include "FormDialog.h"
#include "edituns.h"

//{{TDialog = DataReqDialog}}
class DataReqDialog : public FormDialog {
  TTabControl* TabCtl;
  DataRequirements *origRec;
  DataRequirements newRec;

  TCheckBox* CHECKTYPE;
  TCheckBox* LICENSE;
  TCheckBox* DOB;
  TCheckBox* CHECKNUMBER;
  TCheckBox* MICR;
  TCheckBox* LICENSEORMICR;
  TCheckBox* NAME;
  TCheckBox* ADDRESS;
  TCheckBox* RESIDENCETYPE;
  TCheckBox* CITY;
  TCheckBox* CONSUMERSTATE;
  TCheckBox* ZIPCODE;
  TCheckBox* PHONE;
  TCheckBox* SSN;
  TCheckBox* PRODUCTCLASS;
  TCheckBox* TRANSPORTMETHOD;
  TCheckBox* BUSINESSNAME;
  TCheckBox* PRIVATELABEL;
  TCheckBox* TITLE;
  TCheckBox* FINANCEAMOUNT;

  TCheckBox* SPOUSEEMPLOYERPHONE;
  TCheckBox* SPOUSESALARY;
  TCheckBox* SPOUSEYEARSEMPLOYED;
  TCheckBox* SPOUSEPOSITION;
  TCheckBox* SPOUSEEMPLOYERADR;
  TCheckBox* SPOUSEEMPLOYER;
  TCheckBox* SPOUSESSN;
  TCheckBox* SPOUSENAME;

  TCheckBox* POSITION;
  TCheckBox* YEARSEMPLOYED;
  TCheckBox* SALARY;
  TCheckBox* EMPLOYERPHONE;
  TCheckBox* EMPLOYERADR;
  TCheckBox* EMPLOYERNAME;

  TEditUns* CHECKTYPEAMT;
  TEditUns* LICENSESTATEAMT;
  TEditUns* LICENSEAMT;
  TEditUns* DOBAMT;
  TEditUns* CHECKNUMBERAMT;
  TEditUns* MICRAMT;
  TEditUns* LICENSEORMICRAMT;
  TEditUns* NAMEAMT;
  TEditUns* ADDRESSAMT;
  TEditUns* RESIDENCETYPEAMT;
  TEditUns* CITYAMT;
  TEditUns* CONSUMERSTATEAMT;
  TEditUns* ZIPCODEAMT;
  TEditUns* AREACODEAMT;
  TEditUns* PHONEAMT;
  TEditUns* SSNAMT;
  TEditUns* PRODUCTCLASSAMT;
  TEditUns* TRANSPORTMETHODAMT;
  TEditUns* BUSINESSNAMEAMT;
  TEditUns* PRIVATELABELAMT;
  TEditUns* TITLEAMT;
  TEditUns* SPOUSEDATAAMT;
  TEditUns* EMPLOYMENTDATAAMT;
  TEditUns* FINANCEAMOUNTAMT;

  TCheckBox* DOBIFMICRONLY;

  TStatic*   STATIC1;
  TStatic*   STATIC2;
  TStatic*   STATIC3;
  TStatic*   STATIC4;

  void UpdateWin();
  void ChangeHandler(uint Id);
  int CurType;
  bool paintingFlag;
  void setEmployerGroup();
  void enableEmployerGroup();
  void setSpouseGroup();
  void enableSpouseGroup();


  public:
    DataReqDialog(TWindow* parent, DataRequirements* OrigRec );
    virtual ~DataReqDialog();
    virtual bool IsModified();
    virtual bool Save();
    virtual bool SaveTemp(TFile* File);
    virtual void ResetModified();
    virtual bool Validate();
//{{DataReqDialogVIRTUAL_BEGIN}}
  public:
    virtual void SetupWindow();
    TResult EvCommand(uint id, THandle hWndCtl, uint notifyCode);
//{{DataReqDialogVIRTUAL_END}}

//{{DataReqDialogRSP_TBL_BEGIN}}
  protected:
    void TCNSelchange(TNotify& tcn);
    bool TCNSelchanging(TNotify&);
    void EvKillFocus(THandle hWndGetFocus );
    void EvChar(uint key, uint repeatCount, uint flags);
//{{DataReqDialogRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(DataReqDialog);
};    //{{DataReqDialog}}


#endif  // datareqdialog_h sentry.


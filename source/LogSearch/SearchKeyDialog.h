//----------------------------------------------------------------------------
//  Project LogSearch
//  Secure Payment Systems
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    LogSearch.apx Application
//  FILE:         searchkeydialog.h
//  AUTHOR:       Jack Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for SearchKeyDialog (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(searchkeydialog_h)              // Sentry, use file only if it's not already included.
#define searchkeydialog_h

#include <owl/dialog.h>
#include <owl/edit.h>

#include <owl/commctrl.h>
#include <owl/radiobut.h>
#include <owl/checkbox.h>

#include "SearchKeyDialog.rh"
#include "SearchInfo.h"

//{{TDialog = SearchKeyDialog}}
class SearchKeyDialog : public TDialog {
  SEARCHINFO  newInfo;
  SEARCHINFO& oldInfo;

  TEdit* KEYMERCHID;
  TEdit* KEYLICENSESTATE;
  TEdit* KEYLICENSE;
  TEdit* KEYBANKNUMBER;
  TEdit* KEYBANKACCOUNT;
  TEdit* KEYSSN;
  TEdit* KEYPHONE;
  TEdit* KEYAPPROVALNUM;
  TEdit* KEYOPERATORID;
  TEdit* FROMDATE;
  TEdit* DATE;
  TEdit* TODATE;
  TEdit* LASTNDAYS;
  TRadioButton* FULLKEYBTN;
  TRadioButton* PARTIALKEYBTN;
  TCheckBox* KEYALLBTN;

  TRadioButton* SEARCHENTIREBTN;
  TRadioButton* CURRENTDAYBTN;
  TRadioButton* CURRENTYEARBTN;
  TRadioButton* LASTNDAYSBTN;
  TRadioButton* DATEBTN;
  TRadioButton* RANGEBTN;

  bool keyFieldsLocked;
  bool editKeys();
  bool editDates();
  bool getKey(TEdit*,char*,bool mustFill=true);
  void ChangeHandler(uint);
  void clearAllKeys(TEdit*,uint not1=0,uint not2=0);
  void clearDateGroup();
  bool isValidDate(char*);
  void mmddyyTOyyyymmdd(char* mmddyy,char *yyyymmdd);
  void yyyymmddTOmmddyy(char* yyyymmdd,char *mmddyy);    

  public:
    SearchKeyDialog(TWindow* parent,SEARCHINFO& Info);
    virtual ~SearchKeyDialog();

//{{SearchKeyDialogVIRTUAL_BEGIN}}
  public:
    virtual void SetupWindow();
    virtual TResult EvCommand(uint id, THandle hWndCtl, uint notifyCode);
    virtual bool CanClose();
//{{SearchKeyDialogVIRTUAL_END}}

//{{SearchKeyDialogRSP_TBL_BEGIN}}
  protected:
    void BNKeyAllClicked();
    void BNSearcnEntireClicked();
    void BNFindClicked();
    void BNFullKeyClicked();
    void BNPartialKeyClicked();
    void BNCurrentDayClicked();
    void BNLastNDaysClicked();
    void BNCurrentYearClicked();
    void BNRangeClicked();
    void BNDateClicked();
//{{SearchKeyDialogRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(SearchKeyDialog);
};    //{{SearchKeyDialog}}


#endif  // searchkeydialog_h sentry.


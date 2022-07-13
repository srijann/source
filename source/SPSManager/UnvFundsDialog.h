//----------------------------------------------------------------------------
//  Project SpsManager
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    SpsManager.apx Application
//  FILE:         unvfundsdialog.h
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for UnvFundsDialog (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(unvfundsdialog_h)              // Sentry, use file only if it's not already included.
#define unvfundsdialog_h

#include <owl/dialog.h>
#include <owl/edit.h>
#include <owl/validate.h>

#include "UnvFundsDialog.rh"            // Definition of all resources.
#include "UnvFundsRules.h"
#include "FormDialog.h"
#include "EditVar.h"


//{{TDialog = UnvFundsDialog}}
class UnvFundsDialog : public FormDialog {
  void CalcMinMax();
  void SortGroup(uint Id);
  bool SetupComplete;
  uint idWithFocus;

  UnvFundsRules* origRec;
  UnvFundsRules  newRec;

  class sortedGroup
   {
    uint NumItems;
    uint StartingValueId;
    uint StartingPointsId;
    uint ValuesType;
    union
     {
      unsigned char* cValues;
      unsigned short* sValues;
      unsigned long* lValues;
     } Values;
    signed char* Points;
    TEditVar** valueEdit;
    TEditVar** pointsEdit;
    struct sortBufDef
     {
      unsigned long Value;
      signed char Points;
     } *sortBuf;
    public:
    sortedGroup(TWindow* parent,uint numItems,uint startingPointsId,
                signed char* points,uint startingValuesId,
                void* values,uint valuesType,uint valueLen,
                uint valueOptions,bool todValidate);
    ~sortedGroup();
    void SortAndPaint();
    void enableFields();
    bool isEmpty();
   };

  sortedGroup* AmountGroup;
  sortedGroup* CheckNumGroup;
  sortedGroup* AgeGroup;
  sortedGroup* SSNScoreGroup;
  sortedGroup* YearsOnJobGroup;
  sortedGroup* VelocityAmountGroup;
  sortedGroup* VelocityTransGroup;
  sortedGroup* BankVerifyAmountGroup;
  sortedGroup* TimeOfDayGroup;

  TEdit* MinScore;
  TEdit* MaxScore;

  TEditVar* CUTOFFSCORE;
  TEditVar* BANKVERIFYGOODDAYS;
  TEditVar* BANKVERIFYNGDAYS;
  TEditVar* VELOCITYDAYS;

  LRESULT SortGroup(WPARAM,LPARAM);
  LRESULT CalcMinMax(WPARAM,LPARAM);

  uint SortGroupMsgNum;
  uint CalcMinMaxMsgNum;

  public:
    UnvFundsDialog(TWindow* parent, UnvFundsRules* OrigRec );
    virtual ~UnvFundsDialog();
    virtual bool IsModified();
    virtual bool Save();
    virtual bool SaveTemp(TFile* File);
    virtual void ResetModified();
    //virtual bool Validate();

//{{UnvFundsDialogVIRTUAL_BEGIN}}
  public:
    virtual void SetupWindow();
    virtual TResult EvCommand(uint id, THandle hWndCtl, uint notifyCode);
    bool validate();
    void EvChar(uint key, uint repeatCount, uint flags);
//{{UnvFundsDialogVIRTUAL_END}}

//{{UnvFundsDialogRSP_TBL_BEGIN}}
  protected:
//{{UnvFundsDialogRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(UnvFundsDialog);
};    //{{UnvFundsDialog}}


#endif  // unvfundsdialog_h sentry.


//----------------------------------------------------------------------------
//  Project spsmanager
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    spsmanager.apx Application
//  FILE:         datetimedialog.h
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for DateTimeDialog (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(datetimedialog_h)              // Sentry, use file only if it's not already included.
#define datetimedialog_h

#include <owl/dialog.h>
#include <owl/edit.h>

#include <owl/commctrl.h>
#include <owl/updown.h>
#include <owl/validate.h>
#include <owl/button.h>

#include "AuthRulesDialog.rh"            // Definition of all resources.
#include "AuthRules.h"
#include "FormDialog.h"
#include "edituns.h"

//{{TDialog = DateTimeDialog}}
class DateTimeDialog : public FormDialog {
    AuthRules* Rules;

    struct
    {
     WORD* FromPtr;
     WORD* ToPtr;
    } RowTbl[4];

    TEditUns* DT1From;
    TEditUns* DT1To;
    TEdit* DT1Mon;
    TEdit* DT1Tue;
    TEdit* DT1Wed;
    TEdit* DT1Thu;
    TEdit* DT1Fri;
    TEdit* DT1Sat;
    TEdit* DT1Sun;
    TEditUns* DT2From;
    TEditUns* DT2To;
    TEdit* DT2Mon;
    TEdit* DT2Tue;
    TEdit* DT2Wed;
    TEdit* DT2Thu;
    TEdit* DT2Fri;
    TEdit* DT2Sat;
    TEdit* DT2Sun;
    TEditUns* DT3From;
    TEditUns* DT3To;
    TEdit* DT3Mon;
    TEdit* DT3Tue;
    TEdit* DT3Wed;
    TEdit* DT3Thu;
    TEdit* DT3Fri;
    TEdit* DT3Sat;
    TEdit* DT3Sun;
    TEditUns* DT4From;
    TEditUns* DT4To;
    TEdit* DT4Mon;
    TEdit* DT4Tue;
    TEdit* DT4Wed;
    TEdit* DT4Thu;
    TEdit* DT4Fri;
    TEdit* DT4Sat;
    TEdit* DT4Sun;

    TUpDown* DT1FromUPDN;
    TUpDown* DT1ToUPDN;
    TUpDown* DT1MonUPDN;
    TUpDown* DT1TueUPDN;
    TUpDown* DT1WedUPDN;
    TUpDown* DT1ThuUPDN;
    TUpDown* DT1FriUPDN;
    TUpDown* DT1SatUPDN;
    TUpDown* DT1SunUPDN;
    TUpDown* DT2FromUPDN;
    TUpDown* DT2ToUPDN;
    TUpDown* DT2MonUPDN;
    TUpDown* DT2TueUPDN;
    TUpDown* DT2WedUPDN;
    TUpDown* DT2ThuUPDN;
    TUpDown* DT2FriUPDN;
    TUpDown* DT2SatUPDN;
    TUpDown* DT2SunUPDN;
    TUpDown* DT3FromUPDN;
    TUpDown* DT3ToUPDN;
    TUpDown* DT3MonUPDN;
    TUpDown* DT3TueUPDN;
    TUpDown* DT3WedUPDN;
    TUpDown* DT3ThuUPDN;
    TUpDown* DT3FriUPDN;
    TUpDown* DT3SatUPDN;
    TUpDown* DT3SunUPDN;
    TUpDown* DT4FromUPDN;
    TUpDown* DT4ToUPDN;
    TUpDown* DT4MonUPDN;
    TUpDown* DT4TueUPDN;
    TUpDown* DT4WedUPDN;
    TUpDown* DT4ThuUPDN;
    TUpDown* DT4FriUPDN;
    TUpDown* DT4SatUPDN;
    TUpDown* DT4SunUPDN;

    TButton* INSBUTTON1;
    TButton* DELBUTTON1;
    TButton* INSBUTTON2;
    TButton* DELBUTTON2;
    TButton* INSBUTTON3;
    TButton* DELBUTTON3;
    TButton* INSBUTTON4;
    TButton* DELBUTTON4;

    void ClickFromTime(bool Up,int Row);
    void ClickToTime(bool Up,int Row);
    void ClickAdjustment(BYTE& Parm,TEdit* Ctl,bool Up);
    void OutPutAdjustment(BYTE Value,TEdit* Ctl);
    void EnableButtons();

    void InsertRow(int Row);
    void DeleteRow(int Row);
    void DisplayRow(int);
    void OutputRow(WORD tFrom,TEditUns* cFrom,
                   WORD tTo,TEditUns* cTo,
                   BYTE nMon,TEdit* cMon,
                   BYTE nTue,TEdit* cTue,
                   BYTE nWed,TEdit* cWed,
                   BYTE nThu,TEdit* cThu,
                   BYTE nFri,TEdit* cFri,
                   BYTE nSat,TEdit* cSat,
                   BYTE nSun,TEdit* cSun);

    bool IncrTime(WORD& Time);
    bool DecrTime(WORD& Time);

  public:
    DateTimeDialog(FormDialog* parent, AuthRules* TheRules,
                   TResId resId = IDD_DATETIMEADJUST);
    virtual ~DateTimeDialog();

//{{DateTimeDialogVIRTUAL_BEGIN}}
  public:
    virtual void SetupWindow();
//{{DateTimeDialogVIRTUAL_END}}

//{{DateTimeDialogRSP_TBL_BEGIN}}
  protected:
    bool UDNDeltapos(TNmUpDown& udn);
    void BNClickINS1();
    void BNClickINS2();
    void BNClickINS3();
    void BNClickINS4();
    void BNClickDEL1();
    void BNClickDEL2();
    void BNClickDEL3();
    void BNClickDEL4();
//{{DateTimeDialogRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(DateTimeDialog);
};    //{{DateTimeDialog}}


#endif  // datetimedialog_h sentry.


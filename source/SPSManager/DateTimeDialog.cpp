//----------------------------------------------------------------------------
//  Project spsmanager
//
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    spsmanager.apx Application
//  FILE:         datetimedialog.cpp
//  AUTHOR:
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of DateTimeDialog (TDialog).
//
//----------------------------------------------------------------------------
#include "win4me.h"
#pragma hdrstop
#include <stdio.h>
#include "datetimedialog.h"
#include "weekdays.h"

//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(DateTimeDialog, TDialog)
//{{DateTimeDialogRSP_TBL_BEGIN}}
  EV_UDN_DELTAPOS(IDC_DT1FriUPDN, UDNDeltapos),
  EV_UDN_DELTAPOS(IDC_DT1FromUPDN, UDNDeltapos),
  EV_UDN_DELTAPOS(IDC_DT1MonUPDN, UDNDeltapos),
  EV_UDN_DELTAPOS(IDC_DT1SatUPDN, UDNDeltapos),
  EV_UDN_DELTAPOS(IDC_DT1SunUPDN, UDNDeltapos),
  EV_UDN_DELTAPOS(IDC_DT1ThuUPDN, UDNDeltapos),
  EV_UDN_DELTAPOS(IDC_DT1ToUPDN, UDNDeltapos),
  EV_UDN_DELTAPOS(IDC_DT1TueUPDN, UDNDeltapos),
  EV_UDN_DELTAPOS(IDC_DT1WedUPDN, UDNDeltapos),
  EV_UDN_DELTAPOS(IDC_DT2FriUPDN, UDNDeltapos),
  EV_UDN_DELTAPOS(IDC_DT2FromUPDN, UDNDeltapos),
  EV_UDN_DELTAPOS(IDC_DT2MonUPDN, UDNDeltapos),
  EV_UDN_DELTAPOS(IDC_DT2SatUPDN, UDNDeltapos),
  EV_UDN_DELTAPOS(IDC_DT2SunUPDN, UDNDeltapos),
  EV_UDN_DELTAPOS(IDC_DT2ThuUPDN, UDNDeltapos),
  EV_UDN_DELTAPOS(IDC_DT2ToUPDN, UDNDeltapos),
  EV_UDN_DELTAPOS(IDC_DT2TueUPDN, UDNDeltapos),
  EV_UDN_DELTAPOS(IDC_DT2WedUPDN, UDNDeltapos),
  EV_UDN_DELTAPOS(IDC_DT3FriUPDN, UDNDeltapos),
  EV_UDN_DELTAPOS(IDC_DT3FromUPDN, UDNDeltapos),
  EV_UDN_DELTAPOS(IDC_DT3MonUPDN, UDNDeltapos),
  EV_UDN_DELTAPOS(IDC_DT3SatUPDN, UDNDeltapos),
  EV_UDN_DELTAPOS(IDC_DT3SunUPDN, UDNDeltapos),
  EV_UDN_DELTAPOS(IDC_DT3ThuUPDN, UDNDeltapos),
  EV_UDN_DELTAPOS(IDC_DT3ToUPDN, UDNDeltapos),
  EV_UDN_DELTAPOS(IDC_DT3TueUPDN, UDNDeltapos),
  EV_UDN_DELTAPOS(IDC_DT3WedUPDN, UDNDeltapos),
  EV_UDN_DELTAPOS(IDC_DT4FriUPDN, UDNDeltapos),
  EV_UDN_DELTAPOS(IDC_DT4FromUPDN, UDNDeltapos),
  EV_UDN_DELTAPOS(IDC_DT4MonUPDN, UDNDeltapos),
  EV_UDN_DELTAPOS(IDC_DT4SatUPDN, UDNDeltapos),
  EV_UDN_DELTAPOS(IDC_DT4SunUPDN, UDNDeltapos),
  EV_UDN_DELTAPOS(IDC_DT4ThuUPDN, UDNDeltapos),
  EV_UDN_DELTAPOS(IDC_DT4ToUPDN, UDNDeltapos),
  EV_UDN_DELTAPOS(IDC_DT4TueUPDN, UDNDeltapos),
  EV_UDN_DELTAPOS(IDC_DT4WedUPDN, UDNDeltapos),
  EV_BN_CLICKED(IDC_INSBUTTON1, BNClickINS1),
  EV_BN_CLICKED(IDC_INSBUTTON2, BNClickINS2),
  EV_BN_CLICKED(IDC_INSBUTTON3, BNClickINS3),
  EV_BN_CLICKED(IDC_INSBUTTON4, BNClickINS4),
  EV_BN_CLICKED(IDC_DELBUTTON1, BNClickDEL1),
  EV_BN_CLICKED(IDC_DELBUTTON2, BNClickDEL2),
  EV_BN_CLICKED(IDC_DELBUTTON3, BNClickDEL3),
  EV_BN_CLICKED(IDC_DELBUTTON4, BNClickDEL4),
//{{DateTimeDialogRSP_TBL_END}}
END_RESPONSE_TABLE;
//{{DateTimeDialog Implementation}}

// Macros for simplifying the creation of control interface objects
#define TIMECTL(name) name=new TEditUns(this,IDC_##name,5)
#define DAYCTL(name) name=new TEdit(this,IDC_##name,4)
#define UPDN(name) name##UPDN=new TUpDown(this,IDC_##name##UPDN)
#define BTN(name) name=new TButton(this,IDC_##name)

//------------------
// Class Constructor
//------------------
DateTimeDialog::DateTimeDialog(FormDialog* parent, AuthRules* TheRules,
      TResId resId)
:
 FormDialog(parent, resId),
 Rules(TheRules)
{
   // Create Day/Time Edit controls
   TIMECTL(DT1From);
   TIMECTL(DT1To);
   DAYCTL(DT1Mon);
   DAYCTL(DT1Tue);
   DAYCTL(DT1Wed);
   DAYCTL(DT1Thu);
   DAYCTL(DT1Fri);
   DAYCTL(DT1Sat);
   DAYCTL(DT1Sun);
   TIMECTL(DT2From);
   TIMECTL(DT2To);
   DAYCTL(DT2Mon);
   DAYCTL(DT2Tue);
   DAYCTL(DT2Wed);
   DAYCTL(DT2Thu);
   DAYCTL(DT2Fri);
   DAYCTL(DT2Sat);
   DAYCTL(DT2Sun);
   TIMECTL(DT3From);
   TIMECTL(DT3To);
   DAYCTL(DT3Mon);
   DAYCTL(DT3Tue);
   DAYCTL(DT3Wed);
   DAYCTL(DT3Thu);
   DAYCTL(DT3Fri);
   DAYCTL(DT3Sat);
   DAYCTL(DT3Sun);
   TIMECTL(DT4From);
   TIMECTL(DT4To);
   DAYCTL(DT4Mon);
   DAYCTL(DT4Tue);
   DAYCTL(DT4Wed);
   DAYCTL(DT4Thu);
   DAYCTL(DT4Fri);
   DAYCTL(DT4Sat);
   DAYCTL(DT4Sun);

   //Create UpDown interfaces
   UPDN(DT1From);
   UPDN(DT1To);
   UPDN(DT1Mon);
   UPDN(DT1Tue);
   UPDN(DT1Wed);
   UPDN(DT1Thu);
   UPDN(DT1Fri);
   UPDN(DT1Sat);
   UPDN(DT1Sun);
   UPDN(DT2From);
   UPDN(DT2To);
   UPDN(DT2Mon);
   UPDN(DT2Tue);
   UPDN(DT2Wed);
   UPDN(DT2Thu);
   UPDN(DT2Fri);
   UPDN(DT2Sat);
   UPDN(DT2Sun);
   UPDN(DT3From);
   UPDN(DT3To);
   UPDN(DT3Mon);
   UPDN(DT3Tue);
   UPDN(DT3Wed);
   UPDN(DT3Thu);
   UPDN(DT3Fri);
   UPDN(DT3Sat);
   UPDN(DT3Sun);
   UPDN(DT4From);
   UPDN(DT4To);
   UPDN(DT4Mon);
   UPDN(DT4Tue);
   UPDN(DT4Wed);
   UPDN(DT4Thu);
   UPDN(DT4Fri);
   UPDN(DT4Sat);
   UPDN(DT4Sun);

   // Create Button Interfaces
   BTN(INSBUTTON1);
   BTN(DELBUTTON1);
   BTN(INSBUTTON2);
   BTN(DELBUTTON2);
   BTN(INSBUTTON3);
   BTN(DELBUTTON3);
   BTN(INSBUTTON4);
   BTN(DELBUTTON4);

   // Build a table of pointers to the time parameters to simplify
   // the clicking functions.
   RowTbl[0].FromPtr=&Rules->Data.DT.DT1From;
   RowTbl[0].ToPtr=&Rules->Data.DT.DT1To;
   RowTbl[1].FromPtr=&Rules->Data.DT.DT2From;
   RowTbl[1].ToPtr=&Rules->Data.DT.DT2To;
   RowTbl[2].FromPtr=&Rules->Data.DT.DT3From;
   RowTbl[2].ToPtr=&Rules->Data.DT.DT3To;
   RowTbl[3].FromPtr=&Rules->Data.DT.DT4From;
   RowTbl[3].ToPtr=&Rules->Data.DT.DT4To;
}

//-----------------
// Class Destructor
//-----------------
DateTimeDialog::~DateTimeDialog()
{
 Destroy(IDCANCEL);
 // We don't think we allocated anything
}

//----------------------------------
// Click a Time Adjustment up or down
//----------------------------------
void DateTimeDialog::ClickFromTime(bool Up,int Row)
{
 WORD NewTime=*RowTbl[Row].FromPtr;

 // Has to be something to click
 if ( isEmpty(NewTime) )
  return;

 // Increment Time
 if ( Up )
  {
   if ( ! IncrTime(NewTime) ||
        NewTime >= *RowTbl[Row].ToPtr )
    return;
  }
 else
  {
   // Decrement Time
   if ( ! DecrTime(NewTime) )
     return;
   if (Row != 0 &&
       NewTime < *RowTbl[Row-1].ToPtr )
    return;
  }

 *RowTbl[Row].FromPtr = NewTime;
 DisplayRow(Row);
}

//-----------------------------------
// Click a Time Adjustment up or down
//-----------------------------------
void DateTimeDialog::ClickToTime(bool Up,int Row)
{
 WORD NewTime = *RowTbl[Row].ToPtr;

 // Can't click an empty time
 if ( isEmpty(NewTime) )
  return;

 // Increment time
 if ( Up )
  {
   if ( ! IncrTime(NewTime) )
     return;
   if ( Row != 3 &&
        ! isEmpty(*RowTbl[Row+1].FromPtr) &&
        NewTime > *RowTbl[Row+1].FromPtr )
     return;
  }
 else
  {
   if ( ! DecrTime(NewTime) ||
        NewTime <= *RowTbl[Row].FromPtr )
    return;
  }

 *RowTbl[Row].ToPtr=NewTime;
 DisplayRow(Row);
}

//-----------------------------------
// Handle the Adjustment field clicks
//-----------------------------------
void DateTimeDialog::ClickAdjustment(BYTE& Parm,TEdit* Ctl,bool Up)
{
 if ( isEmpty(Parm) )
  {
   Parm = Up ? (BYTE)11 : (BYTE)9;
   OutPutAdjustment(Parm,Ctl);
   return;
  }

 if ( Up )
  Parm++;
 else
  Parm--;

 // Valid values are .1 to 2.0  ... consider 0, 1.0 and 2.1 reset to empty.  
 if ( Parm > 20 || Parm==0 || Parm==10 )
  Empty(Parm);

 OutPutAdjustment(Parm,Ctl);
}

//---------------------------------------
// Output Adjustment factor to the window
//---------------------------------------
void DateTimeDialog::OutPutAdjustment(BYTE Value,TEdit* Ctl)
{
 char FmtBuf[10];

 if ( isEmpty(Value) )
  FmtBuf[0]=0;
 else
  sprintf(FmtBuf,"%u.%u",Value/10,Value%10);
 Ctl->Transfer(FmtBuf,tdSetData); 
}

//-------------------------
// Handle the UpDown Clicks
//-------------------------
#define CLICKDAY(name,dow,row) case IDC_##name##dow##UPDN:\
  if ( ! isEmpty(*RowTbl[row].FromPtr) )\
    ClickAdjustment(Rules->Data.DT.##name##Time[dow][0],name##dow,IsUp);  break
#define CLICKFROMTIME(name,row) case IDC_##name##UPDN:\
  ClickFromTime(IsUp,row); EnableButtons(); break
#define CLICKTOTIME(name,row) case IDC_##name##UPDN:\
  ClickToTime(IsUp,row); EnableButtons(); break
bool DateTimeDialog::UDNDeltapos(TNmUpDown& udn)
{
 bool IsUp;
 switch( udn.iDelta )
  {
   case -1: IsUp=true; break;
   case  1: IsUp=false; break;
   default: return true;
  }

 switch( udn.hdr.idFrom )
  {
   CLICKFROMTIME(DT1From,0);
   CLICKTOTIME(DT1To,0);
   CLICKDAY(DT1,Mon,0);
   CLICKDAY(DT1,Tue,0);
   CLICKDAY(DT1,Wed,0);
   CLICKDAY(DT1,Thu,0);
   CLICKDAY(DT1,Fri,0);
   CLICKDAY(DT1,Sat,0);
   CLICKDAY(DT1,Sun,0);
   CLICKFROMTIME(DT2From,1);
   CLICKTOTIME(DT2To,1);
   CLICKDAY(DT2,Mon,1);
   CLICKDAY(DT2,Tue,1);
   CLICKDAY(DT2,Wed,1);
   CLICKDAY(DT2,Thu,1);
   CLICKDAY(DT2,Fri,1);
   CLICKDAY(DT2,Sat,1);
   CLICKDAY(DT2,Sun,1);
   CLICKFROMTIME(DT3From,2);
   CLICKTOTIME(DT3To,2);
   CLICKDAY(DT3,Mon,2);
   CLICKDAY(DT3,Tue,2);
   CLICKDAY(DT3,Wed,2);
   CLICKDAY(DT3,Thu,2);
   CLICKDAY(DT3,Fri,2);
   CLICKDAY(DT3,Sat,2);
   CLICKDAY(DT3,Sun,2);
   CLICKFROMTIME(DT4From,3);
   CLICKTOTIME(DT4To,3);
   CLICKDAY(DT4,Mon,3);
   CLICKDAY(DT4,Tue,3);
   CLICKDAY(DT4,Wed,3);
   CLICKDAY(DT4,Thu,3);
   CLICKDAY(DT4,Fri,3);
   CLICKDAY(DT4,Sat,3);
   CLICKDAY(DT4,Sun,3);
  }
 return true;
}

//------------------------------------------------
//Override of virtual function for initial display
//------------------------------------------------
void DateTimeDialog::SetupWindow()
{
 int i;

 TDialog::SetupWindow();
 for (i=0; i<4; ++i)
  DisplayRow(i);
 EnableButtons();
}

//------------------------------------------------------------
// A macro for use by the DisplayRow function so we don't have
// to repeat it 4 times
//------------------------------------------------------------
#define ROW(n) OutputRow(Rules->Data.DT.DT##n##From,DT##n##From,\
                         Rules->Data.DT.DT##n##To,DT##n##To,\
                         Rules->Data.DT.DT##n##Time[Mon][0],DT##n##Mon,\
                         Rules->Data.DT.DT##n##Time[Tue][0],DT##n##Tue,\
                         Rules->Data.DT.DT##n##Time[Wed][0],DT##n##Wed,\
                         Rules->Data.DT.DT##n##Time[Thu][0],DT##n##Thu,\
                         Rules->Data.DT.DT##n##Time[Fri][0],DT##n##Fri,\
                         Rules->Data.DT.DT##n##Time[Sat][0],DT##n##Sat,\
                         Rules->Data.DT.DT##n##Time[Sun][0],DT##n##Sun); break

//------------------------
// Display a row of values
//------------------------
void DateTimeDialog::DisplayRow(int n)
{
 switch(n)
  {
   case 0: ROW(1);
   case 1: ROW(2);
   case 2: ROW(3);
   case 3: ROW(4);
  }
}

//-------------------------------------
// Output a row of values to the window
//-------------------------------------
void DateTimeDialog::OutputRow(WORD tFrom,TEditUns* cFrom,
                   WORD tTo,TEditUns* cTo,
                   BYTE nMon,TEdit* cMon,
                   BYTE nTue,TEdit* cTue,
                   BYTE nWed,TEdit* cWed,
                   BYTE nThu,TEdit* cThu,
                   BYTE nFri,TEdit* cFri,
                   BYTE nSat,TEdit* cSat,
                   BYTE nSun,TEdit* cSun)
{
 if ( isEmpty(tFrom) )
  {
   cFrom->Transfer("",tdSetData);
   cTo->Transfer("",tdSetData);
   cMon->Transfer("",tdSetData);
   cTue->Transfer("",tdSetData);
   cWed->Transfer("",tdSetData);
   cThu->Transfer("",tdSetData);
   cFri->Transfer("",tdSetData);
   cSat->Transfer("",tdSetData);
   cSun->Transfer("",tdSetData);
   return;
  }
 cFrom->Set(tFrom,4);
 cTo->Set(tTo,4);
 OutPutAdjustment(nMon,cMon);
 OutPutAdjustment(nTue,cTue);
 OutPutAdjustment(nWed,cWed);
 OutPutAdjustment(nThu,cThu);
 OutPutAdjustment(nFri,cFri);
 OutPutAdjustment(nSat,cSat);
 OutPutAdjustment(nSun,cSun);
}

//--------------------------------------------
// Increment 24 hr military time by 15 minutes
//--------------------------------------------
bool DateTimeDialog::IncrTime(WORD& Time)
{
 if ( Time >= 2400 )
  return false;
 Time += (WORD)15;
 if ( (Time %(WORD)100) == 60 )
  Time += (WORD)40;
 return true;
}

//--------------------------------------------
// Decrement 24 hr military time by 15 minutes
//--------------------------------------------
bool DateTimeDialog::DecrTime(WORD& Time)
{
 if ( Time==0 )
  return false;
 Time -= (WORD) 15;
 if ( (Time%(WORD)100) == 85 )
  Time -= (WORD)40;
 return true;
}

//--------------------------------------------------------------
// Enable/Disable appropriate Insert/Delete buttons for all rows
//--------------------------------------------------------------
void DateTimeDialog::EnableButtons()
{
 // The Delete buttons simply depend on the presence of data
 DELBUTTON1->EnableWindow( ! isEmpty(Rules->Data.DT.DT1From) );
 DELBUTTON2->EnableWindow( ! isEmpty(Rules->Data.DT.DT2From) );
 DELBUTTON3->EnableWindow( ! isEmpty(Rules->Data.DT.DT3From) );
 DELBUTTON4->EnableWindow( ! isEmpty(Rules->Data.DT.DT4From) );

 // The insert buttons are smart buttons that are only enabled
 // when insertion is allowed for a row.

 // If all rows are full, no insertion allowed
 if ( ! isEmpty(Rules->Data.DT.DT4From) )
  {
   INSBUTTON1->EnableWindow(false);
   INSBUTTON2->EnableWindow(false);
   INSBUTTON3->EnableWindow(false);
   INSBUTTON4->EnableWindow(false);
   return;
  }

 // If no rows present, allow insertion into first row
 if ( isEmpty(Rules->Data.DT.DT1From) )
  {
   INSBUTTON1->EnableWindow(true);
   INSBUTTON2->EnableWindow(false);
   INSBUTTON3->EnableWindow(false);
   INSBUTTON4->EnableWindow(false);
   return;
  }
 else
  // Can insert into row 1 as long as the time is not 0000
  INSBUTTON1->EnableWindow(Rules->Data.DT.DT1From);


 if ( isEmpty(Rules->Data.DT.DT2From) )
  {
   INSBUTTON2->EnableWindow(Rules->Data.DT.DT1To != 2400);
   INSBUTTON3->EnableWindow(false);
   INSBUTTON4->EnableWindow(false);
   return;
  }
 else
   INSBUTTON2->EnableWindow( Rules->Data.DT.DT2From != Rules->Data.DT.DT1To );

 if ( isEmpty(Rules->Data.DT.DT3From) )
  {
   INSBUTTON3->EnableWindow(Rules->Data.DT.DT2To != 2400);
   INSBUTTON4->EnableWindow(false);
   return;
  }
 else
  INSBUTTON3->EnableWindow( Rules->Data.DT.DT3From != Rules->Data.DT.DT2To );

 INSBUTTON4->EnableWindow( isEmpty(Rules->Data.DT.DT4From) &&
                           Rules->Data.DT.DT3To != 2400 );
}

//----------------------------------------------------
// Macros for simplifying MOVEing and EMPTYing of rows
//----------------------------------------------------
#define MOVE(d,s) Rules->Data.DT.DT##d##From=Rules->Data.DT.DT##s##From;\
                  Rules->Data.DT.DT##d##To=Rules->Data.DT.DT##s##To;\
                  Rules->Data.DT.DT##d##Time[Mon][0]=Rules->Data.DT.DT##s##Time[Mon][0];\
                  Rules->Data.DT.DT##d##Time[Tue][0]=Rules->Data.DT.DT##s##Time[Tue][0];\
                  Rules->Data.DT.DT##d##Time[Wed][0]=Rules->Data.DT.DT##s##Time[Wed][0];\
                  Rules->Data.DT.DT##d##Time[Thu][0]=Rules->Data.DT.DT##s##Time[Thu][0];\
                  Rules->Data.DT.DT##d##Time[Fri][0]=Rules->Data.DT.DT##s##Time[Fri][0];\
                  Rules->Data.DT.DT##d##Time[Sat][0]=Rules->Data.DT.DT##s##Time[Sat][0];\
                  Rules->Data.DT.DT##d##Time[Sun][0]=Rules->Data.DT.DT##s##Time[Sun][0]
#define EMPTY(n)  Empty(Rules->Data.DT.DT##n##From);\
                  Empty(Rules->Data.DT.DT##n##To);\
                  Empty(Rules->Data.DT.DT##n##Time[Mon][0]);\
                  Empty(Rules->Data.DT.DT##n##Time[Tue][0]);\
                  Empty(Rules->Data.DT.DT##n##Time[Wed][0]);\
                  Empty(Rules->Data.DT.DT##n##Time[Thu][0]);\
                  Empty(Rules->Data.DT.DT##n##Time[Fri][0]);\
                  Empty(Rules->Data.DT.DT##n##Time[Sat][0]);\
                  Empty(Rules->Data.DT.DT##n##Time[Sun][0])


//------------------------
// Insert Button for row 1
//------------------------
void DateTimeDialog::BNClickINS1()
{
 if ( isEmpty(Rules->Data.DT.DT1From) )
  {
   Rules->Data.DT.DT1From=0;
   Rules->Data.DT.DT1To=2400;
   DisplayRow(0);
   EnableButtons();
   return;
  }
 MOVE(4,3);
 MOVE(3,2);
 MOVE(2,1);
 EMPTY(1);
 Rules->Data.DT.DT1From=0;
 if ( isEmpty(Rules->Data.DT.DT2From) )
   Rules->Data.DT.DT2To=2400;
 else
   Rules->Data.DT.DT1To=Rules->Data.DT.DT2From;
 DisplayRow(0);
 DisplayRow(1);
 DisplayRow(2);
 DisplayRow(3);
 EnableButtons();
}

//------------------------
// Insert Button for row 2
//------------------------
void DateTimeDialog::BNClickINS2()
{
 if ( isEmpty(Rules->Data.DT.DT2From) )
  {
   Rules->Data.DT.DT2From=Rules->Data.DT.DT1To;
   Rules->Data.DT.DT2To=2400;
   DisplayRow(1);
   EnableButtons();
   return;
  }
 MOVE(4,3);
 MOVE(3,2);
 EMPTY(2);
 Rules->Data.DT.DT2From=Rules->Data.DT.DT1To;
 if ( isEmpty(Rules->Data.DT.DT3From) )
   Rules->Data.DT.DT2To=2400;
 else
   Rules->Data.DT.DT2To=Rules->Data.DT.DT3From;
 DisplayRow(1);
 DisplayRow(2);
 DisplayRow(3);
 EnableButtons();
}

//------------------------
// Insert Button for row 4
//------------------------
void DateTimeDialog::BNClickINS3()
{
 if ( isEmpty(Rules->Data.DT.DT3From) )
  {
   Rules->Data.DT.DT3From=Rules->Data.DT.DT2To;
   Rules->Data.DT.DT3To=2400;
   DisplayRow(2);
   EnableButtons();
   return;
  }
 MOVE(4,3);
 EMPTY(3);
 Rules->Data.DT.DT3From=Rules->Data.DT.DT2To;
 if ( isEmpty(Rules->Data.DT.DT4From) )
   Rules->Data.DT.DT3To=2400;
 else
   Rules->Data.DT.DT3To=Rules->Data.DT.DT4From;
 DisplayRow(2);
 DisplayRow(3);
 EnableButtons();
}

//------------------------
// Insert Button for row 4
//------------------------
void DateTimeDialog::BNClickINS4()
{
 Rules->Data.DT.DT4From=Rules->Data.DT.DT3To;
 Rules->Data.DT.DT4To=2400;
 DisplayRow(3);
 EnableButtons();
}

//------------------------
// Delete Button for row 1
//------------------------
void DateTimeDialog::BNClickDEL1()
{
 MOVE(1,2);
 MOVE(2,3);
 MOVE(3,4);
 EMPTY(4);
 DisplayRow(0);
 DisplayRow(1);
 DisplayRow(2);
 DisplayRow(3);
 EnableButtons();
}

//------------------------
// Delete Button for row 2
//------------------------
void DateTimeDialog::BNClickDEL2()
{
 MOVE(2,3);
 MOVE(3,4);
 EMPTY(4);
 DisplayRow(1);
 DisplayRow(2);
 DisplayRow(3);
 EnableButtons();
}

//------------------------
// Delete Button for row 3
//------------------------
void DateTimeDialog::BNClickDEL3()
{
 MOVE(3,4);
 EMPTY(4);
 DisplayRow(2);
 DisplayRow(3);
 EnableButtons();
}

//------------------------
// Delete Button for row 4
//------------------------
void DateTimeDialog::BNClickDEL4()
{
 EMPTY(4);
 DisplayRow(3);
 EnableButtons();
}



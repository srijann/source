//----------------------------------------------------------------------------
//  Project SpsManager
//
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    SpsManager.apx Application
//  FILE:         unvfundsdialog.cpp
//  AUTHOR:
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of UnvFundsDialog (TDialog).
//
//----------------------------------------------------------------------------
#include "win4me.h"

#include <owl/validate.h>
#include "UnvFundsDialog.h"
#include "TranType.h"
#include "CheckAuthFn.h"
#include "DBManMsg.h"
#include "SaveFile.h"
#include "UnsNum.h"

// We are making certain assumptions in this module about the assignment
// of ID numbers as follows:
//
// All of the Values belonging to Point/Value pairs are in the range
// of 1000-9999. Groups begin at numbers divisible by 100 and are
// assigned sequentially.
//
// Points are in the range of 10000 to 29999. Groups of points begin
// at numbers divisible by 100 and are assigned sequentially.
//
// All other controls (those not belonging to Value/Points pairs) are
// numbered >= 30000

#define IS_VALUE_ID(id)  (id>=1000 && id<10000)
#define SORT_GROUP(id) (id / 100 * 100)
#define IS_POINTS_ID(id) (id>=10000 && id<30000)


// We have to send some messages to ourself in this dialog because
// of the use of the TEditVar control. We have to wait for the control
// values to get transferred from the window and it's not always been
// done when we need it. Instead of calling functions when things happen,
// sometimes we have to post a message.
#define SORTGROUPMSG "SPSMANSORTGROUP"
#define MINMAXMSG "SPSMANCALCMINMAX"

//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(UnvFundsDialog, TDialog)
//{{UnvFundsDialogRSP_TBL_BEGIN}}
EV_REGISTERED(SORTGROUPMSG,SortGroup),
EV_REGISTERED(MINMAXMSG,CalcMinMax),
//{{UnvFundsDialogRSP_TBL_END}}
END_RESPONSE_TABLE;

//{{UnvFundsDialog Implementation}}

// Validates time of day in 24 hour format
class TimeOfDayValidator : public TValidator {
  public:
  virtual bool IsValidInput(char far* str, bool suppressFill);
 };

// Validate correct time of day format (0000-2359)
// We don't allow invalid characters to be keyed
bool TimeOfDayValidator::IsValidInput(char far* str,bool)
{
 uint Len;
 uint i;

 Len=strlen(str);

 for (i=0; i<Len; ++i)
  {
   if ( ! isdigit(str[i]) )
    return false;
   switch(i)
    {
     case 0:  if (str[0] > '2')
               return false;
              break;
     case 1:  if ( str[0] == '2' &&
                   str[1] > '3' )
                return false;
              break;
     case 2:  if ( str[2] > '5' )
                return false;
              break;
    }
  }

 return true;
}

// Validator for Points
class TPointsValidator : public TValidator {
  public:
  virtual bool IsValidInput(char far* str, bool suppressFill);
 };

// Allow a negative or positive 1 digit number for points
bool TPointsValidator::IsValidInput(char far* str,bool)
{
 switch( strlen(str) )
  {
   case 1: if ( str[0] == '-' )
            return true;

           return isdigit(str[0]);

   case 2: return str[0] == '-' && isdigit(str[1]);

   default: return true;
  }
}

//------------------------------------------------------------
// Class Constructor for the sorted Group of controls
//
// This object handles the complexity of our spread-sheet like
// columns, sorting them for us, etc.
//-------------------------------------------------------------
UnvFundsDialog::sortedGroup::sortedGroup(TWindow* parent,
                uint numItems,uint startingPointsId,
                signed char* points,uint startingValueId,
                void* values,uint valuesType,uint valueLen,
                uint valueOptions,bool todValidate) :
 NumItems(numItems),
 StartingValueId(startingValueId),
 StartingPointsId(startingPointsId),
 ValuesType(valuesType),
 Points(points)
{
 uint ValId, PtId, cnt;

 valueEdit=new TEditVar*[numItems];
 pointsEdit=new TEditVar*[numItems];
 sortBuf = new struct sortBufDef[numItems];

 Values.cValues = (unsigned char*) values;

 for (ValId=startingValueId, PtId=startingPointsId, cnt=0;
        cnt<numItems; ++ValId, ++PtId, ++cnt)
  {
   switch( valuesType )
    {
     case sizeof(unsigned char):
       valueEdit[cnt] = new TEditVar(parent,Values.cValues[cnt],
                            ValId,valueLen,EVOPT_MAXISNULL|valueOptions);
       break;
     case sizeof(unsigned short):
       valueEdit[cnt] = new TEditVar(parent,Values.sValues[cnt],
                            ValId,valueLen,EVOPT_MAXISNULL|valueOptions);
       break;
     case sizeof(unsigned long):
       valueEdit[cnt] = new TEditVar(parent,Values.lValues[cnt],
                            ValId,valueLen,EVOPT_MAXISNULL|valueOptions);
       break;
    }

   if ( todValidate )
     valueEdit[cnt]->SetValidator(new TimeOfDayValidator);

   pointsEdit[cnt] = new TEditVar(parent,points[cnt],PtId,3,
                                    EVOPT_ZEROISNULL);

   pointsEdit[cnt]->SetValidator(new TPointsValidator);
  }
}

UnvFundsDialog::sortedGroup::~sortedGroup()
{
 delete[] valueEdit;
 delete[] pointsEdit;
 delete[] sortBuf;
}

//-----------------------------------------------
// Compare callback function for quick sort below
//-----------------------------------------------
_USERENTRY compare(const void* i1,const void* i2)
{
 if ( *((unsigned long*)i1) > *((unsigned long*)i2) )
  return 1;
 if ( *((unsigned long*)i1) < *((unsigned long*)i2) )
  return -1;
  
 return 0;
}

//-----------------------------------------------------------
// Set readonly and Tab status of fields, depending on whether
// there is a corresponding value present
//-----------------------------------------------------------
void UnvFundsDialog::sortedGroup::enableFields()
{
 uint i;
 bool readOnly;

 // Do the points
 for (i=0; i<NumItems; ++i)
  {
   switch(ValuesType)
    {
     case sizeof(char):   readOnly = (Values.cValues[i] == UCHAR_MAX); break;
     case sizeof(short):  readOnly = (Values.sValues[i] == USHRT_MAX); break;
     case sizeof(long):   readOnly = (Values.lValues[i] == ULONG_MAX); break;
    }

   pointsEdit[i]->SetReadOnly(readOnly);
   if ( readOnly )
     pointsEdit[i]->ModifyStyle(WS_TABSTOP,0);
   else
     pointsEdit[i]->ModifyStyle(0,WS_TABSTOP);
  }
}

// Sort the group and repaint the rows
void UnvFundsDialog::sortedGroup::SortAndPaint()
{
 uint i;

 // Ignore request if no changes have been made to any of the values
 for (i=0; i<NumItems; ++i)
  if ( valueEdit[i]->IsModified() )
   break;
 if ( i==NumItems )
  return;

 // Build an array for quick sorting
 memset(sortBuf,0xff,NumItems * sizeof(sortBufDef));
 for (i=0; i < NumItems-1; ++i)
  {
   sortBuf[i].Points = Points[i];
   switch( ValuesType )
    {
     case sizeof(char):
        if ( Values.cValues[i] != UCHAR_MAX )
         {
          sortBuf[i].Value = Values.cValues[i];
          sortBuf[i].Points = Points[i];
         }
        else
         sortBuf[i].Points=0;
        break;

     case sizeof(short):
        if ( Values.sValues[i] != USHRT_MAX )
         {
          sortBuf[i].Value = Values.sValues[i];
          sortBuf[i].Points = Points[i];
         }
        else
         sortBuf[i].Points=0;
        break;

     case sizeof(long):
        sortBuf[i].Value = Values.lValues[i];
        if ( sortBuf[i].Value != ULONG_MAX )
         sortBuf[i].Points = Points[i];
        else
         sortBuf[i].Points = 0;
        break;
    }
  }

 // Sort
 qsort(sortBuf,NumItems-1,sizeof(sortBuf[0]),compare);

 for (i=0; i < NumItems-1; ++i)
  {
   Points[i] = sortBuf[i].Points;
   switch( ValuesType )
    {
     case sizeof(char):
        Values.cValues[i] = (unsigned char) sortBuf[i].Value; break;
     case sizeof(short):
        Values.sValues[i] = (unsigned short) sortBuf[i].Value; break;
     case sizeof(long):
        Values.lValues[i] = sortBuf[i].Value; break;
    }
  }

 // re-Display All
 for (i=0; i < NumItems-1; ++i)
  {
   valueEdit[i]->Display();
   pointsEdit[i]->Display();
  }

 // Get the highest value for the bottom row
 sortBuf[NumItems-1].Value = ULONG_MAX;
 for (i=NumItems-1; (int)i >= 0; --(int)i)
  if ( sortBuf[i].Value != ULONG_MAX )
   {
    sortBuf[NumItems-1].Value = sortBuf[i].Value;
    break;
   }
 if ( i==NumItems-1 )
  sortBuf[NumItems-1].Value = ULONG_MAX;

 switch( ValuesType )
  {
   case sizeof(char):
      Values.cValues[NumItems-1] =
         (unsigned char) sortBuf[NumItems-1].Value; break;
   case sizeof(short):
      Values.sValues[NumItems-1] =
         (unsigned short) sortBuf[NumItems-1].Value; break;
   case sizeof(long):
      Values.lValues[NumItems-1] = sortBuf[NumItems-1].Value; break;
  }

 valueEdit[NumItems-1]->Display();
 enableFields();
 for (i=0; i<NumItems; ++i)
   valueEdit[i]->ClearModify();
}

// Determine if all the values in a group are empty
bool UnvFundsDialog::sortedGroup::isEmpty()
{
 uint i;

 for (i=0; i<NumItems-1; ++i)
  switch(ValuesType)
   {
    case sizeof(char):   if ( ! ::isEmpty(Values.cValues[i]) ) return false;
                         break;

    case sizeof(short):  if ( ! ::isEmpty(Values.sValues[i]) ) return false;
                         break;

    case sizeof(long):   if ( ! ::isEmpty(Values.lValues[i]) ) return false;
   }

 return true;
}

// MACRO to define a GROUP of controls
#define DEFGROUP(NUM,VALID,LEN,PTID,POINTS,VALUES,OPT,VALIDATOR)\
 new sortedGroup(this,NUM,PTID,(signed char*)newRec.Data.POINTS,\
                 VALID,newRec.Data.VALUES,sizeof(newRec.Data.VALUES[0]),\
                 LEN,OPT,VALIDATOR)

// MACRO to define controls for a Pair of Point Values
#define DEFPOINTS(ID,VAR) (new TEditVar(this,\
          (signed char&)newRec.Data.VAR,ID,3,EVOPT_ZEROISNULL))->\
               SetValidator(new TPointsValidator)

// MACRO to define a simple Value control
#define DEFVAL(NAME,VAR,LEN) NAME = new TEditVar(this,\
          newRec.Data.VAR,IDC_##NAME,LEN,EVOPT_MAXISNULL)

//------------------
// Class Constructor
//------------------
UnvFundsDialog::UnvFundsDialog(TWindow* parent,UnvFundsRules* OrigRec)
:
 FormDialog(parent,IDD_UNVFUNDS,RecTypeUnvFundsRules),
 origRec(OrigRec),
 SetupComplete(false)
{
 char Title[100];
 int i;

 // Set up the Window title
 strcpy(Title,(char*)origRec->Data.UnvFundsRulesClassName);
 strcat(Title," Transaction Scoring Rules");
 SetCaption(Title);
 memcpy(&newRec.Data,&origRec->Data,sizeof(newRec.Data));

 // Set up the controls
 AmountGroup = DEFGROUP(NUMAMOUNTROWS,IDC_AMOUNTVALUE,6,IDC_AMOUNTPOINTS,
                        AmountPoints,AmountValue,0,0);

 CheckNumGroup = DEFGROUP(NUMCHECKNUMROWS,IDC_CHECKNUMVALUE,7,
              IDC_CHECKNUMPOINTS,CheckNumPoints,CheckNumValue,0,0);

 AgeGroup = DEFGROUP(NUMAGEROWS,IDC_AGEVALUE,3,IDC_AGEPOINTS,
                       AgePoints,AgeValue,0,0);

 SSNScoreGroup = DEFGROUP(NUMSSNSCOREROWS,IDC_SSNSCOREVALUE,4,
          IDC_SSNSCOREPOINTS,SSNScorePoints,SSNScoreValue,0,0);

 YearsOnJobGroup = DEFGROUP(NUMYEARSONJOBROWS,IDC_YEARSONJOBVALUE,3,
           IDC_YEARSONJOBPOINTS,YearsOnJobPoints,YearsOnJobValue,0,0);

 VelocityAmountGroup = DEFGROUP(NUMVELOCITYAMOUNTROWS,IDC_VELOCITYAMOUNTVALUE,6,
    IDC_VELOCITYAMOUNTPOINTS,VelocityAmountPoints,VelocityAmountValue,0,0);

 VelocityTransGroup = DEFGROUP(NUMVELOCITYTRANSROWS,IDC_VELOCITYTRANSVALUE,3,
    IDC_VELOCITYTRANSPOINTS,VelocityTransPoints,VelocityTransValue,0,0);

 BankVerifyAmountGroup = DEFGROUP(NUMBANKVERIFYAMOUNTROWS,
    IDC_BANKVERIFYGOODYESVALUE,6,
    IDC_BANKVERIFYGOODYESPOINTS,
    BankVerifyAmountPoints,BankVerifyAmountValue,0,0);

 TimeOfDayGroup = DEFGROUP(NUMTIMEOFDAYROWS,IDC_TIMEVALUE,5,
     IDC_TIMEPOINTS,TimePoints,TimeValue,EVOPT_ZEROFILL,true);

 DEFVAL(CUTOFFSCORE,TransactionCutoffValue,5);
 DEFVAL(BANKVERIFYGOODDAYS,BankVerifyGoodDays,3);
 DEFVAL(BANKVERIFYNGDAYS,BankVerifyNGDays,3);
 DEFVAL(VELOCITYDAYS,VelocityDays,3);

 for (i=0; i<7; ++i)
  DEFPOINTS(IDC_DAYPOINTS+i,DayOfWeekPoints[i]);

 DEFPOINTS(IDC_BANKVERIFYNGNOPOINTS,BankVerifyNGNoPoints);
 DEFPOINTS(IDC_BANKVERIFYNGYESPOINTS,BankVerifyNGYesPoints);
 DEFPOINTS(IDC_BANKVERIFYGOODNOPOINTS,BankVerifyGoodNoPoints);
 DEFPOINTS(IDC_DELIVERYYESPOINTS,DeliveryYesPoints);
 DEFPOINTS(IDC_DELIVERYNOPOINTS,DeliveryNoPoints);
 DEFPOINTS(IDC_LICENSEINPOINTS,LicenseInPoints);
 DEFPOINTS(IDC_LICENSEOUTPOINTS,LicenseOutPoints);
 DEFPOINTS(IDC_FEDREGIONINPOINTS,FedRegionInPoints);
 DEFPOINTS(IDC_FEDREGIONOUTPOINTS,FedRegionOutPoints);
 DEFPOINTS(IDC_RESIDENCEHOUSEPOINTS,ResidenceHousePoints);
 DEFPOINTS(IDC_RESIDENCEAPTPOINTS,ResidenceAptPoints);
 DEFPOINTS(IDC_CONSUMERPHONEGOODPOINTS,ConsumerPhoneGoodPoints);
 DEFPOINTS(IDC_CONSUMERPHONENGPOINTS,ConsumerPhoneNoGoodPoints);
 DEFPOINTS(IDC_EMPLOYERPHONEGOODPOINTS,EmployerPhoneGoodPoints);
 DEFPOINTS(IDC_EMPLOYERPHONENGPOINTS,EmployerPhoneNoGoodPoints);
 DEFPOINTS(IDC_POSFILENOPOINTS,PositiveFileNoPoints);
 DEFPOINTS(IDC_POSFILEYESPOINTS,PositiveFileYesPoints);

 MinScore = new TEdit(this,IDC_MINSCORE);
 MaxScore = new TEdit(this,IDC_MAXSCORE);

 SortGroupMsgNum=RegisterWindowMessage(SORTGROUPMSG);
 CalcMinMaxMsgNum=RegisterWindowMessage(MINMAXMSG);
}

//-----------------
// Class Destructor
//-----------------
UnvFundsDialog::~UnvFundsDialog()
{
 Destroy(IDCANCEL);
 delete origRec;
 delete AmountGroup;
 delete CheckNumGroup;
 delete AgeGroup;
 delete SSNScoreGroup;
 delete YearsOnJobGroup;
 delete VelocityAmountGroup;
 delete VelocityTransGroup;
 delete BankVerifyAmountGroup;
 delete TimeOfDayGroup;
}

//------------------------------------
// Handle Changes to points and values
//------------------------------------
TResult UnvFundsDialog::EvCommand(uint id, THandle hWndCtl, uint notifyCode)
{
  TResult result;

  result = FormDialog::EvCommand(id, hWndCtl, notifyCode);

  switch ( notifyCode )
   {
    case 0:
    // Sort and Paint Group when Enter is pressed and a value
    // cell has the focus
         if ( id==IDOK && IS_VALUE_ID(idWithFocus) )
           PostMessage(SortGroupMsgNum,0,idWithFocus);
         return 0;

    // If Points change, re-calculate Min/Max values
    case EN_CHANGE:     if ( SetupComplete &&
                             IS_POINTS_ID(id) )
                          PostMessage(CalcMinMaxMsgNum,0,idWithFocus);
                         break;

    // If a Value changes, sort and re-paint the group
    case EN_KILLFOCUS:  if ( IS_VALUE_ID(id) )
                          PostMessage(SortGroupMsgNum,0,id);
                        idWithFocus=0;
                        break;

    // Remember who has the focus
    case EN_SETFOCUS:   idWithFocus = id;
                        break;
   }

  return result;
}

//-----------------------------------------------------
// Override virtual function to provide initial display
//-----------------------------------------------------
void UnvFundsDialog::SetupWindow()
{
 TDialog::SetupWindow();
 AmountGroup->enableFields();
 CheckNumGroup->enableFields();
 AgeGroup->enableFields();
 SSNScoreGroup->enableFields();
 YearsOnJobGroup->enableFields();
 VelocityAmountGroup->enableFields();
 VelocityTransGroup->enableFields();
 BankVerifyAmountGroup->enableFields();
 TimeOfDayGroup->enableFields();
 CalcMinMax(0,0);
 SetupComplete=true;
}

//----------------------
// Query modified status
//----------------------
bool UnvFundsDialog::IsModified()
{
 if ( FormDialog::IsModified() )
  return true;
 else
  return (memcmp(&origRec->Data,&newRec.Data,
                  sizeof(origRec->Data)) ==0) ? false : true;
}

//-------------------------
// Save Changes to DataBase
//-------------------------
bool UnvFundsDialog::Save()
{
 DBManMessage Msg;

 if ( ! validate() )
  return false;

 if ( ! Msg.DBPut(this,T_PUTUNVFUNDSRULES,FN_UNVFUNDSRULESRECORD,&newRec) )
  return false;

 ResetModified();
 return true;
}

//--------------------------
// Save changes to temp file
//--------------------------
bool UnvFundsDialog::SaveTemp(TFile* File)
{
 SAVERECORD_SENTINEL Sentinel;

 Sentinel.Type = GetType();
 Sentinel.Version = SaveRecordVersion[Sentinel.Type];
 Sentinel.Len = sizeof(newRec.Data);

 if ( File->Write(&Sentinel,sizeof(Sentinel)) != sizeof(Sentinel) ||
      File->Write(&newRec.Data,sizeof(newRec.Data))
            != sizeof(newRec.Data) )
  return false;

 ResetModified();
 return true;

}

//------------------------
// Reset the modified flag
//------------------------
void UnvFundsDialog::ResetModified()
{
 memcpy(&origRec->Data,&newRec.Data,sizeof(origRec->Data));
 FormDialog::ResetModified();
}

//-------------------------------------
// Calculate and Display Min/Max Points
//-------------------------------------
LRESULT UnvFundsDialog::CalcMinMax(WPARAM,LPARAM)
{
 int Min,Max;
 char Buf[20];

 newRec.calcMinMaxScore(Min,Max);
 sprintf(Buf,"%d",Min);
 MinScore->Transfer(Buf,tdSetData);
 sprintf(Buf,"%d",Max);
 MaxScore->Transfer(Buf,tdSetData);

 return 1;
}

//-----------------------------------------------
// Sort and re-paint the group that Id belongs in
//-----------------------------------------------
LRESULT UnvFundsDialog::SortGroup(WPARAM, LPARAM Id)
{
 sortedGroup* Group;

 switch(SORT_GROUP(Id))
  {
   case IDC_AMOUNTVALUE: Group=AmountGroup; break;
   case IDC_CHECKNUMVALUE: Group=CheckNumGroup; break;
   case IDC_AGEVALUE: Group=AgeGroup; break;
   case IDC_TIMEVALUE: Group=TimeOfDayGroup; break;
   case IDC_SSNSCOREVALUE: Group=SSNScoreGroup; break;
   case IDC_YEARSONJOBVALUE: Group=YearsOnJobGroup; break;
   case IDC_VELOCITYAMOUNTVALUE: Group=VelocityAmountGroup; break;
   case IDC_VELOCITYTRANSVALUE: Group=VelocityTransGroup; break;
   case IDC_BANKVERIFYGOODYESVALUE: Group=BankVerifyAmountGroup; break;
   default: return 0;
  }

 Group->SortAndPaint();

 return 1;
}

//------------------------------------------
// Validate that required fields are present
//------------------------------------------
bool UnvFundsDialog::validate()
{
 if (isEmpty(newRec.Data.TransactionCutoffValue) )
  {
   MessageBox("Score Required");
   CUTOFFSCORE->SetFocus();
   return false;
  }

 // Require velocity days if there are any Velocity items present
 if ( ( ! VelocityTransGroup->isEmpty() ||
        ! VelocityAmountGroup->isEmpty() ) &&
        isEmpty(newRec.Data.VelocityDays) )
  {
   MessageBox("Velociy Days Required");
   VELOCITYDAYS->SetFocus();
   return false;
  }

 // Require Bank Verify Good days if any values present
 if ( (! BankVerifyAmountGroup->isEmpty() ||
       newRec.Data.BankVerifyGoodNoPoints != 0)  &&
      isEmpty(newRec.Data.BankVerifyGoodDays) )
  {
   MessageBox("Bank Verify Good Days Required");
   BANKVERIFYGOODDAYS->SetFocus();
   return false;
  }

 // Require Bank Verify No Good days if any values present
 if ( ( newRec.Data.BankVerifyNGNoPoints != 0 ||
        newRec.Data.BankVerifyNGYesPoints != 0 ) &&
      isEmpty(newRec.Data.BankVerifyNGDays) )
  {
   MessageBox("Bank Verify NG Days Required");
   BANKVERIFYNGDAYS->SetFocus();
   return false;
  }

 return true;
}


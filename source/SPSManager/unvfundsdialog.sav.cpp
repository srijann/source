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
// number >= 30000

#define IS_VALUE_ID(id)  (id>=1000 && id<10000)
#define SORT_GROUP(id) (id / 100 * 100)
#define IS_POINTS_ID(id) (id>=10000 && id<30000)


//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(UnvFundsDialog, TDialog)
//{{UnvFundsDialogRSP_TBL_BEGIN}}
//{{UnvFundsDialogRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{UnvFundsDialog Implementation}}


// Validator for Points
class TPointsValidator : public TValidator {
  public:
  virtual bool IsValidInput(char far* str, bool suppressFill);
 };

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

//-----------------------------------------------------------
// Set readonly and Tab status of fields, depending on whether
// there is a corresponding value present
//-----------------------------------------------------------
void UnvFundsDialog::sortedGroup::setReadOnly()
{
 uint i;
 bool readOnly;
 bool firstReadOnly;

 // Do the points
 for (firstReadOnly=true, i=0; i<NumItems; ++i)
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

   // The last item is reserved for the >= value and is always
   // read only.
   // The first empty item is always enabled to allow inserts
   if ( i == NumItems -1 )
    readOnly = true;
   else
    if ( readOnly && firstReadOnly )
     readOnly=firstReadOnly=false;

   valueEdit[i]->SetReadOnly(readOnly);
   if ( readOnly )
     valueEdit[i]->ModifyStyle(WS_TABSTOP,0);
   else
     valueEdit[i]->ModifyStyle(0,WS_TABSTOP);
  }


}

//---------------------------------------------------
// Class Constructor for the sorted Group of controls
//---------------------------------------------------
UnvFundsDialog::sortedGroup::sortedGroup(TWindow* parent,
                uint numItems,uint startingPointsId,
                signed char* points,uint startingValueId,
                void* values,uint valuesType,uint valueLen,
                uint valueOptions) :
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

void UnvFundsDialog::sortedGroup::SortAndPaint()
{
 int i;

 // Build an array for quick sorting
 for (i=0; (uint)i < NumItems-1; ++i)
  {
   sortBuf[i].Points = Points[i];
   switch( ValuesType )
    {
     case sizeof(char):
        sortBuf[i].Value = (unsigned long) Values.cValues[i]; break;
     case sizeof(short):
        sortBuf[i].Value = (unsigned short) Values.sValues[i]; break;
     case sizeof(long):
        sortBuf[i].Value = Values.lValues[i]; break;
    }
  }

 // Sort
 qsort(sortBuf,NumItems-1,sizeof(sortBuf[0]),compare);
 for (i=0; (uint) i < NumItems; ++i)
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
 for (i=0; (uint)i < NumItems-1; ++i)
  {
   valueEdit[i]->Display();
   pointsEdit[i]->Display();
  }

 // Get the highest value for the bottom row
 sortBuf[NumItems-1].Value = ULONG_MAX;
 for (i=NumItems-1; i>=0; --i)
  if ( sortBuf[i].Value != ULONG_MAX )
   {
    sortBuf[NumItems-1].Value = sortBuf[i].Value;
    break;
   }
 if ( (uint) i==NumItems-1 )
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
 setReadOnly();
}

#define DEFGROUP(NUM,VALID,LEN,PTID,POINTS,VALUES,OPT)\
 new sortedGroup(this,NUM,PTID,(signed char*)newRec.Data.POINTS,\
                 VALID,newRec.Data.VALUES,sizeof(newRec.Data.VALUES[0]),\
                 LEN,OPT)

#define DEFPOINTS(ID,VAR) (new TEditVar(this,\
          (signed char&)newRec.Data.VAR,ID,3,EVOPT_ZEROISNULL))->\
               SetValidator(new TPointsValidator)

#define DEFVAL(ID,VAR,LEN) new TEditVar(this,\
          newRec.Data.VAR,ID,LEN,EVOPT_MAXISNULL)

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

 strcpy(Title,(char*)origRec->Data.UnvFundsRulesClassName);
 strcat(Title," Unverified Funds Rules");
 SetCaption(Title);
 memcpy(&newRec.Data,&origRec->Data,sizeof(newRec.Data));

 AmountGroup = DEFGROUP(NUMAMOUNTROWS,IDC_AMOUNTVALUE,6,IDC_AMOUNTPOINTS,
                        AmountPoints,AmountValue,0);

 CheckNumGroup = DEFGROUP(NUMCHECKNUMROWS,IDC_CHECKNUMVALUE,7,
              IDC_CHECKNUMPOINTS,CheckNumPoints,CheckNumValue,0);

 AgeGroup = DEFGROUP(NUMAGEROWS,IDC_AGEVALUE,3,IDC_AGEPOINTS,
                       AgePoints,AgeValue,0);

 SSNScoreGroup = DEFGROUP(NUMSSNSCOREROWS,IDC_SSNSCOREVALUE,5,
          IDC_SSNSCOREPOINTS,SSNScorePoints,SSNScoreValue,0);

 YearsOnJobGroup = DEFGROUP(NUMYEARSONJOBROWS,IDC_YEARSONJOBVALUE,3,
           IDC_YEARSONJOBPOINTS,YearsOnJobPoints,YearsOnJobValue,0);

 VelocityAmountGroup = DEFGROUP(NUMVELOCITYAMOUNTROWS,IDC_VELOCITYAMOUNTVALUE,6,
    IDC_VELOCITYAMOUNTPOINTS,VelocityAmountPoints,VelocityAmountValue,0);

 VelocityTransGroup = DEFGROUP(NUMVELOCITYTRANSROWS,IDC_VELOCITYTRANSVALUE,3,
    IDC_VELOCITYTRANSPOINTS,VelocityTransPoints,VelocityTransValue,0);

 BankVerifyAmountGroup = DEFGROUP(NUMBANKVERIFYAMOUNTROWS,
    IDC_BANKVERIFYGOODYESVALUE,6,
    IDC_BANKVERIFYGOODYESPOINTS,
    BankVerifyAmountPoints,BankVerifyAmountValue,0);

 TimeOfDayGroup = DEFGROUP(NUMTIMEOFDAYROWS,IDC_TIMEVALUE,5,
     IDC_TIMEPOINTS,TimePoints,TimeValue,EVOPT_ZEROFILL);


 DEFVAL(IDC_CUTOFFSCORE,TransactionCutoffValue,5);
 DEFVAL(IDC_BANKVERIFYGOODDAYS,BankVerifyGoodDays,3);
 DEFVAL(IDC_BANKVERIFYNGDAYS,BankVerifyNGDays,3);
 DEFVAL(IDC_VELOCITYDAYS,VelocityDays,3);

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
    // If Points change, re-calculate Min/Max values
    case EN_CHANGE:  if ( SetupComplete &&
                          IS_POINTS_ID(id) )
                        CalcMinMax();
                     break;

    case EN_KILLFOCUS:  if ( IS_VALUE_ID(id) )
                          SortGroup(id);
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
 AmountGroup->SortAndPaint();
 CheckNumGroup->SortAndPaint();
 AgeGroup->SortAndPaint();
 SSNScoreGroup->SortAndPaint();
 YearsOnJobGroup->SortAndPaint();
 VelocityAmountGroup->SortAndPaint();
 VelocityTransGroup->SortAndPaint();
 BankVerifyAmountGroup->SortAndPaint();
 TimeOfDayGroup->SortAndPaint();

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






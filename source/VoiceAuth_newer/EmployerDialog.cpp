//----------------------------------------------------------------------------
//  Project VoiceAuth
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    VoiceAuth.apx Application
//  FILE:         employerdialog.cpp
//  AUTHOR:
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of EmployerDialog (TDialog).
//
//----------------------------------------------------------------------------
#include "win4me.h"

#include "employerdialog.h"
#include "AppNotify.h"
#include "Chkauth.h"
#include "Msgid.h"
#include "CheckAuthFn.h"
#include "UnsNum.h"


//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(EmployerDialog, TDialog)
//{{EmployerDialogRSP_TBL_BEGIN}}
  EV_CBN_SELCHANGE(IDC_SPOUSEINFO, CBNSelchange),
//{{EmployerDialogRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{EmployerDialog Implementation}}

#define NUMCONTROLS 40
#define SPOUSEGROUP 2
#define EMPLOYERGROUP 1
#define NOGROUP 3

#define EDITCTL(n,len,group) DCMan->Add(IDC_##n,len,FN_##n,MID_HC##n,group)

#define SPOUSECTL(n,len) DCMan->Add(IDC_##n,len,FN_##n,MID_HC##n,SPOUSEGROUP);\
     DCMan->AddStatic(IDS_##n,IDC_##n,SPOUSEGROUP)
#define EMPCTL(n,len) DCMan->Add(IDC_##n,len,FN_##n,MID_HC##n,EMPLOYERGROUP);\
     DCMan->AddStatic(IDS_##n,IDC_##n,EMPLOYERGROUP)


EmployerDialog::EmployerDialog(TWindow* parent,CheckAuth& Msg,TMsgWin* MsgWin)
:
 TDialog(parent, IDD_EMPLOYER),
 authMsg(Msg),
 displayMode(false)
{
  DCMan = new DialogControlManager(this,NUMCONTROLS,
               (IPCMessage*)&Msg,MsgWin);

  // INSERT>> Your constructor code here.

  // Spouse Stuff
  SPOUSECTL(SPOUSESSN,9);
  SPOUSECTL(SPOUSENAME,25);
  SPOUSECTL(SPOUSESALARY,7);
  SPOUSECTL(SPOUSEEMPLOYERPHONE,10);
  EDITCTL(SPOUSEEMPLOYERADR3,25,SPOUSEGROUP);
  EDITCTL(SPOUSEEMPLOYERADR2,25,SPOUSEGROUP);
  SPOUSECTL(SPOUSEEMPLOYERADR1,25);
  SPOUSECTL(SPOUSEEMPLOYERNAME,25);
  SPOUSECTL(SPOUSEPOSITION,25);
  SPOUSECTL(SPOUSEYEARSEMPLOYED,2);
  EDITCTL(FINANCEAMOUNT,7,NOGROUP);

  EMPCTL(SALARY,7);
  EMPCTL(EMPLOYERPHONE,10);
  EDITCTL(EMPLOYERADR3,25,EMPLOYERGROUP);
  EDITCTL(EMPLOYERADR2,25,EMPLOYERGROUP);
  EMPCTL(EMPLOYERADR1,25);
  EMPCTL(EMPLOYERNAME,25);
  EMPCTL(POSITION,25);
  EMPCTL(YEARSEMPLOYED,2);
  DCMan->AddComboBox(IDC_SPOUSEINFO,FN_SPOUSEINFO,MID_HCSPOUSEINFO);
  DCMan->AddStatic(IDS_SPOUSE,IDC_SPOUSEINFO);
  DCMan->AddStatic(IDS_SPOUSEGROUP,0,SPOUSEGROUP);
  DCMan->AddStatic(IDS_EMPLOYERGROUP);
  DCMan->AddStatic(IDS_SPOUSEEMPLOYER,0,SPOUSEGROUP);
}


EmployerDialog::~EmployerDialog()
{
  Destroy(IDCANCEL);
  delete DCMan;
}


//---------------------------------
// Prohibit ESCAPE from this window
//---------------------------------
TResult EmployerDialog::EvCommand(uint id, THandle hWndCtl, uint notifyCode)
{
 TResult result;
  switch( notifyCode )
   {
    case 0:
       if ( id==IDCANCEL )
         return 0;
       if ( id==IDOK )
        {
         editData();
         return 0;
        }
      break;

    case EN_SETFOCUS:
    case CBN_SETFOCUS:
      if ( displayMode )
        return 0;
      DCMan->SetFocus((int)id);
      break;

    default:
      break;
   }

  result = TDialog::EvCommand(id, hWndCtl, notifyCode);
  return result;
}


void EmployerDialog::SetupWindow()
{
 TComboBoxEx* Box;
 int Amount=authMsg.CheckAmount();
 WORD testAmount = (WORD)((Amount > USHRT_MAX) ? USHRT_MAX : Amount);

 TDialog::SetupWindow();
 spouseEnabled = authMsg.dataReq.isRequired(authMsg.Voice,
                   authMsg.dataReq.SPOUSEDATA,testAmount) ||
                 authMsg.dataReq.isRequired(authMsg.Security,
                   authMsg.dataReq.SPOUSEDATA,testAmount);


 employerEnabled = authMsg.dataReq.isRequired(authMsg.Voice,
                     authMsg.dataReq.EMPLOYMENTDATA,testAmount) ||
                   authMsg.dataReq.isRequired(authMsg.Security,
                     authMsg.dataReq.EMPLOYMENTDATA,testAmount);

 Box=DCMan->ComboBoxPtr(IDC_SPOUSEINFO);
 Box->AddItem("Yes",'Y');
 Box->AddItem("No",'N');
 if ( spouseEnabled )
  DCMan->Enable(IDC_SPOUSEINFO);
 if ( authMsg.dataReq.isRequired(authMsg.Voice,
            authMsg.dataReq.FINANCEAMOUNT,testAmount) ||
      authMsg.dataReq.isRequired(authMsg.Voice,
            authMsg.dataReq.FINANCEAMOUNT,testAmount) )
  DCMan->Enable(IDC_FINANCEAMOUNT);          

 enableSpouseGroup();
 enableEmployerGroup();
}


//------------------
// Edit all the Data
//------------------
bool EmployerDialog::editData()
{
 FLDNUM Fn;
 BYTE   isSpouseCoSigner;

 DCMan->CopyAllToMsg();

 // Check for any Missing Required Fields
 if ( (Fn=authMsg.needEmployerData(authMsg.Voice)) != 0 ||
      (Fn=authMsg.needEmployerData(authMsg.Security)) != 0 )
  {
   DCMan->SetFocus(Fn);
   return false;
  }

 // If Spouse is Enabled make sure I have Spouse Yes/No answer
 if ( spouseEnabled )
  {
   if ( ! authMsg.GetFld(FN_SPOUSEINFO,isSpouseCoSigner) )
    {
     DCMan->SetFocus(FN_SPOUSEINFO);
     return false;
    }
   if ( isSpouseCoSigner == 'Y' &&
        ((Fn=authMsg.needSpouseCoSignerData(authMsg.Voice)) != 0 ||
         (Fn=authMsg.needSpouseCoSignerData(authMsg.Security)) != 0 ) )
    {
     DCMan->SetFocus(Fn);
     return false;
    }
  }

 if ( ! authMsg.isValidPhone(FN_EMPLOYERPHONE) )
  {
   DCMan->SetFocus(IDC_EMPLOYERPHONE,MID_INVALIDPHONE);
   DCMan->EditPtr(IDC_EMPLOYERPHONE)->SetSelection(0,-1);
   return false;
  }
 if ( ! authMsg.isValidSSN(FN_SPOUSESSN) )
  {
   DCMan->SetFocus(IDC_SPOUSESSN,MID_INVALIDSSN);
   DCMan->EditPtr(IDC_SPOUSESSN)->SetSelection(0,-1);
   return false;
  }
 if ( ! authMsg.isValidPhone(FN_SPOUSEEMPLOYERPHONE) )
  {
   DCMan->SetFocus(IDC_SPOUSEEMPLOYERPHONE,MID_INVALIDPHONE);
   DCMan->EditPtr(IDC_SPOUSEEMPLOYERPHONE)->SetSelection(0,-1);
   return false;
  }

 DCMan->SetAllReadOnly();
 AppNotifyMsg(Parent,NOTIFY_DIALOGCOMPLETE,0);
 ShowWindow(SW_HIDE);
 displayMode=true;
 return true;
}

//-----------------------------
// Enable/Disable Spouse fields
//-----------------------------
#define TEST(bit,fldnum)\
 if ( authMsg.GetFld(fldnum) ||\
      authMsg.dataReq.isRequired(authMsg.Voice,authMsg.dataReq.bit) ||\
      authMsg.dataReq.isRequired(authMsg.Security,authMsg.dataReq.bit) )\
   DCMan->Enable(fldnum)

bool EmployerDialog::enableSpouseGroup()
{
 BYTE spouseEnable;

 DCMan->CopyToMsg(IDC_SPOUSEINFO);

 if ( spouseEnabled &&
      authMsg.GetFld(FN_SPOUSEINFO,spouseEnable) &&
      spouseEnable=='Y' )
  {
   TEST(SPOUSENAME,FN_SPOUSENAME);
   TEST(SPOUSESSN,FN_SPOUSESSN);
   TEST(SPOUSEEMPLOYER,FN_SPOUSEEMPLOYERNAME);
   TEST(SPOUSEEMPLOYERADR,FN_SPOUSEEMPLOYERADR1);
   TEST(SPOUSEEMPLOYERADR,FN_SPOUSEEMPLOYERADR2);
   TEST(SPOUSEEMPLOYERADR,FN_SPOUSEEMPLOYERADR3);
   TEST(SPOUSEEMPLOYERPHONE,FN_SPOUSEEMPLOYERPHONE);
   TEST(SPOUSEPOSITION,FN_SPOUSEPOSITION);
   TEST(SPOUSEYEARSEMPLOYED,FN_SPOUSEYEARSEMPLOYED);
   TEST(SPOUSESALARY,FN_SPOUSESALARY);
   DCMan->Enable(IDS_SPOUSEGROUP);
   DCMan->Enable(IDS_SPOUSEEMPLOYER);
  }
 else
   DCMan->DisableAll(SPOUSEGROUP);

 return false;
}

void EmployerDialog::CBNSelchange()
{
 enableSpouseGroup();
}

void EmployerDialog::enableEmployerGroup()
{
 if ( employerEnabled )
  {
   DCMan->Enable(IDS_EMPLOYERGROUP);
   TEST(EMPLOYERNAME,FN_EMPLOYERNAME);
   TEST(EMPLOYERADR,FN_EMPLOYERADR1);
   TEST(EMPLOYERADR,FN_EMPLOYERADR2);
   TEST(EMPLOYERADR,FN_EMPLOYERADR3);
   TEST(EMPLOYERPHONE,FN_EMPLOYERPHONE);
   TEST(POSITION,FN_POSITION);
   TEST(YEARSEMPLOYED,FN_YEARSEMPLOYED);
   TEST(SALARY,FN_SALARY);
  }

}


//----------------------------------------------------------------------------
//  Project Classlib
//
//  Copyright © 1996. All Rights Reserved.
//
//  FILE:         EditStateCodes.cpp
//  AUTHOR:       Jack Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of EditStateCodes (TEdit).
//
//  Provide an Edit Control with a popup list of state codes. The
//  operator types the first character of the state and a list pops
//  up to help with the second character (it's actually a menu but
//  the user doesn't select from it).
//----------------------------------------------------------------------------
#include "win4me.h"

#include <string.h>
#include "EditStateCodes.h"
#include "ClassLib.h"

//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(TEditStateCodes, TEdit)
//{{TEditStateCodesRSP_TBL_BEGIN}}
  EV_WM_CHAR,
//{{TEditStateCodesRSP_TBL_END}}
END_RESPONSE_TABLE;


static char* StateCodeArray[] = {
  "AB Alberta" ,
  "AK Alaska",
  "AL Alabama",
  "AR Arkansas",
  "AS American Samoa",
  "AZ Arizona",
  "BC British Columbia",
  "CA California",
  "CO Colorado",
  "CT Connecticut",
  "CZ Canal Zone",
  "DC District of Columbia",
  "DE Delaware",
  "DS-Dept of State",
  "FL-Florida",
  "GA-Georgia",
  "GU-Guam",
  "HI-Hawaii",
  "IA-Iowa",
  "ID-Idaho",
  "IL-Illinois",
  "IN-Indiana",
  "KS-Kansas",
  "KY-Kentucky",
  "LA-Louisiana",
  "MA-Massachusetts",
  "MB-Manitoba",
  "MD-Maryland",
  "ME-Maine",
  "MI-Michigan",
  "MN-Minnesota",
  "MO-Missouri",
  "MS-Mississippi",
  "MT-Montana",
  "NB-New Brunswick",
  "NC-North Carolina",
  "ND-North Dakota",
  "NE-Nebraska",
  "NF-Newfoundland",
  "NH-New Hampshire",
  "NJ-New Jersey",
  "NM-New Mexico",
  "NS-Nova Scotia",
  "NT-Northwest Territory",
  "NV-Nevada",
  "NY-New York",
  "OH-Ohio",
  "OK-Oklahoma",
  "ON-Ontario",
  "OR-Oregon",
  "PA-Pennsylvannia",
  "PE-Prince Edward Island",
  "PQ-Province of Quebec",
  "PR-Puerto Rico",
  "QP-Province of Quebec (PQ)",
  "RI-Rhode Island",
  "SC-South Carolina",
  "SD-South Dakota",
  "SK-Saskatchewan",
  "TN-Tennessee",
  "TX-Texas",
  "UT-Utah",
  "VA-Virginia",
  "VI-Virgin Islands",
  "VT-Vermont",
  "WA-Washington",
  "WI-Wisconsin",
  "WV-West Virginia",
  "WY-Wyoming",
  "YT-Yukon Territory",
  NULL };

static char* CanadianProvinces[] =
 {
  "AB", "BC", "MB", "NB", "NF", "NS", "NT", "ON", "PE", "PQ", "SK",
  "YT", NULL
 };

//------------------
// Class Constructor
//------------------
TEditStateCodes::TEditStateCodes(TWindow* parent,int ResourceId,int Pos) :
 CanadianFlag(true),
 TEdit(parent,ResourceId,3),
 PosType(Pos)
{
 Menu=new TPopupMenu();
}

//-----------------
// Class Destructor
//-----------------
TEditStateCodes::~TEditStateCodes()
{
 Destroy(IDCANCEL);
 delete Menu;
}

//----------------------------
// Set the Canadian Flag value
//----------------------------
void TEditStateCodes::SetCanadian(bool flag)
{
 CanadianFlag=flag;
}

//-----------------------------
// Fetch the CanadianFlag value
//-----------------------------
bool TEditStateCodes::GetCanadian()
{
 return CanadianFlag;
}

//----------------------------------
// Set the Position Type (OVER/UNDER)
//----------------------------------
void TEditStateCodes::SetPosType(int Type)
{
 PosType=Type;
}

//------------------------
// Fetch the Position Type
//------------------------
int TEditStateCodes::GetPosType()
{
 return PosType;
}

//--------------------------------------------------
// OVERRIDING EvCommand to handle the menu selection
//--------------------------------------------------
TResult TEditStateCodes::EvCommand(uint id, THandle hWndCtl, uint notifyCode)
{
  TResult result;

  result = TEdit::EvCommand(id, hWndCtl, notifyCode);
  // Menu Selection?
  if ( notifyCode==0 )
   {
    char Buf[3];
    memcpy(Buf,StateCodeArray[id-300],2);
    Buf[2]=0;
    // Quebec is the only state code that doesn't start with the
    // first letter. We offer a QP selection on the menu but here
    // have to change it to PQ.
    if ( strcmp(Buf,"QP") == 0 )
     strcpy(Buf,"PQ");
    Transfer(Buf,tdSetData);
   }

  return result;
}


//---------------------------------------------------------
// Determine if a STATE code represents a Canadian province
//---------------------------------------------------------
bool TEditStateCodes::IsCanadian(char* State)
{
 register i, cc;
 for (i=0; CanadianProvinces[i] != NULL; ++i)
  {
   if ( (cc=memcmp(State,CanadianProvinces[i],2)) == 0 )
     return true;
   if ( cc < 0 )
     return false;
  }

 return false;
}

//----------------------------------------------
// Respond to ALL keystrokes by Popping the Menu
//----------------------------------------------
#pragma argsused
void TEditStateCodes::EvChar(uint key, uint repeatCount, uint flags)
{
 register i;
 char CopyBuf[50];
 TPoint Point;

 if ( ! isascii(key) )
  return;

 // Delete all ITEMS from current Menu
 while (Menu->GetMenuItemCount())
   Menu->DeleteMenu(0,MF_BYPOSITION);

 // Find Starting Position
 key=toupper(key);
 for (i=0; StateCodeArray[i] != NULL &&
           (uint) StateCodeArray[i][0] != key; ++i);
 if ( StateCodeArray[i]==NULL )
   return;

 // Loop through and build the Menu
 do
  {
   // Filter out Non-Canadian ID's unless wanted
   if ( CanadianFlag ||
        IsCanadian(StateCodeArray[i]) == false )
    {
     CopyBuf[0] = StateCodeArray[i][0];
     CopyBuf[1] = '&';
     strcpy(&CopyBuf[2],&StateCodeArray[i][1]);
     Menu->AppendMenu(MF_STRING,300+i,CopyBuf);
    }

   ++i;
  }
 while ( StateCodeArray[i] != NULL &&
         (uint) StateCodeArray[i][0] == key );

 // No State Code Matches?
 if ( Menu->GetMenuItemCount() == 0 )
   return;

 // Calc where to put it
 CalcMenuPos(Point,this,Menu,PosType==POS_OVER);

 TEdit::EvChar(key,repeatCount,flags);
 
 // Pop the Menu
 Menu->TrackPopupMenu(TPM_LEFTBUTTON|TPM_CENTERALIGN,Point,0,HWindow);
}


/*
#pragma argsused
//-------------------------------------------------------------
// Validator function. Called by TValidator for each keystroke.
//-------------------------------------------------------------
bool TEditStateCodeValidator::IsValidInput(char far* str,bool Suppressfill)
{
 register i;
 int Len;
 bool WantCanadian;
 char CopyBuf[50];
 TPoint Point;

 // Find start position in State Code list
 for (i=0; StateCodeArray[i] != NULL &&
           StateCodeArray[i][0] != str[0]; ++i);

 // If not in LIST, don't accept the INPUT
 if (StateCodeArray[i]==NULL )
  {
   strcpy(str,"");
   return false;
  }

 // Delete all ITEMS from current Menu
 while (PopUpMenu->GetMenuItemCount())
   PopUpMenu->DeleteMenu(0,MF_BYPOSITION);

 // Loop through and build the Menu
 Len=strlen(str);
 WantCanadian = NotifyWindow->GetCanadian();
 do
  {
   // Filter out Non-Canadian ID's unless wanted
   if ( WantCanadian ||
        IsCanadian(StateCodeArray[i]) == false )
    {
     // If it's an exact match then no need to pop up a menu. Just
     // let it alone
     if ( Len==2 &&
          memcmp(StateCodeArray[i],str,2) == 0 )
       return true;
     // Format Menu item with 2nd character underscored
     CopyBuf[0] = StateCodeArray[i][0];
     CopyBuf[1] = '&';
     strcpy(&CopyBuf[2],&StateCodeArray[i][1]);
     PopUpMenu->AppendMenu(MF_STRING,300+i,CopyBuf);
    }

   ++i;
  }
 while ( StateCodeArray[i] != NULL &&
         StateCodeArray[i][0] == str[0] );

 // This empties the field so it will be cleared if the Menu is escaped
 *str=0;

 // No State Code Matches?
 if ( PopUpMenu->GetMenuItemCount() == 0 )
   return false;

 // Calc where to put it
 CalcMenuPos(Point,NotifyWindow,PopUpMenu,
             NotifyWindow->GetPosType()==POS_OVER);

 // Pop up the Menu
 PopUpMenu->TrackPopupMenu(TPM_LEFTBUTTON|TPM_CENTERALIGN,
                           Point,0,NotifyWindow->HWindow);

 return true;
}
*/



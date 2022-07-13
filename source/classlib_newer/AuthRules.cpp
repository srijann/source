//----------------------------------------------------------------------------
//  Project Classlib
//
//  Copyright © 1996. All Rights Reserved.
//
//  FILE:         AuthRules.cpp
//  AUTHOR:       Jack Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of AuthRules (DBManRecord)
//
//  This is a Derived class of DBManRecord to handle the Authorization
//  Rules Record.
//----------------------------------------------------------------------------

#include "Win4me.h"
#pragma hdrstop
#include <mem.h>
#include <string.h>
#define NOFLDNAMES
#include "AuthRules.h"
#include "TimeZoneInfo.h"
#include "UnsNum.h"

#define NUMDTS 4
// Another mapping of the Date/Time structures for convenience
struct DTDef
 {
  WORD fromTime;
  WORD toTime;
  BYTE days[7];
 };

// Structure for Saving SubClass Product Adjustments for
// Global/Class/SubClass rules
typedef struct
 {
  BYTE Factors[3][NUMPRODUCTSUBGROUPS];
 } PRODUCTADJUSTMENTS;

//----------------------------
// Make a new (empty) rule set
//----------------------------
AuthRules::AuthRules()
{
 memset(&Data,0,sizeof(Data));
}

//-----------------------------------------------
// Make a rule set, copying from another rule set
//-----------------------------------------------
AuthRules::AuthRules(struct AuthRulesRecord* Rules)
{
 memcpy(&Data,Rules,sizeof(Data));
}

// Make a rule set, getting data from an IPC message
AuthRules::AuthRules(FLDNUM fn,IPCMessage* Msg)
{
 int Len=sizeof(Data);

 if ( ! Msg->GetFld(fn,&Data,Len) )
  memset(&Data,0,sizeof(Data));
}


AuthRules::~AuthRules()
{
}

//-----------------------------------------
// Format a a new (empty) Auth Rules record
//-----------------------------------------
void AuthRules::FormatNew(const char* ClassName,const char* SubClassName)
{
 unsigned char Level;

 // Default Level to Class for SubClass records, Global for all
 // others.
 if ( SubClassName &&
      SubClassName[0] )
  Level='C';
 else
  Level='G';

 // Hi values marks all the fields empty
 memset(&Data,0xff,sizeof(Data));
 Data.Countable.CountableEnable=Level;
 Data.R1.R1Enable=Data.R2.R2Enable=Data.R3.R3Enable=Level;
 Data.R4.R4Enable=Data.R5.R5Enable=Data.R6.R6Enable=Level;
 Data.R7.R7Enable=Data.R8.R8Enable=Level;
 Data.AuthRulesTStamp=0;
 memset(Data.AuthRulesKey.ClassName,0,sizeof(Data.AuthRulesKey.ClassName));
 memset(Data.AuthRulesKey.SubClassName,0,sizeof(Data.AuthRulesKey.SubClassName));
 strcpy((char*)Data.AuthRulesKey.ClassName,ClassName);
 if ( SubClassName )
   strcpy((char*)Data.AuthRulesKey.SubClassName,SubClassName);
 Data.R1.R1Product=Data.R2.R2Product=Data.R3.R3Product=Level;
 Data.R4.R4Product=Data.R5.R5Product=Data.R6.R6Product=Level;
 Data.R7.R7Product=Data.R8.R8Product=Level;
 Data.R1.R1DateTime=Data.R2.R2DateTime=Data.R3.R3DateTime=Level;
 Data.R4.R4DateTime=Data.R5.R5DateTime=Data.R6.R6DateTime=Level;
 Data.R7.R7DateTime=Data.R8.R8DateTime=Level;
}

//-----------------------------------------
// Format a a new (empty) Auth Rules record
//-----------------------------------------
void AuthRules::FormatNew(const char* ClassName)
{
 FormatNew(ClassName,0);
}

//---------------------------------------------------
// Validate all of the Levels in a set of Auth rules.
//---------------------------------------------------
#define VALIDATE(n,name) if ( ! strchr(ValidLevels,Data.n.n##name) )\
  Data.n.n##name=DefaultLevel
void AuthRules::ValidateLevels(const char* ValidLevels,char DefaultLevel)
{
 VALIDATE(Countable,Enable);
 VALIDATE(R1,Enable);
 VALIDATE(R2,Enable);
 VALIDATE(R3,Enable);
 VALIDATE(R4,Enable);
 VALIDATE(R5,Enable);
 VALIDATE(R6,Enable);
 VALIDATE(R7,Enable);
 VALIDATE(R8,Enable);
 VALIDATE(R1,Product);
 VALIDATE(R2,Product);
 VALIDATE(R3,Product);
 VALIDATE(R4,Product);
 VALIDATE(R5,Product);
 VALIDATE(R6,Product);
 VALIDATE(R7,Product);
 VALIDATE(R8,Product);
 VALIDATE(R1,DateTime);
 VALIDATE(R2,DateTime);
 VALIDATE(R3,DateTime);
 VALIDATE(R4,DateTime);
 VALIDATE(R5,DateTime);
 VALIDATE(R6,DateTime);
 VALIDATE(R7,DateTime);
 VALIDATE(R8,DateTime);
}

int AuthRules::KeyType()
{
 return AUTHRULESKEY;
}

int AuthRules::RecType()
{
 return AUTHRULESRECORD;
}

#define PICKCLASSRULES(name)\
 if ( (classRules->Data.name.name##Enable=='C' ||\
        classRules->Data.name.name##Enable==' ') )\
     memcpy(&Data.name,&classRules->Data.name,sizeof(Data.name))

#define PICKSUBCLASSRULES(name)\
 if ( (subclassRules->Data.name.name##Enable=='S' ||\
        subclassRules->Data.name.name##Enable==' ') )\
   memcpy(&Data.name,&subclassRules->Data.name,sizeof(Data.name));\
 else\
   PICKCLASSRULES(name)

#define SETLEVEL3(n,type) chooseLevel(Data.n.n##type,\
                                      globalRules->Data.n.n##type,\
                                      classRules->Data.n.n##type,\
                                      subclassRules->Data.n.n##type)
#define SETLEVEL2(n,type) chooseLevel(Data.n.n##type,\
                                      globalRules->Data.n.n##type,\
                                      classRules->Data.n.n##type)

//---------------------------------------
// Merge all levels of AuthRules together
//
// Merge three levels of Rules together and apply adjustment factors
// The Date/Time factors are always applied
// The Product factors are applied if the ProductSubGroup is <> 0
// If the The ProductSubGroup is not known, the Product adjustments
// can be saved for future application by passing a pointer to the
// IPC Message. When the SubGroup is known, call the ApplyProductAdjustment
// member function to apply.
//---------------------------------------
void AuthRules::Merge(AuthRules* globalRules,AuthRules* classRules,
                      AuthRules* subclassRules,uint TimeZone,
                      uint ProductGroup,uint ProductSubGroup,
                      IPCMessage* Msg)
{
 PRODUCTADJUSTMENTS ProdAdj;

 // Use global rules for a base
 memcpy(&Data,&globalRules->Data,sizeof(Data));
 // Merge subclass or class as appropriate
 if ( subclassRules )
  {
   PICKSUBCLASSRULES(Countable);
   PICKSUBCLASSRULES(R1);
   PICKSUBCLASSRULES(R2);
   PICKSUBCLASSRULES(R3);
   PICKSUBCLASSRULES(R4);
   PICKSUBCLASSRULES(R5);
   PICKSUBCLASSRULES(R6);
   PICKSUBCLASSRULES(R7);
   PICKSUBCLASSRULES(R8);
  }
 else
 if ( classRules )
  {
   PICKCLASSRULES(Countable);
   PICKCLASSRULES(R1);
   PICKCLASSRULES(R2);
   PICKCLASSRULES(R3);
   PICKCLASSRULES(R4);
   PICKCLASSRULES(R5);
   PICKCLASSRULES(R6);
   PICKCLASSRULES(R7);
   PICKCLASSRULES(R8);
  }

 // Change Date/Time Product Class Enablers to handle "double-hop"
 // situation, ie., the Subclass Adjustments point up to the Class Level
 // adjustments and the Class Level Adjustments point up to the
 // Global adjustments. We will lose the ability to do this after we
 // merge so we do it here.
 if ( subclassRules )
  {
   SETLEVEL3(R1,DateTime);
   SETLEVEL3(R2,DateTime);
   SETLEVEL3(R3,DateTime);
   SETLEVEL3(R4,DateTime);
   SETLEVEL3(R5,DateTime);
   SETLEVEL3(R6,DateTime);
   SETLEVEL3(R7,DateTime);
   SETLEVEL3(R8,DateTime);
   SETLEVEL3(R1,Product);
   SETLEVEL3(R2,Product);
   SETLEVEL3(R3,Product);
   SETLEVEL3(R4,Product);
   SETLEVEL3(R5,Product);
   SETLEVEL3(R6,Product);
   SETLEVEL3(R7,Product);
   SETLEVEL3(R8,Product);
  }
 else
  if ( classRules )
   {
    SETLEVEL2(R1,DateTime);
    SETLEVEL2(R2,DateTime);
    SETLEVEL2(R3,DateTime);
    SETLEVEL2(R4,DateTime);
    SETLEVEL2(R5,DateTime);
    SETLEVEL2(R6,DateTime);
    SETLEVEL2(R7,DateTime);
    SETLEVEL2(R8,DateTime);
    SETLEVEL2(R1,Product);
    SETLEVEL2(R2,Product);
    SETLEVEL2(R3,Product);
    SETLEVEL2(R4,Product);
    SETLEVEL2(R5,Product);
    SETLEVEL2(R6,Product);
    SETLEVEL2(R7,Product);
    SETLEVEL2(R8,Product);
   }

 // Do the Date/Time Adjustment 
 TimeAdjust(classRules,subclassRules,TimeZone);

 // Either apply the Product Group Adjustments or save them for
 // later use
 if ( ProductGroup && ProductSubGroup )
   ProductAdjust(classRules,subclassRules,ProductGroup,ProductSubGroup);
 else
 if ( ProductGroup && ProductGroup <= NUMPRODUCTGROUPS && Msg )
  {
   memset(&ProdAdj,0xff,sizeof(ProdAdj));
   memcpy(&ProdAdj.Factors[0],
          &globalRules->Data.ProdAdj[ProductGroup-1][0],
          sizeof(ProdAdj.Factors[0]));
   if ( classRules )
     memcpy(&ProdAdj.Factors[1],
            &classRules->Data.ProdAdj[ProductGroup-1][0],
            sizeof(ProdAdj.Factors[1]));
   if ( subclassRules )
     memcpy(&ProdAdj.Factors[2],
            &subclassRules->Data.ProdAdj[ProductGroup-1][0],
            sizeof(ProdAdj.Factors[2]));
   Msg->PutFld(FN_PRODUCTADJUSTMENTS,&ProdAdj,sizeof(ProdAdj));
  }
}

//-----------------------------------------------------------
// Look at Date/Time Adjustments and see if the caller's time
// is selected.
//-----------------------------------------------------------
BYTE AuthRules::pickAdjustment(WORD Time,DWORD Day,
                              struct DTDef* DT)
{
 register i;
 for (i=0; i<NUMDTS; ++i)
  {
   if ( DT[i].fromTime == USHRT_MAX )
    return 0;
   if ( DT[i].days[Day]==UCHAR_MAX )
    continue;
   if ( Time >= DT[i].fromTime &&
        Time <= DT[i].toTime )
    return DT[i].days[Day];    
  }

 return 0;
}

//-----------------------------------------------
// Pick which set of rules to use for adjustments
//-----------------------------------------------
void* AuthRules::pickRules(uint Enabler,
                           void* globalRules,
                           void* classRules,
                           void* subclassRules)
{

 switch (Enabler)
  {
   default:
   case ' ': break;

   case 'S': if ( subclassRules )
               return subclassRules;
             break;

   case 'C': if ( classRules )
               return classRules;
             break;

   case 'G': return globalRules;
  }

 return NULL;
}

//------------------------------------------
// Adjust an amount by the appropriate value
//------------------------------------------
void AuthRules::Adjust(unsigned short& amount,BYTE adjustValue)
{
 unsigned int Amount=amount;

 if ( Amount==USHRT_MAX ||
      adjustValue==10 ||
      adjustValue==0  ||
      adjustValue==UCHAR_MAX )
  return;

 // Increase by an order of magnitude for more precision in the
 // math
 Amount *= 10;

 if ( adjustValue < 10 )
   Amount=Amount/10 * adjustValue;
 else
   if ( adjustValue < 20)
    Amount += Amount/10 * (adjustValue-10);
   else
    {
     Amount *= adjustValue / 10;
     Amount += Amount/10 * (adjustValue%10);
    }

 // Go back to original order of magnitude
 Amount /= 10;
 if ( Amount > USHRT_MAX -1 )
  amount = USHRT_MAX -1;
 else
  amount = (unsigned short) Amount;
}

//-----------------------------
// Do the Date Time adjustments
//-----------------------------
#define IFADJUST(n)\
 if ( Data.n.n##Enable != ' ' &&\
      Data.n.n##DateTime != ' ' &&\
      (Rules=(AuthRules*)pickRules(Data.n.n##DateTime,\
                   this,classRules,subclassRules)) != NULL &&\
      (Adjustment=pickAdjustment(Time,t->tm_wday,\
                            (struct DTDef*)&Rules->Data.DT)) != UCHAR_MAX )

#define ADJUST(n,name)\
 Adjust(Data.n.n##name,Adjustment)

void AuthRules::TimeAdjust(AuthRules* classRules,
                           AuthRules* subclassRules,
                           uint TimeZone)
{
 TimeZoneInfo TZInfo;
 struct tm* t = TZInfo.LocalTime(TimeZone);
 AuthRules* Rules;
 unsigned short Time=(unsigned short) (t->tm_hour * 100 + t->tm_min);
 BYTE Adjustment;

 IFADJUST(R1)
  {
   ADJUST(R1,OvrdAmt1);
   ADJUST(R1,OvrdAmt2);
   ADJUST(R1,ReferAmt1);
   ADJUST(R1,ReferAmt2);
  }
 IFADJUST(R2)
  {
   ADJUST(R2,Sic1Amt);
   ADJUST(R2,Sic2Amt);
   ADJUST(R2,Acct1Amt);
   ADJUST(R2,Acct2Amt);
   ADJUST(R2,Loc1Amt);
   ADJUST(R2,Loc2Amt);
  }
 IFADJUST(R3)
  {
   ADJUST(R3,ReferAmt1From);
   ADJUST(R3,ReferAmt2From);
   ADJUST(R3,ReferAmt3From);
   ADJUST(R3,ReferAmt4From);
   ADJUST(R3,AgeReferAmt1From);
   ADJUST(R3,AgeReferAmt2From);
   ADJUST(R3,AgeReferAmt3From);
   ADJUST(R3,AgeReferAmt4From);
   ADJUST(R3,OvrdAmt1From);
   ADJUST(R3,OvrdAmt2From);
   ADJUST(R3,OvrdAmt3From);
   ADJUST(R3,OvrdAmt4From);
   ADJUST(R3,AgeOvrdAmt1From);
   ADJUST(R3,AgeOvrdAmt2From);
   ADJUST(R3,AgeOvrdAmt3From);
   ADJUST(R3,AgeOvrdAmt4From);
   ADJUST(R3,ReferAmt1To);
   ADJUST(R3,ReferAmt2To);
   ADJUST(R3,ReferAmt3To);
   ADJUST(R3,ReferAmt4To);
   ADJUST(R3,AgeReferAmt1To);
   ADJUST(R3,AgeReferAmt2To);
   ADJUST(R3,AgeReferAmt3To);
   ADJUST(R3,AgeReferAmt4To);
   ADJUST(R3,OvrdAmt1To);
   ADJUST(R3,OvrdAmt2To);
   ADJUST(R3,OvrdAmt3To);
   ADJUST(R3,OvrdAmt4To);
   ADJUST(R3,AgeOvrdAmt1To);
   ADJUST(R3,AgeOvrdAmt2To);
   ADJUST(R3,AgeOvrdAmt3To);
   ADJUST(R3,AgeOvrdAmt4To);
  }
 /* R4 has no amounts
 IFADJUST(R4)
  {
  }
 */
 IFADJUST(R5)
  {
   ADJUST(R5,Amt);
  }
 IFADJUST(R6)
  {
   ADJUST(R6,Amt);
   ADJUST(R6,Accum);
  }
 IFADJUST(R7)
  {
   ADJUST(R7,DailyAmt);
   ADJUST(R7,Per1Amt);
   ADJUST(R7,Per2Amt);
   ADJUST(R7,Per3Amt);
  }
 IFADJUST(R8)
  {
   ADJUST(R8,Amt);
   ADJUST(R8,Accum);
  }
}

//---------------------------
// Do the Product Adjustments
//---------------------------
#undef IFADJUST
#define IFADJUST(n)\
 if ( Data.n.n##Enable != ' ' &&\
      Data.n.n##Product != ' ' &&\
      (Rules=(AuthRules*)pickRules(Data.n.n##Product,this,classRules,subclassRules)) != NULL &&\
      (Adjustment=Rules->Data.ProdAdj[Group-1][SubGroup-1]) != UCHAR_MAX )

void AuthRules::ProductAdjust(AuthRules* classRules,
                              AuthRules* subclassRules,
                              uint Group,uint SubGroup)
{
 AuthRules* Rules;
 BYTE Adjustment;

 if ( Group == 0 ||
      SubGroup == 0 ||
      Group > NUMPRODUCTGROUPS ||
      SubGroup > NUMPRODUCTSUBGROUPS )
  return;

 IFADJUST(R1)
  {
   ADJUST(R1,OvrdAmt1);
   ADJUST(R1,OvrdAmt2);
   ADJUST(R1,ReferAmt1);
   ADJUST(R1,ReferAmt2);
  }
 IFADJUST(R2)
  {
   ADJUST(R2,Sic1Amt);
   ADJUST(R2,Sic2Amt);
   ADJUST(R2,Acct1Amt);
   ADJUST(R2,Acct2Amt);
   ADJUST(R2,Loc1Amt);
   ADJUST(R2,Loc2Amt);
  }
 IFADJUST(R3)
  {
   ADJUST(R3,ReferAmt1From);
   ADJUST(R3,ReferAmt2From);
   ADJUST(R3,ReferAmt3From);
   ADJUST(R3,ReferAmt4From);
   ADJUST(R3,AgeReferAmt1From);
   ADJUST(R3,AgeReferAmt2From);
   ADJUST(R3,AgeReferAmt3From);
   ADJUST(R3,AgeReferAmt4From);
   ADJUST(R3,OvrdAmt1From);
   ADJUST(R3,OvrdAmt2From);
   ADJUST(R3,OvrdAmt3From);
   ADJUST(R3,OvrdAmt4From);
   ADJUST(R3,AgeOvrdAmt1From);
   ADJUST(R3,AgeOvrdAmt2From);
   ADJUST(R3,AgeOvrdAmt3From);
   ADJUST(R3,AgeOvrdAmt4From);
   ADJUST(R3,ReferAmt1To);
   ADJUST(R3,ReferAmt2To);
   ADJUST(R3,ReferAmt3To);
   ADJUST(R3,ReferAmt4To);
   ADJUST(R3,AgeReferAmt1To);
   ADJUST(R3,AgeReferAmt2To);
   ADJUST(R3,AgeReferAmt3To);
   ADJUST(R3,AgeReferAmt4To);
   ADJUST(R3,OvrdAmt1To);
   ADJUST(R3,OvrdAmt2To);
   ADJUST(R3,OvrdAmt3To);
   ADJUST(R3,OvrdAmt4To);
   ADJUST(R3,AgeOvrdAmt1To);
   ADJUST(R3,AgeOvrdAmt2To);
   ADJUST(R3,AgeOvrdAmt3To);
   ADJUST(R3,AgeOvrdAmt4To);
  }
 /* R4 has no amounts
 IFADJUST(R4)
  {
  }
 */
 IFADJUST(R5)
  {
   ADJUST(R5,Amt);
  }
 IFADJUST(R6)
  {
   ADJUST(R6,Amt);
   ADJUST(R6,Accum);
  }
 IFADJUST(R7)
  {
   ADJUST(R7,DailyAmt);
   ADJUST(R7,Per1Amt);
   ADJUST(R7,Per2Amt);
   ADJUST(R7,Per3Amt);
  }
 IFADJUST(R8)
  {
   ADJUST(R8,Amt);
   ADJUST(R8,Accum);
  }

}

#undef IFADJUST
#define IFADJUST(n)\
 if ( Data.n.n##Enable != ' ' &&\
      Data.n.n##Product != ' ' &&\
      (Rules=(BYTE*)pickRules(Data.n.n##Product,&ProdAdj.Factors[0],\
                   &ProdAdj.Factors[1],&ProdAdj.Factors[2])) != NULL &&\
      (Adjustment=Rules[ProductClass-1]) != UCHAR_MAX )


//----------------------------------------------------------------
// Applies Product Adjustments which were saved in the Merge phase
// once the Product SubGroup is known.
//-----------------------------------------------------------------
bool AuthRules::ApplyProductAdjustments(IPCMessage* Msg)
{
 PRODUCTADJUSTMENTS ProdAdj;
 int Len=sizeof(ProdAdj);
 DWORD ProductClass;
 BYTE* Rules;
 BYTE Adjustment;

 // Must Have Previously Saved Product Adjustments and the
 // Product class (subgroup) to do this
 if ( ! Msg->GetFld(FN_PRODUCTADJUSTMENTS,&ProdAdj,Len) ||
      ! Msg->GetFld(FN_PRODUCTCLASS,ProductClass) ||
      ProductClass == 0 ||
      ProductClass > NUMPRODUCTSUBGROUPS )
  return false;

 IFADJUST(R1)
  {
   ADJUST(R1,OvrdAmt1);
   ADJUST(R1,OvrdAmt2);
   ADJUST(R1,ReferAmt1);
   ADJUST(R1,ReferAmt2);
  }
 IFADJUST(R2)
  {
   ADJUST(R2,Sic1Amt);
   ADJUST(R2,Sic2Amt);
   ADJUST(R2,Acct1Amt);
   ADJUST(R2,Acct2Amt);
   ADJUST(R2,Loc1Amt);
   ADJUST(R2,Loc2Amt);
  }
 IFADJUST(R3)
  {
   ADJUST(R3,ReferAmt1From);
   ADJUST(R3,ReferAmt2From);
   ADJUST(R3,ReferAmt3From);
   ADJUST(R3,ReferAmt4From);
   ADJUST(R3,AgeReferAmt1From);
   ADJUST(R3,AgeReferAmt2From);
   ADJUST(R3,AgeReferAmt3From);
   ADJUST(R3,AgeReferAmt4From);
   ADJUST(R3,OvrdAmt1From);
   ADJUST(R3,OvrdAmt2From);
   ADJUST(R3,OvrdAmt3From);
   ADJUST(R3,OvrdAmt4From);
   ADJUST(R3,AgeOvrdAmt1From);
   ADJUST(R3,AgeOvrdAmt2From);
   ADJUST(R3,AgeOvrdAmt3From);
   ADJUST(R3,AgeOvrdAmt4From);
   ADJUST(R3,ReferAmt1To);
   ADJUST(R3,ReferAmt2To);
   ADJUST(R3,ReferAmt3To);
   ADJUST(R3,ReferAmt4To);
   ADJUST(R3,AgeReferAmt1To);
   ADJUST(R3,AgeReferAmt2To);
   ADJUST(R3,AgeReferAmt3To);
   ADJUST(R3,AgeReferAmt4To);
   ADJUST(R3,OvrdAmt1To);
   ADJUST(R3,OvrdAmt2To);
   ADJUST(R3,OvrdAmt3To);
   ADJUST(R3,OvrdAmt4To);
   ADJUST(R3,AgeOvrdAmt1To);
   ADJUST(R3,AgeOvrdAmt2To);
   ADJUST(R3,AgeOvrdAmt3To);
   ADJUST(R3,AgeOvrdAmt4To);
  }
 /* R4 has no amounts
 IFADJUST(R4)
  {
  }
 */
 IFADJUST(R5)
  {
   ADJUST(R5,Amt);
  }
 IFADJUST(R6)
  {
   ADJUST(R6,Amt);
   ADJUST(R6,Accum);
  }
 IFADJUST(R7)
  {
   ADJUST(R7,DailyAmt);
   ADJUST(R7,Per1Amt);
   ADJUST(R7,Per2Amt);
   ADJUST(R7,Per3Amt);
  }
 IFADJUST(R8)
  {
   ADJUST(R8,Amt);
   ADJUST(R8,Accum);
  }

 // Delete the Factors once applied so they wont be applied again
 Msg->DelFld(FN_PRODUCTADJUSTMENTS);
 // Put the AuthRules back to the Message with changes
 Msg->PutFld(FN_AUTHRULES,&Data,sizeof(Data));
 return true;
}


//---------------------------------------------------------------------
// Handles situations where the Enabler points to another level (which
// may point to another level. Use the highest level's enabler
//---------------------------------------------------------------------
void AuthRules::chooseLevel(BYTE& level,BYTE globalLevel,BYTE classLevel,
                            BYTE subClassLevel)
{
 if ( subClassLevel &&
      level=='S' )
  return;

 if ( level=='C' )
  level=classLevel;

 if ( level=='G' )
  level=globalLevel;
}



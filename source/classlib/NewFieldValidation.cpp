// This is just some copy of FIELDVALIDATION.CPP
Probably quite useless

//----------------------------------------------------------------------------
//  Project Classlib
//
//  Copyright © 1996. All Rights Reserved.
//
//  FILE:         FieldValidation.cpp
//  AUTHOR:       Jack Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of FieldValidation.
//
//  These are member functions of the CheckAuth object related to
//  the validation of fields. It uses the Data Requirements record
//  to check for missing fields and contains validation routines for
//  fields that are required.
//
// MAB111797 - Business checks are allowed for Hold Check Merchants
// MAB112697 - Validate licences by state
// MAB112897 - if licence is all '0's, make blank
//
//----------------------------------------------------------------------------

#include "win4me.h"
#include <ctype.h>
#include "CheckAuth.h"
#include "CheckAuthFn.h"
#include "Rcodes.h"
#include "unsnum.h"
#include "splib.h"
#include <stdio.h>

// MAB112697
#include <stdlib.h>
#include <string.h>
#include <mem.h>

// if defined, uses full licence validation
#define DO_LICENCE_CHECK

#define WAS_DISABLED           -1
#define FILE_OPEN_ERROR      1001
#define STATE_NOT_FOUND      1002
#define LENGTH_ERROR         1003
#define LEADING_ALPHA_ERROR  1004
#define ALPHA_COUNT_ERROR    1005
#define WAS_NOT_SSN          1006
#define NUM_STATES             64
#define LEN_NUM_BUF            71
typedef struct _STATE_LICENSE_DATA
    {   long lState;
        int  iMaxChars;
        int  iMinChars;
        int  iMaxAlphas;
        int  iMinAlphas;
        int  iHasLeadAlpha;
        int  iIsSSNType;
        int  iMoreFlag;
        int  iIsDisabled;
    }STATE_LICENSE_DATA;

static int isValidGenericLicense(char* License, char *szState);
static int isValidTableLicense(char* License, long lState, int* piMoreFlag);
static int isValidStatesLicense(char* License, long lState);
static int iIsCorrectNumberOfDigits(char* License, int iCount);
// MAB112697 end

static bool isAllNumeric(char* String,int Len);
static bool ValidStateCode(char* StateCode);
bool isCanadian(char* StateCode);
#define FN(default) FLDNUM Fn=(fn==-1) ? FN_##default : (FLDNUM) fn

void logg1(int iLine, char *buf1, char *buf2, int i, long l);

void logg1(int iLine, char *buf1, char *buf2, int i, long l)
{
   FILE  *fout;
   fout = fopen("err.log", "a");
   fprintf(fout,"@%d %s - %s. .%ld. .%d.\n", iLine, buf1, buf2, i, l);
   fclose(fout);
}

//-------------------------------------------------------------
// Do the Field validations (ie., make sure required fields are
// present and have valid data. Returns fieldnumber of first
// field that's invalid or in error).
//
// Note: Validation for fields can be overridden. See the Override
// function.
//
// Provide Type of Validation required (POS,VOICE,BANKVERIFY).
// Returns true if okay, false if not.
//
// Also returns the Field Number of the field that is Missing or Invalid,
// A boolean indicator of whether it's missing or invalid, and an
// appropriate Rcode for use in a POS response.
//------------------------------------------------------------------
#define NOTOVERRIDE(fn)\
  (OverrideFields==0 || ! isOverrideField(fn))
#define VALIDATE(fn,reqfn,valfunc,missingRc,invalidRc)\
 if ( NOTOVERRIDE(fn) )\
      if ( GetFldInfo(fn) )\
                   {\
                    if ( ! valfunc() )\
                     {\
                      isMissing=false;\
                      Fn=fn;\
                      Rcode=invalidRc;\
                      return false;\
                     }\
                   }\
                  else\
                  if ( dataReq.isRequired(Type,dataReq.reqfn,TestAmount) )\
                   {\
                    isMissing=true;\
                    Fn=fn;\
                    Rcode=missingRc;\
                    return false;\
                   }

// This requires only if not a business check
#define PERSONALCHECKVALIDATE(fn,reqfn,valfunc,missingRc,invalidRc)\
 if ( NOTOVERRIDE(fn) )\
      if ( GetFldInfo(fn) )\
                   {\
                    if ( ! valfunc() )\
                     {\
                      isMissing=false;\
                      Fn=fn;\
                      Rcode=invalidRc;\
                      return false;\
                     }\
                   }\
                  else\
                  if ( dataReq.isRequired(Type,dataReq.reqfn,TestAmount) &&\
                       CheckType != 'B' )\
                   {\
                    isMissing=true;\
                    Fn=fn;\
                    Rcode=missingRc;\
                    return false;\
                   }


bool CheckAuth::fieldValidation(int Type,FLDNUM& Fn,bool& isMissing,
                                BYTE& Rcode)
{
 unsigned int Amount;
 WORD  TestAmount;
 BYTE  CheckType=' ';

 if ( NOTOVERRIDE(FN_MERCHID) )
  {
   if ( ! GetFld(FN_MERCHID) )
    {
     Fn=FN_MERCHID;
     isMissing=true;
     Rcode=RCODE_MISSINGDATA;
     return false;
    }

#if 0
// MAB112797
   if ( ! GetFld(FN_MERCHID) )
    {
     Fn=FN_MERCHID;
     isMissing=true;
     Rcode=RCODE_MISSINGDATA;
     return false;
    }
#endif

   if ( ! isValidMerchantID() )
    {
     Fn=FN_MERCHID;
     isMissing=false;
     Rcode=RCODE_INVALIDDATA;
     return false;
    }
  }

 if ( NOTOVERRIDE(FN_AMOUNT) )
  {
   // Always have to have a valid Check Amount
   if ( ! isValidAmount() )
    {
     Fn=FN_AMOUNT;
     Rcode=RCODE_INVALIDAMOUNT;
     isMissing=false;
     return false;
    }

   if ( (Amount=CheckAmount()) == 0 )
    {
     Fn=FN_AMOUNT;
     Rcode=RCODE_MISSINGAMOUNT;
     isMissing=true;
     return false;
    }
  }
 else
  Amount = USHRT_MAX;

 if (Amount>USHRT_MAX)
  TestAmount = USHRT_MAX;
 else
  TestAmount = (WORD)Amount;

 VALIDATE(FN_CHECKTYPE,CHECKTYPE,isValidCheckType,
       RCODE_MISSINGDATA,RCODE_INVALIDDATA);
 GetFld(FN_CHECKTYPE,CheckType);
 PERSONALCHECKVALIDATE(FN_LICENSESTATE,LICENSE,isValidState,
       RCODE_MISSINGSTATE,RCODE_INVALIDSTATE);
 PERSONALCHECKVALIDATE(FN_LICENSE,LICENSE,isValidLicense,
       RCODE_MISSINGLICENSE,RCODE_INVALIDLICENSE);
 PERSONALCHECKVALIDATE(FN_DOB,DOB,isValidDob,
       RCODE_MISSINGDOB,RCODE_INVALIDDOB);
 VALIDATE(FN_CHECKNUMBER,CHECKNUMBER,isValidCheckNumber,
       RCODE_MISSINGCHECKNUMBER,RCODE_INVALIDCHECKNUMBER);
 VALIDATE(FN_BANKNUMBER,MICR,isValidABA,
       RCODE_MISSINGMICR,RCODE_INVALIDMICR);
 VALIDATE(FN_BANKACCOUNT,MICR,isValidAccount,
       RCODE_MISSINGMICR,RCODE_INVALIDMICR);
 PERSONALCHECKVALIDATE(FN_CONSUMERNAME,NAME,isValidName,RCODE_MISSINGDATA,
       RCODE_INVALIDDATA);
 VALIDATE(FN_TITLE,TITLE,isValidTitle,
       RCODE_MISSINGDATA,RCODE_INVALIDDATA);
 VALIDATE(FN_ADDRESS,ADDRESS,isValidAddress,
       RCODE_MISSINGDATA,RCODE_INVALIDDATA);
 VALIDATE(FN_CITY,CITY,isValidCity,
       RCODE_MISSINGDATA,RCODE_INVALIDDATA);
 VALIDATE(FN_CONSUMERSTATE,CONSUMERSTATE,isValidConsumerState,
       RCODE_MISSINGDATA,RCODE_INVALIDDATA);
 VALIDATE(FN_ZIPCODE,ZIPCODE,isValidZipCode,
       RCODE_MISSINGZIPCODE,RCODE_INVALIDZIPCODE);
 VALIDATE(FN_APARTMENT,RESIDENCETYPE,isValidResidenceType,
       RCODE_MISSINGDATA,RCODE_INVALIDDATA);
 VALIDATE(FN_PHONE,PHONE,isValidPhone,
       RCODE_MISSINGPHONE,RCODE_INVALIDPHONE);
 VALIDATE(FN_SSN,SSN,isValidSSN,
       RCODE_MISSINGSSN,RCODE_INVALIDSSN);
 VALIDATE(FN_PRODUCTCLASS,PRODUCTCLASS,isValidProductClass,
       RCODE_MISSINGDATA,RCODE_INVALIDDATA);
 VALIDATE(FN_TRANSPORTMETHOD,TRANSPORTMETHOD,isValidTransportMethod,
       RCODE_MISSINGDATA,RCODE_INVALIDDATA);

 // Business name depends on type of check
 if ( NOTOVERRIDE(FN_BUSINESSNAME) )
   if ( GetFldInfo(FN_BUSINESSNAME) )
    {
     if ( ! isValidBusinessName() )
      {
       isMissing=false;
       Fn=FN_BUSINESSNAME;
       Rcode=RCODE_INVALIDDATA;
       return false;
      }
     }
    else
     if ( dataReq.isRequired(Type,dataReq.BUSINESSNAME,TestAmount) &&
          CheckType == 'B' )
      {
       isMissing=true;
       Fn=FN_BUSINESSNAME;
       Rcode=RCODE_MISSINGDATA;
       return false;
      }

 VALIDATE(FN_PRIVATELABEL,PRIVATELABEL,isValidPrivateLabel,
       RCODE_MISSINGDATA,RCODE_INVALIDDATA);

 // If POS, check for missing Spouse/Employer Data. In voice,
 // this is taken care of in a separate step.
 if ( Type==Pos &&
      (Fn=needEmployerData(Type)) != 0 )
  {
   isMissing=true;
   Rcode=RCODE_MISSINGDATA;
   return false;
  }
 if ( Type==Pos &&
      (Fn=needSpouseCoSignerData(Type)) != 0 )
  {
   isMissing=true;
   Rcode=RCODE_MISSINGDATA;
   return false;
  }

 isMissing=false;
 return true;
}

//-------------------------------------------------------------
// Override field validation for a field by passing it's number
// to this member function before calling field validation.
//--------------------------------------------------------------
void CheckAuth::OverrideFieldValidation(FLDNUM fn)
{
 FLDNUM* newOverrideFields;
 register uint i;

 if ( OverrideFields == 0 )
  {
   MaxOverrideFields=10;
   NumOverrideFields=1;
   OverrideFields = new FLDNUM[MaxOverrideFields];
   OverrideFields[0] = fn;
   return;
  }

 // See if it's already in the list
 for (i=0; i<NumOverrideFields; ++i)
  if ( OverrideFields[i] == fn )
   return;

 // If buffer is full, allocate a new one
 if ( NumOverrideFields == MaxOverrideFields )
  {
   MaxOverrideFields += 10;
   newOverrideFields = new FLDNUM[MaxOverrideFields];
   memcpy(newOverrideFields,OverrideFields,NumOverrideFields * sizeof(FLDNUM));
   delete[] OverrideFields;
   OverrideFields = newOverrideFields;
  }

 OverrideFields[ NumOverrideFields++ ] = fn;
}

//----------------------------------------------
// Check if validation is overridden for a field
//----------------------------------------------
bool CheckAuth::isOverrideField(FLDNUM fn)
{
 register uint i;
 for (i=0; i<NumOverrideFields; ++i)
  if ( OverrideFields[i] == fn )
   return true;

 return false;
}

//----------------------------
// Check for Valid Merchant ID
//----------------------------
bool CheckAuth::isValidMerchantID(int fn)
{
 EditLen=sizeof(EditBuf);
 int i;
 FN(MERCHID);

 if ( GetFld(Fn,EditBuf,EditLen) )
  {
   if ( EditLen != 5 )
     return false;
   for (i=0; i<EditLen; ++i)
    if ( ! isalnum(EditBuf[i]) )
      return false;
  }

 return true;
}

//-----------------------
// Check for Valid Amount
//-----------------------
bool CheckAuth::isValidAmount(int fn)
{
 EditLen=sizeof(EditBuf);
 int i, rdp;
 bool hasDp;
 FN(AMOUNT);

 if ( GetFld(Fn,EditBuf,EditLen) )
  {
   for (i=rdp=0, hasDp=false; i<EditLen; ++i)
    if ( EditBuf[i] == '.' )
     {
      if ( hasDp )
       return false;
      hasDp=true;
     }
    else
     if ( ! isdigit(EditBuf[i]) )
      return false;
     else
      if ( hasDp )
       ++rdp;

   if ( ! hasDp )
    return false;
   if ( rdp != 2 )
    return false;
  }

 return true;
}

//-----------------------
// Validate Type of Check
//-----------------------
bool CheckAuth::isValidCheckType(int fn)
{
 EditLen=sizeof(EditBuf);
 FN(CHECKTYPE);

 // Must be P(ersonal) or B(usiness)
 if ( GetFld(Fn,EditBuf,EditLen) )
  {
   if ( EditBuf[0] != 'P' &&
        EditBuf[0] != 'B' )
    return false;
   // Business checks not allowed for Hold Check Merchants
   // MAB111797 - Business checks are allowed for Hold Check Merchants
   //if ( EditBuf[0] == 'B' &&
   //     isHoldCheckMerchant() )
   // return false;
  }

 return true;
}

//----------------------------
// Maryland License Validation
//----------------------------
static bool isValidMarylandLicense(char* License)
{
 register i;
 if (strlen(License) != 13 )
  return false;
 if ( ! isalpha(License[0]) )
  return false;
 for (i=1; i<13; ++i)
  if ( ! isdigit(License[i]) )
   return false;

 for (i=1; i<=3; ++i)
  if ( License[i] > '6' )
   return false;

 return true;
}

//----------------------------
// Lousiana License Validation
//----------------------------
static bool isValidLouisianaLicense(char* License)
{
 register i;

 if ( strlen(License) != 9 || memcmp(License,"00",2) )
  return false;

 for (i=0; i<9; ++i)
  if ( ! isdigit(License[i]) )
   return false;

 return true;
}

//--------------------------
// Validate Driver's License
//--------------------------
bool CheckAuth::isValidLicense(int statefn,int licfn)
{
 char State[3];
 unsigned iReturn;

 EditLen=sizeof(State);
 FLDNUM StateFn=(statefn==-1) ? FN_LICENSESTATE : (FLDNUM)statefn;
 FLDNUM LicFn=(licfn==-1) ? FN_LICENSE : (FLDNUM)licfn;

 if ( GetFld(StateFn,State,EditLen) )  // get the state
  {
   EditLen=sizeof(EditBuf);
   if ( ! GetFld(LicFn,EditBuf,EditLen) ) // get the licence
      return true;                        // if no licence, accept it


   // MAB 112897 - if licence is all '0's, make blank
    iReturn = strspn(EditBuf, "0 ");   // '0' and ' '
    if(iReturn == strlen(EditBuf))  // only '0' or ' ' found
    {
       PutFld(LicFn,""); // set the licence
       return true;
    }

   EditBuf[EditLen]=0;  // terminate licence string

  //logg1(__LINE__, "In IsValidLicence()-state string is",State, 0, 0);
  //logg1(__LINE__, "In IsValidLicence()-EditBuf string is",EditBuf, 0, 0);

#ifdef DO_LICENCE_CHECK
   // MAB112697
   return isValidGenericLicense(EditBuf, State);

#else
   if ( memcmp(State,"MD",2) == 0 )
     return isValidMarylandLicense(EditBuf);
   if ( memcmp(State,"LA",2) == 0 )
     return isValidLouisianaLicense(EditBuf);


   // Only Validating the CA license at this time
   if ( memcmp(State,"CA",2) )
   {
      return true;
   }

   if ( EditLen < 5 ||
        EditLen > 8 ||
        ! isalpha(EditBuf[0]) ||
        ! isAllNumeric(&EditBuf[1],EditLen-1) )
    return false;

//logg1(__LINE__, "In IsValidLicence()- dropping out true","", 0, 0);
   return true;

#endif
  }
 return true;
}

//--------------------
// Validate State Code
//--------------------
bool CheckAuth::isValidState(int fn)
{
 char szbuf[3] = "ZZ";
 EditLen=sizeof(EditBuf);
 FN(LICENSESTATE);

 if ( GetFld(Fn,EditBuf,EditLen) )
 {
    szbuf[0]=EditBuf[0];szbuf[1]=EditBuf[1];szbuf[2]='\0';
    if ( EditLen != 2 ||
         ! ValidStateCode(EditBuf) )
       return false;
 }
 return true;
}

//-----------------------
// Validate Date of Birth
//-----------------------
static unsigned char daysInMonth[13] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
bool CheckAuth::isValidDob(int fn)
{
 EditLen=sizeof(EditBuf);
 int v1, v2;
 FN(DOB);

 if ( GetFld(Fn,EditBuf,EditLen) )
  {
   if ( EditLen != 6 ||
        ! isAllNumeric(EditBuf,6) )
    return false;

   // Accept Either MMDDYY or DDMMYY
   daysInMonth[2]= (unsigned char) ((_atoi(&EditBuf[4],2) % 4) ? 28 : 29);
   v1 = _atoi(EditBuf,2);
   v2 = _atoi(&EditBuf[2],2);

   if ( ! (v1 >= 1 &&
           v1 <= 12 &&
           v2 >= 1 &&
           v2 <= daysInMonth[v1]) &&
        ! (v2 >= 1 &&
           v2 <= 12 &&
           v1 >= 1 &&
           v1 <= daysInMonth[v2]) )
   return false;
  }

 // Return a DOB error on any electronic transaction with an age
 // Greater than or equal to 90. We think it might be an expiration
 // date incorrectly keyed 
 if ( ! isVoice() && ConsumerAge() >= 90 )
   return false;

 return true;
}

//----------------------
// Validate Check Number
//----------------------
bool CheckAuth::isValidCheckNumber(int fn)
{
 EditLen=sizeof(EditBuf);
 FN(CHECKNUMBER);

 // Check Number just has to be all numeric
 if ( GetFld(Fn,EditBuf,EditLen) &&
      ! isAllNumeric(EditBuf,EditLen) )
  return false;

 return true;
}

//-----------------------------------------
// Validate Bank Transit and Routing Number
//-----------------------------------------
bool CheckAuth::isValidABA(int fn)
{
 EditLen=sizeof(EditBuf);
 FN(BANKNUMBER);

 if ( GetFld(Fn,EditBuf,EditLen) )
   if ( EditLen < 8  ||
        EditLen > 9  ||
        ! isAllNumeric(EditBuf,EditLen) )
    return false;

 return true;
}

//-----------------------------
// Validate Bank Account Number
//------------------------------
bool CheckAuth::isValidAccount(int)
{
 return true;
}

//-----------------------
// Validate Consumer Name
//-----------------------
bool CheckAuth::isValidName(int)
{
 // Not sure how to do this yet
 return true;
}

//------------------
// Validate Zip Code
//------------------
bool CheckAuth::isValidZipCode(int fn)
{
 EditLen=sizeof(EditBuf);
 FN(ZIPCODE);

 if ( GetFld(Fn,EditBuf,EditLen) )
  {
   if ( EditLen < 5 ||
        ! isAllNumeric(EditBuf,EditLen) )
    return false;
  }

 return true;
}

//----------------------
// Validate Phone Number
//----------------------
bool CheckAuth::isValidPhone(int fn)
{
 EditLen=sizeof(EditBuf);
 FN(PHONE);

 if ( GetFld(Fn,EditBuf,EditLen) )
   if ( EditLen != 10 ||
       ! isAllNumeric(EditBuf,10) )
    return false;

 return true;
}

//--------------------------------
// Validate Social Security Number
//--------------------------------
bool CheckAuth::isValidSSN(int fn)
{
 EditLen=sizeof(EditBuf);
 FN(SSN);

 if ( GetFld(Fn,EditBuf,EditLen) )
  {
   if ( EditLen != 9 )
    return false;
   if ( ! isAllNumeric(EditBuf,9) )
    return false;
   // 728 is the highest number as of this date
   if ( memcmp(EditBuf,"728",3) > 0 )
    return false;
   // No SSN has ever been issued with 0 group number or 0 serial number
   if ( memcmp(EditBuf,"000",3) == 0 ||
        memcmp(&EditBuf[3],"00",2) == 0 ||
        memcmp(&EditBuf[5],"0000",4) == 0 )
    return false;
  }

 return true;
}

//--------------------------------------------------------
// These are ones that don't need any validation right now
//--------------------------------------------------------

//------------------------
// Validate Consumer State
//------------------------
bool CheckAuth::isValidConsumerState(int)
{
 // This comes from a menu in Voice
 return true;
}


//-----------------------
// Validate Product Class
//-----------------------
bool CheckAuth::isValidProductClass(int)
{
 // Who knows?
 return true;
}

//--------------------------
// Validate Transport Method
//--------------------------
bool CheckAuth::isValidTransportMethod(int)
{
 // Voice Menu only, don't worry about it
 return true;
}

//-----------------------
// Validate Business Name
//-----------------------
bool CheckAuth::isValidBusinessName(int)
{
 // Nothing to really look at right now
 return true;
}

//-----------------------
// Validate Private Label
//-----------------------
bool CheckAuth::isValidPrivateLabel(int)
{
 // Voice Only From Menu. No validation routine needed
 return true;
}

//---------------
// Validate Title
//---------------
bool CheckAuth::isValidTitle(int)
{
 // This comes from menu in voice only so we don't need a validation
 // routine.
 return true;
}

//-----------------
// Validate Address
//-----------------
bool CheckAuth::isValidAddress(int)
{
 // What can we do?
 return true;
}

//------------------------
// Validate Residence Type
//------------------------
bool CheckAuth::isValidResidenceType(int)
{
 // Voice Menu Only, don't worry about it
 return true;
}

//--------------
// Validate City
//--------------
bool CheckAuth::isValidCity(int)
{
 return true;
}

//----------------------------------
// Return if a string is all numeric
//----------------------------------
static bool isAllNumeric(char* String,int Len)
{
 register i;
 for(i=0; i<Len; ++i)
  if ( ! isdigit(String[i]) )
   return false;

 return true;
}

static char StateCodeArray[] = {
  "AB"  "AK" "AL"  "AR" "AS"  "AZ"
  "BC"
  "CA" "CO" "CT" "CZ"
  "DC" "DE" "DS"
  "FL"
  "GA" "GU"
  "HI"
  "IA" "ID" "IL" "IN"
  "KS" "KY"
  "LA"
  "MA" "MB" "MD" "ME" "MI" "MN" "MO" "MS" "MT"
  "NB" "NC" "ND" "NE" "NF" "NH" "NJ" "NM" "NS" "NT" "NV" "NY"
  "OH" "OK" "ON" "OR"
  "PA" "PE" "PR"
  "QP"
  "RI"
  "SC" "SD" "SK"
  "TN" "TX"
  "UT"
  "VA" "VI" "VT"
  "WA" "WI" "WV" "WY"
  "YT" };

static char CanadianProvinces[] =
 { "AB" "BC" "MB" "NB" "NF" "NS" "NT" "ON" "PE" "PQ" "SK" "YT" };

static int comparefunc(const void* item1,const void* item2)
{
 return memcmp(item1,item2,2);
}

//---------------------------
// Return if valid state code
//---------------------------
static bool ValidStateCode(char* StateCode)
{
 return bsearch(StateCode,StateCodeArray,sizeof(StateCodeArray)/2,2,
                comparefunc);
}

bool isCanadian(char* StateCode)
{
 return bsearch(StateCode,CanadianProvinces,
               sizeof(CanadianProvinces)/2,2,comparefunc);
}

//-------------------------------------------------
// See if the transaction needs any Employment Data
//--------------------------------------------------
#define TEST(bit,fn)\
  if ( NOTOVERRIDE(fn) && dataReq.isRequired(Type,dataReq.bit) &&\
         ! GetFld(fn) )\
    return fn

FLDNUM CheckAuth::needEmployerData(int Type)
{
 unsigned int Amount=CheckAmount();
 unsigned short TestAmount;
 if (Amount>USHRT_MAX)
  TestAmount = USHRT_MAX;
 else
  TestAmount = (WORD) Amount;

 // Check for any missing Employer Data fields
 if ( dataReq.isRequired(Type,dataReq.EMPLOYMENTDATA,TestAmount) )
  {
   TEST(EMPLOYERNAME,FN_EMPLOYERNAME);
   TEST(EMPLOYERADR,FN_EMPLOYERADR1);
   TEST(EMPLOYERPHONE,FN_EMPLOYERPHONE);
   TEST(SALARY,FN_SALARY);
   TEST(YEARSEMPLOYED,FN_YEARSEMPLOYED);
   TEST(POSITION,FN_POSITION);
  }

 // We're including the Finance Amount in Employer Data
 if ( dataReq.isRequired(Type,dataReq.FINANCEAMOUNT,TestAmount) &&
      ! GetFld(FN_FINANCEAMOUNT) )
  return FN_FINANCEAMOUNT;

 return 0;
}

//------------------------------------------------------
// See if the transaction needs any Spouse/Cosigner Data
//------------------------------------------------------
#define TEST(bit,fn)\
  if ( NOTOVERRIDE(fn) && dataReq.isRequired(Type,dataReq.bit) &&\
         ! GetFld(fn) )\
    return fn

FLDNUM CheckAuth::needSpouseCoSignerData(int Type)
{
 unsigned int Amount=CheckAmount();
 unsigned short TestAmount;
 if (Amount>USHRT_MAX)
  TestAmount = USHRT_MAX;
 else
  TestAmount = (WORD) Amount;

 // Check for any Missing Spouse/Co-Signer fields
 if ( dataReq.isRequired(Type,dataReq.SPOUSEDATA,TestAmount) )
  {
   TEST(SPOUSENAME,FN_SPOUSENAME);
   TEST(SPOUSESSN,FN_SPOUSESSN);
   TEST(SPOUSEEMPLOYER,FN_SPOUSEEMPLOYERNAME);
   TEST(SPOUSEEMPLOYERADR,FN_SPOUSEEMPLOYERADR1);
   TEST(SPOUSEEMPLOYERPHONE,FN_SPOUSEEMPLOYERPHONE);
   TEST(SPOUSESALARY,FN_SPOUSESALARY);
   TEST(SPOUSEYEARSEMPLOYED,FN_SPOUSEYEARSEMPLOYED);
   TEST(SPOUSEPOSITION,FN_SPOUSEPOSITION);
  }

 // Check for presence of Finance Amount. Not really part of
 // Spouse/CoSigner data but in Voice it's on the same screen
 if ( NOTOVERRIDE(FN_FINANCEAMOUNT) &&
      dataReq.isRequired(Type,dataReq.FINANCEAMOUNT,TestAmount) &&
      ! GetFld(FN_FINANCEAMOUNT) )
  return FN_FINANCEAMOUNT;

 return 0;
}

#ifdef DO_LICENCE_CHECK
// MAB112697
//--------------------------------------------------------------------
// Generic License Validation
// This function calls the two functions that actually do the
// validation and it interprets their error returns into true
// or false for the calling application
//--------------------------------------------------------------------
static int isValidGenericLicense(char* License, char *szState)
{
    int  iReturn;
    int  iMoreFlag = 0;
    int* piMoreFlag = &iMoreFlag;
    long lState;

    lState = (long)((szState[0] - 64) * 100);
    lState += (long)(szState[1] - 64);

//logg1(__LINE__, "In IsValidLicence()-state string is",License, lState, 0);
    iReturn = isValidTableLicense(License, lState, piMoreFlag);
    if( ((iReturn == true) && (iMoreFlag == 0) )  ||
        (iReturn == WAS_DISABLED)
      )
        return true;

    if(iReturn == FILE_OPEN_ERROR)
        return iReturn;
    if( (iReturn == ALPHA_COUNT_ERROR)   ||
        (iReturn == LEADING_ALPHA_ERROR) ||
        (iReturn == LENGTH_ERROR)        ||
        (iReturn == STATE_NOT_FOUND)     ||
        (iReturn == WAS_NOT_SSN)         ||  // needs SSN
        ( (iReturn == false) && (iMoreFlag == 0) )
      )
     {
//logg1(__LINE__, "Fail IsValidLicence()-Licence is",License, lState, iReturn);
        return false;
     }
    if(iMoreFlag == 1)   // try the rest of the functions
       iReturn = isValidStatesLicense(License,lState);

    if(iReturn == true)
        return true;

    //if(iReturn == false)
    //     return false;

    return false;  // should never reach here
}
                               
//----------------------------------------------------------------------
// Generic Table License Validation
// This function validates licences acording to values in the
// statelic.dat file.  The first time it is called, it reads the
// data from file, into static memory. Subsequent calls, do not read
// from file.  It then checks length, alpha content, is it a SSN type,
// does it have a lead alpha, etc.  It also returns a value that will
// be interpreted as true if a state validation is set to disabled.
//----------------------------------------------------------------------
static int isValidTableLicense(char* License, long lState, int* piMoreFlag)
{
    char szStateBuf[4];
    char szNumBuf[LEN_NUM_BUF];
    char szbuf[LEN_NUM_BUF];
    FILE *fin;
    int ii;
    int iReturn;
    int iCount;
    int iAlphaCounter;
    static int iStateDataHasBeenReadFromFile;
    static STATE_LICENSE_DATA dat[NUM_STATES];

    // get the data from file to memory once
    if(!( iStateDataHasBeenReadFromFile))  // file has not been read yet
    {
       if((fin = fopen("statelic.dat", "r")) == NULL)  // couldn't open file
          return FILE_OPEN_ERROR;

       for(ii = 0; ii < NUM_STATES; ii++)
       {
          memset(szbuf,'\0',sizeof(szbuf));
          memset(szStateBuf,'\0',sizeof(szStateBuf));
          memset(szNumBuf,'\0',sizeof(szNumBuf));

          fgets(szbuf, LEN_NUM_BUF, fin);
          strncpy(szStateBuf,szbuf,3);
          strcpy(szNumBuf,&szbuf[2]);

          sscanf(szNumBuf,"%ld %d %d %d %d %d %d %d %d", //szStateBuf,
            &dat[ii].lState,
            &dat[ii].iMaxChars, &dat[ii].iMinChars, &dat[ii].iMaxAlphas,
            &dat[ii].iMinAlphas, &dat[ii].iHasLeadAlpha, &dat[ii].iIsSSNType,
            &dat[ii].iMoreFlag, &dat[ii].iIsDisabled);
       }
       fclose(fin);
       iStateDataHasBeenReadFromFile = 1;
    }

    // find data for state name
    for(iCount = 0; iCount < NUM_STATES; iCount++)
    {
       if(dat[iCount].lState == lState)
          break;
    }
    if(iCount == NUM_STATES)
       return STATE_NOT_FOUND;

    if(dat[iCount].iIsDisabled)  // has been turned off
       return WAS_DISABLED;

    if(dat[iCount].iIsSSNType)  // if it could be a SSN
    {
       iReturn = iIsCorrectNumberOfDigits(License, 9);   // is it 9 digits?
       if(iReturn == true)   // is it 9 digits?
           return true;       // definate true

       if(dat[iCount].iIsSSNType == 2)  // if it must be a SSN
          if(iReturn == false)
             return WAS_NOT_SSN;  // false that overrides MoreFlag
    }

    if(dat[iCount].iMoreFlag)  // save the more flag for the next function up
       *piMoreFlag = true;

    // check length
    if ( ((int)strlen(License) > dat[iCount].iMaxChars) ||
         ((int)strlen(License) < dat[iCount].iMinChars) )
       return LENGTH_ERROR;

    if(dat[iCount].iHasLeadAlpha)  // check for leading alpha
       if(!(isalpha(License[0])))
          return LEADING_ALPHA_ERROR;

    // check for number of alphas
    iAlphaCounter = 0;
    for(ii = 0; ii < (int)strlen(License); ii++)
       if( (isalpha(License[ii])) || (License[ii] == '*') )
          iAlphaCounter++;
    if( (iAlphaCounter > dat[iCount].iMaxAlphas)  ||
        (iAlphaCounter < dat[iCount].iMinAlphas) )
        return ALPHA_COUNT_ERROR;

    return true;
}

//----------------------------------------------------------------------
// These are the functions for each state that cannot be validated using
// information in the table.
//----------------------------------------------------------------------
static int isValidStatesLicense(char* License, long lState)
{
   register ii;
   int iReturn;
   char szbuf[100];
   int  iNumberOfDigits = 0;
   int  iNumberOfAlphas = 0;

   for(ii=0; ii<(int)strlen(License); ii++)  // get number of digits
       if(isdigit(License[ii]))
          iNumberOfDigits++;

   for(ii=0; ii<(int)strlen(License); ii++)  // get number of alphas
       if(isalpha(License[ii])) // || (License[ii] == '*') )
          iNumberOfAlphas++;

   if (lState == 126 )  // Arizona
   {
      if( (isalpha(License[0])) && (iNumberOfDigits == 8))  // A########
         return true;

      if(strlen(License) == 7)
      {
         if( (isalpha(License[0])) && (iNumberOfDigits == 6) )  // A######
            return true;

         if( ( (isalpha(License[0])) && (isalpha(License[1])) ) &&
             (iNumberOfDigits == 5)  //AA#####
           )
         return true;
      }
      return false;
   }

   else if (lState == 315 )  // Colorado - ssn already checked
   {
       if( (iNumberOfDigits > 6) || (iNumberOfAlphas > 5) )
           return false;
   }

   else if (lState == 320 )  // Connecticut
   {
       szbuf[0] = License[0]; szbuf[1] = License[1]; szbuf[2] ='\0';
       iReturn = atoi(szbuf);
       if((iReturn > 24) || (iReturn < 1))
           return false;
       // ### Must get birthday for this
       return true;
   }

   else if (lState == 904 )  // Idaho
   {
      if(isalpha(License[0]))  //AA######A
      {
         if( (isalpha(License[1])) && (isalpha(License[8])) )
             return true;  // it has already been checked for 6# minimum
      }
      return false;
   }

   //else if ( memcmp(State,"IL",2) == 0 )  // Illinois
   //{ all kinds of stuff }

   else if (lState == 901 )  // Iowa
   {
      if( (isalpha(License[3])) && (isalpha(License[4])) )  //###AA####
         return true;  // it has already been checked for 7# minimum
      return false;
   }

   else if (lState == 1118 )  // Kansas
   {
      if(License[0] == 'K')   //K########
         return true;  // it has already been checked for 8# minimum
      return false;
   }

   else if (lState == 1125 )  // Kentucky
   {
      if(iNumberOfDigits == 10)  // was already checked for 10 max
         return true;
      if( (isalpha(License[0])) && (iNumberOfDigits == 8) )  //A########
         return true;  // it has already been checked for 8# minimum
      return false;
   }

   else if (lState == 1201 )  // Louisiana
   {
      if(memcmp(License,"00",2))
         return false;
      return true;
   }

   else if (lState == 1305 )  // Maine ####### or #######X
   {
      if( (strlen(License) == 8)  &&
          (License[7] == 'X') )   //#######X - proqavisional or under 21
             return true;  // it has already been checked for 7# minimum

      if(strlen(License) == 7)
         if(iIsCorrectNumberOfDigits(License, 7))
            return true;

      return false;
   }

   //else if ( memcmp(State,"MD",2) == 0 )  // Maryland A############
   //{;}   // complicated

   else if (lState == 1301 )  // Massachusetts
   {
      if(License[0] == 'S')   //S########
         return true;  // it has already been checked for 8# minimum
      return false;
   }

   //else if ( memcmp(State,"MI",2) == 0 )  // Michigan A############
   //{;}   // first letter of last name and other complicated stuff

   //else if ( memcmp(State,"MN",2) == 0 )  // Minnesota A############
   //{;}   // first letter of last name plus other

   else if (lState == 1315 )
   {  // Missouri ######### for CDL A########
      if(isalpha(License[0]))   //S########
         return true;  // it has already been checked for 1# minimum
      return false;
   }

   else if (lState == 1405 )  //Nebraska
   {  //  H,V,G,E,C,B or A followed by up to 8#
      if( (License[0] == 'H') ||
          (License[0] == 'V') ||
          (License[0] == 'G') ||
          (License[0] == 'E') ||
          (License[0] == 'C') ||
          (License[0] == 'B') ||
          (License[0] == 'A') )
         return true;
      return false;
   }

   else if (lState == 1422 )  //Nevada
   {  //  ############ or X########
      if(iIsCorrectNumberOfDigits(License, 12))  // last 2 digits will be year OB ###
          return true;
      //if( (isalpha(License[0])) && (strlen(License) == 9) )  //S########
      if( (License[0] == 'X') && (strlen(License) == 9) &&  //S########
          (iNumberOfAlphas == 1)
        )
         return true;
      return false;
   }

   else if (lState == 1408 )  // New Hampshire
   {  //  ##AAA##### = MOB, letters from name, YOB, DOB, #
      for (ii=0; ii<2; ii++)
         if ( ! isdigit(License[ii]) )
            return false;
      for (ii=2; ii<5; ii++)
         if ( ! isalpha(License[ii]) )
            return false;
      for (ii=5; ii<10; ii++)
         if ( ! isdigit(License[ii]) )
            return false;
      return true;
   }

   //else if ( memcmp(State,"NJ",2) == 0 )  // New Jersey
   //{ Uses Date }

   else if (lState == 1425 )  // New York
   {
      if( (isalpha(License[0])) &&
          ( (strlen(License) == 19) || (strlen(License) == 22) )
        )
          return true;
      return false;
   }

   else if (lState == 1508 )  // Ohio
   {
      if( (isalpha(License[0])) &&
          (isalpha(License[1])) &&
          (strlen(License) == 8)
        )
          return true;
      return false;
   }

   //else if ( memcmp(State,"RI",2) == 0 )  // Rhode Island

   else if (lState == 2201 )    // VA Virginia
   {
      if( (License[0] == 'R') ||   // R######## or T#######
          (License[0] == 'T') )
         return true;  // it has already been checked for 8# minimum
      return false;
   }

   else if (lState == 2301 )    // Washington   AAAAAAA##---
   {
      iReturn = 0;
      for (ii=0; ii<7; ii++)
         if( (isalpha(License[ii])) || (License[ii] == '*') )
            iReturn++;
      if(iReturn != 7)
            return false;

      for (ii=7; ii<9; ii++)
         if(isdigit(License[ii]))
            iReturn++;
      if(iReturn != 9)
            return false;

      return true;
   }
   return true;
}

//----------------------------------------------------------------------
static int iIsCorrectNumberOfDigits(char* License, int iCount)
{
   register i;
   if((int)strlen(License) > iCount)
      return false;
   for (i=0; i<iCount; i++)
      if ( ! isdigit(License[i]) )  // ##########
         return false;
   return true;
}
#endif


// This is the LICENCE.DAT table.
#if 0
CA    301         8       5       1      1     1     0     0        0
AL    112         7       7       0      0     0     0     0        0
AK    111         7       1       0      0     0     0     0        0
AZ    126         9       7       2      0     0     1     1        0
AR    118         9       5       0      0     0     1     0        0
CO    315        11       2       5      0     0     1     1        0
CT    320         9       9       0      0     0     0     1        0
DE    405         7       1       0      0     0     0     0        0
DC    403         9       1       0      0     0     1     0        0
FL    612        13      12       1      1     1     0     0        0
GA    701         9       1       0      0     0     1     0        0
HI    809         9       9       0      0     0     2     0        0
ID    904         9       9       3      0     0     1     1        0
IL    912        12      12       1      1     1     0     0        0
IN    914        10      10       0      0     0     0     0        0
IA    901         9       7       2      0     0     1     1        0
KS   1119         9       9       1      0     0     1     1        0
KY   1125        10       9       1      0     0     1     1        0
LA   1201         9       9       0      0     0     0     1        0
ME   1305         8       7       1      0     0     0     1        0
MD   1304        13      13       1      1     1     0     0        0
MA   1301         9       9       1      0     0     1     1        0
MI   1309        13      13       1      1     1     0     0        0
MN   1314        13      13       1      1     1     0     0        0
MS   1319         9       1       0      0     0     1     0        0
MO   1315        10       2       1      0     0     1     1        0
MT   1320         9       9       9      0     0     1     0        0
NE   1405         9       1       1      1     1     0     1        0
NV   1422        12       9       1      0     0     0     1        0
NH   1408        10      10       3      3     0     0     1        0
NJ   1410        15      15       1      1     1     0     0        0
NM   1413         9       8       0      0     0     1     0        0
NY   1425        22       9       1      1     0     1     1        0
NC   1403        12       1       0      0     0     0     0        0
ND   1404         9       1       0      0     0     1     0        0
OH   1508         9       8       2      0     0     1     1        0
OK   1511         9       9       0      0     0     2     0        0
OR   1518         7       1       0      0     0     0     0        0
PA   1601         8       8       0      0     0     0     0        0
RI   1809         7       7       0      0     0     0     0        0
SC   1903         9       9       0      0     0     2     0        0
SD   1904         9       8       0      0     0     1     0        0
TN   2014         8       8       0      0     0     0     0        0
TX   2024         8       8       0      0     0     0     0        0
UT   2120         9       4       0      0     0     0     0        0
VT   2220         8       8       1      0     0     0     0        0
VA   2201         9       9       1      0     0     1     1        0
WA   2301        12      12       9      0     1     0     0        0
WV   2322         7       7       1      0     0     0     0        0
WI   2309        14      14       1      1     1     0     0        0
WY   2325        10       9       0      0     0     0     0        0
AB    102         9       1       0      0     0     1     0        0
BC    203         7       7       0      0     0     0     0        0
MB   1302        12      12       9      7     0     0     0        0
NB   1402         7       1       7      0     0     0     0        0
NF   1406        10      10       1      1     1     0     0        0
NS   1419        14      12       5      5     0     0     0        0
ON   1514        15      15       1      1     1     0     0        0
PE   1605        12       1       0      0     0     0     0        0
PQ   1617        13      13       1      1     1     0     0        0
SK   1911         8       8       0      0     0     0     0        0
GU    721         9       1       0      0     0     1     0        0
CZ    326         9       1       0      0     0     1     0        0
PR   1618        11       0       0      0     0     0     0        0

     int         MaxC    MinC    MaxA   MinA  Lead  SSN   More   Disable

NT - North West Territories
YT - Yukon Territory

    int  - literally, the 2 chars converted ascii wise to an int. A=01 B=02 Z=26
    MaxC - Maximum characters in Licence Number
    MinC - Minimum characters in Licence Number
    MaxA - Maximum Alpha characters in Licence Number
    MinA - Minimum Alpha characters in Licence Number
    Lead - Is there a leading Alpha character in Licence Number
    SSN  - 1 == it might be a SSN, 2 == it must be a SSN
    More - There is another test function after the table test
    Disable - All validation for the state is disabled.

    Setting SSN to 2 and More to 1 are mutually exclusive


#endif

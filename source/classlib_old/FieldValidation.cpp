//----------------------------------------------------------------------------
//  Project Classlib
//
//  Copyright ? 1996. All Rights Reserved.
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
// MAB112697 - Validate licenses by state
// MAB112897 - if license is all '0's, make blank
// JHE061698 - Do not required DL fields if Business Check
//           - Handle new LICENSEORMICR data requirement
//           - Validate Account number to insure not all zeroes
// JHE070398 - Always Require License and MICR to be entered in Pairs
//             (ie., State Code and License, Bank Number and Account Number).
//
// JHE071798 - Modified to not require DL if MICR only
// JHE120898 - Modified to not require DL/STATE CODE pair if LICENSEORMICR
//             flag is on and MICR is present.       
// MAB060999 - Make sure that the relationship between the Bank Number and the
//             Bank Account is validated
// MAB060999 - Do a Check Sum on the Bank Routing Number
// MAB061599 - Make sure that Bank Account Number is at least 5 digits long
// MAB111599 - Allow for two lengths of Bank Account Number
// MAB011700 - Add a validation for California Licenses
// MAB042402 - Allow DL verification to have more than one line per state
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

// MAB061599
#define MIN_ACCOUNT_LEN    5

// if defined, uses full license validation
#define DO_LICENSE_CHECK

#define WAS_DISABLED           -1
#define FILE_OPEN_ERROR      1001
#define STATE_NOT_FOUND      1002
#define LENGTH_ERROR         1003
#define LEADING_ALPHA_ERROR  1004
#define ALPHA_COUNT_ERROR    1005
#define WAS_NOT_SSN          1006
#define NUM_STATES            100
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
//static int isValidTableLicense(char* License, long lState, int* piMoreFlag);
static int isValidStatesLicense(char* License, long lState);
static int iIsCorrectNumberOfDigits(char* License, int iCount);
// MAB112697 end


#define VERSION_060900    0
// Moved to CheckAuth.cpp
//#if VERSION_060900
#if 1
// MAB060999
// This makes sure that the relationship between the Bank Number and the
// Bank Account is validated
#define DO_BANK_ACCOUNT_X_BANK_NUMBER     1
#define NUM_ROUTES_TO_READ_FROM_FILE    100
#define LEN_ACCT_BUF                     32

#define NUM_ROUTES_TO_READ_FROM_FILE    100
#define ROUTE_NOT_FOUND                1011

typedef struct _BANK_ROUTING_DATA
   {   long lBankNumber;
       int  iMaxChars;
       int  iMinChars;
   }BANK_ROUTING_DATA;

//static int isValidTableBankNumber(char* pszBankNumber, char *pszBankAccount);

// MAB060999 end
#endif

static bool isAllNumeric(char* String,int Len);
static bool ValidStateCode(char* StateCode);
bool isCanadian(char* StateCode);

#define FN(default) FLDNUM Fn=(fn==-1) ? FN_##default : (FLDNUM) fn
//#define FN_(default) FLDNUM Fn_=(fn==-1) ? FN_##default : (FLDNUM) fn

#if 0
void logg1(int iLine, char *buf1, char *buf2, int i, long l)
{
   FILE  *fout;
   fout = fopen("err.log", "a");
   fprintf(fout,"@%d %s - %s. .%ld. .%d.\n", iLine, buf1, buf2, i, l);
   fclose(fout);
}
#endif
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

#define REQUIREPAIR(fn1,fn2,missingRc)\
 if ( GetFld(fn1) && ! GetFld(fn2) )\
  {\
   isMissing=true;\
   Fn=fn2;\
   Rcode=missingRc;\
   return false;\
  }


bool CheckAuth::fieldValidation(int Type,FLDNUM& Fn,bool& isMissing,
                                BYTE& Rcode)
{
 unsigned int Amount;
 WORD  TestAmount;
 BYTE  CheckType = ' ';

 // MAB082900
 // This variable is declared in CheckAuth.h so that this can go from
 // CheckAuth.cpp to FieldValidation.cpp to do a serious override.
 // Nothing else can't be overridden.
 //iReturnErrorFromRoutingAccountLength = 0;
 //logg1(__LINE__, "FV In FieldValidation","",0,0);

 if ( NOTOVERRIDE(FN_MERCHID) )
  {
   if ( ! GetFld(FN_MERCHID) )
    {
     Fn=FN_MERCHID;
     isMissing=true;
     Rcode=RCODE_MISSINGDATA;
     return false;
    }

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

 // State Code
 PERSONALCHECKVALIDATE(FN_LICENSESTATE,LICENSE,isValidState,
       RCODE_MISSINGSTATE,RCODE_INVALIDSTATE);

 // JHE071798 only require pair when LICENSE is required field
 // JHE120898 do not require pair if LICENSEORMICR and MICR is present
 //if ( dataReq.isRequired(Type,dataReq.LICENSE,TestAmount) ||
 //     dataReq.isRequired(Type,dataReq.LICENSEORMICR,TestAmount) )
 //  REQUIREPAIR(FN_LICENSESTATE,FN_LICENSE,RCODE_MISSINGLICENSE);
 if ( dataReq.isRequired(Type,dataReq.LICENSE,TestAmount) ||
      ( dataReq.isRequired(Type,dataReq.LICENSEORMICR,TestAmount) &&
        (! GetFld(FN_BANKNUMBER) || ! GetFld(FN_BANKACCOUNT)) ) )
   REQUIREPAIR(FN_LICENSESTATE,FN_LICENSE,RCODE_MISSINGLICENSE);

 // Drivers License
 PERSONALCHECKVALIDATE(FN_LICENSE,LICENSE,isValidLicense,
       RCODE_MISSINGLICENSE,RCODE_INVALIDLICENSE);

 // JHE 071798 ditto above
 // JHE 120898 ditto above
 //if ( dataReq.isRequired(Type,dataReq.LICENSE,TestAmount) ||
 //     dataReq.isRequired(Type,dataReq.LICENSEORMICR,TestAmount) )
 //  REQUIREPAIR(FN_LICENSE,FN_LICENSESTATE,RCODE_MISSINGSTATE);
 if ( dataReq.isRequired(Type,dataReq.LICENSE,TestAmount) ||
      ( dataReq.isRequired(Type,dataReq.LICENSEORMICR,TestAmount) &&
        (! GetFld(FN_BANKNUMBER) || ! GetFld(FN_BANKACCOUNT)) ) )
    REQUIREPAIR(FN_LICENSE,FN_LICENSESTATE,RCODE_MISSINGSTATE);

// PERSONALCHECKVALIDATE(FN_DOB,DOB,isValidDob,
//      RCODE_MISSINGDOB,RCODE_INVALIDDOB);
// JHE - Changed to not require DOB if MICR unless the flag is set on
 if ( NOTOVERRIDE(FN_DOB) )
      if ( GetFldInfo(FN_DOB) )
        {
         if ( ! isValidDob() )
          {
           isMissing=false;
           Fn=FN_DOB;
           Rcode=RCODE_INVALIDDOB;
           return false;
          }
        }
       else
         if ( dataReq.isRequired(Type,dataReq.DOB,TestAmount) &&
              CheckType != 'B' )
           if (  dataReq.isRequired(Type,dataReq.DOBIFMICRONLY) ||
                 GetFldInfo(FN_LICENSESTATE) )
            {
             isMissing=true;
             Fn=FN_DOB;
             Rcode=RCODE_MISSINGDOB;
             return false;
            }


 VALIDATE(FN_CHECKNUMBER,CHECKNUMBER,isValidCheckNumber,
       RCODE_MISSINGCHECKNUMBER,RCODE_INVALIDCHECKNUMBER);

 // Bank Number
 VALIDATE(FN_BANKNUMBER,MICR,isValidABA,
       RCODE_MISSINGMICR,RCODE_INVALIDMICR);
 REQUIREPAIR(FN_BANKACCOUNT,FN_BANKNUMBER,RCODE_INVALIDMICR);

 // Bank Account
 VALIDATE(FN_BANKACCOUNT,MICR,isValidAccount,
       RCODE_MISSINGMICR,RCODE_INVALIDMICR);
 REQUIREPAIR(FN_BANKNUMBER,FN_BANKACCOUNT,RCODE_INVALIDMICR);

 // JHE 0698 Handle new LICENSEORMICR flag, requiring one or the
 // other of the two ID's to be present
 if ( dataReq.isRequired(Type,dataReq.LICENSEORMICR,TestAmount) )
  if ( ! ( (GetFld(FN_LICENSESTATE) && GetFld(FN_LICENSE)) ||
           (GetFld(FN_BANKNUMBER) && GetFld(FN_BANKACCOUNT)) ) )
    {
     if ( CheckType != 'B' )
      {
       if ( NOTOVERRIDE(FN_LICENSESTATE) && ! GetFld(FN_LICENSESTATE) )
        {
         Rcode=RCODE_MISSINGSTATE;
         Fn=FN_LICENSESTATE;
         isMissing=true;
         return false;
        }
       if ( NOTOVERRIDE(FN_LICENSE) && ! GetFld(FN_LICENSE) )
        {
         Rcode=RCODE_MISSINGLICENSE;
         Fn=FN_LICENSE;
         isMissing=true;
         return false;
        }
      }

     if ( NOTOVERRIDE(FN_BANKNUMBER) && ! GetFld(FN_BANKNUMBER) )
      {
       Rcode=RCODE_MISSINGMICR;
       Fn=FN_BANKNUMBER;
       isMissing=true;
       return false;
      }
     else
     if ( NOTOVERRIDE(FN_BANKACCOUNT) && ! GetFld(FN_BANKACCOUNT) )
      {
       Fn=FN_BANKACCOUNT;
       Rcode=RCODE_MISSINGMICR;
       isMissing=true;
       return false;
      }
    }

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
   //logg1(__LINE__, "ValidMerchID is",EditBuf,Fn,EditLen);
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

#if 0
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
#endif

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
   if ( ! GetFld(LicFn,EditBuf,EditLen) ) // get the license
      return true;                        // if no license, accept it

   // MAB 112897 - if license is all '0's, make blank
    iReturn = strspn(EditBuf, "0 ");   // '0' and ' '
    if(iReturn == strlen(EditBuf))  // only '0' or ' ' found
    {
       PutFld(LicFn,""); // set the license
       return true;
    }

   EditBuf[EditLen]=0;  // terminate license string

  //logg1(__LINE__, "In IsValidLicense()-state string is",State, 0, 0);
  //logg1(__LINE__, "In IsValidLicense()-EditBuf string is",EditBuf, 0, 0);

#ifdef DO_LICENSE_CHECK
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

//logg1(__LINE__, "In IsValidLicense()- dropping out true","", 0, 0);
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
 int ii, jj;
 char szbuf[3];

 // MAB092900
 memset(gszBankNumber, '\0', 16);

 EditLen=sizeof(EditBuf);
 FN(BANKNUMBER);

 if ( GetFld(Fn,EditBuf,EditLen) )
 {
   //logg1(__LINE__, "ValidABA()-BankNumber is",EditBuf,Fn,EditLen);
   if ( EditLen < 8  ||
        EditLen > 9  ||
        ! isAllNumeric(EditBuf,EditLen) )
    return false;
 }
 // MAB060999
 if(EditLen == 9) // U.S. Bank
 {
   // Do test on first 2 digits for range
   szbuf[0] = EditBuf[0];
   szbuf[1] = EditBuf[1];
   szbuf[2] = '\0';
   jj = atoi(szbuf);

   if( (jj < 1)  ||
       (jj > 32) ||
       ((jj > 12) && (jj < 21))
     )
    {

     //logg1(__LINE__, "ValidABA()-invalid BankNumber prefix",szbuf,0,jj);
     return false;
    }

   // Do Check Sum for 9 digit (US) Bank Routing Numbers
   ii =
      (((int)EditBuf[0] - 48) * 3) +
      (((int)EditBuf[1] - 48) * 7) +
      (((int)EditBuf[2] - 48)    ) +
      (((int)EditBuf[3] - 48) * 3) +
      (((int)EditBuf[4] - 48) * 7) +
      (((int)EditBuf[5] - 48)    ) +
      (((int)EditBuf[6] - 48) * 3) +
      (((int)EditBuf[7] - 48) * 7);

   ii %= 10;
   jj = 10;
   jj -= ii;
   ii = jj;

   if(ii == 10)
      ii = 0;

   //jj = (int)EditBuf[8] - 48;

   //logg1(__LINE__, "ValidABA()-CheckSum is","",(long)ii,jj);
   if(ii != (int)EditBuf[8] - 48)
     return false;

 }

 // MAB092900
 // Hold onto Routing for more validation in BankAccount
 memcpy(gszBankNumber, EditBuf, EditLen);
 gszBankNumber[EditLen] = '\0';
 //logg1(__LINE__, "ValidABA()-Routing",gszBankNumber,0,EditLen);

 return true;
}

//-----------------------------------------------
// Validate Bank Account Number
//
// JHE 06/98 Account number cannot be all zeroes
//----------------------------------------------
bool CheckAuth::isValidAccount(int fn)
{
 int i, j, iReturn, iProblem; //, iOffset;
 char szbuf[4];

 // MAB092900
 char ch;
 char szBankAccount[32];
 memset(szBankAccount, '\0', 32);

 EditLen=sizeof(EditBuf);
 FN(BANKACCOUNT);

   // MAB092900
 //logg1(__LINE__, "ValidAccount()","",Fn,0);

 if( GetFld(Fn,EditBuf,EditLen) )
 {
     //logg1(__LINE__, "ValidAccount()-Account is",EditBuf,Fn,EditLen);

     // MAB092900
     // Hold onto Routing for more validation in BankAccount
     memcpy(szBankAccount, EditBuf, EditLen);
     szBankAccount[EditLen] = '\0';
     //logg1(__LINE__, "ValidABA()-Account",szBankAccount,0,EditLen);
     //logg1(__LINE__, "ValidABA()-Routing",gszBankNumber,0,EditLen);

     // MAB061599
     if(EditLen < MIN_ACCOUNT_LEN)
        return false;

     iReturn = isValidTableBankNumber(gszBankNumber, szBankAccount);
     if(iReturn == LENGTH_ERROR)
     {
        //logg1(__LINE__, "FV InValid Bank Num Acct","From Table",0,iReturn);
        return false;
     }

     // Do Bank Number - Bank Account Match
     if(!(strcmp(szBankAccount, gszBankNumber)))  // if they match
     {
        //logg1(__LINE__, "FV Invalid - Bank Num Acct Match","",0,0);
        return false;
     }
     //logg1(__LINE__, "FV digits","",(long)EditLen, 0);

     // Remove all 1 digit strings
     iProblem = 1;
     ch = EditBuf[0];
     for (i=1; i<EditLen - 2; i++)
      if ( EditBuf[i] != ch )
      {
       iProblem = 0;
       // logg1(__LINE__, "FV Not Single Digit","iProblem",(long)i,iProblem);
       break;
      }
     //logg1(__LINE__, "FV After Single Digit","iProblem",(long)i,iProblem);
     if(iProblem)
        return false;

     if(!(iProblem))  // Look for Sequential Digits. err is - ascending
     {
       iProblem = 1;
       iReturn = EditLen - 1;
       //if(iReturn > 9)
       //  iReturn = 9;
       //iReturn--;  // for the last digit

       //logg1(__LINE__, "FV digits","",(long)EditLen, iReturn );
       szbuf[1] = '\0';
       for (i=0; i<iReturn; i++)
       {
          j = (int)EditBuf[i + 1] - 1;

        if(EditBuf[i] == '9')
        {
          if(EditBuf[i + 1] != '0')  // next in sequence  9 to 0
          {
            iProblem = 0;
            //logg1(__LINE__, "FV No Sequential Digits ascending. err is",szbuf,(long)EditBuf[i],j);
            break;
          }
          else  // still ascending - you guess why
          {
            szbuf[0] = EditBuf[i];
            //logg1(__LINE__, "FV digits",szbuf,(long)EditBuf[i], j);
            //szbuf[0] = EditBuf[i + 1];
            //logg1(__LINE__, "FV digits","",(long)iReturn, i );
          }
        }

        else
        {
          szbuf[0] = EditBuf[i];
          //logg1(__LINE__, "FV digits",szbuf,(long)EditBuf[i], j);
          //szbuf[0] = EditBuf[i + 1];
          //logg1(__LINE__, "FV digits","",(long)iReturn, i );
          //if ( ( (int)EditBuf[i]) != ( ((int)(EditBuf[i + 1])) - 1) )
          if ( ( (int)EditBuf[i]) != j )
          {
           iProblem = 0;
           //logg1(__LINE__, "FV Not Sequential Digits asc. err is",szbuf,(long)EditBuf[i],i);
           break;
          }
        }
       }
     //logg1(__LINE__, "End Sequential Digits asc. err is",szbuf,(long)EditBuf[i],i);
     }
     //logg1(__LINE__, "FV Done testing Seq Digs asc","iProblem",(long)i,iProblem);
     if(iProblem)
        return false;

     if(!(iProblem))  // Look for Sequential Digits - decending
     {
       iProblem = 1;
       iReturn = EditLen;
       if(iReturn > 9)
         iReturn = 9;
       iReturn--;  // for the last digit

       //logg1(__LINE__, "FV digits","",(long)EditLen, iReturn );
       szbuf[1] = '\0';
       for (i=0; i<iReturn; i++)
       {
          j = (int)EditBuf[i + 1] + 1;

        if(EditBuf[i] == '0')
        {
          if(EditBuf[i + 1] != '9')
          {
            iProblem = 0;
           //logg1(__LINE__, "FV 9Not Sequential Digits desc. err is",szbuf,(long)EditBuf[i],j);
            break;
          }
/*          else
          {
            szbuf[0] = EditBuf[i];
            logg1(__LINE__, "FV digits",szbuf,(long)EditBuf[i], j);
            szbuf[0] = EditBuf[i + 1];
            logg1(__LINE__, "FV digits","",(long)iReturn, i );
          } */
        }
        else
        {
          //szbuf[0] = EditBuf[i];
          //logg1(__LINE__, "FV digits",szbuf,(long)EditBuf[i], j);
          //szbuf[0] = EditBuf[i + 1];
          //logg1(__LINE__, "FV digits","",(long)iReturn, i );
          //if ( ( (int)EditBuf[i]) != ( ((int)(EditBuf[i + 1])) - 1) )
          if ( ( (int)EditBuf[i]) != j )
          {
           iProblem = 0;
           //logg1(__LINE__, "FV Not Sequential Digits desc. err is",szbuf,(long)EditBuf[i],i);
           break;
          }
        }
       }
     //logg1(__LINE__, "End Sequential Digits desc. err is",szbuf,(long)EditBuf[i],i);
     }
     //logg1(__LINE__, "FV End testing","iProblem",(long)i,iProblem);
     if(iProblem)
        return false;

     return true;
 }
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
  "PA" "PE" "PQ" "PR"
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

//$$**********************************************************************
int CheckAuth::isValidTableBankNumber(char* pszBankNumber,char *pszBankAccount)
{
    char  szbuf[LEN_ACCT_BUF];
    FILE  *fin;
    int   ii;
    int   iReturn;
    int   iCount;
    long  lBankNumber;
    static int siRecordCount;
    static int siDataHasBeenReadFromFile;
    static BANK_ROUTING_DATA dat[NUM_ROUTES_TO_READ_FROM_FILE];

    //logg1(__LINE__, "ChAuth Start Validation","",0,0);

    // get the data from file to memory once
    if(!( siDataHasBeenReadFromFile))  // file has not been read yet
    {
       //logg1(__LINE__, "ChAuth Start Validation","Read File",0,0);

      iReturn = 0;
      if((fin = fopen("g:\\programs\\routing.dat", "r")) == NULL)  // couldn't open file

      {
         iReturn = FILE_OPEN_ERROR;
         //logg1(__LINE__, "ChAuth File open fail on G Drive","",0,0);
      }
      if(iReturn)
      {
         iReturn = 0;
         //logg1(__LINE__, "ChAuth File open 2","",0,0);
         if((fin = fopen("d:\\programs\\routing.dat", "r")) == NULL)  // couldn't open file
         {
            iReturn = FILE_OPEN_ERROR;
            //logg1(__LINE__, "ChAuth File open fail on D drive","",0,0);
         }
      }
      if(iReturn)
      {
         iReturn = 0;
         //logg1(__LINE__, "ChAuth File open 3","",0,0);
         if((fin = fopen("routing.dat", "r")) == NULL)  // couldn't open file
         {
            iReturn = FILE_OPEN_ERROR;
            //logg1(__LINE__, "ChAuth File open fail in . directory","",0,0);
         }
      }

      if(iReturn)
         return iReturn;

      for(ii = 0; ii < NUM_ROUTES_TO_READ_FROM_FILE; ii++)
      {
         memset(szbuf,'\0',sizeof(szbuf));
         fgets(szbuf, LEN_ACCT_BUF, fin);
         //logg1(__LINE__, "ChAuth Buf is",szbuf,0,0);

         if(feof(fin))
            break;

         sscanf(szbuf,"%ld %d %d ",
           &dat[ii].lBankNumber, &dat[ii].iMinChars, &dat[ii].iMaxChars);

         //logg1(__LINE__, "ChAuth Test dat","",dat[ii].lBankNumber,ii);

         siRecordCount++;
       }

       fclose(fin);
       siDataHasBeenReadFromFile = 1;
    }

    lBankNumber = atol(pszBankNumber);

    // MAB111599
    iReturn = 0;

    //logg1(__LINE__, "ChAuth Validation","Past Read File",lBankNumber,siRecordCount);
    // find data for Bank Route
    for(iCount = 0; iCount < siRecordCount; iCount++)
    {
       //logg1(__LINE__, "ChAuth Test dat","",dat[iCount].lBankNumber,iCount);
       if(dat[iCount].lBankNumber == lBankNumber)
       {
          //logg1(__LINE__, "ChAuth Test dat","",(long)dat[iCount].iMaxChars,dat[iCount].iMinChars);
          // check length
          if ( ((int)strlen(pszBankAccount) <= dat[iCount].iMaxChars) &&
               ((int)strlen(pszBankAccount) >= dat[iCount].iMinChars) )
             return 0;
          else
             iReturn = LENGTH_ERROR; // this is the only one that matters
       }
    }
    //logg1(__LINE__, "ChAuth Test dat","",0, iReturn);
    return iReturn;
}

#ifdef DO_LICENSE_CHECK
// MAB112697
//--------------------------------------------------------------------
// Generic License Validation
// It has to match ALL Conditions for a true, but is false if it
// fails on ANY conditions
//--------------------------------------------------------------------
#define BIT_LENGTH          2
#define BIT_ALPHA_COUNT     4
#define BIT_LEADING_ALPHA   8
#define BIT_MORE           16
static int isValidGenericLicense(char* License, char *szState)
{
  int iBits = 0;  // Needs to be true for all conditions for true result
  int iReturn = -1;
  int iIsValid;
  int ii;
  int iCount;
  int iAlphaCounter;
  int iFoundState = 0;   // 0 = not found yet
  char szStateBuf[4];    // For making lState
  char szNumBuf[LEN_NUM_BUF]; // For parsing data read from file
  char szbuf[LEN_NUM_BUF];
  long lState;           // integer representation of state
  FILE *fin;

  static int siActualStateCount;
  static int iStateDataHasBeenReadFromFile;
  static STATE_LICENSE_DATA dat[NUM_STATES];

  lState = (long)((szState[0] - 64) * 100);
  lState += (long)(szState[1] - 64);
  //logg1(__LINE__, "In IsValidLicense()-state is",szState, lState, 0);

  // get the data from file to memory once
  if(!( iStateDataHasBeenReadFromFile))  // file has not been read yet
  {
    if((fin = fopen("g:\\programs\\statelic.dat", "r")) == NULL)
    {
      // If it didn't open, then try for a local copy
      if((fin = fopen("statelic.dat", "r")) == NULL)  // couldn't open file
        return FILE_OPEN_ERROR;
    }

    siActualStateCount = NUM_STATES; // Most likely will change
    for(iCount = 0; iCount < NUM_STATES; iCount++)
    {
      memset(szbuf,'\0',sizeof(szbuf));
      memset(szStateBuf,'\0',sizeof(szStateBuf));
      memset(szNumBuf,'\0',sizeof(szNumBuf));

      fgets(szbuf, LEN_NUM_BUF, fin);

      // The list is no longer completely limited in length so as to allow
      // multiple entries for a single state.
      if(strstr(szbuf, "END LIST"))
      {
          siActualStateCount = iCount;
          //logg1(__LINE__, "In IsValidLicense()-state and StateCount",szbuf, lState, siActualStateCount);
          break;
      }

      strncpy(szStateBuf,szbuf,3);
      strcpy(szNumBuf,&szbuf[2]);

      sscanf(szNumBuf,"%ld %d %d %d %d %d %d %d %d %d", //szStateBuf,
        &dat[iCount].lState,
        &dat[iCount].iMaxChars, &dat[iCount].iMinChars,
        &dat[iCount].iMaxAlphas, &dat[iCount].iMinAlphas,
        &dat[iCount].iHasLeadAlpha, &dat[iCount].iIsSSNType,
        &dat[iCount].iMoreFlag, &dat[iCount].iIsDisabled);
    }
    fclose(fin);
    iStateDataHasBeenReadFromFile = 1;
    //logg1(__LINE__, "In IsValidLicense()-states were read","", lState, siActualStateCount);
  }

  // find data for state name
  for(iCount = 0; iCount < siActualStateCount; iCount++)
  {
    // If a line for that state is found, validate the DL
    if(dat[iCount].lState == lState)
    {
      iFoundState = 1; // means it was found
      //logg1(__LINE__, "In IsValidLicense()-found state",License, lState, iCount);

      // Change
      if(dat[iCount].iIsDisabled)  // has been turned off
      {
         //logg1(__LINE__, "In IsValidLicense()-Test is disabled",License, lState, iCount);
         return true; // !! Turn off one and all are turned off for that state
      }

      // Do SSN checks
      if(dat[iCount].iIsSSNType)  // if it could be a SSN - 1 or 2
      {
         iReturn = iIsCorrectNumberOfDigits(License, 9);   // is it 9 digits?
         if(iReturn == true)    // is it 9 digits?
         {
           //logg1(__LINE__, "In IsValidLicense()-is SSN",License, lState, iCount);
           return true;       // definate true
         }
      }
      if((dat[iCount].iIsSSNType == 2) && // if it must be a SSN
         (iReturn == false)
        )
      {
        //logg1(__LINE__, "In IsValidLicense()-SSN type 2 false",License, lState, iCount);
        iIsValid = false;  // false that overrides MoreFlag
        continue; // It's invalid for this line, so try the next one
      }

      // check max/min length - All entries have a length
      if ( ((int)strlen(License) > dat[iCount].iMaxChars) ||
           ((int)strlen(License) < dat[iCount].iMinChars) )
      {
        //logg1(__LINE__, "In IsValidLicense()-Length false",License, (long)dat[iCount].iMaxChars,dat[iCount].iMinChars);
        iIsValid = false;  // false that overrides MoreFlag
        continue; // It's invalid for this line, so try the next one
      }
      else
      {
        //logg1(__LINE__, "In IsValidLicense()-Length true",License, (long)dat[iCount].iMaxChars,dat[iCount].iMinChars);
        iBits |= BIT_LENGTH; // Must be true for this and the rest
        iIsValid = true;  // Still true
      }

      // If it's set, check for leading alpha
      // 1 means must be leading Alpha
      if(dat[iCount].iHasLeadAlpha)
      {
        if(isalpha(License[0]))
        {
          //logg1(__LINE__, "In IsValidLicense()-leading Alpha true",License, (long)dat[iCount].iMaxChars,dat[iCount].iMinChars);
          iBits |= BIT_LEADING_ALPHA; // Must be true for this and the rest
          iIsValid = true;  // Still true
        }
        else
        {
          if(dat[iCount].iHasLeadAlpha == 1) // Must be leading alpha
          {
            //logg1(__LINE__, "In IsValidLicense()-leading Alpha false",License, lState, iCount);
            iIsValid = false;  // false that overrides MoreFlag
            continue; // It's invalid for this line, so try the next one
          }
        }
      }
      else  // Not an alpha
      {
          //logg1(__LINE__, "In IsValidLicense()-leading Alpha false",License, lState, iCount);
          iBits |= BIT_LEADING_ALPHA; // Must be true for this and the rest
      }

/*
      else  // Not an alpha
      {
        if(isalpha(License[0]))
        {
          //logg1(__LINE__, "In IsValidLicense()-leading Alpha false",License, lState, iCount);
          iBits |= BIT_LEADING_ALPHA; // Must be true for this and the rest
        }
        else // if(dat[iCount].iHasLeadAlpha)
        {
          //logg1(__LINE__, "In IsValidLicense()-leading Alpha true",License, lState, iCount);
          iIsValid = false;  // false that overrides MoreFlag
          continue; // It's invalid for this line, so try the next one
        }
      }  // end lead alpha
*/

      // Count alphas in DL
      iAlphaCounter = 0;
      for(ii = 0; ii < (int)strlen(License); ii++)
        if( (isalpha(License[ii])) || (License[ii] == '*') )
          iAlphaCounter++;

      // if Alpha Count is set at all
      if(dat[iCount].iMaxAlphas + dat[iCount].iMinAlphas) // either are set
      {
        // check for max/min of alphas
        if( (iAlphaCounter > dat[iCount].iMaxAlphas)  ||
            (iAlphaCounter < dat[iCount].iMinAlphas) )
        {
          //logg1(__LINE__, "In IsValidLicense()-AlphaCount false",License,(long)dat[iCount].iMaxAlphas,dat[iCount].iMinAlphas);
          iIsValid = false;  // false that overrides MoreFlag
          continue; // It's invalid for this line, so try the next one
        }
        else
        {
          //logg1(__LINE__, "In IsValidLicense()-AlphaCount true",License, (long)dat[iCount].iMaxChars,dat[iCount].iMinChars);
          iBits |= BIT_ALPHA_COUNT; // Must be true for this and the rest
          iIsValid = true; // Still true
        }
      }
      else  // Should be no alphas
      {
        if(iAlphaCounter == 0)
        {
          //logg1(__LINE__, "In IsValidLicense()-AlphaCount false",License,(long)dat[iCount].iMaxAlphas,dat[iCount].iMinAlphas);
          iBits |= BIT_ALPHA_COUNT; // Must be true for this and the rest
        }
        else
        {
          //logg1(__LINE__, "In IsValidLicense()-AlphaCount false",License,(long)dat[iCount].iMaxAlphas,dat[iCount].iMinAlphas);
          iIsValid = false;  // false that overrides MoreFlag
          continue; // It's invalid for this line, so try the next one
        }
      } // end alpha counter

      // try the rest of the functions. Any falses would have already returned
      if(dat[iCount].iMoreFlag== 1)
      {
        iReturn = isValidStatesLicense(License,lState);
        if(iReturn == true)
        {
          //logg1(__LINE__, "In IsValidLicense()-More true",License, lState, iCount);
          return true;
        }
        else
        {
          iIsValid = false;
          //logg1(__LINE__, "In IsValidLicense()-More false",License, lState, iCount);
          continue;
        }
      }

      // Tests are done. If it's true for this whole line
      if(iBits == (BIT_LENGTH | BIT_ALPHA_COUNT | BIT_LEADING_ALPHA))
      {
        //logg1(__LINE__, "In IsValidLicense()-past state ",License,(int)iBits,iIsValid);
        return true;
      }
    }     // end if(state matched) ... one validation line
    else  // Past the lines for that state
    if(iFoundState == 1) // means it was found. If all true, return true
    {
      //logg1(__LINE__, "In IsValidLicense()-past state ",License,(int)iBits,iIsValid);
      // Cuz this line has been tested for that state.
      // Also means state lines must be sequential.
      return iIsValid; // Should always be false
    }
  }   // end for loop

  //logg1(__LINE__, "In IsValidLicense()-past loop ..should never ",License,0,iIsValid);
  return iIsValid;
}
                               
#endif

// This is the LICENSE.DAT table. Figure that this copy is obsolete by now.
#if 0
CA    301         8       5       1      1     1     0     0        0
AL    112         7       7       0      0     0     0     0        0
AK    111         7       5       0      0     0     0     0        0
AZ    126         9       9       0      0     0     2     0        0
AZ    126         9       9       1      1     1     0     0        0
AR    118         9       9       0      0     0     1     0        0
CO    315         9       9       0      0     0     2     0        0
CO    315         7       5       1      1     0     0     0        0
CT    320         9       9       0      0     0     0     1        0
DE    405         7       5       0      0     0     0     0        0
DC    403         9       9       0      0     0     2     0        0
DC    403         7       7       0      0     0     0     0        0
FL    612        13      12       1      1     1     0     0        0
GA    701         9       7       0      0     0     1     0        0
HI    809         9       9       1      0     0     1     0        0
ID    904         9       9       3      3     0     0     1        0
IL    912        12      12       1      1     0     0     0        0
IN    914        10      10       0      0     0     0     0        0
IA    901         9       9       2      0     0     1     1        0
KS   1119         9       9       1      0     0     1     1        0
KY   1125         9       9       1      0     0     0     0        0
LA   1201         9       9       0      0     0     0     1        0
ME   1305         8       7       1      0     0     0     1        0
MD   1304        13      13       1      1     1     0     0        0
MA   1301         9       9       1      0     0     1     1        0
MI   1309        13      13       1      1     1     0     0        0
MN   1314        13      13       1      1     1     0     0        0
MS   1319         9       9       0      0     0     2     0        0
MO   1315         9       9       0      0     0     2     0        0
MO   1315        10      10       1      0     1     0     0        0
MT   1320        13       9       0      0     0     1     0        0
NE   1405         9       7       1      1     1     0     1        0
NV   1422        10      10       0      0     0     0     0        0
NV   1422        12      12       0      0     0     0     0        0
NV   1422         9       9       1      1     1     0     1        0
NH   1408        10      10       3      3     0     0     1        0
NJ   1410        15      15       1      1     1     0     0        0
NM   1413         9       9       0      0     0     2     0        0
NY   1425         9       9       0      0     0     2     0        0
NC   1403        12       7       0      0     0     0     0        0
ND   1404         9       9       3      3     0     1     0        0
OH   1508         8       8       2      2     0     0     1        0
OK   1511         9       9       0      0     0     2     0        0
OR   1518         7       5       0      0     0     0     0        0
PA   1601         8       8       0      0     0     0     0        0
RI   1809         7       7       0      0     0     0     0        0
SC   1903         9       9       0      0     0     2     0        0
SD   1904         9       8       0      0     0     1     0        0
TN   2014         9       8       0      0     0     1     0        0
TX   2024         8       8       0      0     0     0     0        0
UT   2120         9       4       0      0     0     0     0        0
VA   2201         9       9       0      0     0     2     0        0
VA   2201         9       9       1      1     1     0     1        0
VT   2220         8       8       1      0     0     0     0        0
WA   2301        12      12       9      5     1     0     0        0
WV   2322         7       7       1      0     0     0     0        0
WI   2309        14      14       1      1     1     0     0        0
WY   2325        10       9       0      0     0     0     0        0
AB    102         9       6       0      0     0     1     0        0
BC    203         7       7       0      0     0     0     0        0
MB   1302        12      12       9      7     0     0     0        0
NB   1402         7       4       7      0     0     0     0        0
NF   1406        10      10       1      1     1     0     0        0
NS   1419        14      12       5      5     0     0     0        0
ON   1514        15      15       1      1     1     0     0        0
PE   1605         6       1       0      0     0     0     0        0
PQ   1617        13      13       1      1     1     0     0        0
SK   1911         8       8       0      0     0     0     0        0
GU    721         9       7       0      0     0     1     0        0
CZ    326         9       6       0      0     0     1     0        0
PR   1618        11       7       0      0     0     0     0        0
END LIST  <- that has to be there to stop data read

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

   Be aware!!! Changes MAB042402...
1. List is limited to 100 entries. There can be multiple entries for
   the same state

2. State lines must be sequential, cuz it only looks at groups. If they
   are not grouped, the later rows will not be looked at.

3. Do not set disabled if there is more than one entry for state.
   Remove all disabled entries if there is another enabled entry or
   the diabled entry will return a true in all cases, by overriding
   earlier falses.

4. This file is looked for only in g:\\programs\\statelic.dat and the
   local directory so there has to be a local copy in the directory of
   POSAUTH and any workstations that don't map to the G: drive... This
   could be changed to read the registry like CHECKAUTH does.



/****** Notes from 08/20/01 update - MAB
Old -
CZ    326        10      10       1      1     1     1     0        0
AZ - more disabled .. New format 04/24/02
NV - more disabled
IL - max is 12 instead of 13
MI - min is 13 instead of 11

04/22/02
HI    809         9       9       0      0     0     2     0        0
All these changes were confirmed or fixed on the 02/25/02 update
*******/
#endif


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

/* Obsolete
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
      if(strlen(License) == 6)
      {
         if( (isalpha(License[0])) && (iNumberOfDigits == 5) )  // A######
            return true;
      }
      return false;
   }

   // MAB011700

   else*/
   if (lState == 301 )  // California
   {
       if( (License[0] == 'M')    &&
           (strlen(License) == 7) &&
           (License[1] != '0')
         )
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
         return true;  // it has already been checked for 9# minimum
      return false;
   }

   else if (lState == 1118 )  // Kansas
   {
      if(License[0] == 'K')   //K########
         return true;  // it has already been checked for 8# minimum
      return false;
   }

/* else if (lState == 1125 )  // Kentucky
   {
      if(iNumberOfDigits == 10)  // was already checked for 10 max
         return true;
      if( (isalpha(License[0])) && (iNumberOfDigits == 8) )  //A########
         return true;  // it has already been checked for 8# minimum
      return false;
   }*/ // obsolete

   else if (lState == 1201 )  // Louisiana
   {
      if(memcmp(License,"00",2))
         return false;
      return true;
   }

   else if (lState == 1305 )  // Maine ####### or #######X
   {
      if( (strlen(License) == 8)  &&
          (License[7] == 'X') )   //#######X - provisional or under 21
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

/* else if (lState == 1315 )
   {  // Missouri ######### for CDL A########
      if(isalpha(License[0]))   //S########
         return true;  // it has already been checked for 1# minimum
      return false;
   }*/ // obsolete

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
   {  //  ############ or X######## or ##########
      /*if(iIsCorrectNumberOfDigits(License, 12))  // last 2 digits will be year OB ###
          return true;
      if(iIsCorrectNumberOfDigits(License, 10))  // 1998 license format
          return true;*/
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
          (License[0] == 'T') ||
          (License[0] == 'A') )
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





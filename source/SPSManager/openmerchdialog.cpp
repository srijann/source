//----------------------------------------------------------------------------
//  Project spsmanager
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    spsmanager.apx Application
//  FILE:         openmerchdialog.cpp
//  AUTHOR:
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of OpenMerchDialog (TDialog).
//
//----------------------------------------------------------------------------
#include "win4me.h"
#include <ctype.h>

#include "openmerchdialog.h"

//{{OpenMerchDialog Implementation}}


OpenMerchDialog::OpenMerchDialog(TWindow* parent, char* Buf, TResId resId, TModule* module)
:
 TDialog(parent, resId, module),
 BufPtr(Buf)
{
 MerchId=new TEdit(this,IDC_OPENMERCHID,16);
}


OpenMerchDialog::~OpenMerchDialog()
{
  Destroy(IDCANCEL);
}



bool OpenMerchDialog::CanClose()
{
 register i;

 MerchId->Transfer(BufPtr,tdGetData);

 if ( strlen(BufPtr) == 3 ||
      strlen(BufPtr) == 5 )
  {
   for (i=0; BufPtr[i] != 0; ++i)
    if ( ! isalnum(BufPtr[i]) )
     {
      if ( strlen(BufPtr)==3 )
       MessageBox("ACCOUNT NUMBERS MUST BE ALL 0-9, A-Z");
      else
       MessageBox("STATION NUMBERS MUST BE ALL 0-9, A-Z");
      MerchId->SetFocus();
      return false;
     }
   return true;
  }

 if ( strlen(BufPtr)==0 )
  {
   MessageBox("INPUT REQUIRED");
   MerchId->SetFocus();
   return false;
  }

 if ( strlen(BufPtr) < 6 )
  {
   MessageBox("LENGTH MUST BE 3, 5, OR >5");
   MerchId->SetFocus();
   return false;
  }

 return true;
}


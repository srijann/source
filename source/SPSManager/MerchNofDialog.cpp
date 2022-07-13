//----------------------------------------------------------------------------
//  Project spsmanager
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    spsmanager.apx Application
//  FILE:         merchnofdialog.cpp
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of MerchNofDialog (TDialog).
//
//----------------------------------------------------------------------------
#include "win4me.h"
#include <stdio.h>
#include "merchnofdialog.h"

//{{MerchNofDialog Implementation}}


MerchNofDialog::MerchNofDialog(TWindow* parent, char* Key,TResId resId, TModule* module)
:
    TDialog(parent, resId, module),
 key(Key)   
{
 Line1=new TStatic(this,IDC_LINE1);
 Line2=new TStatic(this,IDC_LINE2);
}


MerchNofDialog::~MerchNofDialog()
{
  Destroy(IDCANCEL);

  // INSERT>> Your destructor code here.

}


void MerchNofDialog::SetupWindow()
{
 char Buf[40];

 TDialog::SetupWindow();
 if ( strlen(key) == 3 )
  sprintf(Buf,"ACCOUNT %s DOES NOT EXIST",key);
 else
  sprintf(Buf,"STATION %s DOES NOT EXIST",key);
 Line1->Transfer(Buf,tdSetData);
}


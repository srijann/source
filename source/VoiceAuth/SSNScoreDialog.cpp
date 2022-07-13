//----------------------------------------------------------------------------
//  Project VoiceAuth
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    VoiceAuth.apx Application
//  FILE:         ssnscoredialog.cpp
//  AUTHOR:
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of SSNScoreDialog (TDialog).
//
//  Get SSN Score from operator.
//----------------------------------------------------------------------------
#include "Win4me.h"

#include "ssnscoredialog.h"


//{{SSNScoreDialog Implementation}}


SSNScoreDialog::SSNScoreDialog(TWindow* parent, char* ssn,int& Score)
:
 TDialog(parent, IDD_SSNSCORE),
 score(Score),
 theSSN(ssn)
{
 score = 0;
 new TEditVar(this,score,IDC_SSNSCORE,4,EVOPT_ZEROISNULL);
 SSN=new TEdit(this,IDC_THESSN);
}


SSNScoreDialog::~SSNScoreDialog()
{
  Destroy(IDCANCEL);
}


void SSNScoreDialog::SetupWindow()
{
 char Buf[9+3];
  TDialog::SetupWindow();

 strcpy(Buf,"999-99-9999");
 memcpy(Buf,theSSN,3);
 memcpy(&Buf[4],&theSSN[3],2);
 memcpy(&Buf[7],&theSSN[5],4);
 SSN->Transfer(Buf,tdSetData);
}


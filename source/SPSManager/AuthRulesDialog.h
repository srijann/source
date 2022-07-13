//----------------------------------------------------------------------------
//  Project spsmanager
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    spsmanager.apx Application
//  FILE:         AuthRulesdialog.h
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for AuthRulesDialog (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(AuthRulesdialog_h)              // Sentry, use file only if it's not already included.
#define AuthRulesdialog_h

#include <owl/dialog.h>
#include <owl/tabctrl.h>
#include <owl/commctrl.h>

#include "AuthRulesDialog.rh"
#include "RuleParamsDialog.h"
#include "RuleParams2Dialog.h"
#include "ProductAdjustDialog.h"
#include "DateTimeDialog.h"
#include "FormDialog.h"

//{{TDialog = AuthRulesDialog}}
class AuthRulesDialog : public FormDialog {
  AuthRules* globalRules;
  AuthRules* workingRules;
  AuthRules* classRules;
  AuthRules* subClassRules;
  AuthRules* ruleSets;
  AuthRules* origRules;
  FormDialog* CurrentWin;
  RuleParamsDialog* RulesWin;
  RuleParams2Dialog* Rules2Win;
  ProductAdjustDialog* ProductWin;
  DateTimeDialog* DateTimeWin;
  TTabControl* Tab;
  bool IsSubClass;
  public:
    AuthRulesDialog(TWindow* parent,AuthRules* GlobalRules,
                    AuthRules* classRules, AuthRules* subClassRules,
                    bool IsNew);
    virtual ~AuthRulesDialog();
    virtual bool IsModified();
    virtual bool Save();
    virtual bool SaveTemp(TFile* File);
    virtual void ResetModified();
    virtual bool Validate();

//{{AuthRulesDialogVIRTUAL_BEGIN}}
  public:
    virtual void SetupWindow();
//{{AuthRulesDialogVIRTUAL_END}}

//{{AuthRulesDialogRSP_TBL_BEGIN}}
  protected:
    void TCNSelchange(TNotify& tcn);
    bool TCNSelchanging(TNotify&);
//{{AuthRulesDialogRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(AuthRulesDialog);
};    //{{AuthRulesDialog}}


#endif  // AuthRulesdialog_h sentry.


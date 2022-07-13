#ifndef __RULESDIALOGMANAGER_H
#define __RULESDIALOGMANAGER_H
#include "AuthRules.h"

class RulesDialogManager {
  AuthRules* workingRules;
  AuthRules* Rules;
  AuthRules* parentRules;
  AuthRules* globalRules;
  public:
  RulesDialogManager(AuthRules* TheRules,
                     AuthRules* GlobalRules=0,
                     AuthRules* ParentRules=0);
  ~RulesDialogManager();
 };

#endif
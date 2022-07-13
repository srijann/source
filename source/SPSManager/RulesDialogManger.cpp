#include "Win4me.h"
#include "RulesDialogManager.h"
#include "BasicAuthDialog.rh"




RulesDialogManager::RulesDialogManager(AuthRules* TheRules,
               AuthRules* GlobalRules,AuthRules* ParentRules) :
 Rules(TheRules),
 globalRules(GlobalRules),
 parentRules(ParentRules)
{
 workingRules=new AuthRules(Rules);
}

RulesDialogManager::~RulesDialogManager()
{
}

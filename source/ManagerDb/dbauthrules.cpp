#include "ServerApp.h"
#include "ListDefs.h"
#include "OpenInfo.h"
#include "AuthRules.h"

static AuthRules globalAuthRules;
static AuthRules classAuthRules;
static AuthRules subclassAuthRules;
static AuthRules mergedAuthRules;
static OpenTreeInfo TreeInfo;
static StringTreeList* RulesClassList;
struct AuthRulesKey
  {
   char ClassName[RECNAME_LEN];
   char SubClassName[RECNAME_LEN];
  };

//-------------------------------
// Get Authorization Rules Record
//-------------------------------
void ServerApp::GetAuthRules()
{
 int Len=sizeof(TreeInfo);
 struct AuthRulesKey Key;

 // Get the TreeInfo structure from the record and use it as the key.
 // It must be present.
 if ( ! Msg.GetFld(FN_DBKEY,&TreeInfo,Len) )
  {
   DBReply(DB_ERROR);
   BADDATA("KEY DATA");
   return;
  }

 // Put in the Global Rules
 Msg.PutFld(FN_GLOBALAUTHRULES,&globalAuthRules.Data,
                sizeof(globalAuthRules.Data));

 // If Class is Global return only the DEFAULT record
 if ( strcmp(TreeInfo.ClassName,"DEFAULT") == 0 )
  {
   DBReply(DB_OKAY);
   return;
  }

 // Handle request for existing record
 if ( ! TreeInfo.IsNew )
  {
   // Get Class Record
   memset(&Key,0,sizeof(Key));
   strcpy(Key.ClassName,TreeInfo.ClassName);
   if ( ! GetRecord(FN_CLASSAUTHRULES,&Key,classAuthRules) )
     return;

   // Get Subclass Record, if any
   if ( TreeInfo.SubClassName[0] )
    {
     strcpy(Key.SubClassName,TreeInfo.SubClassName);
     if ( ! GetRecord(FN_SUBCLASSAUTHRULES,&Key,subclassAuthRules) )
       return;
    }

   // return the records
   DBReply(DB_OKAY);
   return;
  }

 // Creating New Class?
 if ( ! TreeInfo.SubClassName[0] )
  {
   // if there's a template, read it
   if ( TreeInfo.TemplateClassName[0] )
    {
     if ( ! GetRecord(0,TreeInfo.TemplateClassName,classAuthRules) )
      return;
     // Erase name from template
     memset(classAuthRules.Data.AuthRulesKey.ClassName,0,
              sizeof(classAuthRules.Data.AuthRulesKey.ClassName));
     memset(classAuthRules.Data.AuthRulesKey.SubClassName,0,
              sizeof(classAuthRules.Data.AuthRulesKey.SubClassName));
     // Insert the Class name
     strcpy(classAuthRules.Data.AuthRulesKey.ClassName,TreeInfo.ClassName);
     // We set the Timestamp to a non-zero value to trigger timestamp
     // checking when the record is saved (we want to know if someone
     // else added it before this client did).
     classAuthRules.Data.AuthRulesTStamp=(DBTIMESTAMP)-1;
     // If we're using a subclass template to create a class
     // record, we have to get rid of the subclass levels. This
     // will change them all to class level.
     classAuthRules.ValidateLevels(" GC",'C');
    }
   else
     classAuthRules.FormatNew(TreeInfo.ClassName,NULL);

   // Return the Global and the Class rule Set
   Msg.PutFld(FN_CLASSAUTHRULES,&classAuthRules.Data,
                sizeof(classAuthRules.Data));
   DBReply(DB_OKAY);
   return;
  }

 // Creating new subclass....

 // Get Class Record for new subclass
 memset(&Key,0,sizeof(Key));
 strcpy(Key.ClassName,TreeInfo.ClassName);
 if ( ! GetRecord(FN_CLASSAUTHRULES,&Key,classAuthRules) )
   return;

 // If Template is present, read it
 if ( TreeInfo.TemplateClassName[0] )
  {
   if ( ! GetRecord(0,TreeInfo.TemplateClassName,subclassAuthRules) )
    return;
   // Erase name from template
   memset(subclassAuthRules.Data.AuthRulesKey.ClassName,0,sizeof(subclassAuthRules.Data.AuthRulesKey.ClassName));
   memset(subclassAuthRules.Data.AuthRulesKey.SubClassName,0,sizeof(subclassAuthRules.Data.AuthRulesKey.SubClassName));
   // Insert Class and SubClass name into new record
   strcpy(subclassAuthRules.Data.AuthRulesKey.ClassName,TreeInfo.ClassName);
   strcpy(subclassAuthRules.Data.AuthRulesKey.SubClassName,TreeInfo.SubClassName);
   // See above
   subclassAuthRules.Data.AuthRulesTStamp=(DBTIMESTAMP)-1;
   // If using a Class Template to create a SubClass, change all of
   // the CLASS levels to SubClass Level
   if ( TreeInfo.TemplateSubClassName[0] == 0 )
     subclassAuthRules.ValidateLevels(" GS",'S');
  }
 else
   // No template, create empty record
   subclassAuthRules.FormatNew(TreeInfo.ClassName,TreeInfo.SubClassName);

 // Return the Global, Class and SubClass records
 Msg.PutFld(FN_SUBCLASSAUTHRULES,&subclassAuthRules.Data,sizeof(subclassAuthRules.Data));
 DBReply(DB_OKAY);
}

//-------------------------------
// Put Authorization Rules Record
//-------------------------------
void ServerApp::PutAuthRules()
{
 bool IsNew;

 if ( ! PutRecord(FN_AUTHRULES,mergedAuthRules,IsNew) )
  return;

 // If it's a change to the DEFAULT record, save it in memory
 if ( strcmp(mergedAuthRules.Data.AuthRulesKey.ClassName,"DEFAULT") == 0 )
  globalAuthRules.TransferDataIn(&mergedAuthRules.Data);
 else
 // If new record, insert keys into tree
 if ( IsNew )
    RulesClassList->AddNode(mergedAuthRules.Data.AuthRulesKey.ClassName,
                            mergedAuthRules.Data.AuthRulesKey.SubClassName);
}

//-----------------------------------------
// Initialize the Authorization Rules stuff
//-----------------------------------------
bool ServerApp::InitAuthRules()
{
 char  NameBuf[RECNAME_LEN+1];
 int   Status;
 struct AuthRulesKey Key;

 // Init the Rules Class List
 RulesClassList = new StringTreeList(8192,1024,AUTHRULES_LIST);
 RulesClassList->Add("GDEFAULT");
 while( (Status=d_keynext(AUTHRULESKEY,DBHandle)) == S_OKAY )
  {
   if ( (Status=d_keyread(AUTHRULESKEY,&Key,DBHandle)) != S_OKAY )
    {
     DBError(__FILE__,__LINE__,Status,"d_reyread",-1,AUTHRULESKEY);
     return false;
    }
   // Build either a parent or child key and insert into the list.
   // Skip the Global record. We already inserted it at the top of
   // the list.
   if ( strcmp(Key.ClassName,"DEFAULT") )
    {
     if ( Key.SubClassName[0] )
      {
       NameBuf[0] = 'C';
       strcpy(&NameBuf[1],Key.SubClassName);
      }
     else
      {
       NameBuf[0] = 'P';
       strcpy(&NameBuf[1],Key.ClassName);
      }
     RulesClassList->Add(NameBuf);
    }
  }

 // Get the Global RULES record. We store it in memory because we will
 // be accessing it frequently.
 memset(&Key,0,sizeof(Key));
 strcpy(Key.ClassName,"DEFAULT");
 if ( d_keyfind(AUTHRULESKEY,&Key,DBHandle) != S_OKAY ||
      d_recread(AUTHRULESRECORD,&globalAuthRules.Data,DBHandle) != S_OKAY )
  globalAuthRules.FormatNew("DEFAULT",NULL);

 return true;
}

void ServerApp::GetAuthRulesList()
{
 RulesClassList->DbAnswer(&Msg);
}

//--------------------------------------------------------------------
// Fetch the Auth rules for a check authorization transaction, merging
// and adjusting as required.
//--------------------------------------------------------------------
void ServerApp::FetchAuthRules(MerchantRec& merchRec)
{
 struct AuthRulesKey Key;
 char* Class;
 char* SubClass;
 uint  ProductGroup;
 uint  ProductSubGroup;

 // Can't fetch any rules without a merchant record
 if ( ! checkMsg.hasMerchantRec )
  return;

 // Decide whether to use Guarantee or Verify Classes
 if ( checkMsg.isGuarantee() )
  {
   Class=merchRec.Data.Classes.GUAuthRules;
   SubClass=merchRec.Data.Classes.GUAuthRulesSub;
  }
 else
  {
   Class=merchRec.Data.Classes.VFYAuthRules;
   SubClass=merchRec.Data.Classes.VFYAuthRulesSub;
  }

 checkMsg.ProductInfo(ProductGroup,ProductSubGroup);

 // If it's the global class, just store it
 if ( strcmp(Class,"DEFAULT") == 0 )
  {
   mergedAuthRules.Merge(&globalAuthRules,0,0,
                       merchRec.Data.MerchantTimeZone,
                       ProductGroup,ProductSubGroup,&checkMsg);
   checkMsg.PutFld(FN_AUTHRULES,&mergedAuthRules.Data,
                                     sizeof(mergedAuthRules.Data));
   return;
  }

 // Get the Class record
 memset(&Key,0,sizeof(Key));
 strcpy(Key.ClassName,Class);
 if ( ! GetRecord(0,&Key,classAuthRules) )
  {
   LogMissingClass(checkMsg.MerchantID(),"AUTHRULES",Class);
   return;
  }

 // If no SubClass, merge Class and global and store
 if ( ! SubClass[0] )
  {
   mergedAuthRules.Merge(&globalAuthRules,&classAuthRules,0,
             merchRec.Data.MerchantTimeZone,
             ProductGroup,ProductSubGroup,&checkMsg);
   checkMsg.PutFld(FN_AUTHRULES,&mergedAuthRules.Data,
                 sizeof(mergedAuthRules.Data));
   return;
  }

 // Get the SubClass record
 strcpy(Key.SubClassName,SubClass);
 if ( ! GetRecord(0,&Key,subclassAuthRules) )
  {
   LogMissingClass(checkMsg.MerchantID(),"AUTHRULES",Class,SubClass);
   return;
  }

 // Merge global, class, subclass and store
 mergedAuthRules.Merge(&globalAuthRules,&classAuthRules,&subclassAuthRules,
                       merchRec.Data.MerchantTimeZone,
                       ProductGroup,ProductSubGroup,&checkMsg);

 checkMsg.PutFld(FN_AUTHRULES,&mergedAuthRules.Data,
               sizeof(mergedAuthRules.Data));
}


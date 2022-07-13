#ifndef __OPENINFO_H
#define __OPENINFO_H

#define MAXCLASSNAME 10

class OpenTreeInfo
 {
  public:
  bool IsNew;
  char ClassName[MAXCLASSNAME+1];
  char SubClassName[MAXCLASSNAME+1];
  char TemplateClassName[MAXCLASSNAME+1];
  char TemplateSubClassName[MAXCLASSNAME+1];
 };

class OpenClassInfo
 {
  public:
  bool IsNew;
  char ClassName[MAXCLASSNAME+1];
  char TemplateClassName[MAXCLASSNAME+1];
 };

#endif
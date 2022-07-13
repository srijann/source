#ifndef __StringTreeList_H
#define __StringTreeList_H
#include "StringList.h"

class StringTreeList : public StringList {
  public:
  // This Constructor fetches a list from the database, using the
  // TranType, Dest, and AltDest provided
  StringTreeList(char* ListName,int SlackBytes=0);
  // This Constructor initializes the List from the callers'
  // buffer.
  StringTreeList(char* TheList,int ListLen,int SlackBytes,bool Delete=true);
  // Construct an empty list with InitialBuffer Size set as
  // indicated
  StringTreeList(int InitialSize,int SlackBytes,char* ListName);
  // Construct from data in File
  StringTreeList(TFile* File,int Len,int SlackBytes=0);
  // Construct from the contents of an IPCMessage
  StringTreeList(IPCMessage* Msg,int SlackBytes=0);

  bool AddNode(char* ParentName,char* ChildName=0);
  // Find a Node by relative number. Return Parent and Child names
  bool FindNode(int Num,char *ParentName,char *ChildName);
  // Find node by name, return relative number (-1 if not found)
  int  FindNode(char* ParentName,char* ChildName=0);
 };
#endif


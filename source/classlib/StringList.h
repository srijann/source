#ifndef __STRINGLIST_H
#define __STRINGLIST_H

#include <classlib/file.h>
#include "Dbdefs.h"
#include "ipcmsg.h"
#include <owl/combobox.h>

typedef struct
 {
  DBTIMESTAMP TimeStamp;
  char ListName[MAX_LISTNAME+1];
 } LISTHDR;

class StringList {
  // Was the List initialized okay?
  bool  InitFail;
  // Delete the List when destructed?
  bool  ShouldDelete;
  // Number of Bytes of Slack to keep at the end
  int   NumSlackBytes;
  bool  DbFetch(char* ListName,IPCMessage* MsgBuf);
  int   IteratorIndex;
  char  stringBuf[250];

  public:
  // This Constructor fetches a list from the database, using the
  // TranType, Dest, and AltDest provided
  StringList(char* ListName,int SlackBytes=0);
  // This Constructor initializes the List from the callers'
  // buffer.
  StringList(char* TheList,int ListLen,int SlackBytes,bool Delete=true);
  // Construct an empty list with InitialBuffer Size set as
  // indicated
  StringList(int InitialSize,int SlackBytes,char* ListName);
  // Construct from data in File
  StringList(TFile* File,int Len,int SlackBytes=0);
  // Construct from the contents of an IPCMessage
  StringList(IPCMessage* Msg,int SlackBytes=0);
  // Construct from another List
  StringList(StringList*);
  // Destructor
  ~StringList();
  // Add an Item to the List. Optionally keeping it in sorted order
  virtual void Add(char* Item,bool Sorted=false);
  // Empty the List. Clears all but reserved bytes
  void Clear();
  // Iterator -- Passes String, Index, and args to callers' function for
  // each member
  void ForEach(void(*)(char* String,int Index,void* args),void* args=0);
  // Get the Length of the List, including the Reserved Bytes
  int  GetLen() { return ListLen; };
  // Answer a List request (call from server)
  bool  DbAnswer(IPCMessage* Msg);
  // Save the List to a file
  virtual bool Save(TFile* File);
  // Save the list back to the Database
  virtual bool Save();
  // Find by Number
  virtual char* Find(int);
  // Find by String
  virtual int Find(char*);
  // Replace by Number
  virtual bool Replace(int,char*);
  // Get next String
  virtual char* Next(bool Reset=false);
  virtual char* Next(int& Val,bool Reset=false);
  bool GetInitFail() { return InitFail; };
  void InitComboBox(TComboBox*);
  // Compare List with this list
  bool isSame(StringList*);
  int  GetLength() { return ListLen; };

  // Protected member functions

  protected:
  // Make room for a new item at the designated insertion point (or
  // at the end if InsertPoint=0). Return where to insert.
  char* MakeFit(int Len,char *InsertPoint=0);
  // Pointer to the list
  char* TheList;
  // Length of List
  int   ListLen;
  // Length of list buffer
  int   ListBufLen;
 };
#endif

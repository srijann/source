#ifndef __IPCTRANSFER_H
#define __IPCTRANSFER_H
#include <owl/edit.h>
#include <owl/static.h>
#include "ComboBoxEx.h"
#include "ipc.h"
class IPCTransfer {
  IPCMessage* IPCMsg;
  char* Buf;
  int   BufLen;
  bool  Transfer(TEdit*);
  public:
  IPCTransfer(IPCMessage* Msg=0);
  ~IPCTransfer();
  void SetIPCMsg(IPCMessage*);
  bool ToCtl(FLDNUM,TEdit*);
  bool ToMsg(FLDNUM,TEdit*,FLDTYPE Type=FLDTYPE_STRING);
  bool ToCtl(FLDNUM,TComboBoxEx*);
  bool ToMsg(FLDNUM,TComboBoxEx*,FLDTYPE Type=FLDTYPE_BYTE);
  bool ToCtl(FLDNUM,TStatic*);
 };
#endif

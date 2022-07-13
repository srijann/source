#ifndef __IPCMSGTRANSFER_H
#define __IPCMSGTRANSFER_H
#include "ipc.h"
#include "EditEx.h"
class MsgIO {
  IPCMessage* Msg;
  public:
  MsgIO(IPCMsg* Msg=0);
  ~MsgIO();
  void SetIPCMsg(IPCMessage*);
  bool FldOut(FLDNUM,TEditEx*);
  bool FldIn(FLDNUM,TEditEx*);
 };
#endif

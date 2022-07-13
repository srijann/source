#ifndef __CHKAUTHCTLMAN_H
#define __CHKAUTHCTLMAN_H

typedef struct ctlitem {
  int    Id;      // The Control ID
  FLDNUM IPCFn;   // The IPC Field Number
  int    MsgNum;  // The Default Message Number for the Message Window
  void*  CtlWin;  // The Control Window pointer
 } CTLITEM;

class ChkauthControlManager : public  {
  TWindow* ParentWindow;
  IPCMessage* IPCMsg;
  public:
  ChkauthControlManager(TWindow* Parent,IPCMessage* Msg,int NumControls)
  ~ChkauthControlManager();
  void AddEdit(int Id,int Len,int Fn,int MsgNum);
  void AddList(int Id,int Fn,int MsgNum);
  void SetFocus(int Id,int MsgNum=-1);
  void EnableAll();
  void DisableAll();
  void ClearAll();
  void CopyFromMsg(IPCMessage* Msg=0);
  void CopyToMsg(IPCMessage* Msg=0;
 };
#endif

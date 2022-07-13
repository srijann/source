#ifndef __IPCSERVER_H
#define __IPCSERVER_H

class IPCWindow
{
 protected:
  HWND hWnd;
 public:
  virtual LRESULT WndProc( UINT iMessage, WPARAM wParam, LPARAM lParam ) = 0;
  HWND GetHandle(void) { return hWnd; };
};

class FUNCTYPE IPCServer : public IPCWindow
{
 private:
    HWND  hWnd;
    static void FAR PASCAL LineFunc( int X, int Y, LPSTR lpData );
    void Register(HINSTANCE,char*);
    DWORD  ProcessStartedTS;
    DWORD  MessageCounterTS;
    DWORD  ApplicationMsgTS;
    DWORD  AdminMsgTS;
    DWORD  MessageCounter;
    DWORD  StartDisplayMessageCounter;
    char   LastAdminMessage[31];
    void   Display(void);
    void   UpdateWindowText(HDC hdc);
    char   ServerProcessName[MAX_IPCNAME+1];
    char   ProgramVersionInfo[10];
    IPCMessage* Msg;
    void   (CALLBACK* ApplFunc)(void);
    int    (CALLBACK* AdminFunc)(void);
    bool ProcessAdminMessage(void);
    void ProcessDebugCommand(void);
    void Paint( void );
    LRESULT WndProc( UINT iMessage, WPARAM wParam, LPARAM lParam );
    UINT IPCNotify;
 public:
    IPCServer(HINSTANCE,char*,char*,IPCMessage*,
              void (CALLBACK*)(void),int (CALLBACK*)(void));
    ~IPCServer();
};
#endif




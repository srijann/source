//----------------------------------------------------------------------------
//  Project SpsManager
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    SpsManager Application
//  FILE:         spsmanagerapp.h
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for TSpsManagerApp (TApplication).
//
//----------------------------------------------------------------------------
#if !defined(spsmanagerapp_h)              // Sentry, use file only if it's not already included.
#define spsmanagerapp_h

#include <owl/opensave.h>


#include "spsmanagerapp.rh"            // Definition of all resources.
#include "FormDialog.h"
#define MBOX GetMainWindow()->MessageBox

//
// FrameWindow must be derived to override Paint for Preview and Print.
//
//{{TDecoratedFrame = TSDIDecFrame}}
class TSDIDecFrame : public TDecoratedFrame {
  public:
    TSDIDecFrame(TWindow* parent, const char far* title, TWindow* clientWnd, bool trackMenuSelection = false, TModule* module = 0);
    ~TSDIDecFrame();

//{{TSpsManagerAppVIRTUAL_BEGIN}}
  public:
    virtual void SetupWindow();
//{{TSpsManagerAppVIRTUAL_END}}
};    //{{TSDIDecFrame}}


//{{TApplication = TSpsManagerApp}}
class TSpsManagerApp : public TApplication {
  private:
    bool            HelpState;                          // Has the help engine been used.
    bool            ContextHelp;                        // SHIFT-F1 state(context sensitive HELP)
    TCursor*        HelpCursor;                         // Context sensitive help cursor.
    TFile* OpenSaveFile(char* FileName);
    TFile* OpenFile(char* FileName);
    TFile* ValidSaveFile(char* FileName,int& Status);
    void FileError(char* Caption);
    void DoCreateDialog(FormDialog* Win);
    void Waiting(bool); 
    HWND CaptureWin;
    
  public:
    TSpsManagerApp();
    virtual ~TSpsManagerApp();

//{{TSpsManagerAppVIRTUAL_BEGIN}}
  public:
    virtual void InitMainWindow();
    virtual bool CanClose();
    virtual bool ProcessAppMsg(MSG& msg);
//{{TSpsManagerAppVIRTUAL_END}}

//{{TSpsManagerAppRSP_TBL_BEGIN}}
  protected:
    void CmHelpContents();
    void CmHelpUsing();
    void CmHelpAbout();
    void CeFilesave(TCommandEnabler &tce);
    void CmOpenproductclasses();
    void CmFilesave();
    void CmFilesaveall();
    void CmFilesavetoWork();
    void CmCloseAllWindows();
    void CmMinimizeAllWindows();
    void CmSelectWindow(int Id);
    LRESULT ProcessAppNotify(WPARAM MsgType, LPARAM Id);
    void CeSaveall(TCommandEnabler &tce);
    void CeSavetoWork(TCommandEnabler &tce);
    void CeChooseWindow(TCommandEnabler &tce);
    void CmReloadchanges();
    void CmExit();
    void CmOpenauthrules();
    void CmOpendatareq();
    void CmOpenposfilerules();
    void CmOpenUnvFundsrules();
    void CeCloseallwindows(TCommandEnabler &tce);
    void CeMinimizeall(TCommandEnabler &tce);
    void CmOpenaccount();
    void CmOpenstation();
    void CmOpenmerchant();
    void CmOpenrespmsg();
//{{TSpsManagerAppRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(TSpsManagerApp);
};    //{{TSpsManagerApp}}


#endif  // spsmanagerapp_h sentry.

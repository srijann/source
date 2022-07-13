//----------------------------------------------------------------------------
//  Project PosFileMan
//  Secure Payment Systems
//  Copyright © 1997. All Rights Reserved.
//
//  SUBSYSTEM:    PosFileMan Application
//  FILE:         FileEditDialog.h
//  AUTHOR:       Jack Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for FileEditDialog (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(FileEditDialog_h)              // Sentry, use file only if it's not already included.
#define FileEditDialog_h

#include "destname.h"
#include "checkauthfn.h"
#include "TranType.h"
#include "dbdefs.h"

//{{TDialog = FileEditDialog}}
class FileEditDialog : public TDialog {
  void ChangeHandler(uint);

  public:
    FileEditDialog(TWindow* parent, TResId resId);
    virtual ~FileEditDialog();
    IPCMessage Msg;

//{{FileEditDialogVIRTUAL_BEGIN}}
  public:
    virtual TResult EvCommand(uint id, THandle hWndCtl, uint notifyCode);
//{{FileEditDialogVIRTUAL_END}}

//{{FileEditDialogRSP_TBL_BEGIN}}
  public:
    virtual void CeGetrecord(TCommandEnabler &tce);
    virtual void CeFiledelete(TCommandEnabler &tce);
    virtual void CeFilechange(TCommandEnabler &tce);
    virtual void CeFileadd(TCommandEnabler &tce);
    virtual void CmEditclearall();
    virtual void CeEditclearall(TCommandEnabler &tce);
    virtual void CmGetrecord();
    virtual void CmFileadd();
    virtual void CmFilechange();
    virtual void CmFiledelete();
//{{FileEditDialogRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(FileEditDialog);
};    //{{FileEditDialog}}


#endif  // FileEditDialog_h sentry.

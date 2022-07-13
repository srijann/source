//----------------------------------------------------------------------------
//  Project Classlib
//
//  Copyright © 1996. All Rights Reserved.
//
//  FILE:         IPCTransfer.cpp
//  AUTHOR:       Jack Ely
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of IPCTransfer (Base Class)
//
//  Provides a mechanism for transferring date to/from Window Controls
//  and IPCMessage Fields.
//----------------------------------------------------------------------------

#include "win4me.h"
#include "IPCTransfer.h"

//-----------------------------------------------------------
// Class Constructor
//
// Provide an IPC Message Pointer or you can set it later
// with SetIPCMsg.
//----------------------------------------------------------
IPCTransfer::IPCTransfer(IPCMessage* Msg) :
 IPCMsg(Msg),
 BufLen(50)
{
 Buf=new char[BufLen+1];
}

//-----------------
// Class Destructor
//-----------------
IPCTransfer::~IPCTransfer()
{
 delete[] Buf;
}

//------------------------------
// Change the IPCMessage Pointer
//------------------------------
void IPCTransfer::SetIPCMsg(IPCMessage* Msg)
{
 IPCMsg=Msg;
}

//----------------------------------------------
// Transfer from the Message to the Edit Control
//----------------------------------------------
bool IPCTransfer::ToCtl(FLDNUM Fn,TEdit* Ctrl)
{
 FLDTYPE FldType;
 int FldLen;

 Ctrl->Clear();

 if ( ! IPCMsg ||
      ! IPCMsg->GetFldInfo(Fn,FldType,FldLen) )
   return false;

 if ( (uint) FldLen > Ctrl->TextLimit )
  return false;

 if ( FldLen > BufLen )
  {
   delete[] Buf;
   BufLen=FldLen;
   Buf=new char[BufLen+1];
  }

 if (  IPCMsg->GetFld(Fn,BufLen,Buf) )
   Ctrl->Transfer(Buf,tdSetData);
 return true;
}

//----------------------------------------------
// Transfer from the Message to a Static Control
//----------------------------------------------
bool IPCTransfer::ToCtl(FLDNUM Fn,TStatic* Ctrl)
{
 FLDTYPE FldType;
 int FldLen;

 Ctrl->Clear();

 if ( ! IPCMsg ||
      ! IPCMsg->GetFldInfo(Fn,FldType,FldLen) )
   return false;

 if ( FldLen > BufLen )
  {
   delete[] Buf;
   BufLen=FldLen;
   Buf=new char[BufLen+1];
  }

 if (  IPCMsg->GetFld(Fn,BufLen,Buf) )
   Ctrl->Transfer(Buf,tdSetData);
   
 return true;
}


//------------------------------------------------------------------
// Private Function:
//
// Transfer the data from the Control to my own buffer, allocating a
// larger one if the one I have isn't big enough.
//-------------------------------------------------------------------
bool IPCTransfer::Transfer(TEdit* Ctrl)
{
// If the buffer's not big enough, delete it and get one that is
 if ( Ctrl->TextLimit > (uint) BufLen )
  {
   delete[] Buf;
   BufLen=Ctrl->TextLimit;
   Buf=new char[BufLen+1];
  }
 Ctrl->Transfer(Buf,tdGetData);
 return Buf[0] ? true : false;
}

//----------------------------------------------
// Transfer from the Edit Control to the Message
//----------------------------------------------
bool IPCTransfer::ToMsg(FLDNUM Fn,TEdit* Ctrl,FLDTYPE Type)
{
 if ( ! IPCMsg )
   return false;

 if ( ! Transfer(Ctrl) )
  IPCMsg->DelFld(Fn);

 IPCMsg->PutFldFail(false);

 switch( Type )
  {
   case FLDTYPE_BYTE:  IPCMsg->PutFld(Fn,(BYTE)Buf[0]); break;
   case FLDTYPE_WORD:  IPCMsg->PutFld(Fn,(WORD)atoi(Buf)); break;
   case FLDTYPE_DWORD: IPCMsg->PutFld(Fn,(DWORD)atoi(Buf)); break;
   default: IPCMsg->PutFld(Fn,Buf);
            break;
  }

 return IPCMsg->PutFldFail() ? false : true;
}

//-----------------------------------------
// Transfer From the message to a Combo Box
//-----------------------------------------
bool IPCTransfer::ToCtl(FLDNUM Fn,TComboBoxEx* ComboBox)
{
 int FldLen;
 FLDTYPE FldType;
 BYTE  CVal;
 DWORD DVal;
 bool  ret;

 if ( ! IPCMsg ||
      ! IPCMsg->GetFldInfo(Fn,FldType,FldLen) )
  return false;

 if ( FldType==FLDTYPE_BYTE )
  {
   IPCMsg->GetFld(Fn,CVal);
   ret=ComboBox->Select(CVal);
  }
 else
  {
   IPCMsg->GetFld(Fn,DVal);
   ret=ComboBox->Select((int)DVal);
  }

 if ( ! ret )
  {
   ComboBox->SetSelIndex(-1);
   return false;
  }

 return true;
}

//---------------------------------------
// Transfer from a Combo Box to a Message
//---------------------------------------
bool IPCTransfer::ToMsg(FLDNUM Fn,TComboBoxEx* ComboBox,FLDTYPE Type)
{
 int Value;
 char StrBuf[2];
 if ( ! ComboBox->GetSelectedValue(Value) )
  return false;

 switch(Type)
  {
   case FLDTYPE_BYTE:   IPCMsg->PutFld(Fn,(BYTE)Value); break;
   case FLDTYPE_WORD:   IPCMsg->PutFld(Fn,(WORD)Value); break;
   case FLDTYPE_DWORD:  IPCMsg->PutFld(Fn,(DWORD)Value); break;
   default:
   case FLDTYPE_STRING: StrBuf[0]=(BYTE)Value;
                        IPCMsg->PutFld(Fn,StrBuf);
                        break;
  }
  
 return true;
}




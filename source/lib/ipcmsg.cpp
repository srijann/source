//---------------------------------------------------------------------
// IPCMSG.CPP
// Author: Jack H Ely
// Copyright (c) 1996. JHE Software, Inc.
//
// This is a BASE class for handling Message Objects.
//
// The message object provides an abstract and somewhat
// type-independent interface to interprocess messages.
//
// When you instantiate the class, you create a private message buffer
// which allows no direct access. Access to fields in the buffer is
// provided only via the member functions Put, Get, and Del, copying
// fields to/from the message buffer.
//
// The message buffer is a fixed length buffer whose size is declared
// in the IPCMSG.H file. It should be big enough to hold the largest
// message your application can handle. ALL application programs should
// use the same buffer length. Attempting to Put a field to a buffer that
// won't fit will fail. To insure the integrity of the message you can
// check the return on each Put or, optionally, test for failure
// conditions after a series of Puts by invoking the PutFail function.
// Another option is to make your buffers big enough so you never have
// to worry about it.
//
// Fields are referenced using FIELD NUMBERS ranging from 0-255. Field
// numbers are provided by the caller and have no inherent meaning within
// the object other than to identify a field. It is the programmer's
// responsibility to coordinate the use of field numbers between
// application programs via header files, etc.
//
// FIELD TYPES are derived from the caller's parameters to the Get and
// Put functions. The FIELD TYPE is stored in the message along with the
// FIELD NUMBER. The FIELD TYPES supported are:
//
//     BINARY FIELDS
//        WORD - (unsigned short)
//        DWORD  - (unsigned long)
//        BYTE   - (unsigned char)
//     STRING FIELDS - Null terminated C-string (char* )
//     VLEN FIELDS - Variable length field (length is supplied by caller)
//
// TYPE INDEPENDENCE
// -----------------
//
// A degree of type-indepence is provided, especially
// for numeric fields, ie., you can Put a field as one type and Get it
// as another. You can also safely replace a field of one type with a
// field of another type as long as it's numeric value does not
// change. WARNING: watch for truncation problems when you use integers
// of varying lengths. If you want to achieve full type-independence for
// a particular numeric field, there are two ways to do it:
//
//    1. Always use LONG integers
//    2. Access numeric fields as VLEN fields and use the
//       length parameter to derive the type (ie., 1, 2, 4).
//
// TYPE CONVERSION occurs when you invoke the Get function with
// parameters which do not match the type of the field you are
// requesting.
//
// The following rules apply to type-conversions:
//
//   BINARIES -
//     to another BINARY by using C type-casting
//     to STRING by using ultoa (radix 10)
//     to VLEN by moving raw binary to caller's buffer and setting
//      the length appropriately (ie., 1, 2, or 4 ).
//
//   STRINGS -
//     to BINARY by using atol (no checking for valid numeric digits
//                              or overflow occurs)
//     to VLEN
//         if all NUMERIC -- convert to DWORD using ultoa, move to
//           caller's buffer, and set len=4
//         if not all Numeric -- move string to caller's buffer and
//           set length to length of string (without terminator).
//
//   VLENS -  The stored field will be considered a binary if it has
//            an appropriate binary length, (ie., 1, 2, or 4).
//     to BINARY
//       if VLEN is BINARY (1,2,4) -- C type-cast to requested type
//       if VLEN is NOT BINARY -- Set to zero and return field
//                                not present indicator.
//     to STRING
//       if VLEN is BINARY (1,2,4) -- ultoa to caller's buffer
//       if VLEN is NOT BINARY -- move to caller's buffer and append
//             a null-terminator.
//
// Note that when a field of one type is replaced in a message by a
// field of another type, no conversion takes place. The old field
// is simply deleted and replaced by the new one.
//
// PROGRAMMER NOTES -
//
//  Tacked onto the end of this source module is a stub you can use
//  to test it when making changes or porting it to a new environment.
//---------------------------------------------------------------------

#include "defs.h"

#ifdef WINDOWSNT
#define STRICT
#include <windows.h>
#include <mem.h>
#endif

#ifdef SCOUNIX
#include <memory.h>    // for memcpy
#endif

#include <string.h> // for strlen
#include <stdlib.h> // for ultoa
#include <ctype.h>  // for isdigit

#ifdef IPCLIB
#include "ipc.h"
#else
#include "ipcmsg.h"
#endif

// Define Value in OFFSET Table when Field is not present
#define NOT_PRESENT     (WORD) -1

// This is the format of embedded fields in the message.
#ifdef SCOUNIX
#pragma pack(1)
#endif

typedef struct
 {
  FLDNUM  fldnum;
  FLDTYPE fldtype;
  union
   {
    BYTE   byte;
    WORD ushort;
    DWORD  ulong;
    char   string[1];
    struct
     {
      BYTE len;
      char data[1];
     } vlen1;
    struct
     {
      WORD len;
      char data[1];
     } vlen2;
   } type;
 } FLDPTR;

//----------------------------------------------------------------------
//                    PUBLIC MEMBER FUNCTIONS
//----------------------------------------------------------------------

//----------------------------
// Constructor for new message
//----------------------------
IPCMessage::IPCMessage()
{
 Length=sizeof(IPCROUTEINFO);
 memset(MsgData,0,sizeof(IPCROUTEINFO));
 PutFailFlag=false;
 memset(OffsetTbl,0xff,sizeof(OffsetTbl));
}

//------------------------------------------------------------------------
// Constructor for an existing message
// If the okay flag does not come back true, then the message was trash
// and the buffer is empty.
//------------------------------------------------------------------------
IPCMessage::IPCMessage(void* msgbuf,WORD msglen,bool& okay)
{
 PutFailFlag=false;

 if ( msglen > MAX_MSG )
  {
   IPCMessage();
   okay=false;
   return;
  }

 // Copy the message into the message buffer and parse. If it doesn't
 // parse, init as new message and return not okay
 memcpy(MsgData,msgbuf,msglen);
 Length=msglen;
 if ( ! ParseMsg() )
  {
   IPCMessage();
   okay=false;
   return;
  }

 okay = true;
}

//-----------
// Destructor
//-----------
IPCMessage::~IPCMessage()
{
}

//----------------------------------------------
// Put a Null-Terminated String into the Message
//----------------------------------------------
bool IPCMessage::PutFld(FLDNUM fn,char* val)
{
 // Do not put zero-length field .. consider it a delete
 if ( val[0] == '\0' )
  {
   DelFld(fn);
   return true;
  }

 // put the field
 return putfld(fn,FLDTYPE_STRING,val,strlen(val));
}

//-------------------------------------
// Put a short integer into the message
//-------------------------------------
bool IPCMessage::PutFld(FLDNUM fn,WORD val)
{
 return putfld(fn,FLDTYPE_WORD,&val,sizeof(WORD));
}

//------------------------------------
// Put a long integer into the message
//------------------------------------
bool IPCMessage::PutFld(FLDNUM fn,DWORD val)
{
 return putfld(fn,FLDTYPE_DWORD,&val,sizeof(DWORD));
}

//---------------------------------------------
// Put a variable length field into the message
//---------------------------------------------
bool IPCMessage::PutFld(FLDNUM fn,void* val,int len)
{
 // Do not put zero length field .. consider it a delete
 if ( len==0 )
  {
   DelFld(fn);
   return true;
  }

 if ( len > 0xff )
  return putfld(fn,FLDTYPE_VLEN2,val,len);
 else
  return putfld(fn,FLDTYPE_VLEN1,val,len);
}

//----------------------------------------------------
// Put a byte (a 1 byte binary) field into the message
//----------------------------------------------------
bool IPCMessage::PutFld(FLDNUM fn,BYTE val)
{
 return putfld(fn,FLDTYPE_BYTE,&val,sizeof(BYTE));
}

//-------------------------------------------------
// Test the PutFail flag and, optionally, clear it
//-------------------------------------------------
bool IPCMessage::PutFldFail(bool clear)
{
 bool flag=PutFailFlag;

 if (clear)
   PutFailFlag=false;
   
 return flag;
}

//-----------------------------
// Query Presence of Field Only
//-----------------------------
bool IPCMessage::GetFld(FLDNUM fn)
{
 return getfld(fn) ? true : false;
}

//--------------------------------------------------------------
// Get Pointer to a Field and optionally return  type and Length
//--------------------------------------------------------------
void* IPCMessage::GetFldPtr(FLDNUM fn,int* Len,FLDTYPE* Type)
{
 FLDPTR *pt=(FLDPTR*)getfld(fn);

 if ( pt==NULL )
   return pt;

 if ( Type )
  *Type=pt->fldtype;

 switch ( pt->fldtype )
  {
   case FLDTYPE_BYTE:   if (Len)
                         *Len=sizeof(BYTE);
                        return &pt->type.byte;

   case FLDTYPE_WORD:   if (Len)
                         *Len=sizeof(WORD);
                        return &pt->type.ushort;

   case FLDTYPE_DWORD:  if (Len)
                         *Len=sizeof(DWORD);
                        return &pt->type.ulong;

   case FLDTYPE_STRING: if (Len)
                         *Len=strlen(pt->type.string)+1;
                        return &pt->type.string; 

   case FLDTYPE_VLEN1:  if (Len)
                         *Len=pt->type.vlen1.len;
                        return &pt->type.vlen1.data;

   case FLDTYPE_VLEN2:  if (Len)
                         *Len=pt->type.vlen2.len;
                        return &pt->type.vlen2.data; 
  }

 return NULL;
}

//--------------------------
// Get Field type and Length
//--------------------------
bool IPCMessage::GetFldInfo(FLDNUM fn,FLDTYPE& type,int &len)
{
 FLDPTR *pt=(FLDPTR*)getfld(fn);

 if ( pt==NULL )
   return false;

 switch ( type = pt->fldtype )
  {
   case FLDTYPE_BYTE:      len=sizeof(BYTE); break;
   case FLDTYPE_WORD:      len=sizeof(WORD); break;
   case FLDTYPE_DWORD:     len=sizeof(DWORD); break;
   case FLDTYPE_STRING:    len=strlen(pt->type.string)+1; break;
   case FLDTYPE_VLEN1:     len=pt->type.vlen1.len; break;
   case FLDTYPE_VLEN2:     len=pt->type.vlen2.len; break;
   default:             return false;
  }

 return true;
}

//--------------------------
// Get Field type and Length
//--------------------------
int IPCMessage::GetFldLen(FLDNUM fn)
{
 FLDTYPE type;
 int len;

 return GetFldInfo(fn,type,len) ? len : 0;
}

bool IPCMessage::GetFldInfo(FLDNUM fn,FLDTYPE& type)
{
 int len;
 return GetFldInfo(fn,type,len);
}
bool IPCMessage::GetFldInfo(FLDNUM fn)
{
 return OffsetTbl[fn] != NOT_PRESENT;
}


#ifdef SCOUNIX
#include <stdio.h>
void ultoa(unsigned long Value,char* String,int Radix)
{
 sprintf(String,"%u",Value);
}
#endif


//------------------------------------------------------------------
// Get a Null-Terminated String from the message, converting TYPE if
// necessary.
//------------------------------------------------------------------
bool IPCMessage::GetFld(FLDNUM fn,int maxlen,char* val)
{
 FLDPTR* pt=(FLDPTR *)getfld(fn);
 DWORD lbin;
 // This is max size of a DWORD converted to string
 char  numstr[11];

 if ( maxlen==0 )
   return false;

 // NOT PRESENT: SET TO ZERO LENGTH STRING
 if (pt==NULL )
  {
   val[0] = '\0';
   return false;
  }

 // CONVERSIONS
 switch( pt->fldtype )
  {
   // Binary to string: Assume string value requested
   case FLDTYPE_BYTE:     ultoa(pt->type.byte,numstr,10); break;
   case FLDTYPE_WORD:     ultoa(pt->type.ushort,numstr,10); break;
   case FLDTYPE_DWORD:    ultoa(pt->type.ulong,numstr,10); break;

   // No conversion
   case FLDTYPE_STRING:   if ( (int)strlen(pt->type.string) >= maxlen )
                         return false;
                       strcpy(val,pt->type.string);
                       return true;

   // VLEN1 - If len is valid for BINARY, assume it's a number,
   // otherwise copy it to caller's buffer as a string.
   case FLDTYPE_VLEN1:
     if ( vlen_is_binary(pt->type.vlen1.len,pt->type.vlen1.data,lbin) )
      {
       ultoa(lbin,numstr,10);
       break;
      }
     if ( pt->type.vlen1.len >= maxlen )
       return false;
     memcpy(val,pt->type.vlen1.data,pt->type.vlen1.len);
     val[pt->type.vlen1.len] = '\0';
     return true;

   // Copy it as a string to caller's buffer and null-terminate it.
   case FLDTYPE_VLEN2:
       if ( pt->type.vlen2.len >= maxlen )
         return false;
       memcpy(val,pt->type.vlen2.data,pt->type.vlen2.len);
       val[pt->type.vlen2.len] = '\0';
       return true;
  }

 // We broke out of the switch table only if the string was numeric 
 if ( (int) strlen(numstr) >= maxlen )
  return false;

 strcpy(val,numstr);
 return true;
}

//--------------------------------------------------------------------
// Get a short integer from the message, Converting TYPE if necessary.
//--------------------------------------------------------------------
bool IPCMessage::GetFld(FLDNUM fn,WORD& val)
{
 FLDPTR *pt=(FLDPTR *)getfld(fn);
 DWORD lbin;

 // NOT PRESENT .. SET TO ZERO
 if (pt==NULL )
  {
   val = 0;
   return false;
  }

 // CONVERSIONS
 switch( pt->fldtype )
  {
   // BINARIES: Simply Cast to TYPE
   case FLDTYPE_BYTE:     val = (WORD) pt->type.byte; break;
   case FLDTYPE_WORD:     val =  pt->type.ushort; break;
   case FLDTYPE_DWORD:    val = (WORD) pt->type.ulong; break;

   // STRING: Assume it's a number string and convert to binary
   // It's caller's problem if it wasn't a number string
   case FLDTYPE_STRING:   val = (WORD) atol(pt->type.string); break;

   // Variable length fields: If length is appropriate for binary,
   // return it, otherwise consider the field not present.
   case FLDTYPE_VLEN1:    if ( vlen_is_binary(pt->type.vlen1.len,
                                           pt->type.vlen1.data,
                                           lbin) )
                        {
                         val = (WORD) lbin;
                         break;
                        }

   case FLDTYPE_VLEN2:    val = 0;
                       return false;
  }


 return true;
}

//------------------------------------------------------------------
// Get a long integer from the message, Converting TYPE if necessary
//-------------------------------------------------------------------
bool IPCMessage::GetFld(FLDNUM fn,DWORD& val)
{
 FLDPTR* pt=(FLDPTR *)getfld(fn);
 DWORD lbin;

 // If not present, set to zero
 if (pt==NULL )
  {
   val = 0;
   return false;
  }

 // CONVERSIONS
 switch( pt->fldtype )
  {
   // BINARIES: Simply cast to TYPE
   case FLDTYPE_BYTE:     val = (DWORD) pt->type.byte; break;
   case FLDTYPE_WORD:     val = (DWORD) pt->type.ushort; break;
   case FLDTYPE_DWORD:    val =  pt->type.ulong; break;

   // String: Assume it's a number string and convert to binary
   // It's caller's problem if it wasn't a number string
   case FLDTYPE_STRING:   val = (DWORD) atol(pt->type.string); break;

   // Variable length fields: If length is appropriate for binary,
   // return it, otherwise consider the field not present.
   case FLDTYPE_VLEN1:    if ( vlen_is_binary(pt->type.vlen1.len,
                                           pt->type.vlen1.data,
                                           lbin) )
                        {
                         val = lbin;
                         break;
                        }

   case FLDTYPE_VLEN2:    val = 0;
                       return false;
  }


 return true;
}

//----------------------------------------------------------------
// Get a BYTE (1 byte binary) from the message, converting TYPE if
// necessary.
//----------------------------------------------------------------
bool IPCMessage::GetFld(FLDNUM fn,BYTE& val)
{
 FLDPTR* pt=(FLDPTR *)getfld(fn);
 DWORD lbin;

 if (pt==NULL )
  {
   val = 0;
   return false;
  }

 // CONVERSIONS
 switch( pt->fldtype )
  {
   // No Conversion
   case FLDTYPE_BYTE:     val = pt->type.byte;
                       break;

   // Other BINARY TYPES: Assume caller knows what they are doing and
   // these are small integers that can be returned as 8-bit binaries.
   case FLDTYPE_WORD:     val = (BYTE) pt->type.ushort;
                       break;
   case FLDTYPE_DWORD:    val = (BYTE) pt->type.ulong;
                       break;
   // STRING: Assume it's a number string and convert to binary
   // It's caller's problem if it wasn't a number string
   case FLDTYPE_STRING:   val = (BYTE) atol(pt->type.string);
                       break;
   // Variable length fields: If length is appropriate for binary,
   // return it, otherwise consider the field not present.
   case FLDTYPE_VLEN1:    if ( vlen_is_binary(pt->type.vlen1.len,
                                           pt->type.vlen1.data,
                                           lbin) )
                        {
                         val = (BYTE) lbin;
                         break;
                        }

   case FLDTYPE_VLEN2:    val = 0;
                       return false;
 }

 return true;
}

//-----------------------------------------------------------------
// Get a variable length field from the message, converting TYPE if
// necessary. Length should be set to max length of caller's buffer.
//-----------------------------------------------------------------
bool IPCMessage::GetFld(FLDNUM fn,void* val,int& len)
{
 FLDPTR* pt=(FLDPTR *)getfld(fn);
 int buflen=len;

 // Not present, set length to zero
 if (pt==NULL || buflen==0 )
  {
   len = 0;
   return false;
  }

 // CONVERSIONS
 switch( pt->fldtype )
  {
   // BINARIES: return raw binary value and set length appropriately.
   case FLDTYPE_BYTE:     ((char* )val)[0] = pt->type.byte;
                       len = sizeof(BYTE);
                       break;
   case FLDTYPE_WORD:     if ( buflen < sizeof(WORD) )
                        return false;
                       memcpy(val,&pt->type.ushort,len=sizeof(WORD));
                       break;
   case FLDTYPE_DWORD:    if ( buflen < sizeof(DWORD) )
                        return false;
                       memcpy(val,&pt->type.ulong,len=sizeof(DWORD));
                       break;

   // STRING: move string to caller's buffer
   case FLDTYPE_STRING: if ( buflen <
                              (len=strlen(pt->type.string)) )
                           return false;
                         memcpy(val,pt->type.string,len);
                        break;

   // VLEN:  No conversion
   case FLDTYPE_VLEN1:    if ( buflen < (len=pt->type.vlen1.len) )
                         return false;
                       memcpy(val,pt->type.vlen1.data,len);
                       break;
   case FLDTYPE_VLEN2:    if ( buflen < (len=pt->type.vlen2.len) )
                         return false;
                       memcpy(val,pt->type.vlen2.data,len);
                       break;
  }

 return true;
}

//-----------------------------------------------------------
// Get the Maximum size of field that will fit in the message
//-----------------------------------------------------------
int IPCMessage::GetMaxField()
{
 return MAX_MSG - Length - 4;
}

//--------------------------------
// Delete a field from the Message
//--------------------------------
void IPCMessage::DelFld(FLDNUM fn)
{
 FLDPTR* ptr=(FLDPTR *)getfld(fn);
 int    len;
 register offset;
 register i;

 if ( ! ptr )
   return;

 len = sizeof(ptr->fldnum) + sizeof(ptr->fldtype);

 switch( ptr->fldtype )
  {
   case FLDTYPE_BYTE:      len += sizeof(BYTE); break;
   case FLDTYPE_WORD:      len += sizeof(WORD); break;
   case FLDTYPE_DWORD:     len += sizeof(DWORD); break;
   case FLDTYPE_STRING:    len += strlen(ptr->type.string) + 1; break;
   case FLDTYPE_VLEN1:     len += sizeof(BYTE) + ptr->type.vlen1.len; break;
   case FLDTYPE_VLEN2:     len += sizeof(WORD) + ptr->type.vlen2.len; break;
  }

 memcpy(ptr,&((char* )ptr)[len],Length - len - OffsetTbl[fn]);
 Length -= (WORD) len;
 offset = OffsetTbl[fn];
 OffsetTbl[fn] = NOT_PRESENT;

 // Recalculate offsets for all fields beyond this one
 for (i=0; i<MAX_FIELDS; ++i)
  if ( OffsetTbl[i] != NOT_PRESENT && 
       OffsetTbl[i] > offset )
   OffsetTbl[i] -= (WORD) len;
}

//----------------------------------------------------
// COPY Message Data from Message Object to Raw Buffer
//----------------------------------------------------
void IPCMessage::CopyMsgOut(void* buf,WORD& len)
{
 memcpy(buf,MsgData,len=Length);
}
void IPCMessage::CopyMsgOut(void* buf)
{
 memcpy(buf,MsgData,Length);
}

//--------------------------------------------------------
// Copy Raw Buffer into a Message Object and Initialize it
//--------------------------------------------------------
bool IPCMessage::CopyMsgIn(void* buf,WORD len)
{
 if ( len > MAX_MSG )
   return false;

 Length=len;
 memcpy(MsgData,buf,Length);
 PutFailFlag=false;
 return( ParseMsg() );
}

//------------------------------
// Copy one message from another
//------------------------------
void IPCMessage::CopyMsgIn(IPCMessage* Msg)
{
 memcpy(MsgData,Msg->MsgData,Msg->Length);
 memcpy(OffsetTbl,Msg->OffsetTbl,sizeof(OffsetTbl));
 Length=Msg->Length;
 PutFailFlag=false;
}

//----------------------------------
// Copy a field from another message
//----------------------------------
bool IPCMessage::CopyFld(FLDNUM Fn,IPCMessage* Msg)
{
 FLDPTR* pt=(FLDPTR*)Msg->getfld(Fn);

 if ( pt==NULL )
  return false;

 switch( pt->fldtype )
  {
   case FLDTYPE_BYTE:    return PutFld(Fn,pt->type.byte);
   case FLDTYPE_WORD:    return PutFld(Fn,pt->type.ushort);
   case FLDTYPE_DWORD:   return PutFld(Fn,pt->type.ulong);
   case FLDTYPE_STRING:  return PutFld(Fn,pt->type.string);
   case FLDTYPE_VLEN1:   return PutFld(Fn,pt->type.vlen1.data,
                                        pt->type.vlen1.len);
   case FLDTYPE_VLEN2:   return PutFld(Fn,pt->type.vlen2.data,
                                        pt->type.vlen2.len);
  }

 return false;
}

bool IPCMessage::CopyFld(FLDNUM toFld,FLDNUM fromFld,IPCMessage* fromMsg)
//--------------------------------------------------------------------
// Copy a field from another field and optionally from another message
//--------------------------------------------------------------------
{
 FLDPTR *pt;

 if ( fromMsg && fromMsg != this )
   pt=(FLDPTR*)fromMsg->getfld(fromFld);
 else
  {
   // If copying from the same message then delete the To field first
   // so that it will be appended to the end and our pointer will
   // not be invalidated while putting.
   DelFld(toFld);
   pt=(FLDPTR*)getfld(fromFld);
  }

 if ( pt==NULL )
  return false;

 switch( pt->fldtype )
  {
   case FLDTYPE_BYTE:    return PutFld(toFld,pt->type.byte);
   case FLDTYPE_WORD:    return PutFld(toFld,pt->type.ushort);
   case FLDTYPE_DWORD:   return PutFld(toFld,pt->type.ulong);
   case FLDTYPE_STRING:  return PutFld(toFld,pt->type.string);
   case FLDTYPE_VLEN1:   return PutFld(toFld,pt->type.vlen1.data,
                                        pt->type.vlen1.len);
   case FLDTYPE_VLEN2:   return PutFld(toFld,pt->type.vlen2.data,
                                        pt->type.vlen2.len);
  }

 return false;
}


//-------------------------------------------------------
// Clear a Message (Reset Length to Zero). Does not clear
// the message header
//-------------------------------------------------------
void IPCMessage::ClearMsg()
{
 Length=sizeof(IPCROUTEINFO);
 PutFailFlag=false;
 memset(OffsetTbl,0xff,sizeof(OffsetTbl));
 memset(&MsgData[Length],0,sizeof(MsgData)-Length);
}

//------------------------------------------------------------------
// These Functions extend the functionality of the IPCMessage class
// to include an interface to the low-level direct IPC functions for
// Sending and Receiving Messages.
//-------------------------------------------------------------------
#ifdef IPCLIB
//---------------------------------------------------------
// Send the Message to a Named Process, expecting no answer
//---------------------------------------------------------
bool IPCMessage::SendMsg(char* ProcessName)
{
 return IPCSend(this,ProcessName);
}

//------------------------------------------------------------
// Send a Message to a Named Process, expecting an answer back
//------------------------------------------------------------
bool IPCMessage::SendMsg(char* ProcessName,MSGQ ReplyWin,WORD TimeOut)
{
 return IPCSend(this,ProcessName,ReplyWin,TimeOut);
}

//----------------------------------------------------
// Send a Message Synchronously, waiting for an answer
//----------------------------------------------------
bool IPCMessage::SendRecv(char* ProcessName,WORD TimeOut,
                         bool UIRetry,int Retries,char* AltProcessName)
{
 return IPCSend(this,ProcessName,TimeOut,UIRetry,Retries,AltProcessName);
}


//-------------------
// Reply to a Message
//-------------------
bool IPCMessage::ReplyMsg()
{
 return IPCReply(this);
}

//------------------
// Receive a Message
//------------------
bool IPCMessage::RecvMsg(LPARAM BufId)
{
 return IPCRecv(this,BufId);
}

//------------------------
// Receive a Message Reply
//------------------------
bool IPCMessage::RecvReply(LPARAM BufId)
{
 return IPCRecvReply(this,BufId);
}

//---------------------------------
// Get SYNCID from a Message Buffer
//---------------------------------
bool IPCMessage::GetMsgSyncId(WORD& SyncId)
{
 return IPCGetSyncId(this,SyncId);
}

//---------------------------------------
// Reset SYNCID of Message (set to zero).
//---------------------------------------
void IPCMessage::ResetMsgSyncId()
{
 IPCResetSyncId(this);
}

//----------------------------------------------------------
// Cleanup Function for Releasing Any Buffers Queued to this
// Window.
//----------------------------------------------------------
void IPCMessage::IPCDestroy(MSGQ Queue)
{
 IPCPurgeMessageQueue(Queue);
}
#endif

//--------------------------------------------------------------------
//                   BEGIN PRIVATE MEMBER FUNCTIONS
//--------------------------------------------------------------------

//-------------------------------------------
// Parse the message (build the offset table)
//-------------------------------------------
bool IPCMessage::ParseMsg()
{
 int offset;
 FLDPTR* fld;
 register i;

 memset(OffsetTbl,0xff,sizeof(OffsetTbl));

 for (offset=sizeof(IPCROUTEINFO); offset < Length;  )
  {
   // Point to field
   fld = (FLDPTR *)&MsgData[offset];

   // Make sure I don't have two of the same field
   if ( OffsetTbl[fld->fldnum] != NOT_PRESENT )
    return false;

   // Set offset in offset table for this field
   OffsetTbl[ fld->fldnum ] = (WORD) offset;

   // Determine the field length and insure that it does not go beyond
   // the end of the message
   switch(fld->fldtype)
    {
     case FLDTYPE_BYTE:     offset += 2 + sizeof(BYTE);
                         break;

     case FLDTYPE_WORD:     offset += 2 + sizeof(WORD);
                         break;

     case FLDTYPE_DWORD:    offset += 2 + sizeof(DWORD);
                         break;

     case FLDTYPE_STRING:   offset += 2 + sizeof(BYTE);
                         for (i=0;
                              fld->type.string[i] != '\0' &&
                                 offset <= Length;
                              ++i, ++offset );
                         break;


     case FLDTYPE_VLEN1:    offset += 2 + sizeof(BYTE);
                         if ( offset < Length )
                           offset += fld->type.vlen1.len;
                         break;

     case FLDTYPE_VLEN2:    offset += 2 + sizeof(WORD);
                         if ( offset < Length )
                           offset += fld->type.vlen2.len;
                         break;


     default: return false;                              
    }

  }

 return ( (offset == Length) ? true : false );
}

//---------------------------
// Put a field to the message
//---------------------------
bool IPCMessage::putfld(FLDNUM fn,BYTE fldtype,void* fld,int fldlen)
{
 FLDPTR* pt;
 int len;

 // See if the field already exists .. If so, it must be the same
 // length or we have to delete it before we replace it.
 if ( (pt = (FLDPTR *)getfld(fn)) != NULL )
  {
   pt = (FLDPTR *)&MsgData[ OffsetTbl[fn] ];
   if ( pt->fldtype != fldtype ||
        (fldtype == FLDTYPE_VLEN1 && pt->type.vlen1.len != fldlen) ||
        (fldtype == FLDTYPE_VLEN2 && pt->type.vlen2.len != fldlen) ||
        (fldtype == FLDTYPE_STRING && fldlen != (int) strlen(pt->type.string)) )
     DelFld(fn);
   else
    {
     switch(fldtype)
      {
       case FLDTYPE_VLEN1:  memcpy(&pt->type.vlen1.data,fld,fldlen);
                         break;

       case FLDTYPE_VLEN2:  memcpy(&pt->type.vlen2.data,fld,fldlen);
                         break;

       default:          memcpy(&pt->type,fld,fldlen);
                         break;
      }
     return true;
    }
  }

 // Appending a new field to the end of the message .. Make sure
 // There is room for it.
 len = sizeof(pt->fldtype) + sizeof(pt->fldnum);
 switch( fldtype )
  {
   case FLDTYPE_BYTE:      len += sizeof(BYTE); break;
   case FLDTYPE_WORD:      len += sizeof(WORD); break;
   case FLDTYPE_DWORD:     len += sizeof(DWORD); break;
   case FLDTYPE_STRING:    len += fldlen + 1; break;
   case FLDTYPE_VLEN1:     len += sizeof(BYTE) + fldlen; break;
   case FLDTYPE_VLEN2:     len += sizeof(WORD) + fldlen; break;
  }

 if ( Length + len > MAX_MSG )
  {
   PutFailFlag = true;
   return false;
  }

 OffsetTbl[fn] = Length;
 Length += (WORD) len;
 pt = (FLDPTR*)getfld(fn);
 pt->fldnum = fn;
 pt->fldtype = fldtype;

 switch( fldtype )
  {
   case FLDTYPE_VLEN1:  pt->type.vlen1.len = (WORD) fldlen;
                     memcpy(&pt->type.vlen1.data,fld,fldlen);
                     break;

   case FLDTYPE_VLEN2:  pt->type.vlen2.len = (WORD) fldlen;
                     memcpy(&pt->type.vlen2.data,fld,fldlen);
                     break;

   case FLDTYPE_STRING: pt->type.string[ fldlen ] = '\0';
   default:          memcpy(&pt->type,fld,fldlen);
                     break;
  }

 return true;
}

//----------------------------------------
// Get a pointer to a field in the message
//----------------------------------------
void* IPCMessage::getfld(FLDNUM fn)
{
 if ( OffsetTbl[fn] == NOT_PRESENT )
   return NULL;

 return &MsgData[ OffsetTbl[fn] ];
}


//-----------------------------------------------------
// Check if a variable length field represents a binary
//-----------------------------------------------------
bool IPCMessage::vlen_is_binary(BYTE len,char* fld,DWORD& bin)
{
 union b
  {
   char   *c;
   BYTE   *b;
   WORD *u;
   DWORD  *l;
  } x;

 x.c = fld; 

 switch(len)
  {
   case sizeof(BYTE):    bin = *x.b; break;
   case sizeof(WORD):    bin = *x.u; break;
   case sizeof(DWORD):   bin = *x.l; break;
   default:              return false;
  }

 return true;
}

//----------------------------------------------------------------------
//                        BEGIN TEST STUB
//----------------------------------------------------------------------

#ifdef TEST_STUB
#include <iostream.h>
#include <stdlib.h>
#include <stdio.h>
#ifdef WINDOWSNT
#include <conio.h>
#define RANDOM random
#define FLUSH
#endif

#ifdef SCOUNIX
#define getch getchar
#define RANDOM(num) (int) (((long)rand()*(num))/(RAND_MAX+1))  
#define FLUSH << flush;
#endif

// THIS IS MESSAGE BUF TO TEST WITH
struct
 {
  WORD len;
  char   msg[MAX_MSG];
 } msgbuf;

IPCMessage *msg;

//------------------------------
// COME HERE WHEN THE TEST FAILS
//------------------------------
void fail(void)
{
 cout << "IPCMessage TEST FAILED" FLUSH;
 getch();
 exit(EXIT_FAILURE);
}

//------------------------------------------------------------------
// COPY THE CURRENT MESSAGE AND MAKE A NEW ONE. THIS TESTS THE COPY
// FUNCTION AND THE PARSING ROUTINE.
//------------------------------------------------------------------
void make_new(void)
{
 bool status;
 msg->CopyMsgOut(msgbuf.msg,msgbuf.len);
 delete msg;
 msg=new IPCMessage(msgbuf.msg,msgbuf.len,status);
 if ( ! status )
  {
   cout << "\nMessage Copying/Parsing Failed\n" FLUSH;
   fail();
  }
}

//------------------------------------------------------------------
// TEST NUMERIC TYPE INDEPENDENCE BY RETRIEVING ALL THE FIELDS ONCE
// FOR EACH TYPE AND MAKING SURE THE VALUE IS ALWAYS THE SAME
//------------------------------------------------------------------
void retreive_all(DWORD start)
{
 char buf[100];
 BYTE b;
 WORD u;
 DWORD  l;
 int len;
 int fn;


 // First test it to see if it Parses
 make_new();

 // FETCH BYTES (only if numbers are small enough)
 if ( start <= 0xff )
  {
   for (fn=0; fn<256; ++fn)
    if ( ! msg->GetFld((BYTE)fn,b) )
     {
      cout << "\nFIELD " <<  fn << " BYTE NOT PRESENT\n" FLUSH;
      fail();
     }
    else
    if ( b != fn )
     {
      cout << "\nFIELD " << fn << " BAD BYTE VALUE\n" FLUSH;
      fail();
     }
   }

 // FETCH WORDS (only if less than or = 0xffff)
 if ( start <=0xffff )
  {
   for (fn=0; fn<256; ++fn)
     if ( ! msg->GetFld((BYTE)fn,u) )
      {
       cout << "\nFIELD " << fn << " WORD NOT PRESENT\n" FLUSH;
       fail();
      }
     else
     if ( u != fn + start )
      {
       cout << "\nFIELD " << fn << " BAD WORD VALUE\n" FLUSH;
       fail();
      }
   }

  // FETCH DWORDS
  for (fn=0; fn<256; ++fn)
    if ( ! msg->GetFld((BYTE) fn,l) )
     {
      cout << "\nFIELD " << fn << " DWORD NOT PRESENT\n" FLUSH;
      fail();
     }
    else
    if ( l != fn + start )
     {
      cout << "\nFIELD " << fn << " BAD DWORD VALUE\n" FLUSH;
      fail();
     }

  // FETCH STRINGS
  for (fn=0; fn<256; ++fn)
    if ( ! msg->GetFld((BYTE)fn,sizeof(buf),buf) )
     {
      cout << "\nFIELD " << fn << " STRING NOT PRESENT\n" FLUSH;
      fail();
     }
    else
    if ( atoi(buf) != fn + start )
     {
      cout << "\nFIELD " << fn << " BAD STRING VALUE\n" FLUSH;
      fail();
     }

  // FETCH VLENS
  for (fn=0; fn<256; ++fn)
    if ( ! msg->GetFld((BYTE)fn,buf,len=sizeof(buf)) )
     {
      cout << "\nFIELD " << fn << " VLEN NOT PRESENT\n" FLUSH;
      fail();
     }
    else
      switch(len)
       {
        case 1:  memcpy(&b,buf,1);
                 if ( b != fn + start)
                  {
                   cout << "\nFIELD " << fn << " BAD VLEN VALUE\n" FLUSH;
                   fail();
                  }
                 break;
        case 2:  memcpy(&u,buf,2);
                 if ( u != fn + start)
                  {
                   cout << "\nFIELD " << fn << " BAD VLEN VALUE\n" FLUSH;
                   fail();
                  }
                 break;
        case 4:  memcpy(&l,buf,4);
                 if ( l != fn + start)
                  {
                   cout << "\nFIELD " << fn << " BAD VLEN VALUE\n" FLUSH;
                   fail();
                  }
                 break;
        default: cout << "\nFIELD " << fn << " BAD VLEN VALUE\n" FLUSH;
                 fail();
       }
}

//---------------------------------------------------------------
// PUT 256 NUMERIC FIELDS TO THE RECORD USING VARYING FIELD TYPES
//---------------------------------------------------------------
void put_var(DWORD start)
{
 int ntypes, fn;
 DWORD v;
 char buf[100];
 bool status;

 if ( start < 0xff )
   ntypes = 5;
 else
 if ( start < 0xffff )
   ntypes = 4;
 else
   ntypes = 3;

 // Put a lot of numeric fields
 for (fn=0; fn<256; ++fn)
  {
   v = fn + start;
   switch(fn%ntypes)
    {
     case 4: status=msg->PutFld((BYTE)fn,(BYTE) v); break;
     case 3: status=msg->PutFld((BYTE)fn,(WORD) v); break;
     case 2: status=msg->PutFld((BYTE)fn,(DWORD) v); break;
     case 1: ultoa(v,buf,10); status=msg->PutFld((BYTE)fn,buf); break;
     case 0: status=msg->PutFld((BYTE)fn,&v,sizeof(v)); break;
    }
   if ( status != true )
    {
     cout << "\nPut of Field " << fn << " failed\n" FLUSH;
     fail();
    }
  }
}

//---------------
// TEST STUB MAIN
//---------------
void main(void)
{
 char   buf[100];
 WORD u;
 DWORD  l;
 BYTE   b;
 bool   status;
 DWORD  i;
 int    len, maxlen, n, tries, j;
 int    fn;

 cout << "sizeof(IPCROUTEINFO) = " << sizeof(IPCROUTEINFO) << "\n" FLUSH;
 cout << "sizeof(FLDPTR) = " << sizeof(FLDPTR) << "\n" FLUSH;

 // MAKE SURE THE BINARY SIZES ARE COMPATIBLE
 cout << "TESTING BINARY COMPATIBILITY..." FLUSH;
 if ( (i=sizeof(b)) != 1 )
  {
   cout << "\nizeof(BYTE) is not 1\n" FLUSH;
   fail();
  }
 if ( (i=sizeof(u)) != 2 )
  {
   cout << "\nsizeof(WORD) is not 2\n" FLUSH;
   fail();
  }
 if ( (i=sizeof(l)) != 4 )
  {
   cout << "\nsizeof(DWORD) is not 4\n" FLUSH;
   fail();
  }
 cout << "OKAY\n" FLUSH;

 // Start with a bad message
 cout << "TESTING BAD MESSAGE STATUS...." FLUSH;
 msgbuf.len = MAX_MSG + 1;
 msg = new IPCMessage(msgbuf.msg,msgbuf.len,status);
 if ( ! status )
   delete(msg);
 else
  {
   cout << "\nMessage status was okay but message was too big\n" FLUSH;
   fail();
  }

 msgbuf.len=MAX_MSG;
 msg = new IPCMessage(msgbuf.msg,msgbuf.len,status);
 if ( ! status )
   delete(msg);
 else
  {
   cout << "\nMessage status was okay but message was trash\n" FLUSH;
   fail();
  }
 cout << "OKAY\n" FLUSH;

 msg = new IPCMessage;

 cout << "TESTING NUMERIC TYPE-INDEPENDENCE...." FLUSH;
 put_var(0);
 retreive_all(0);

 // Put all the fields back as bytes
 for (fn=0; fn<256; ++fn)
   msg->PutFld((BYTE)fn,(BYTE)fn);
 retreive_all(0);

 // Put all the fields back as WORD
 for (fn=0; fn<256; ++fn)
   msg->PutFld((BYTE)fn,(WORD)fn);
 retreive_all(0);

 // Put all the fields back as DWORD
 for (fn=0; fn<256; ++fn)
   msg->PutFld((BYTE)fn,(DWORD) fn);
 retreive_all(0);

 // Put all the fields back as STRING
 for (fn=0; fn<256; ++fn)
  {
   ultoa(fn,buf,10);
   msg->PutFld((BYTE)fn,buf);
  }
 retreive_all(0);

 // Put all the fields back as VLEN
 for (fn=0; fn<256; ++fn)
  {
   b=(BYTE)fn;
   msg->PutFld((BYTE)fn,&b,sizeof(b));
  }
 retreive_all(0);

 // Put all the fields back as VLEN
 for (fn=0; fn<256; ++fn)
  {
   u=(WORD)fn;
   msg->PutFld((BYTE)fn,&u,sizeof(u));
  }
 retreive_all(0);

 // Put all the fields back as VLEN
 for (fn=0; fn<256; ++fn)
  {
   l=fn;
   msg->PutFld((BYTE)fn,&l,sizeof(l));
  }
 retreive_all(0);
 cout << "OKAY\n" FLUSH;

 // Cycle through some big numbers
 cout << "TESTING SOME BIG NUMBERS..." FLUSH;
 for (i=256; i < 1000000; i+= 1000)
  {
   put_var(i);
   retreive_all(i);
  }
 cout << "OKAY\n" FLUSH;

 cout << "TESTING SOME REALLY BIG NUMBERS...." FLUSH;
 for (i=3000000000; i<4000000000; i+= 1000000)
  {
   put_var(i);
   retreive_all(i);
  }
 cout << "OKAY\n" FLUSH;

 delete msg;

 cout << "TESTING VARIABLE LENGTH FIELDS...." FLUSH;
 msg = new IPCMessage;
 // Try Putting in a field that fills the whole buffer and
 // make sure it works.
 memset(msgbuf.msg,0xff,sizeof(msgbuf.msg));
 if ( ! msg->PutFld(0,msgbuf.msg,MAX_MSG-4-sizeof(IPCROUTEINFO)) )
  {
   cout << "\nFailed to Put maximum length field\n" FLUSH;
   fail();
  }
 // Get it back and make sure it's okay
 make_new();
 memset(msgbuf.msg,0,sizeof(msgbuf.msg));
 if ( ! msg->GetFld(0,msgbuf.msg,len=MAX_MSG) ||
      len != MAX_MSG - 4 -sizeof(IPCROUTEINFO) )
  {
   cout << "\nFailed to Get maximum length field\n" FLUSH;
   fail();
  }
 // Make sure it's all hi-values
 for (i=0; i<len; ++i)
  if ( (BYTE) msgbuf.msg[i] != 0xff )
   {
    cout << "\nLarge field was corrupt at byte offset " << i << "\n" FLUSH;
    fail();
   }
 // Make sure a field too large fails
 delete msg;
 msg = new IPCMessage;
 if ( msg->PutFld(0,msgbuf.msg,MAX_MSG-3) )
  {
   cout << "\nField too big but Put didn't fail\n" FLUSH;
   fail();
  }
 // Test the PutFail flag
 if ( ! msg->PutFldFail(true) )
  {
   cout << "\nPutFail Flag not set correctly\n" FLUSH;
   fail();
  }
 // The PutFail flag should now be clear
 if ( msg->PutFldFail() )
  {
   cout << "\nThe PutFail flag did not clear correctly\n" FLUSH;
   fail();
  }
 delete msg;

 // Cycle through Filling the buffer with variable length fields
 for (tries=0; tries<5000; ++tries)
  {
   msg = new IPCMessage;
   for (fn=0; (maxlen=MAX_MSG-msg->GetMsgLength()) >= 5 && fn < 256; ++fn)
    {
     maxlen -= 4;
     len=RANDOM(maxlen-1)+1;
     memset(msgbuf.msg,fn,len);
     if ( ! msg->PutFld((BYTE)fn,msgbuf.msg,len) )
      {
       cout << "\nFailure putting field " << fn << "\n" FLUSH;
       fail();
      }
    }
   make_new();
   for (n=0; n<fn; ++n)
    {
     if ( ! msg->GetFld((BYTE)n,msgbuf.msg,len=MAX_MSG) )
      {
       cout << "\nField " << n << " Not in Message\n" FLUSH;
       fail();
      }
     for (j=0; j<len; ++j)
      if ( (BYTE) msgbuf.msg[j] != n )
       {
        cout << "\nCorrupt field # " << n << "\n" FLUSH;
        fail();
       }
    }
   delete msg;
  }
 cout << "OKAY\n" FLUSH;

 // ALL DONE
 cout << "IPCMessage TEST OKAY....PUT IT TO BED" FLUSH;
 getch();
}
#endif

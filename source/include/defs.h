#ifndef __DEFS_H
#define __DEFS_H
 
//#define SCOUNIX
#define WINDOWSNT

typedef unsigned short WORD;
typedef unsigned long  DWORD;
typedef unsigned char  BYTE;
typedef unsigned int   uint;

#ifdef WINDOWSNT
#ifndef FUNCTYPE
#ifdef EXPORT
#define FUNCTYPE _export
#else
#define FUNCTYPE _import
#endif // IFDEF EXPORT
#endif // IFNDEF FUNCTYPE
#endif // WINDOWSNT

#ifdef SCOUNIX
typedef int bool;
#define false 0
#define true 1
#pragma pack(1)
#define FUNCTYPE
#endif 

#endif

#ifndef __WIN4ME_H
#define __WIN4ME_H
// This excludes everything possible from compilation of headers
// Include it first in all your programs. #UNDEF the things you
// need immediately after it's included
#define NOATOM
//#define NOCLIPBOARD
//#define NOCOLOR
#define NOCOMM   
//#define NOCTLMGR
#define NODEFERWINDOWPOS
#define NODRAWTEXT
//#define NOGDI
#define NOGDICAPMASKS
//#define NOHELP
#define NOICONS
#define NOKANJI
#define NOKERNEL
#define NOKEYSTATES
//#define NOMB
#define NOMCX
#define NOMEMMGR
//#define NOMENUS
//#define NOMETAFILE
#define NOMINMAX
//#define NOMSG
#define NONLS
#define NOOPENFILE
#define NOPROFILER
//#define NORASTEROPS
//#define NOSCROLL
#define NOSERVICE
//#define NOSHOWWINDOW
#define NOSOUND
//#define NOSYSCOMMANDS
#define NOSYSMETRICS
//#define NOTEXTMETRIC
//#define NOUSER
#define NOVIRTUALKEYS
//#define NOWH
//#define NOWINMESSAGES
//#define NOWINOFFSETS
//#define NOWINSTYLES
//#define OEMRESOURCE
//#define WIN32_LEAN_AND_MEAN
//#define WIN32_NO_STATUS
#define NOIMAGE
#define NOTAPE
#define ANSI_ONLY
#include <owl/pch.h>
#include <string.h>
#include "ipc.h"
#endif

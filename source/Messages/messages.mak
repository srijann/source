#
# Borland C++ IDE generated makefile
# Generated 9/11/96 at 11:46:51 AM 
#
.AUTODEPEND


#
# Borland C++ tools
#
IMPLIB  = Implib
BCC32   = Bcc32 +BccW32.cfg 
TLINK32 = TLink32
TLIB    = TLib
BRC32   = Brc32
TASM32  = Tasm32
#
# IDE macros
#
#
# External tools
#
Message Compiler = \bc5\sdktools\MC.EXE  # IDE Command Lin -h ..\include


#
# Options
#
IDE_LinkFLAGS32 =  -L\BC5\LIB
IDE_ResFLAGS32 = 
LinkerLocalOptsAtW32_messagesdlib =  -Tpd -aa -V4.0 -c
ResLocalOptsAtW32_messagesdlib = 
BLocalOptsAtW32_messagesdlib = 
CompInheritOptsAt_messagesdlib = -I\ALLPROG\INCLUDE;./;\BC5\INCLUDE -D_RTLDLL;_BIDSDLL;_OWLDLL;STRICT;_OWLPCH;
LinkerInheritOptsAt_messagesdlib = -x
LinkerOptsAt_messagesdlib = $(LinkerLocalOptsAtW32_messagesdlib)
ResOptsAt_messagesdlib = $(ResLocalOptsAtW32_messagesdlib)
BOptsAt_messagesdlib = $(BLocalOptsAtW32_messagesdlib)
CompLocalOptsAtW32_messagesdmc = 
LinkerLocalOptsAtW32_messagesdmc = 
ResLocalOptsAtW32_messagesdmc = 
BLocalOptsAtW32_messagesdmc = 
CompOptsAt_messagesdmc = $(CompOptsAt_messagesdlib) $(CompLocalOptsAtW32_messagesdmc)
CompInheritOptsAt_messagesdmc = -I..\INCLUDE;./;\BC5\INCLUDE -D_RTLDLL;_BIDSDLL;_OWLDLL;STRICT;_OWLPCH;
LinkerInheritOptsAt_messagesdmc = -x
LinkerOptsAt_messagesdmc = $(LinkerOptsAt_messagesdlib) $(LinkerLocalOptsAtW32_messagesdmc)
ResOptsAt_messagesdmc = $(ResOptsAt_messagesdlib) $(ResLocalOptsAtW32_messagesdmc)
BOptsAt_messagesdmc = $(BOptsAt_messagesdlib) $(BLocalOptsAtW32_messagesdmc)

#
# Dependency List
#
Dep_messages = \
   messages.lib

messages : BccW32.cfg $(Dep_messages)
  echo MakeNode

messages.lib : messages.dll
  $(IMPLIB) $@ messages.dll


Dep_messagesddll = \
   msg00001.bin\
   messages.res

messages.dll : $(Dep_messagesddll)
  $(TLINK32) @&&|
 /v $(IDE_LinkFLAGS32) $(LinkerOptsAt_messagesdlib) $(LinkerInheritOptsAt_messagesdlib) +
\BC5\LIB\c0d32.obj
$<,$*
\BC5\LIB\owlwfi.lib+
\BC5\LIB\bidsfi.lib+
\BC5\LIB\import32.lib+
\BC5\LIB\cw32i.lib

|
   $(BRC32) messages.res $<

msg00001.bin :  messages.mc
  $(Message Compiler) -h ..\Include messages

messages.res :  messages.rc
  $(BRC32) -R @&&|
 $(IDE_ResFLAGS32) $(ROptsAt_messagesdlib) $(CompInheritOptsAt_messagesdlib)  -FO$@ messages.rc
|
# Compiler configuration file
BccW32.cfg :
   Copy &&|
-w
-R
-v
-vi
-H
-H=messages.csm
-5
-K
-d
-r-
-WD
-H"owl\pch.h"
| $@



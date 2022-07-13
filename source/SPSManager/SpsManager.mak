#
# Borland C++ IDE generated makefile
# Generated 2/11/97 at 12:14:44 AM 
#
.AUTODEPEND


#
# Borland C++ tools
#
IMPLIB  = Implib
BCC32   = Bcc32 +BccW32.cfg 
BCC32I  = Bcc32i +BccW32.cfg 
TLINK32 = TLink32
TLIB    = TLib
BRC32   = Brc32
TASM32  = Tasm32
#
# IDE macros
#


#
# Options
#
IDE_LinkFLAGS32 =  -LD:\BC5\LIB
IDE_ResFLAGS32 = 
LinkerLocalOptsAtW32_spsmanagerdexe =  -L\BC5\LIB -Tpe -aa -V4.0 -c
ResLocalOptsAtW32_spsmanagerdexe = 
BLocalOptsAtW32_spsmanagerdexe = 
CompInheritOptsAt_spsmanagerdexe = -I.\;..\INCLUDE;..\CLASSLIB;\BC5\INCLUDE -D_RTLDLL;_BIDSDLL;_OWLDLL;STRICT;_OWLPCH;
LinkerInheritOptsAt_spsmanagerdexe = -x
LinkerOptsAt_spsmanagerdexe = $(LinkerLocalOptsAtW32_spsmanagerdexe)
ResOptsAt_spsmanagerdexe = $(ResLocalOptsAtW32_spsmanagerdexe)
BOptsAt_spsmanagerdexe = $(BLocalOptsAtW32_spsmanagerdexe)

#
# Dependency List
#
Dep_SpsManager = \
   spsmanager.exe

SpsManager : BccW32.cfg $(Dep_SpsManager)
  echo MakeNode

Dep_spsmanagerdexe = \
   authrulesdialog.obj\
   dbmanmsg.obj\
   openclassdialog.obj\
   opentreedialog.obj\
   exitdialog.obj\
   opensave.obj\
   ..\lib\splib.lib\
   FormDialog.obj\
   prodclassdialog.obj\
   ..\..\programs\ipclib.lib\
   ..\lib\classlib.lib\
   spsmanagerapp.obj\
   posfilerulesdialog.obj\
   ruleparamsdialog.obj\
   datetimedialog.obj\
   productadjustdialog.obj\
   prodcodedialog.obj\
   SecRulesDialog.obj\
   datareqdialog.obj\
   MerchantDialog.obj\
   spsmanagerdlgclient.obj\
   spsmanageraboutdlg.obj\
   spsmanagerapp.res\
   spsmanagerapp.def

spsmanager.exe : $(Dep_spsmanagerdexe)
  $(TLINK32) @&&|
 /v $(IDE_LinkFLAGS32) $(LinkerOptsAt_spsmanagerdexe) $(LinkerInheritOptsAt_spsmanagerdexe) +
\BC5\LIB\c0w32.obj+
authrulesdialog.obj+
dbmanmsg.obj+
openclassdialog.obj+
opentreedialog.obj+
exitdialog.obj+
opensave.obj+
FormDialog.obj+
prodclassdialog.obj+
spsmanagerapp.obj+
posfilerulesdialog.obj+
ruleparamsdialog.obj+
datetimedialog.obj+
productadjustdialog.obj+
prodcodedialog.obj+
SecRulesDialog.obj+
datareqdialog.obj+
MerchantDialog.obj+
spsmanagerdlgclient.obj+
spsmanageraboutdlg.obj
$<,$*
..\lib\splib.lib+
..\..\programs\ipclib.lib+
..\lib\classlib.lib+
\BC5\LIB\owlwfi.lib+
\BC5\LIB\bidsfi.lib+
\BC5\LIB\import32.lib+
\BC5\LIB\cw32i.lib
spsmanagerapp.def
spsmanagerapp.res

|
Dep_authrulesdialogdobj = \
   authrulesdialog.h\
   authrulesdialog.cpp

authrulesdialog.obj : $(Dep_authrulesdialogdobj)
  $(BCC32) -c @&&|
 $(CompOptsAt_spsmanagerdexe) $(CompInheritOptsAt_spsmanagerdexe) -o$@ authrulesdialog.cpp
|

dbmanmsg.obj :  dbmanmsg.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_spsmanagerdexe) $(CompInheritOptsAt_spsmanagerdexe) -o$@ dbmanmsg.cpp
|

Dep_openclassdialogdobj = \
   openclassdialog.h\
   openclassdialog.cpp

openclassdialog.obj : $(Dep_openclassdialogdobj)
  $(BCC32) -c @&&|
 $(CompOptsAt_spsmanagerdexe) $(CompInheritOptsAt_spsmanagerdexe) -o$@ openclassdialog.cpp
|

Dep_opentreedialogdobj = \
   openclassdialog.h\
   opentreedialog.cpp

opentreedialog.obj : $(Dep_opentreedialogdobj)
  $(BCC32) -c @&&|
 $(CompOptsAt_spsmanagerdexe) $(CompInheritOptsAt_spsmanagerdexe) -o$@ opentreedialog.cpp
|

exitdialog.obj :  exitdialog.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_spsmanagerdexe) $(CompInheritOptsAt_spsmanagerdexe) -o$@ exitdialog.cpp
|

opensave.obj :  opensave.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_spsmanagerdexe) $(CompInheritOptsAt_spsmanagerdexe) -o$@ opensave.cpp
|

Dep_FormDialogdobj = \
   FormDialog.h\
   FormDialog.cpp

FormDialog.obj : $(Dep_FormDialogdobj)
  $(BCC32) -c @&&|
 $(CompOptsAt_spsmanagerdexe) $(CompInheritOptsAt_spsmanagerdexe) -o$@ FormDialog.cpp
|

Dep_prodclassdialogdobj = \
   prodclassdialog.h\
   prodclassdialog.cpp

prodclassdialog.obj : $(Dep_prodclassdialogdobj)
  $(BCC32) -c @&&|
 $(CompOptsAt_spsmanagerdexe) $(CompInheritOptsAt_spsmanagerdexe) -o$@ prodclassdialog.cpp
|

Dep_spsmanagerappdobj = \
   spsmanagerapp.h\
   spsmanagerapp.cpp

spsmanagerapp.obj : $(Dep_spsmanagerappdobj)
  $(BCC32) -c @&&|
 $(CompOptsAt_spsmanagerdexe) $(CompInheritOptsAt_spsmanagerdexe) -o$@ spsmanagerapp.cpp
|

Dep_posfilerulesdialogdobj = \
   posfilerulesdialog.h\
   posfilerulesdialog.cpp

posfilerulesdialog.obj : $(Dep_posfilerulesdialogdobj)
  $(BCC32) -c @&&|
 $(CompOptsAt_spsmanagerdexe) $(CompInheritOptsAt_spsmanagerdexe) -o$@ posfilerulesdialog.cpp
|

Dep_ruleparamsdialogdobj = \
   ruleparamsdialog.h\
   ruleparamsdialog.cpp

ruleparamsdialog.obj : $(Dep_ruleparamsdialogdobj)
  $(BCC32) -c @&&|
 $(CompOptsAt_spsmanagerdexe) $(CompInheritOptsAt_spsmanagerdexe) -o$@ ruleparamsdialog.cpp
|

Dep_datetimedialogdobj = \
   datetimedialog.h\
   datetimedialog.cpp

datetimedialog.obj : $(Dep_datetimedialogdobj)
  $(BCC32) -c @&&|
 $(CompOptsAt_spsmanagerdexe) $(CompInheritOptsAt_spsmanagerdexe) -o$@ datetimedialog.cpp
|

Dep_productadjustdialogdobj = \
   productadjustdialog.h\
   productadjustdialog.cpp

productadjustdialog.obj : $(Dep_productadjustdialogdobj)
  $(BCC32) -c @&&|
 $(CompOptsAt_spsmanagerdexe) $(CompInheritOptsAt_spsmanagerdexe) -o$@ productadjustdialog.cpp
|

Dep_prodcodedialogdobj = \
   prodcodedialog.h\
   prodcodedialog.cpp

prodcodedialog.obj : $(Dep_prodcodedialogdobj)
  $(BCC32) -c @&&|
 $(CompOptsAt_spsmanagerdexe) $(CompInheritOptsAt_spsmanagerdexe) -o$@ prodcodedialog.cpp
|

Dep_SecRulesDialogdobj = \
   SecRulesDialog.h\
   SecRulesDialog.cpp

SecRulesDialog.obj : $(Dep_SecRulesDialogdobj)
  $(BCC32) -c @&&|
 $(CompOptsAt_spsmanagerdexe) $(CompInheritOptsAt_spsmanagerdexe) -o$@ SecRulesDialog.cpp
|

Dep_datareqdialogdobj = \
   datareqdialog.h\
   datareqdialog.cpp

datareqdialog.obj : $(Dep_datareqdialogdobj)
  $(BCC32) -c @&&|
 $(CompOptsAt_spsmanagerdexe) $(CompInheritOptsAt_spsmanagerdexe) -o$@ datareqdialog.cpp
|

Dep_MerchantDialogdobj = \
   MerchantDialog.h\
   MerchantDialog.cpp

MerchantDialog.obj : $(Dep_MerchantDialogdobj)
  $(BCC32) -c @&&|
 $(CompOptsAt_spsmanagerdexe) $(CompInheritOptsAt_spsmanagerdexe) -o$@ MerchantDialog.cpp
|

Dep_spsmanagerdlgclientdobj = \
   spsmanagerdlgclient.h\
   spsmanagerdlgclient.cpp

spsmanagerdlgclient.obj : $(Dep_spsmanagerdlgclientdobj)
  $(BCC32) -c @&&|
 $(CompOptsAt_spsmanagerdexe) $(CompInheritOptsAt_spsmanagerdexe) -o$@ spsmanagerdlgclient.cpp
|

Dep_spsmanageraboutdlgdobj = \
   spsmanageraboutdlg.h\
   spsmanageraboutdlg.cpp

spsmanageraboutdlg.obj : $(Dep_spsmanageraboutdlgdobj)
  $(BCC32) -c @&&|
 $(CompOptsAt_spsmanagerdexe) $(CompInheritOptsAt_spsmanagerdexe) -o$@ spsmanageraboutdlg.cpp
|

Dep_spsmanagerappdres = \
   spsmanagerapp.rh\
   spsmanagerapp.rc

spsmanagerapp.res : $(Dep_spsmanagerappdres)
  $(BRC32) -R @&&|
 $(IDE_ResFLAGS32) $(ROptsAt_spsmanagerdexe) $(CompInheritOptsAt_spsmanagerdexe)  -FO$@ spsmanagerapp.rc
|
MerchantDialog.res :  MerchantDialog.rc
  $(BRC32) -R @&&|
 $(IDE_ResFLAGS32) $(ROptsAt_spsmanagerdexe) $(CompInheritOptsAt_spsmanagerdexe)  -FO$@ MerchantDialog.rc
|
DataReqDialog.res :  DataReqDialog.rc
  $(BRC32) -R @&&|
 $(IDE_ResFLAGS32) $(ROptsAt_spsmanagerdexe) $(CompInheritOptsAt_spsmanagerdexe)  -FO$@ DataReqDialog.rc
|
AuthRulesDialog.res :  AuthRulesDialog.rc
  $(BRC32) -R @&&|
 $(IDE_ResFLAGS32) $(ROptsAt_spsmanagerdexe) $(CompInheritOptsAt_spsmanagerdexe)  -FO$@ AuthRulesDialog.rc
|
SecRulesDialog.res :  SecRulesDialog.rc
  $(BRC32) -R @&&|
 $(IDE_ResFLAGS32) $(ROptsAt_spsmanagerdexe) $(CompInheritOptsAt_spsmanagerdexe)  -FO$@ SecRulesDialog.rc
|
ProdCodeDialog.res :  ProdCodeDialog.rc
  $(BRC32) -R @&&|
 $(IDE_ResFLAGS32) $(ROptsAt_spsmanagerdexe) $(CompInheritOptsAt_spsmanagerdexe)  -FO$@ ProdCodeDialog.rc
|
PosFileDialog.res :  PosFileDialog.rc
  $(BRC32) -R @&&|
 $(IDE_ResFLAGS32) $(ROptsAt_spsmanagerdexe) $(CompInheritOptsAt_spsmanagerdexe)  -FO$@ PosFileDialog.rc
|
ProdClassDialog.res :  ProdClassDialog.rc
  $(BRC32) -R @&&|
 $(IDE_ResFLAGS32) $(ROptsAt_spsmanagerdexe) $(CompInheritOptsAt_spsmanagerdexe)  -FO$@ ProdClassDialog.rc
|
OpenTreeDialog.res :  OpenTreeDialog.rc
  $(BRC32) -R @&&|
 $(IDE_ResFLAGS32) $(ROptsAt_spsmanagerdexe) $(CompInheritOptsAt_spsmanagerdexe)  -FO$@ OpenTreeDialog.rc
|
OpenClassDialog.res :  OpenClassDialog.rc
  $(BRC32) -R @&&|
 $(IDE_ResFLAGS32) $(ROptsAt_spsmanagerdexe) $(CompInheritOptsAt_spsmanagerdexe)  -FO$@ OpenClassDialog.rc
|
# Compiler configuration file
BccW32.cfg : 
   Copy &&|
-w
-R
-v
-vi
-H
-H=SpsManager.csm
-Hc-
-w-ncf
-wdef
-wnod
-wamb
-wuse
-wstv
-wasm
-wamp
-wobs
-wpch
-Od
-K
-d-
-R
-N
-O-M
-O-I
-O-S
-O-c
-O-i
-O-v
-5
-w-ucp
-H=WIN4ME.CSM
-W
-H"owl\pch.h"
-H
| $@



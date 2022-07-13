SPSOut: SPSOut.o SPSLog.o IPCMsg.o IPCProcs.o INETMan.o Pipes.o Recfiltr.o
	CC SPSOut.o SPSLog.o IPCMsg.o IPCProcs.o INETMan.o Pipes.o Recfiltr.o -dbx -lsocket -o SPSOut

Pipes.o : Pipes.C
	CC -c -dbx Pipes.C    

SPSOut.o : ipcmsg.h spslog.h SPSOut.C
	CC -c -dbx SPSOut.C

INETMan.o : INETMan.C 
	CC -c -dbx INETMan.C

SPSLog.o : spslog.h SPSLog.C
	CC -c -dbx SPSLog.C

IPCMsg.o : ipcmsg.h IPCMsg.C
	CC -c -dbx IPCMsg.C

Recfiltr.o : Recfiltr.C
	CC -c -dbx Recfiltr.C

IPCProcs.o : ipcmsg.h spslog.h IPCProcs.C
	CC -c -dbx IPCProcs.C


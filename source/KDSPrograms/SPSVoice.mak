SPSVoice: SPSVoice.o SPSLog.o IPCMsg.o IPCProcs.o INETMan.o Pipes.o Recfiltr.o
	CC SPSVoice.o SPSLog.o IPCMsg.o IPCProcs.o INETMan.o Pipes.o Recfiltr.o -dbx -lsocket -o SPSVoice

SPSVoice.o : ipcmsg.h spslog.h SPSVoice.C
	CC -c -dbx SPSVoice.C

Pipes.o : Pipes.C
	CC -c -dbx Pipes.C   

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


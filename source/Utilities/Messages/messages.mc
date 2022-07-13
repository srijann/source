;//This be's the Message file
FacilityNames=(SPS=0x7)

MessageId=1
Facility=SPS
Severity=Error
SymbolicName=MissingParm
Language=English
%1: Unable To Read Key '%2' From System Registry. This is a
configuration error that should be reported to the technical 
Staff.
.

MessageId=2
Severity=Error
SymbolicName=IllegalFunctionParm
Language=English
%1: An Invalid or Illegal Parameter (%2) was passed to function '%3'.
This is a Programming  error that should be reported to the technical
staff.
.

MessageId=3
Severity=Error
SymbolicName=SysCallError
Language=English
%1: A call to system function '%2' in function '%3' failed due to
error code %4 (%5). This is a system error that should be reported
to the technical staff.
.

MessageId=4
Severity=Informational
SymbolicName=ServerStarted
Language=English
%1: Server Started
.

MessageId=5
Severity=Error
SymbolicName=ResourceFull
Language=English
%1: A System Resoure is full and cannot be expanded. Function=%2,
Resource=%3. This is a system or configuration error that should be
reported to the technical staff.
.

MessageId=6
Severity=Warning
SymbolicName=TrashedBuffer
Language=English
%1: A Trashed buffer was detected in function %2. This is a
Serious internal Programming Error that should be reported to
the technical Staff.
.

MessageId=7
Severity=Informational
SymbolicName=ServerStopped
Language=English
%1: Server Stopped
.

MessageId=8
Severity=Error
SymbolicName=ServerAlreadyRunning
Language=English
%1: The Server Process could not be started because a process
with the same name is already running on this computer. This
is an operational or configuration error.
.


MessageId=9
Severity=Error
SymbolicName=InvalidRegistryProcessName
Language=English
%1: An invalid Process Name (%2) was encountered in the local registry
under key '%3'. This is a configuration error that must be 
corrected by the Technical Staff.
.


MessageId=10
Severity=Warning
SymbolicName=ProcessNotFound
Language=English
%1: The Physical or Logical Process Name '%2' was not found in the
local registry. A Server process that is supposed to be running may
be stopped, otherwise this is a configuration problem which must be
corrected by the Technical Staff.
.

MessageId=11
Severity=Informational
SymbolicName=MessageTrace
Language=English
%1: Message Trace - "%2". This is a diagnostic message that requires no
action.
.

MessageId=12
Severity=Informational
SymbolicName=MessageTraceDump
Language=English
%1: Message Trace - "%2". This is a diagnostic message that requires no
action. Message Dump Included.
.


MessageId=13
Severity=Error
SymbolicName=HostNotFound
Language=English
%1: The Computer Named "%2" could not be located. gethostbyname returned
%3 (%4). This may indicate a problem with the DOMAIN name server or it
may be a configuration error that must be corrected by the technical staff.
.

MessageId=14
Severity=Error
SymbolicName=BadMessageFromHost
Language=English
%1: An improperly formatted message was received from host %2. This is an
error that should be reported to the technical staff. Message Dump follows:
.

MessageId=15
Severity=Error
SymbolicName=VelocisSecurityError
Language=English
%1: Velocis Login Failed
.

MessageId=16
Severity=Error
SymbolicName=VelocisError
Language=English
%1: A Velocis Error has occurred in source module %2 at line %3.
Status=%4 Function=%5 RecType=%6 KeyType=%7
d_errinfo = %8
.

MessageId=17
Severity=Error
SymbolicName=BadData
Language=English
%1: A Message Was received with invalid or missing data (%2). This is a
programming or configuration error that should be reported to the technical
staff. Message dump follows:
.

MessageId=18
Severity=Error
SymbolicName=FileError
Language=English
%1: An I/O error occurred %2 file '%3' (%4).
.

MessageId=19
Severity=Error
SymbolicName=DBManRecordMissing
Language=English
%1:  Missing class record for MerchantID %2: Type=%3 Class=%4
.

MessageId=20
Severity=Error
SymbolicName=AccountMissing
Language=English
%1: No Account Record for %2
.

MessageId=21
Severity=Informational
SymbolicName=PurgeComplete
Language=English
%1: Purge Complete. Retained=%2 Purged=%3.
.












 

 


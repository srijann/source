//This be's the Message file
//
//  Values are 32 bit values layed out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-+-----------------------+-------------------------------+
//  |Sev|C|R|     Facility          |               Code            |
//  +---+-+-+-----------------------+-------------------------------+
//
//  where
//
//      Sev - is the severity code
//
//          00 - Success
//          01 - Informational
//          10 - Warning
//          11 - Error
//
//      C - is the Customer code flag
//
//      R - is a reserved bit
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//
//
// Define the facility codes
//


//
// Define the severity codes
//


//
// MessageId: MissingParm
//
// MessageText:
//
//  %1: Unable To Read Key '%2' From System Registry. This is a
//  configuration error that should be reported to the technical 
//  Staff.
//
#define MissingParm                      0xC0070001L

//
// MessageId: IllegalFunctionParm
//
// MessageText:
//
//  %1: An Invalid or Illegal Parameter (%2) was passed to function '%3'.
//  This is a Programming  error that should be reported to the technical
//  staff.
//
#define IllegalFunctionParm              0xC0070002L

//
// MessageId: SysCallError
//
// MessageText:
//
//  %1: A call to system function '%2' in function '%3' failed due to
//  error code %4 (%5). This is a system error that should be reported
//  to the technical staff.
//
#define SysCallError                     0xC0070003L

//
// MessageId: ServerStarted
//
// MessageText:
//
//  %1: Server Started
//
#define ServerStarted                    0x40070004L

//
// MessageId: ResourceFull
//
// MessageText:
//
//  %1: A System Resoure is full and cannot be expanded. Function=%2,
//  Resource=%3. This is a system or configuration error that should be
//  reported to the technical staff.
//
#define ResourceFull                     0xC0070005L

//
// MessageId: TrashedBuffer
//
// MessageText:
//
//  %1: A Trashed buffer was detected in function %2. This is a
//  Serious internal Programming Error that should be reported to
//  the technical Staff.
//
#define TrashedBuffer                    0x80070006L

//
// MessageId: ServerStopped
//
// MessageText:
//
//  %1: Server Stopped
//
#define ServerStopped                    0x40070007L

//
// MessageId: ServerAlreadyRunning
//
// MessageText:
//
//  %1: The Server Process could not be started because a process
//  with the same name is already running on this computer. This
//  is an operational or configuration error.
//
#define ServerAlreadyRunning             0xC0070008L

//
// MessageId: InvalidRegistryProcessName
//
// MessageText:
//
//  %1: An invalid Process Name (%2) was encountered in the local registry
//  under key '%3'. This is a configuration error that must be 
//  corrected by the Technical Staff.
//
#define InvalidRegistryProcessName       0xC0070009L

//
// MessageId: ProcessNotFound
//
// MessageText:
//
//  %1: The Physical or Logical Process Name '%2' was not found in the
//  local registry. A Server process that is supposed to be running may
//  be stopped, otherwise this is a configuration problem which must be
//  corrected by the Technical Staff.
//
#define ProcessNotFound                  0x8007000AL

//
// MessageId: MessageTrace
//
// MessageText:
//
//  %1: Message Trace - "%2". This is a diagnostic message that requires no
//  action.
//
#define MessageTrace                     0x4007000BL

//
// MessageId: MessageTraceDump
//
// MessageText:
//
//  %1: Message Trace - "%2". This is a diagnostic message that requires no
//  action. Message Dump Included.
//
#define MessageTraceDump                 0x4007000CL

//
// MessageId: HostNotFound
//
// MessageText:
//
//  %1: The Computer Named "%2" could not be located. gethostbyname returned
//  %3 (%4). This may indicate a problem with the DOMAIN name server or it
//  may be a configuration error that must be corrected by the technical staff.
//
#define HostNotFound                     0xC007000DL

//
// MessageId: BadMessageFromHost
//
// MessageText:
//
//  %1: An improperly formatted message was received from host %2. This is an
//  error that should be reported to the technical staff. Message Dump follows:
//
#define BadMessageFromHost               0xC007000EL

//
// MessageId: VelocisSecurityError
//
// MessageText:
//
//  %1: Velocis Login Failed
//
#define VelocisSecurityError             0xC007000FL

//
// MessageId: VelocisError
//
// MessageText:
//
//  %1: A Velocis Error has occurred in source module %2 at line %3.
//  Status=%4 Function=%5 RecType=%6 KeyType=%7
//  d_errinfo = %8
//
#define VelocisError                     0xC0070010L

//
// MessageId: BadData
//
// MessageText:
//
//  %1: A Message Was received with invalid or missing data (%2). This is a
//  programming or configuration error that should be reported to the technical
//  staff. Message dump follows:
//
#define BadData                          0xC0070011L

//
// MessageId: FileError
//
// MessageText:
//
//  %1: An I/O error occurred %2 file '%3' (%4).
//
#define FileError                        0xC0070012L

//
// MessageId: DBManRecordMissing
//
// MessageText:
//
//  %1:  Missing class record for MerchantID %2: Type=%3 Class=%4
//
#define DBManRecordMissing               0xC0070013L

//
// MessageId: AccountMissing
//
// MessageText:
//
//  %1: No Account Record for %2
//
#define AccountMissing                   0xC0070014L

//
// MessageId: PurgeComplete
//
// MessageText:
//
//  %1: Purge Complete. Retained=%2 Purged=%3.
//
#define PurgeComplete                    0x40070015L


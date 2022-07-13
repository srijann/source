#ifndef __DESTNAME_H
#define __DESTNAME_H
// Logical and Physical Destination name macros.

// IPCLogicalNames (30 chars max) "123456789012345678901234567890"

// These are general
#define MANAGERDB_DEST            "MANAGERDB"
#define POSAUTH_DEST              "POSAUTHDEST"
#define REFERRALQUEUE_DEST        "REFERRALQUEUE"
#define PREAUTHDB_DEST            "PREAUTHDEST"
#define VOICEAUTH_DEST            "VOICEAUTH"
#define LOGGER_DEST               "SPSLOGGER"
#define POSITIVEFILE_DEST         "POSITIVEFILE"
#define BANKPHONE_DEST            "BANKPHONEFILE"
#define LISTMANAGER_DEST          "LISTMANAGER"

// These are specific to the programs that use them
#define POSRESPONSE_DEST          "POSRESPONSE"
#define REFQUEUERESPONSE_DEST     "REFQUEUEREPLY"
#define MANDBVOICEAUTH_DEST       "MANDBVOICEAUTH"
#define MANDBPOSAUTH_DEST         "MANDBPOSAUTH"
#define PREAUTH_REPLY             "PREAUTHREPLY"

// IPCPhysicalName (8 chars max)   "12345678"
#define PROCESSNAME_POSAUTH        "POSAUTH"
#define PROCESSNAME_MANAGERDB      "MANAGEDB"
#define PROCESSNAME_REFERRALQUEUE  "REFQUE"
#define PROCESSNAME_NETROUTE       "NETROUTE"
#define PROCESSNAME_PREAUTHDB      "PREAUTH"
#define PROCESSNAME_LOGGER         "SPSLOG"
#define PROCESSNAME_POSFILE        "POSFILE"
#define PROCESSNAME_BANKPHONE      "BANKPHON"

#endif
12/06/99 - Log started
 Purpose - record necessarys for making to modification to VoiceAuth
 dialog screen, specifically emphasizing the text in the 'negative
 records' display.

 These are the . rc files

06/19/98  04:13p                 5,732 CheckAuth.rc
07/16/98  02:01p                10,757 CheckAuthApp.rc
07/02/98  09:53a                12,747 DecisionDialog.rc
07/24/97  06:44p                 6,491 Dialogs.rc
07/08/97  12:22p                 6,095 EmployerDialog.rc
04/23/97  12:36p                 3,725 HoldCheckDialog.rc
06/22/98  07:44a                 1,438 msgwin.rc
07/04/97  03:24a                    32 ServerApp.rc
               8 File(s)         47,017 bytes


In them is noted that they were made with the Borland Resource Workshop.

Look in C++ Help under Resource Workshop.

Opened DecisionDialog.rc. It gets vague here. I double clicked on an
identifier and it opened the graphic view of some resources, including,
It looks just like the part I need thadt displays retrieved negative
information.

Right Click on first editbox. It says a double click will do. Shows tabbed
menu, but no attribute of the font in the box.
IDC_CK1DATE  IDC_CK1MERCH  IDC_CK1AMT IDC_CK1CKNUM   IDC_CK1TYPE  IDC_CK1IDS
IDC_CK3DATE  IDC_CK3MERCH  IDC_CK3AMT IDC_CK3CKNUM   IDC_CK3TYPE  IDC_CK3IDS

Look in file msgwin.rc. There is a different font used (FONT 8, "System"),
but it seems to be outside the control block. as opposed to
(FONT 8, "MS Sans Serif").

The ID of the Dialog is IDD_DECISION

If this can be grepped, a duplicate set of resource files could be made with
only the contents of the Negative Hits Display in it...
*** Nope.

In DecisionDialog.cpp @ 423, code for
// Format a Bad Checks Row Display

Look for
CheckRows[Row].DATE->Transfer(Buf,tdSetData);















































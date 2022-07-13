//----------------------------------------------------------------------------
//  Project spsmanager
//  
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    spsmanager.apx Application
//  FILE:         OpenSave.cpp
//  AUTHOR:
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of OpenSave
//----------------------------------------------------------------------------

#include "win4me.h"
#pragma hdrstop
#include <classlib/file.h>
#include <errno.h>
#include "SpsManagerApp.h"
#include "SaveFile.h"
#include "RecType.h"

//-----------------------------------
// Opens A Save file and Validates it
//-----------------------------------

enum
 {
  sv_notexist,
  sv_error,
  sv_notsps,
  sv_corrupt,
  sv_badversion,
  sv_okay
 };

//--------------------------------------------------------
// Validate a SAVE file and return one of the values above
//--------------------------------------------------------
TFile* TSpsManagerApp::ValidSaveFile(char* FileName,int& Status)
{
 TFile* File = new TFile(FileName);
 SAVEFILE_SENTINEL Sentinel;

 if ( ! File->IsOpen() )
  {
   if ( errno==ENOENT )
    Status=sv_notexist;
   else
    Status=sv_error;
   delete File;
   return 0;
  }

 // Read the SENTINEL and make sure it's a save file
 if ( File->Read(&Sentinel,sizeof(Sentinel)) != sizeof(Sentinel) ||
      memcmp(Sentinel.Id,"SPSMAN",sizeof(Sentinel.Id)) )
  {
   Status=sv_notsps;
   File->Close();
   delete File;
   return 0;
  }

 // A version number of 0 means this is a prior save attempt that did
 // not successfully complete
 if ( Sentinel.Version == 0 )
  {
   Status=sv_corrupt;
   File->Close();
   delete File;
   return 0;
  }

 // Check the version number to make sure it's current
 if ( Sentinel.Version != SAVEFILE_CURRENT_VERSION )
  {
   Status=sv_badversion;
   File->Close();
   delete File;
   return 0;
  }

 Status=sv_okay;
 return File;
}

//-----------------------------------
// Display an error message for errno
//-----------------------------------
void TSpsManagerApp::FileError(char *Text)
{
 MBOX(Text,"I/O ERROR",MB_OK|MB_ICONERROR);
}

//-----------------------
// Open a file for saving
//-----------------------
TFile* TSpsManagerApp::OpenSaveFile(char* FileName)
{
 SAVEFILE_SENTINEL Sentinel;
 int Status;
 int Reply;
 TFile* File=ValidSaveFile(FileName,Status);

 if ( File )
  {
   File->Close();
   delete File;
  }

 // The COMPILER is generating bad code here.
 switch ( Status )
  {
   case sv_error:      FileError("ERROR OPENING FILE");
                       return 0;

   case sv_notexist:   break;

   case sv_notsps:     if ( MBOX(
                       "THE FILE YOU HAVE SELECTED IS NOT\n"
                       "AN SPS MANAGER SAVE FILE. YOU CAN'T\n"
                       "APPEND YOUR CHANGES TO IT.\n\n"
                       "DO YOU WANT TO OVERWRITE IT?","",
                         MB_OKCANCEL|MB_ICONERROR) == IDCANCEL )
                         return 0;
                       break;

   case sv_corrupt:    if ( MBOX(
                       "THE FILE YOU HAVE SELECTED IS A\n"
                       "CORRUPT SPS MANAGER SAVE FILE.\n\n"
                       "YOU CAN'T APPEND YOUR CHANGES TO IT\n\n"
                       "DO YOU WANT TO OVERWRITE IT?","",
                       MB_OKCANCEL|MB_ICONERROR) == IDCANCEL )
                         return 0;
                       break;

   case sv_badversion:  if ( MBOX(
                       "THE FILE YOU HAVE SELECTED IS AN\n"
                       "OLD VERSION SPS MANAGER SAVE FILE.\n\n"
                       "YOU CAN'T APPEND YOUR CHANGES TO IT!\n\n"
                       "DO YOU WANT TO OVERWRITE IT?","",
                       MB_OKCANCEL|MB_ICONERROR) == IDCANCEL )
                        return 0;
                       break;

                    // It's a good file, See if we want to append or
                    // overwrite
   case sv_okay:
     File->Close();
     Reply = MBOX(
             "THE FILE YOU HAVE SELECTED ALREADY EXISTS.\n"
             "DO YOU WANT TO APPEND YOUR CHANGES TO IT?\n\n"
             "REPLY YES TO APPEND CHANGES TO THE FILE.\n"
             "REPLY NO TO OVERWRITE IT.\n","", MB_YESNOCANCEL|MB_ICONEXCLAMATION);
     if ( Reply==IDCANCEL )
       return 0;
     if ( Reply==IDYES )
      {
        File = new TFile();
        if ( ! File->Open(FileName,File->ReadWrite,File->PermRdWr) )
          {
           FileError("ERROR OPENING FILE");
           delete File;
           return 0;
          }
        Sentinel.Version=0;
        if ( File->Write(&Sentinel.Version,sizeof(Sentinel.Version)) !=
               sizeof(Sentinel.Version) )
         {
          FileError("ERROR WRITING TO FILE");
          File->Close();
          delete File;
          return 0;
         }

        return File;
      }
     break;

  }

 // Create a new file
 File = new TFile();
 if ( ! File->Open(FileName,File->ReadWrite|File->Create,File->PermRdWr) )
  {
   FileError("ERROR OPENING FILE");
   delete File;
   return 0;
  }

 // Write out a sentinel
 strcpy(Sentinel.Id,"SPSMAN");
 Sentinel.Version=0;
 Sentinel.TimeStamp=time(NULL);
 if ( File->Write(&Sentinel,sizeof(Sentinel)) != sizeof(Sentinel) )
  {
   FileError("ERROR WRITING TO FILE");
   File->Close();
   delete File;
   return 0;
  }

 return File;
}

//-----------------------
// Open a file for saving
//-----------------------
TFile* TSpsManagerApp::OpenFile(char* FileName)
{
 int Status;
 TFile* File=ValidSaveFile(FileName,Status);

 switch ( Status )
  {
   default:
   case sv_error:      FileError("ERROR OPENING FILE");
                       return 0;

   case sv_notsps:     MBOX(
                       "THE FILE YOU HAVE SELECTED IS NOT\n"
                       "AN SPS MANAGER SAVE FILE.","",MB_OK|MB_ICONERROR);
                       return 0;

   case sv_corrupt:    MBOX(
                         "THE FILE YOU HAVE SELECTED IS A\n"
                         "CORRUPT SPS MANAGER SAVE FILE.\n\n"
                         "IT CAN'T BE RELOADED","",MB_OKCANCEL|MB_ICONERROR);
                       return 0;

   case sv_badversion: MBOX(
                         "THE FILE YOU HAVE SELECTED IS AN\n"
                         "OLD VERSION SPS MANAGER SAVE FILE.\n\n"
                         "IT CAN'T BE RELOADED!\n","",MB_OKCANCEL|MB_ICONERROR);
                       return 0;

   case sv_okay:       return File;
  }

}


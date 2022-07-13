#include "Win4me.h"
#include "owl/profile.h"
#include "Chkauth.h"
#include "ChkauthIni.h"
#include "ChkauthDialog.h"

#define WORKBUF_SIZE 8192

ChkauthIni::ChkauthIni()
{
 char IniFile[MAX_PATH+1];
 WorkBuf=new char[WORKBUF_SIZE];
 InitComboBoxList(
}

ChkauthIni::~ChkauthIni()
{
 delete[] WorkBuf;
}


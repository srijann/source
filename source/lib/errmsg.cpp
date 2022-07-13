#include <windows.h>
#include <string.h>

static bool GuiFlag=true;
static char application_name[20];

void InitErrmsg(char *name,bool IsGui)
{
 GuiFlag=IsGui;
 memcpy(applicaton_name,name,min(strlen(name),sizeof(application_name)-1);
}






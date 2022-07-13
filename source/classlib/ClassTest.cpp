#include "win4me.h"
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#include "DbmanRec.h"
void Pr(DBManRecord* Rec)
{
 printf("from Generic: %p %d %d\n",Rec->DataPtr(),Rec->DataLen(),Rec->ObjLen());
}



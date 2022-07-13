//------------------------------
// Removes an item from an array
//------------------------------

#include <mem.h>

int RemoveArrayItem(void *array,int numitems,int itemsize,int delitem)
{
 if (numitems==0 ||
     --numitems==0)
  return 0;

 memcpy( &((char*)array)[delitem*itemsize],
         &((char*)array)[(delitem+1)*itemsize],
         (numitems-delitem)*itemsize);

 return numitems;
}

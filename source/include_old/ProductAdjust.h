#ifndef __PRODUCTADJUST_H
#define __PRODUCTADJUST_H
#include "SPSManDB.h"

#define GLOBAL_ADJUSTMENT 0
#define CLASS_ADJUSTMENT 1
#define SUBCLASS_ADJUSTMENT 2

struct
 {
  BYTE Value[NUMPRODUCTSUBGROUPS];
 } ProductClassAdjustment[3];

#endif
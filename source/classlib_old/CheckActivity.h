#ifndef __CheckActivity_H
#include "kwrec.h"

class CheckActivity {
  public:
  bool Count(uint Days,uint CountableAmount,uint CountableAccum,
             uint& NumTrans,uint& Amount,
             char* Account=0,char* Station=0,char* Sic=0);
 };
#endif
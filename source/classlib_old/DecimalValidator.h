#ifndef __DECIMALVAL_H
#define __DECIMALVAL_H

#include <owl/validate>
class TDecimalValidator : public TValidator {
  int MaxLeft;
  int MaxRight;
  public:
  TDecimalValidator(int,int);
  virtual bool IsValidInput(char far* str, bool suppressFill);
 };

#endif
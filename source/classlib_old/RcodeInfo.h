#ifndef __RCODEINFO_H
#define __RCODEINFO_H

class RcodeInfo
 {
  public:
  char* rcodeText(int rcode);
  bool IsKenwoodRcode(int rc) { return (rc>=1 && rc<=20); };
  bool IsApprovalRcode(int rc) { return (rc>=80 && rc<=89); };
  bool IsDeclineRcode(int rc) { return (rc>=21 && rc<=28); };
  bool IsErrorRcode(int rc) { return ! IsApprovalRcode(rc) &&
                                     ! IsDeclineRcode(rc); };
 };
#endif
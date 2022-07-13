database SPSLogDB {
  data file "SPSLog.dat" contains LogRecord;
  data file "SPSLog.seg" contains OverflowRecord;
  key  file "Merchnum.key" contains MerchantKey;
  key  file "Micr.key" contains MicrKey;
  key  file "License.key" contains LicenseKey;
  key  file "SSN.key" contains SSNKey;
  key  file "Phone.key" contains PhoneKey;
  key  file "Approval.key" contains ApprovalNumberKey;
  key  file "Operator.key" contains OperatorKey;
  key  file "LogDate.key" contains DateKey;

  record LogRecord
   {
    unsigned long  Date;               // Date as YYMMDD format
    unsigned long  Time;               // Time as HHMMSS
    char  MerchantID[6];
    char  BankNumber[10];
    char  BankAccount[17];
    char  DriversLicense[18];
    unsigned long  SSn;
    char  PhoneNumber[11];
    unsigned short ApprovalNumber;
    unsigned short Operator;
    unsigned short MsgLength;
    char  PrimarySegment[2048][1];

    compound key DateKey
     {
      Date descending;
      Time ascending;
     } 

    compound unique key MerchantKey
     {
      MerchantID ascending;
      Date descending;
      Time ascending;
     }

    compound unique key OperatorKey
     {
      Operator ascending;
      Date descending;
      Time ascending; 
     }

    compound optional unique key MicrKey
     {
      BankNumber ascending;
      BankAccount ascending;  
      Date descending;
      Time ascending;
     }

    compound optional unique key LicenseKey
     {
      DriversLicense ascending;
      Date descending;
      Time ascending;
     }

    compound optional unique key SSNKey
     {
      SSn ascending;
      Date descending;
      Time ascending;
     }

    compound optional unique key PhoneKey
     {
      PhoneNumber ascending;
      Date descending;
      Time ascending;
     }

    compound optional unique key ApprovalNumberKey
     {
      ApprovalNumber ascending;
      Date descending;
      Time ascending;
     }
   }

  record OverflowRecord {
     unsigned char Segment[1024][1];
   }

  set OverflowSegment {
    order last;
    owner LogRecord;
    member OverflowRecord;
   }
   
}

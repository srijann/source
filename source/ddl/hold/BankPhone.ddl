database BankPhoneDB {
  data file "BankPhon.dat" contains BankPhoneRecord;
  key  file "BankNum.key" contains BankNumber;

  record BankPhoneRecord
   {
    unique key char BankNumber[10];
    char BankName[26];
    char PhoneNumber[11];
    char Comment[31];
    char Filler[51];
  }

}

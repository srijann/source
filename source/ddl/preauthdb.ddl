database PreauthDB {
  data file "Preauth.dat" contains PreauthRecord;
  key  file "Preauth.key" contains PreauthConsumerID;

  record PreauthRecord
   {
    unsigned long TimeofPreauth;
    unsigned long TimeofPostauth;
    unsigned char PreauthRetention;
    char PreauthMerchantID[6];
    key char PreauthConsumerID[30];
   }

}

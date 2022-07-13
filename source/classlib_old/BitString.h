#ifndef __BITSTRING_H
#define __BITSTRING_H

typedef unsigned int BITNUM;

class BitString {
  char* TheString;
  BITNUM NumBits;
  BITNUM NumBytes;
  // Should I delete the Bit string or did the caller provide it for me?
  bool  ShouldDelete;
  public:
  BitString(BITNUM HowMany,char* String=0);
  ~BitString();
  BITNUM BitLen() { return NumBits; };
  int    ByteLen() { return NumBytes; };
  // Get a pointer to the string
  char* Ptr() { return TheString; };
  // Set a Bit to Value
  void Set(BITNUM Bit,bool Value=true);
  // Set Range of bit's to Value
  void Set(BITNUM FirstBit,BITNUM LastBit,bool Value=true);
  // Set All the Bits to Value
  void SetAll(bool Value=true);
  // Test a bit for Value
  bool Test(BITNUM Bit,bool Value=true);
  // Test if Any bits in the range = Value
  bool TestAny(BITNUM FirstBit,BITNUM LastBit,bool Value=true);
  // Test if Any bits in the entire string = Value
  bool TestAny(bool Val=true);
  // Test if All the Bits in the range = Value
  bool TestAll(BITNUM FirstBit,BITNUM LastBit,bool Value=true);
  // Test if ALL the Bits in the String = Value
  bool TestAll(bool Val=true);
  // Count how many bits in the range = Value
  BITNUM HowMany(BITNUM FirstBit,BITNUM LastBit,bool Value=true);
  // Count how many bits in the String = Value
  BITNUM HowMany(bool Value=true);
  // Format callers' string with a representation of the bits
  void FormatString(char* Str);
  // Create a new string with a representaion of the bits (caller must
  // delete.
  char* FormatString();
  void ForEach(void (*Func)(BITNUM ,void*), void* args=0, bool Value=true);
  BITNUM FirstThat(bool (*Func)(BITNUM ,void*), void* args=0, bool Value=true);
  BITNUM LastThat(bool (*Func)(BITNUM ,void*), void* args=0, bool Value=true);

  // Overload [] to test Bit at BITNUM
  bool operator[](BITNUM Bn)
   {
    return Test(Bn);
   }
  // Overload ++ To turn on a bit
  void operator+=(BITNUM Bn)
   {
    Set(Bn);
   }
  // Overload -- To turn off a bit
  void operator-=(BITNUM Bn)
   {
    Set(Bn,false);
   }
  // Overload == to Get the first bit in the string that matches
  // value.
  BITNUM operator==(bool Value);
 };
#endif

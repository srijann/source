#ifdef __FLAT__
  #include <win32\imessage.h>
#else
  #error imessage.h can only be used in a Win32 Application
#endif // __FLAT__
#ifdef __FLAT__
  #include <win32\lmshare.h>
#else
  #error lmshare.h can only be used in a Win32 Application
#endif
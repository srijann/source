#ifdef __FLAT__
  #include <win32\shlobj.h>
#else
  #error shlobj.h can only be used in a Win32 Application
#endif // __FLAT__
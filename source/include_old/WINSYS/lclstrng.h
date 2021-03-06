//----------------------------------------------------------------------------
// Borland WinSys Library
// Copyright (c) 1994, 1996 by Borland International, All Rights Reserved
//
//$Revision:   5.4  $
//
// TLocaleString - localized name support
//----------------------------------------------------------------------------
#if !defined(WINSYS_LCLSTRNG_H)
#define WINSYS_LCLSTRNG_H

#if !defined(WINSYS_DEFS_H)
# include <winsys/defs.h>
#endif
#if !defined(BI_PLAT_MSW)
# error Locale classes are only supported under MSW
#endif
#if !defined(SERVICES_CSTRING_H)
# include <services/cstring.h>
#endif

#define DEFAULT_SYSTEM_LANGID 0x409   // in case 16-bit with no NLS support
typedef uint16 TLangId;               // language ID - same as NT LANGID
typedef int (*TstricmpLang)(const char far*, const char far*, TLangId);
const TLangId LangSysDefault  = 0x0800;
const TLangId LangUserDefault = 0x0400;
const TLangId LangNeutral     = 0x0000;

//----------------------------------------------------------------------------
// TLocaleString - localizable substitute for char*
//

struct TLocaleString {
  const char* Translate(TLangId lang);                      // translate string
  operator const char*();      // return current string
  void operator =(const char* str) { Private = str; }
  int   Compare(const char far* str, TLangId lang);   // insensitive compare

  const char* Private;         // string pointer used for initialization

  static TLangId GetSystemLangId();  // platform-dependent implementation
  static TLangId GetUserLangId();    // platform-dependent implementation
  static int     IsNativeLangId(TLangId lang);  // returns bool true if native
  static TLangId SystemDefaultLangId;// must define and set to system language
  static TLangId UserDefaultLangId;  // same as system language if single user
  static TLangId NativeLangId;   // must define and set to lang. of literals
  static HINSTANCE Module;       // must define and set to resource module
  static TLocaleString Null;     // reference a null string
  static int CompareLang(const char far* s1, const char far* s2, TLangId);
          // CompareLang() may be implemented in another module for NLS support
};

//
// Prefix characters for locale strings
//
#define AUTOLANG_RCID  '#' // indicates name specified by numeric ID
#define AUTOLANG_XLAT  '!' // indicates name to be localized (binary)
#define AUTOLANG_LOAD  '@' // indicates resource name to load (binary)

//
// Custom resource for translations
//
#define RT_LOCALIZATION MAKEINTRESOURCE(201)

#endif  // WINSYS_LCLSTRNG_H

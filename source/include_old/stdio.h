/*  stdio.h

    Definitions for stream input/output.

*/

/*
 *      C/C++ Run Time Library - Version 7.0
 *
 *      Copyright (c) 1987, 1996 by Borland International
 *      All Rights Reserved.
 *
 */

#ifndef __STDIO_H
#define __STDIO_H

#if !defined(___DEFS_H)
#include <_defs.h>
#endif

#if !defined(___NFILE_H)
#include <_nfile.h>
#endif

#ifndef NULL
#include <_null.h>
#endif


#if !defined(RC_INVOKED)

#if defined(__STDC__)
#pragma warn -nak
#endif

#pragma pack(push, 1)

#endif  /* !RC_INVOKED */


#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned size_t;
#endif

/* Definition of the file position type
*/
typedef long    fpos_t;

/* An external reference to _floatconvert (using #pragma extref _floatconvert)
 * forces floating point format conversions to be linked.
 */
extern int _floatconvert;

/* Bufferisation type to be used as 3rd argument for "setvbuf" function
*/
#define _IOFBF  0
#define _IOLBF  1
#define _IONBF  2

/*  "flags" bits definitions
*/
#define _F_RDWR 0x0003                  /* Read/write flag       */
#define _F_READ 0x0001                  /* Read only file        */
#define _F_WRIT 0x0002                  /* Write only file       */
#define _F_BUF  0x0004                  /* Malloc'ed Buffer data */
#define _F_LBUF 0x0008                  /* line-buffered file    */
#define _F_ERR  0x0010                  /* Error indicator       */
#define _F_EOF  0x0020                  /* EOF indicator         */
#define _F_BIN  0x0040                  /* Binary file indicator */
#define _F_IN   0x0080                  /* Data is incoming      */
#define _F_OUT  0x0100                  /* Data is outgoing      */
#define _F_TERM 0x0200                  /* File is a terminal    */

/* End-of-file constant definition
*/
#define EOF (-1)            /* End of file indicator */

/* Default buffer size use by "setbuf" function
*/
#define BUFSIZ  512         /* Buffer size for stdio */

/* Size of an arry large enough to hold a temporary file name string
*/
#define L_ctermid   5       /* CON: plus null byte */
#define P_tmpdir    ""      /* temporary directory */
#define L_tmpnam    13      /* tmpnam buffer size */

/* Constants to be used as 3rd argument for "fseek" function
*/
#define SEEK_CUR    1
#define SEEK_END    2
#define SEEK_SET    0

/* Number of unique file names that shall be generated by "tmpnam" function
*/
#define TMP_MAX     0xFFFF


#if !defined(__FLAT__)

/* Definition of the control structure for streams
*/
typedef struct  {
        int             level;          /* fill/empty level of buffer */
        unsigned        flags;          /* File status flags          */
        char            fd;             /* File descriptor            */
        unsigned char   hold;           /* Ungetc char if no buffer   */
        int             bsize;          /* Buffer size                */
        unsigned char   _FAR *buffer;   /* Data transfer buffer       */
        unsigned char   _FAR *curp;     /* Current active pointer     */
        unsigned        istemp;         /* Temporary file indicator   */
        short           token;          /* Used for validity checking */
}       FILE;                           /* This is the FILE object    */

/* Number of files that can be open simultaneously
*/
#if defined(__STDC__)
#define FOPEN_MAX (_NFILE_ - 2) /* (_NFILE_ - stdaux & stdprn) */
#else
#define FOPEN_MAX (_NFILE_)     /* Able to have 20 files */
#define SYS_OPEN  (_NFILE_)
#endif

#define FILENAME_MAX 80

/* Standard I/O predefined streams
*/

#if !defined( _RTLDLL )
extern  FILE    _RTLENTRY _streams[];
extern  unsigned    _RTLENTRY _nfile;

#define stdin   (&_streams[0])
#define stdout  (&_streams[1])
#define stderr  (&_streams[2])
#define stdaux  (&_streams[3])
#define stdprn  (&_streams[4])

#else

#ifdef __cplusplus
extern "C" {
#endif
FILE far * far _RTLENTRY __getStream(int);
#ifdef __cplusplus
}
#endif

#define stdin   __getStream(0)
#define stdout  __getStream(1)
#define stderr  __getStream(2)

#endif  /* _RTLDLL  */

#ifdef __cplusplus
extern "C" {
#endif
void    _RTLENTRY          clearerr(FILE _FAR *__stream);
int     _RTLENTRY _EXPFUNC fclose(FILE _FAR *__stream);
int     _RTLENTRY _EXPFUNC fflush(FILE _FAR *__stream);
int     _RTLENTRY _EXPFUNC fgetc(FILE _FAR *__stream);
int     _RTLENTRY          fgetpos(FILE _FAR *__stream, fpos_t _FAR *__pos);
char   _FAR *_RTLENTRY _EXPFUNC fgets(char _FAR *__s, int __n, FILE _FAR *__stream);
FILE   _FAR *_RTLENTRY _EXPFUNC fopen(const char _FAR *__path, const char _FAR *__mode);
int     _RTLENTRY _EXPFUNC fprintf(FILE _FAR *__stream, const char _FAR *__format, ...);
int     _RTLENTRY _EXPFUNC fputc(int __c, FILE _FAR *__stream);
int     _RTLENTRY _EXPFUNC fputs(const char _FAR *__s, FILE _FAR *__stream);
size_t  _RTLENTRY _EXPFUNC fread(void _FAR *__ptr, size_t __size, size_t __n,
                     FILE _FAR *__stream);
FILE   _FAR *_RTLENTRY _EXPFUNC freopen(const char _FAR *__path, const char _FAR *__mode,
                            FILE _FAR *__stream);
int     _RTLENTRY _EXPFUNC fscanf(FILE _FAR *__stream, const char _FAR *__format, ...);
int     _RTLENTRY _EXPFUNC fseek(FILE _FAR *__stream, long __offset, int __whence);
int     _RTLENTRY          fsetpos(FILE _FAR *__stream, const fpos_t _FAR *__pos);
long    _RTLENTRY _EXPFUNC ftell(FILE _FAR *__stream);
size_t  _RTLENTRY _EXPFUNC fwrite(const void _FAR *__ptr, size_t __size, size_t __n,
                      FILE _FAR *__stream);
char   _FAR *_RTLENTRY     gets(char _FAR *__s);
void    _RTLENTRY          perror(const char _FAR *__s);
int     _RTLENTRY          printf(const char _FAR *__format, ...);
int     _RTLENTRY          puts(const char _FAR *__s);
int     _RTLENTRYF         remove(const char _FAR *__path);
int     _RTLENTRYF _EXPFUNC rename(const char _FAR *__oldname,const char _FAR *__newname);
void    _RTLENTRY _EXPFUNC rewind(FILE _FAR *__stream);
int     _RTLENTRY          scanf(const char _FAR *__format, ...);
void    _RTLENTRY          setbuf(FILE _FAR *__stream, char _FAR *__buf);
int     _RTLENTRY _EXPFUNC setvbuf(FILE _FAR *__stream, char _FAR *__buf,
                                   int __type, size_t __size);
int     _RTLENTRY _EXPFUNC sprintf(char _FAR *__buffer, const char _FAR *__format, ...);
int     _RTLENTRY _EXPFUNC sscanf(const char _FAR *__buffer,
                                  const char _FAR *__format, ...);
char   _FAR *_RTLENTRY _EXPFUNC strerror(int __errnum);
FILE   _FAR *_RTLENTRY _EXPFUNC tmpfile(void);
char   _FAR *_RTLENTRY _EXPFUNC tmpnam(char _FAR *__s);
int     _RTLENTRY _EXPFUNC ungetc(int __c, FILE _FAR *__stream);
int     _RTLENTRY _EXPFUNC vfprintf(FILE _FAR *__stream, const char _FAR *__format,
                                    void _FAR *__arglist);
int     _RTLENTRY _EXPFUNC vfscanf(FILE _FAR *__stream, const char _FAR *__format,
                                   void _FAR *__arglist);
int     _RTLENTRYF         vprintf(const char _FAR *__format, void _FAR *__arglist);
int     _RTLENTRY          vscanf(const char _FAR *__format, void _FAR *__arglist);
int     _RTLENTRY _EXPFUNC vsprintf(char _FAR *__buffer, const char _FAR *__format,
                                    void _FAR *__arglist);
int     _RTLENTRY _EXPFUNC vsscanf(const char _FAR *__buffer, const char _FAR *__format,
                                   void _FAR *__arglist);
int     _RTLENTRYF         unlink(const char _FAR *__path);
int     _RTLENTRY          getc(FILE _FAR *__fp);

int     _RTLENTRY          getchar(void);
int     _RTLENTRY          putchar(const int __c);

int     _RTLENTRY          putc(const int __c, FILE _FAR *__fp);
int     _RTLENTRY          feof(FILE _FAR *__fp);
int     _RTLENTRY          ferror(FILE _FAR *__fp);
int     _RTLENTRY          fileno(FILE _FAR *__fp);

int     _RTLENTRY _EXPFUNC fcloseall(void);
FILE    _FAR *_RTLENTRY _EXPFUNC fdopen(int __handle, char _FAR *__type);
int     _RTLENTRY _EXPFUNC fgetchar(void);
int     _RTLENTRY _EXPFUNC flushall(void);
int     _RTLENTRY _EXPFUNC fputchar(int __c);
FILE    _FAR * _RTLENTRY   _fsopen (const char _FAR *__path, const char _FAR *__mode,
                  int __shflag);
int     _RTLENTRY          getw(FILE _FAR *__stream);
int     _RTLENTRY          putw(int __w, FILE _FAR *__stream);
int     _RTLENTRY          rmtmp(void);
char    _FAR * _RTLENTRY _EXPFUNC _strerror(const char _FAR *__s);
char    _FAR * _RTLENTRY _EXPFUNC tempnam(char _FAR *__dir, char _FAR *__pfx);

int      _RTLENTRY _EXPFUNC _fgetc(FILE _FAR *__stream);           /* used by getc() macro */
int      _RTLENTRY _EXPFUNC _fputc(char __c, FILE _FAR *__stream); /* used by putc() macro */
void     _RTLENTRY _InitEasyWin(void);  /* Initialization call for Easy Windows */

#ifdef  __cplusplus
}
#endif   /* __cplusplus */

#else    /* defined __FLAT__ */

/* Definition of the control structure for streams
*/
typedef struct  {
        unsigned char  *curp;       /* Current active pointer     */
        unsigned char  *buffer;     /* Data transfer buffer       */
        int             level;      /* fill/empty level of buffer */
        int             bsize;      /* Buffer size                */
        unsigned short  istemp;     /* Temporary file indicator   */
        unsigned short  flags;      /* File status flags          */
        short           token;      /* Used for validity checking */
        char            fd;         /* File descriptor            */
        unsigned char   hold;       /* Ungetc char if no buffer   */
}       FILE;                       /* This is the FILE object    */

/* Number of files that can be open simultaneously
*/
#if defined(__STDC__)
#define FOPEN_MAX (_NFILE_)
#else
#define FOPEN_MAX (_NFILE_)
#define SYS_OPEN  (_NFILE_)
#endif

#define FILENAME_MAX 260

/* Standard I/O predefined streams
*/
extern  FILE        _RTLENTRY _EXPDATA _streams[];
extern  unsigned    _RTLENTRY _EXPDATA _nfile;

#define stdin   (&_streams[0])
#define stdout  (&_streams[1])
#define stderr  (&_streams[2])

#ifdef __cplusplus
extern "C" {
#endif
/* __getStream() is used internally with CG only, but prototyped here for
  consistancy with the 16-bit version.
*/
FILE * _RTLENTRY _EXPFUNC __getStream( int );
#ifdef __cplusplus
}
#endif


#ifdef __cplusplus
extern "C" {
#endif
void    _RTLENTRY _EXPFUNC clearerr(FILE * __stream);
int     _RTLENTRY _EXPFUNC fclose(FILE * __stream);
int     _RTLENTRY _EXPFUNC fflush(FILE * __stream);
int     _RTLENTRY _EXPFUNC fgetc(FILE * __stream);
int     _RTLENTRY _EXPFUNC fgetpos(FILE * __stream, fpos_t*__pos);
char  * _RTLENTRY _EXPFUNC fgets(char * __s, int __n, FILE * __stream);
FILE  * _RTLENTRY _EXPFUNC fopen(const char * __path, const char * __mode);
int     _RTLENTRY _EXPFUNC fprintf(FILE * __stream, const char * __format, ...);
int     _RTLENTRY _EXPFUNC fputc(int __c, FILE * __stream);
int     _RTLENTRY _EXPFUNC fputs(const char * __s, FILE * __stream);
size_t  _RTLENTRY _EXPFUNC fread(void * __ptr, size_t __size, size_t __n,
                                 FILE * __stream);
FILE  * _RTLENTRY _EXPFUNC freopen(const char * __path, const char * __mode,
                                   FILE * __stream);
int     _RTLENTRY _EXPFUNC fscanf(FILE * __stream, const char * __format, ...);
int     _RTLENTRY _EXPFUNC fseek(FILE * __stream, long __offset, int __whence);
int     _RTLENTRY _EXPFUNC fsetpos(FILE * __stream, const fpos_t*__pos);
long    _RTLENTRY _EXPFUNC ftell(FILE * __stream);
size_t  _RTLENTRY _EXPFUNC fwrite(const void * __ptr, size_t __size, size_t __n,
                            FILE * __stream);
char  * _RTLENTRY _EXPFUNC gets(char * __s);

#if defined(__OS2__) || defined(__WIN32__)
int     _RTLENTRY _EXPFUNC _pclose(FILE *__stream);
#endif

void    _RTLENTRY _EXPFUNC perror(const char * __s);

#if defined(__OS2__) || defined(__WIN32__)
FILE *  _RTLENTRY _EXPFUNC _popen(const char * __command, const char * __mode);
#endif

int     _RTLENTRY _EXPFUNC printf(const char * __format, ...);
int     _RTLENTRY _EXPFUNC puts(const char * __s);

int     _RTLENTRYF _EXPFUNC remove(const char * __path);
int     _RTLENTRYF _EXPFUNC rename(const char * __oldname,const char * __newname);
void    _RTLENTRY  _EXPFUNC rewind(FILE * __stream);
int     _RTLENTRY  _EXPFUNC scanf(const char * __format, ...);
void    _RTLENTRY  _EXPFUNC setbuf(FILE * __stream, char * __buf);
int     _RTLENTRY  _EXPFUNC setvbuf(FILE * __stream, char * __buf,
                                    int __type, size_t __size);
int     _RTLENTRY  _EXPFUNC sprintf(char * __buffer, const char * __format, ...);
#if defined(__MFC_COMPAT__)
int     _RTLENTRY  _EXPFUNC swprintf(wchar_t * __buffer, const wchar_t * __format, ...);
/* swprintf is used internally, and does not fully support wide characters */
#endif  /* __MFC_COMPAT__ */

int     _RTLENTRY  _EXPFUNC sscanf(const char * __buffer,
                                   const char * __format, ...);
char  * _RTLENTRY _EXPFUNC strerror(int __errnum);
FILE  * _RTLENTRY _EXPFUNC tmpfile(void);
char  * _RTLENTRY _EXPFUNC tmpnam(char * __s);
int     _RTLENTRY _EXPFUNC ungetc(int __c, FILE * __stream);
int     _RTLENTRY _EXPFUNC vfprintf(FILE * __stream, const char * __format,
                            void * __arglist);
int     _RTLENTRY _EXPFUNC vfscanf(FILE * __stream, const char * __format,
                            void * __arglist);
int     _RTLENTRYF _EXPFUNC vprintf(const char * __format, void * __arglist);
int     _RTLENTRY _EXPFUNC vscanf(const char * __format, void * __arglist);
int     _RTLENTRY _EXPFUNC vsprintf(char * __buffer, const char * __format,
                        void * __arglist);
int     _RTLENTRY _EXPFUNC vsscanf(const char * __buffer, const char * __format,
                        void * __arglist);
int     _RTLENTRYF _EXPFUNC unlink(const char * __path);

int     _RTLENTRY _EXPFUNC getc(FILE * __fp);
int     _RTLENTRY _EXPFUNC getchar(void);
int     _RTLENTRY _EXPFUNC putchar(const int __c);
int     _RTLENTRY _EXPFUNC putc(const int __c, FILE * __fp);
int     _RTLENTRY _EXPFUNC feof(FILE * __fp);
int     _RTLENTRY _EXPFUNC ferror(FILE * __fp);
int     _RTLENTRY _EXPFUNC fileno(FILE _FAR *__fp);

int     _RTLENTRY _EXPFUNC fcloseall(void);
FILE  * _RTLENTRY _EXPFUNC fdopen(int __handle, char * __type);
int     _RTLENTRY _EXPFUNC fgetchar(void);
int     _RTLENTRY _EXPFUNC flushall(void);
int     _RTLENTRY _EXPFUNC fputchar(int __c);
FILE  * _RTLENTRY _EXPFUNC _fsopen (const char * __path, const char * __mode,
                              int __shflag);
int     _RTLENTRY _EXPFUNC getw(FILE * __stream);
int     _RTLENTRY _EXPFUNC putw(int __w, FILE * __stream);
int     _RTLENTRY _EXPFUNC rmtmp(void);
char  * _RTLENTRY _EXPFUNC _strerror(const char * __s);
char  * _RTLENTRY _EXPFUNC tempnam(char * __dir, char * __pfx);

int      _RTLENTRY _EXPFUNC _fgetc(FILE * __stream);           /* used by getc() macro */
int      _RTLENTRY _EXPFUNC _fputc(char __c, FILE * __stream); /* used by putc() macro */
#ifdef  __cplusplus
}
#endif

#if defined(__MFC_COMPAT__)
inline FILE _FAR *_RTLENTRY _fdopen(int __handle, char _FAR *__type)
                            { return fdopen(__handle, __type); }
inline int        _RTLENTRY _fileno(FILE _FAR *__fp) { return fileno(__fp); }
inline int        _RTLENTRY _fgetchar(void) { return fgetchar(); }
inline int        _RTLENTRY _fputchar(int __c) { return fputchar(__c); }
#endif   /* __MFC_COMPAT__ */

#endif  /* __FLAT__ */

/*  The following macros provide for common functions */

#if !defined(__CODEGUARD__)

#define ferror(f)   ((f)->flags & _F_ERR)
#define feof(f)     ((f)->flags & _F_EOF)
#if !defined(__MFC_COMPAT__)
#define fileno(f)   ((f)->fd)
#endif
#if defined(__MSC) && !defined(__MFC_COMPAT__)
#define _fileno(f)  fileno(f)
#endif

#define getc(f) \
  ((--((f)->level) >= 0) ? (unsigned char)(*(f)->curp++) : \
    _fgetc (f))

#define putc(c,f) \
  ((++((f)->level) < 0) ? (unsigned char)(*(f)->curp++=(c)) : \
    _fputc ((c),f))

#define getchar()  getc(stdin)
#define putchar(c) putc((c), stdout)

#endif

#define ungetc(c,f) ungetc((c),f)   /* traditionally a macro */

#if !defined(RC_INVOKED)

/* restore default packing */
#pragma pack(pop)

#if defined(__STDC__)
#pragma warn .nak
#endif

#endif  /* !RC_INVOKED */


#endif  /* __STDIO_H */
#ifndef net_err_h
#define net_err_h

#include <stdio.h>
#include <errno.h>
#include "net_def.h"




#if defined __GNUC__
#define likely(x) __builtin_expect ((x), 1)
#define unlikely(x) __builtin_expect ((x), 0)
#else
#define likely(x) (x)
#define unlikely(x) (x)
#endif

const char *errno_to_string (int errno_);

#ifdef _WIN32

const char *wsa_error ();
const char *wsa_error_no (int no_);
void win_error (char *buffer_, size_t buffer_size_);
void wsa_error_to_errno (); 

//  Provides convenient way to check WSA-style errors on Windows.
#define wsa_assert(x) \
  do {\
  if (unlikely (!(x))) {\
  const char *errstr = wsa_error ();\
  if (errstr != NULL) {\
  fprintf (stderr, "Assertion failed: %s (%s:%d)\n", errstr, \
  __FILE__, __LINE__);\
  abort ();\
  }\
  }\
  } while (0)

//  Provides convenient way to assert on WSA-style errors on Windows.
#define wsa_assert_no(no) \
  do {\
  const char *errstr = wsa_error_no (no);\
  if (errstr != NULL) {\
  fprintf (stderr, "Assertion failed: %s (%s:%d)\n", errstr, \
  __FILE__, __LINE__);\
  abort ();\
  }\
  } while (0)

// Provides convenient way to check GetLastError-style errors on Windows.
#define win_assert(x) \
  do {\
  if (unlikely (!(x))) {\
  char errstr [256];\
  win_error (errstr, 256);\
  fprintf (stderr, "Assertion failed: %s (%s:%d)\n", errstr, \
  __FILE__, __LINE__);\
  abort ();\
  }\
  } while (0)
#endif


//  This macro works in exactly the same way as the normal assert. It is used
//  in its stead because standard assert on Win32 in broken - it prints nothing
//  when used within the scope of JNI library.
#define net_assert(x) \
  do {\
  if (unlikely (!(x))) {\
  fprintf (stderr, "Assertion failed: %s (%s:%d)\n", #x, \
  __FILE__, __LINE__);\
  abort ();\
  }\
  } while (0) 

//  Provides convenient way to check for errno-style errors.
#define errno_assert(x) \
  do {\
  if (unlikely (!(x))) {\
  perror (NULL);\
  fprintf (stderr, "%s (%s:%d)\n", #x, __FILE__, __LINE__);\
  abort ();\
  }\
  } while (0)

//  Provides convenient way to check for POSIX errors.
#define posix_assert(x) \
  do {\
  if (unlikely (x)) {\
  fprintf (stderr, "%s (%s:%d)\n", strerror (x), __FILE__, __LINE__);\
  abort ();\
  }\
  } while (0)

//  Provides convenient way to check for errors from getaddrinfo.
#define gai_assert(x) \
  do {\
  if (unlikely (x)) {\
  const char *errstr = gai_strerror (x);\
  fprintf (stderr, "%s (%s:%d)\n", errstr, __FILE__, __LINE__);\
  abort ();\
  }\
  } while (0)

//  Provides convenient way to check whether memory allocation have succeeded.
#define alloc_assert(x) \
  do {\
  if (unlikely (!x)) {\
  fprintf (stderr, "FATAL ERROR: OUT OF MEMORY (%s:%d)\n",\
  __FILE__, __LINE__);\
  abort ();\
  }\
  } while (0)

#ifdef _WIN32
int wsa_error_to_errid (int no_);
#else
int errno_to_errid(int no_);
#endif


#endif
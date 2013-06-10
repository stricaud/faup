#ifndef _FAUP_COMPAT_H_
#define _FAUP_COMPAT_H_

#ifdef WIN32

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(HAVE_VASPRINTF) || !defined(HAVE_VSNPRINTF)
# include <stdarg.h>
#endif

#ifndef HAVE_VASPRINTF
int vasprintf(char **, const char *, va_list);
#endif

//#ifndef HAVE_VSNPRINTF
//int vsnprintf(char *, size_t, const char *, va_list);
//#endif

#ifndef HAVE_SNPRINTF
int snprintf(char *, size_t, const char *, ...);
#endif

int inet_pton (int af, const char *src, void *dst);
void *memrchr (void const *s, int c_in, size_t n);

#ifdef __cplusplus
}
#endif

#endif

#endif // _FAUP_COMPAT_H_

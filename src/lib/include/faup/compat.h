#ifndef _FAUP_COMPAT_H_
#define _FAUP_COMPAT_H_

#include <stdio.h>

#ifndef HAVE_SNPRINTF
int snprintf(char *, size_t, const char *, ...);
#endif

int inet_pton (int af, const char *src, void *dst);
void *memrchr (void const *s, int c_in, size_t n);

#endif // _FAUP_COMPAT_H_
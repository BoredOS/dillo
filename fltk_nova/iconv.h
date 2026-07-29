#ifndef ICONV_H
#define ICONV_H

#include <stddef.h>

typedef void* iconv_t;
typedef char inbuf_t;

inline iconv_t iconv_open(const char*, const char*) { return (iconv_t)-1; }
inline size_t iconv(iconv_t, char**, size_t*, char**, size_t*) { return (size_t)-1; }
inline int iconv_close(iconv_t) { return 0; }

#endif // ICONV_H

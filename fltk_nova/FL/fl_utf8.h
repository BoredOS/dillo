#ifndef FL_UTF8_H
#define FL_UTF8_H

#include <stddef.h>
#include <ctype.h>

#ifdef __cplusplus
extern "C" {
#endif

int fl_utf8len(char c);
int fl_utf8len1(char c);
unsigned int fl_utf8decode(const char* p, const char* end, int* len);
int fl_utf8encode(unsigned int u, char* buf);
int fl_utf8to_mb(const char* s, unsigned int len, char* buf, unsigned int bufsize);
int fl_utf8from_mb(char* buf, unsigned int bufsize, const char* s, unsigned int len);

typedef unsigned char uchar_t;

inline int fl_toupper(int c) { return toupper(c); }
inline int fl_tolower(int c) { return tolower(c); }
inline int fl_nonspacing(unsigned int) { return 0; }
inline int fl_utf8test(const char*, unsigned int) { return 0; }
inline int fl_utf_nb_char(const unsigned char*, int len) { return len; }
int fl_utf_toupper(const unsigned char* str, int len, char* buf);
int fl_utf_tolower(const unsigned char* str, int len, char* buf);
const char* fl_utf8fwd(const char* p, const char* start, const char* end);
const char* fl_utf8back(const char* p, const char* start, const char* end);

#ifdef __cplusplus
}
#endif

#endif // FL_UTF8_H

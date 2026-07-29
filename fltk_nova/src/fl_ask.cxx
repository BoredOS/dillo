#include "../FL/fl_ask.H"
#include "../FL/fl_utf8.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

void fl_alert(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);
}

int fl_choice(const char* fmt, const char*, const char*, const char*, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);
    return 0;
}

const char* fl_input(const char* fmt, const char* defstr, ...) {
    va_list ap;
    va_start(ap, defstr);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);
    return defstr ? defstr : "";
}

const char* fl_password(const char* fmt, const char* defstr, ...) {
    va_list ap;
    va_start(ap, defstr);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);
    return defstr ? defstr : "";
}

void fl_message(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    printf("\n");
    va_end(ap);
}

// UTF-8 Helpers
extern "C" {

int fl_utf8len(char c) {
    if ((c & 0x80) == 0) return 1;
    if ((c & 0xE0) == 0xC0) return 2;
    if ((c & 0xF0) == 0xE0) return 3;
    if ((c & 0xF8) == 0xF0) return 4;
    return 1;
}

int fl_utf8len1(char c) {
    return fl_utf8len(c);
}

unsigned int fl_utf8decode(const char* p, const char* end, int* len) {
    if (!p) { if (len) *len = 0; return 0; }
    unsigned char c = (unsigned char)*p;
    if (c < 0x80) { if (len) *len = 1; return c; }
    int l = fl_utf8len((char)c);
    if (end && (p + l > end)) { if (len) *len = 1; return c; }
    if (len) *len = l;
    if (l == 2) return ((c & 0x1F) << 6) | (p[1] & 0x3F);
    if (l == 3) return ((c & 0x0F) << 12) | ((p[1] & 0x3F) << 6) | (p[2] & 0x3F);
    if (l == 4) return ((c & 0x07) << 18) | ((p[1] & 0x3F) << 12) | ((p[2] & 0x3F) << 6) | (p[3] & 0x3F);
    return c;
}

int fl_utf8encode(unsigned int u, char* buf) {
    if (u < 0x80) { buf[0] = (char)u; return 1; }
    if (u < 0x800) {
        buf[0] = 0xC0 | (u >> 6);
        buf[1] = 0x80 | (u & 0x3F);
        return 2;
    }
    if (u < 0x10000) {
        buf[0] = 0xE0 | (u >> 12);
        buf[1] = 0x80 | ((u >> 6) & 0x3F);
        buf[2] = 0x80 | (u & 0x3F);
        return 3;
    }
    buf[0] = 0xF0 | (u >> 18);
    buf[1] = 0x80 | ((u >> 12) & 0x3F);
    buf[2] = 0x80 | ((u >> 6) & 0x3F);
    buf[3] = 0x80 | (u & 0x3F);
    return 4;
}

int fl_utf8to_mb(const char* s, unsigned int len, char* buf, unsigned int bufsize) {
    if (!s || !buf || bufsize == 0) return 0;
    unsigned int n = len < bufsize - 1 ? len : bufsize - 1;
    memcpy(buf, s, n);
    buf[n] = '\0';
    return (int)n;
}

int fl_utf8from_mb(char* buf, unsigned int bufsize, const char* s, unsigned int len) {
    return fl_utf8to_mb(s, len, buf, bufsize);
}

int fl_utf_toupper(const unsigned char* str, int len, char* buf) {
    for (int i = 0; i < len; i++) buf[i] = toupper(str[i]);
    buf[len] = '\0';
    return len;
}

int fl_utf_tolower(const unsigned char* str, int len, char* buf) {
    for (int i = 0; i < len; i++) buf[i] = tolower(str[i]);
    buf[len] = '\0';
    return len;
}

const char* fl_utf8fwd(const char* p, const char*, const char* end) {
    if (!p) return p;
    int len = fl_utf8len(*p);
    if (p + len <= end) return p + len;
    return end;
}

const char* fl_utf8back(const char* p, const char* start, const char*) {
    if (!p || p <= start) return start;
    const char* cur = p - 1;
    while (cur > start && (*cur & 0xC0) == 0x80) cur--;
    return cur;
}

}

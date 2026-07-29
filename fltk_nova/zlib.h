#ifndef ZLIB_H
#define ZLIB_H

#include <stddef.h>

typedef unsigned char Bytef;
typedef char charf;

#define Z_OK            0
#define Z_STREAM_END    1
#define Z_NEED_DICT     2
#define Z_ERRNO        (-1)
#define Z_STREAM_ERROR (-2)
#define Z_DATA_ERROR   (-3)
#define Z_MEM_ERROR    (-4)
#define Z_BUF_ERROR    (-5)
#define Z_VERSION_ERROR (-6)

#define Z_NO_FLUSH      0
#define Z_PARTIAL_FLUSH 1
#define Z_SYNC_FLUSH    2
#define Z_FULL_FLUSH    3
#define Z_FINISH        4
#define Z_BLOCK         5

#define MAX_WBITS 15

typedef struct z_stream_s {
    const unsigned char *next_in;
    unsigned int avail_in;
    unsigned long total_in;
    unsigned char *next_out;
    unsigned int avail_out;
    unsigned long total_out;
    const char *msg;
    void *state;
    void *zalloc;
    void *zfree;
    void *opaque;
    int data_type;
    unsigned long adler;
    unsigned long reserved;
} z_stream;

typedef z_stream *z_streamp;

inline int inflateInit_(z_streamp, const char*, int) { return Z_ERRNO; }
inline int inflateInit2_(z_streamp, int, const char*, int) { return Z_ERRNO; }
inline int inflate(z_streamp, int) { return Z_ERRNO; }
inline int inflateEnd(z_streamp) { return Z_OK; }
inline const char* zlibVersion() { return "1.2.11"; }
#define inflateInit(strm) inflateInit_((strm), "1.2.11", sizeof(z_stream))
#define inflateInit2(strm, windowBits) inflateInit2_((strm), (windowBits), "1.2.11", sizeof(z_stream))

#endif // ZLIB_H

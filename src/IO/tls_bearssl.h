#ifndef __TLS_BEARSSL_H__
#define __TLS_BEARSSL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "url.h"
#include <stddef.h>

const char *a_Tls_bearssl_version(char *buf, int n);
void a_Tls_bearssl_init(void);
int a_Tls_bearssl_connect_ready(const DilloUrl *url);
int a_Tls_bearssl_certificate_is_clean(const DilloUrl *url);
void a_Tls_bearssl_reset_server_state(const DilloUrl *url);
void a_Tls_bearssl_connect(int fd, const DilloUrl *url);
void *a_Tls_bearssl_connection(int fd);
void a_Tls_bearssl_freeall(void);
void a_Tls_bearssl_close_by_fd(int fd);
int a_Tls_bearssl_read(void *conn, void *buf, size_t len);
int a_Tls_bearssl_write(void *conn, void *buf, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* __TLS_BEARSSL_H__ */

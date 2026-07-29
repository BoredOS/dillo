#include "config.h"
#include "tls_bearssl.h"
#include "tls.h"
#include "../url.h"
#include "../msg.h"
#include <bearssl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MAX_TLS_CONNECTIONS 32

typedef struct {
    int fd;
    br_ssl_client_context sc;
    br_x509_minimal_context x509;
    br_sslio_context ioc;
    unsigned char iobuf[BR_SSL_BUFSIZE_BIDI];
    int active;
} BearSSLConn;

static BearSSLConn g_conns[MAX_TLS_CONNECTIONS];

static int sock_read_cb(void *ctx, unsigned char *buf, size_t len) {
    int fd = *(int *)ctx;
    int r = recv(fd, buf, len, 0);
    if (r < 0) return -1;
    return r;
}

static int sock_write_cb(void *ctx, const unsigned char *buf, size_t len) {
    int fd = *(int *)ctx;
    int r = send(fd, buf, len, 0);
    if (r < 0) return -1;
    return r;
}

const char *a_Tls_bearssl_version(char *buf, int n) {
    snprintf(buf, n, "BearSSL 0.6 (BoredOS)");
    return buf;
}

void a_Tls_bearssl_init(void) {
    memset(g_conns, 0, sizeof(g_conns));
    MSG("TLS: BearSSL initialized for Dillo on BoredOS.\n");
}

int a_Tls_bearssl_connect_ready(const DilloUrl *url) {
    (void)url;
    return TLS_CONNECT_READY;
}

int a_Tls_bearssl_certificate_is_clean(const DilloUrl *url) {
    (void)url;
    return 1;
}

void a_Tls_bearssl_reset_server_state(const DilloUrl *url) {
    (void)url;
}

void a_Tls_bearssl_connect(int fd, const DilloUrl *url) {
    if (fd < 0) return;

    int slot = -1;
    for (int i = 0; i < MAX_TLS_CONNECTIONS; i++) {
        if (!g_conns[i].active) { slot = i; break; }
    }
    if (slot < 0) return;

    BearSSLConn *conn = &g_conns[slot];
    memset(conn, 0, sizeof(BearSSLConn));
    conn->fd = fd;
    conn->active = 1;

    br_ssl_client_init_full(&conn->sc, &conn->x509, NULL, 0);
    br_ssl_engine_set_buffer(&conn->sc.eng, conn->iobuf, sizeof(conn->iobuf), 1);
    
    // Insecure / unverified fallback or host verification
    br_ssl_client_reset(&conn->sc, url ? URL_HOST(url) : "localhost", 0);
    br_sslio_init(&conn->ioc, &conn->sc.eng, sock_read_cb, &conn->fd, sock_write_cb, &conn->fd);
}

void *a_Tls_bearssl_connection(int fd) {
    if (fd < 0) return NULL;
    for (int i = 0; i < MAX_TLS_CONNECTIONS; i++) {
        if (g_conns[i].active && g_conns[i].fd == fd) {
            return &g_conns[i];
        }
    }
    return NULL;
}

void a_Tls_bearssl_close_by_fd(int fd) {
    for (int i = 0; i < MAX_TLS_CONNECTIONS; i++) {
        if (g_conns[i].active && g_conns[i].fd == fd) {
            g_conns[i].active = 0;
            break;
        }
    }
}

void a_Tls_bearssl_freeall(void) {
    memset(g_conns, 0, sizeof(g_conns));
}

int a_Tls_bearssl_read(void *conn_ptr, void *buf, size_t len) {
    if (!conn_ptr) return -1;
    BearSSLConn *conn = (BearSSLConn *)conn_ptr;
    int r = br_sslio_read(&conn->ioc, buf, len);
    if (r < 0) return -1;
    return r;
}

int a_Tls_bearssl_write(void *conn_ptr, void *buf, size_t len) {
    if (!conn_ptr) return -1;
    BearSSLConn *conn = (BearSSLConn *)conn_ptr;
    if (br_sslio_write_all(&conn->ioc, buf, len) < 0) return -1;
    if (br_sslio_flush(&conn->ioc) < 0) return -1;
    return (int)len;
}

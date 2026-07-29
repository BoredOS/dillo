#include "../FL/Fl.H"
#include "../FL/Fl_Window.H"
#include "../FL/Fl_Widget.H"
#include "../FL/fl_draw.H"
extern "C" {
#include <novaproto.h>
}
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <poll.h>

struct Fl_Timeout_Entry {
    double time; // absolute expiration time in seconds
    Fl_Timeout_Handler cb;
    void* data;
    Fl_Timeout_Entry* next;
};

struct Fl_FD_Entry {
    int fd;
    int when;
    Fl_FD_Handler cb;
    void* data;
    Fl_FD_Entry* next;
};

static Fl_Timeout_Entry* g_timeouts = NULL;
static Fl_FD_Entry* g_fds = NULL;

static int g_event = FL_NO_EVENT;
static int g_event_x = 0;
static int g_event_y = 0;
static int g_event_dx = 0;
static int g_event_dy = 0;
static int g_event_button = 0;
static int g_event_key = 0;
static int g_event_state = 0;
static int g_event_clicks = 0;
static char g_event_text[16] = {0};

static Fl_Widget* g_focus_widget = NULL;
static Fl_Widget* g_belowmouse_widget = NULL;
static Fl_Widget* g_grabbed_widget = NULL;
extern Fl_Window* g_first_window;
static uint32_t g_prev_buttons = 0;

struct Fl_Idle_Entry {
    Fl_Idle_Handler cb;
    void* data;
    Fl_Idle_Entry* next;
};
static Fl_Idle_Entry* g_idles = NULL;

Fl_Color g_colormap[256] = {
    0x00000000, 0x00800000, 0x00008000, 0x00808000,
    0x00000080, 0x00800080, 0x00008080, 0x00FFFFFF,
    0x00808080, 0x00FF0000, 0x0000FF00, 0x00FFFF00,
    0x000000FF, 0x00FF00FF, 0x0000FFFF, 0x00FFFFFF,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00555555, 0x005A5A5A, 0x00606060, 0x00666666,
    0x006C6C6C, 0x00727272, 0x00787878, 0x007E7E7E,
    0x00848484, 0x008A8A8A, 0x00909090, 0x00969696,
    0x009C9C9C, 0x00A2A2A2, 0x00A8A8A8, 0x00AEAEAE,
    0x00B4B4B4, 0x00BABABA, 0x00C0C0C0, 0x00C6C6C6,
    0x00CCCCCC, 0x00D2D2D2, 0x00D8D8D8, 0x00DEDEDE
};

static double current_time_sec() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + (tv.tv_usec / 1000000.0);
}

int Fl::run() {
    while (first_window()) {
        wait(1.0);
    }
    return 0;
}

int Fl::check() {
    return wait(0.0);
}

int Fl::wait(double time) {
    double now = current_time_sec();

    // Check timeouts
    Fl_Timeout_Entry** pcur = &g_timeouts;
    while (*pcur) {
        if ((*pcur)->time <= now) {
            Fl_Timeout_Entry* e = *pcur;
            *pcur = e->next;
            Fl_Timeout_Handler cb = e->cb;
            void* data = e->data;
            free(e);
            if (cb) cb(data);
            now = current_time_sec();
        } else {
            pcur = &(*pcur)->next;
        }
    }

    // Count fds for poll
    int count = 0;
    for (Fl_Window* w = g_first_window; w; w = w->next_window()) {
        if (w->nova_fd() >= 0) count++;
    }
    for (Fl_FD_Entry* e = g_fds; e; e = e->next) {
        if (e->fd >= 0) count++;
    }

    struct pollfd* fds = NULL;
    if (count > 0) {
        fds = (struct pollfd*)calloc(count, sizeof(struct pollfd));
    }

    int idx = 0;
    for (Fl_Window* w = g_first_window; w; w = w->next_window()) {
        int nfd = w->nova_fd();
        if (nfd >= 0) {
            fds[idx].fd = nfd;
            fds[idx].events = POLLIN;
            idx++;
        }
    }

    for (Fl_FD_Entry* e = g_fds; e; e = e->next) {
        if (e->fd >= 0) {
            fds[idx].fd = e->fd;
            fds[idx].events = 0;
            if (e->when & FL_READ) fds[idx].events |= POLLIN;
            if (e->when & FL_WRITE) fds[idx].events |= POLLOUT;
            idx++;
        }
    }

    double next_timeout = -1.0;
    for (Fl_Timeout_Entry* e = g_timeouts; e; e = e->next) {
        double dt = e->time - now;
        if (dt < 0) dt = 0;
        if (next_timeout < 0 || dt < next_timeout) {
            next_timeout = dt;
        }
    }

    int timeout_ms = (time < 0) ? -1 : (int)(time * 1000.0);
    if (g_idles) {
        timeout_ms = 0;
    } else if (next_timeout >= 0) {
        int t_ms = (int)(next_timeout * 1000.0);
        if (timeout_ms < 0 || t_ms < timeout_ms) {
            timeout_ms = t_ms;
        }
    }

    int ret = poll(fds, count, timeout_ms);

    if (ret > 0 && fds) {
        idx = 0;
        for (Fl_Window* w = g_first_window; w; w = w->next_window()) {
            int nfd = w->nova_fd();
            if (nfd >= 0) {
                if (fds[idx].revents & POLLIN) {
                    NovaEvent evt;
                    while (nova_poll_event(nfd, &evt) > 0) {
                        if (evt.type == EVT_POINTER) {
                            g_event_x = evt.data.pointer.x;
                            g_event_y = evt.data.pointer.y;
                            uint32_t b = evt.data.pointer.buttons;
                            if (b != 0 && g_prev_buttons == 0) {
                                g_event = FL_PUSH;
                                g_event_button = (b & 1) ? 1 : ((b & 2) ? 3 : 2);
                                w->handle(FL_PUSH);
                            } else if (b == 0 && g_prev_buttons != 0) {
                                g_event = FL_RELEASE;
                                g_event_button = (g_prev_buttons & 1) ? 1 : ((g_prev_buttons & 2) ? 3 : 2);
                                w->handle(FL_RELEASE);
                            } else if (b != 0) {
                                g_event = FL_DRAG;
                                w->handle(FL_DRAG);
                            } else {
                                g_event = FL_MOVE;
                                w->handle(FL_MOVE);
                            }
                            g_prev_buttons = b;
                        } else if (evt.type == EVT_SCROLL) {
                            g_event = FL_MOUSEWHEEL;
                            g_event_dx = evt.data.scroll.dx;
                            g_event_dy = evt.data.scroll.dy;
                            w->handle(g_event);
                        } else if (evt.type == EVT_KEY) {
                            g_event = evt.data.key.pressed ? FL_KEYDOWN : FL_KEYUP;
                            g_event_key = evt.data.key.keycode;
                            g_event_state = evt.data.key.modifiers;
                            if (evt.data.key.text_len > 0) {
                                memcpy(g_event_text, evt.data.key.text, evt.data.key.text_len);
                                g_event_text[evt.data.key.text_len] = '\0';
                            } else {
                                g_event_text[0] = '\0';
                            }
                            w->handle(g_event);
                        } else if (evt.type == EVT_RESIZE_REQUEST) {
                            w->resize(w->x(), w->y(), evt.data.resize.w, evt.data.resize.h);
                        } else if (evt.type == EVT_CLOSE_REQUEST) {
                            g_event = FL_CLOSE;
                            w->handle(FL_CLOSE);
                        }
                    }
                }
                idx++;
            }
        }

        for (Fl_FD_Entry* e = g_fds; e; e = e->next) {
            if (e->fd >= 0) {
                if (fds[idx].revents) {
                    if (e->cb) e->cb(e->fd, e->data);
                }
                idx++;
            }
        }
    }

    if (fds) free(fds);

    if (g_idles) {
        for (Fl_Idle_Entry* e = g_idles; e; e = e->next) {
            if (e->cb) e->cb(e->data);
        }
    }

    for (Fl_Window* w = g_first_window; w; w = w->next_window()) {
        if (w->damage()) {
            w->draw();
            w->flush();
            w->clear_damage();
        }
    }

    return 1;
}

void Fl::lock() {}
void Fl::unlock() {}
void Fl::awake(void*) {}

void Fl::add_timeout(double time, Fl_Timeout_Handler cb, void* data) {
    remove_timeout(cb, data);
    Fl_Timeout_Entry* e = (Fl_Timeout_Entry*)malloc(sizeof(Fl_Timeout_Entry));
    e->time = current_time_sec() + time;
    e->cb = cb;
    e->data = data;
    e->next = g_timeouts;
    g_timeouts = e;
}

void Fl::repeat_timeout(double time, Fl_Timeout_Handler cb, void* data) {
    add_timeout(time, cb, data);
}

void Fl::remove_timeout(Fl_Timeout_Handler cb, void* data) {
    Fl_Timeout_Entry** pcur = &g_timeouts;
    while (*pcur) {
        if ((*pcur)->cb == cb && (data == NULL || (*pcur)->data == data)) {
            Fl_Timeout_Entry* e = *pcur;
            *pcur = e->next;
            free(e);
        } else {
            pcur = &(*pcur)->next;
        }
    }
}

int Fl::has_timeout(Fl_Timeout_Handler cb, void* data) {
    for (Fl_Timeout_Entry* e = g_timeouts; e; e = e->next) {
        if (e->cb == cb && (data == NULL || e->data == data)) return 1;
    }
    return 0;
}

void Fl::add_idle(Fl_Idle_Handler cb, void* data) {
    remove_idle(cb, data);
    Fl_Idle_Entry* e = (Fl_Idle_Entry*)malloc(sizeof(Fl_Idle_Entry));
    e->cb = cb;
    e->data = data;
    e->next = g_idles;
    g_idles = e;
}

void Fl::remove_idle(Fl_Idle_Handler cb, void* data) {
    Fl_Idle_Entry** pcur = &g_idles;
    while (*pcur) {
        if ((*pcur)->cb == cb && (data == NULL || (*pcur)->data == data)) {
            Fl_Idle_Entry* e = *pcur;
            *pcur = e->next;
            free(e);
        } else {
            pcur = &(*pcur)->next;
        }
    }
}

int Fl::has_idle(Fl_Idle_Handler cb, void* data) {
    for (Fl_Idle_Entry* e = g_idles; e; e = e->next) {
        if (e->cb == cb && (data == NULL || e->data == data)) return 1;
    }
    return 0;
}

void Fl::add_fd(int fd, int when, Fl_FD_Handler cb, void* data) {
    remove_fd(fd, when);
    Fl_FD_Entry* e = (Fl_FD_Entry*)malloc(sizeof(Fl_FD_Entry));
    e->fd = fd;
    e->when = when;
    e->cb = cb;
    e->data = data;
    e->next = g_fds;
    g_fds = e;
}

void Fl::add_fd(int fd, Fl_FD_Handler cb, void* data) {
    add_fd(fd, FL_READ, cb, data);
}

void Fl::remove_fd(int fd, int when) {
    (void)when;
    Fl_FD_Entry** pcur = &g_fds;
    while (*pcur) {
        if ((*pcur)->fd == fd) {
            Fl_FD_Entry* e = *pcur;
            *pcur = e->next;
            free(e);
        } else {
            pcur = &(*pcur)->next;
        }
    }
}

int Fl::event() { return g_event; }
int Fl::event_x() { return g_event_x; }
int Fl::event_y() { return g_event_y; }
int Fl::event_x_root() { return g_event_x; }
int Fl::event_y_root() { return g_event_y; }
int Fl::event_dx() { return g_event_dx; }
int Fl::event_dy() { return g_event_dy; }
int Fl::event_button() { return g_event_button; }
int Fl::event_button1() { return g_event_button == 1; }
int Fl::event_button2() { return g_event_button == 2; }
int Fl::event_button3() { return g_event_button == 3; }
int Fl::event_key() { return g_event_key; }
int Fl::event_state() { return g_event_state; }
int Fl::event_clicks() { return g_event_clicks; }
void Fl::event_clicks(int i) { g_event_clicks = i; }
int Fl::event_is_click() { return g_event_clicks > 0; }
const char* Fl::event_text() { return g_event_text; }
int Fl::event_length() { return g_event_text ? strlen(g_event_text) : 0; }
int Fl::event_inside(const Fl_Widget* w) {
    return w ? (Fl::event_x() >= w->x() && Fl::event_x() < w->x() + w->w() &&
                Fl::event_y() >= w->y() && Fl::event_y() < w->y() + w->h()) : 0;
}

void Fl::delete_widget(Fl_Widget* w) {
    if (w) delete w;
}

Fl_Widget* Fl::focus() { return g_focus_widget; }
void Fl::focus(Fl_Widget* w) { g_focus_widget = w; }
Fl_Widget* Fl::belowmouse() { return g_belowmouse_widget; }
void Fl::belowmouse(Fl_Widget* w) { g_belowmouse_widget = w; }
Fl_Widget* Fl::grabbed() { return g_grabbed_widget; }
void Fl::grabbed(Fl_Widget* w) { g_grabbed_widget = w; }
Fl_Widget* Fl::pushed() { return g_grabbed_widget; }
void Fl::pushed(Fl_Widget* w) { g_grabbed_widget = w; }

Fl_Window* Fl::first_window() { return g_first_window; }
Fl_Window* Fl::next_window(const Fl_Window* win) { return win ? win->next_window() : g_first_window; }

void Fl::redraw() {
    for (Fl_Window* w = g_first_window; w; w = w->next_window()) {
        w->damage(FL_DAMAGE_ALL);
    }
}

void Fl::flush() {
    for (Fl_Window* w = g_first_window; w; w = w->next_window()) {
        if (w->damage()) {
            w->draw();
            w->flush();
            w->clear_damage();
        }
    }
}

void Fl::scheme(const char*) {}
const char* Fl::scheme() { return "gleam"; }
void Fl::background(unsigned char, unsigned char, unsigned char) {}
void Fl::foreground(unsigned char, unsigned char, unsigned char) {}
void Fl::set_color(Fl_Color c, unsigned char r, unsigned char g, unsigned char b) {
    if (c < 256) {
        g_colormap[c] = fl_rgb_color(r, g, b);
    }
}

Fl_Color Fl::get_color(Fl_Color c) {
    if (c < 256) return g_colormap[c];
    return c;
}
int Fl::get_font_sizes(Fl_Font, int*& sizep) { static int sizes[] = {8,10,12,14,16,18,24,32,0}; sizep = sizes; return 8; }
const char* Fl::get_font_name(Fl_Font, int*) { return "Helvetica"; }
void Fl::set_font(Fl_Font, const char*) {}
Fl_Font Fl::set_fonts(const char*) { return 1; }
void Fl::option(int, int) {}

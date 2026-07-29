#include "../FL/fl_draw.H"
#include "../FL/Fl_Window.H"
#include "../FL/Fl.H"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

struct ClipRect {
    int x, y, w, h;
    ClipRect* next;
};

static ClipRect* g_clip_stack = NULL;
int fl_draw_shortcut = 0;
static Fl_Color g_current_color = FL_BLACK;
static Fl_Font g_current_font = FL_HELVETICA;
static Fl_Fontsize g_current_fontsize = 12;

static stbtt_fontinfo g_font_info;
static unsigned char* g_font_buffer = NULL;
static bool g_font_loaded = false;

static void load_font_if_needed() {
    if (g_font_loaded) return;
    const char* font_paths[] = {
        "/Library/Fonts/FiraSans-Regular.ttf",
        "/Library/Fonts/Roboto.ttf",
        "/Library/Fonts/Cantarell-Regular.ttf",
        "/usr/share/fonts/FiraSans-Regular.ttf",
        "/usr/bfonts/fonts/FiraSans-Regular.ttf",
        NULL
    };

    for (int i = 0; font_paths[i]; i++) {
        FILE* f = fopen(font_paths[i], "rb");
        if (f) {
            fseek(f, 0, SEEK_END);
            long size = ftell(f);
            fseek(f, 0, SEEK_SET);
            g_font_buffer = (unsigned char*)malloc(size);
            fread(g_font_buffer, 1, size, f);
            fclose(f);
            if (stbtt_InitFont(&g_font_info, g_font_buffer, 0)) {
                g_font_loaded = true;
                return;
            }
            free(g_font_buffer);
            g_font_buffer = NULL;
        }
    }
}

void fl_color(Fl_Color c) { g_current_color = c; }
Fl_Color fl_color() { return g_current_color; }

void fl_font(Fl_Font face, Fl_Fontsize size) {
    g_current_font = face;
    g_current_fontsize = size;
}

Fl_Font fl_font() { return g_current_font; }
Fl_Fontsize fl_size() { return g_current_fontsize; }

int fl_height() { return g_current_fontsize + 4; }
int fl_height(int, int size) { return size + 4; }
int fl_descent() { return 3; }

double fl_width(const char* text, int n) {
    if (!text) return 0;
    if (n < 0) n = strlen(text);
    load_font_if_needed();
    if (g_font_loaded) {
        float scale = stbtt_ScaleForPixelHeight(&g_font_info, (float)g_current_fontsize);
        int advance = 0, lsb = 0;
        double width = 0;
        for (int i = 0; i < n; i++) {
            stbtt_GetCodepointHMetrics(&g_font_info, text[i], &advance, &lsb);
            width += advance * scale;
        }
        return width;
    }
    return n * (g_current_fontsize * 0.6);
}

double fl_width(char c) {
    char str[2] = {c, '\0'};
    return fl_width(str, 1);
}

void fl_text_extents(const char* c, int& dx, int& dy, int& w, int& h) {
    dx = 0; dy = 0;
    w = (int)fl_width(c);
    h = fl_height();
}

void fl_push_clip(int x, int y, int w, int h) {
    ClipRect* cr = (ClipRect*)malloc(sizeof(ClipRect));
    if (g_clip_stack) {
        int x1 = x > g_clip_stack->x ? x : g_clip_stack->x;
        int y1 = y > g_clip_stack->y ? y : g_clip_stack->y;
        int x2 = (x + w) < (g_clip_stack->x + g_clip_stack->w) ? (x + w) : (g_clip_stack->x + g_clip_stack->w);
        int y2 = (y + h) < (g_clip_stack->y + g_clip_stack->h) ? (y + h) : (g_clip_stack->y + g_clip_stack->h);
        cr->x = x1; cr->y = y1;
        cr->w = x2 > x1 ? x2 - x1 : 0;
        cr->h = y2 > y1 ? y2 - y1 : 0;
    } else {
        cr->x = x; cr->y = y; cr->w = w; cr->h = h;
    }
    cr->next = g_clip_stack;
    g_clip_stack = cr;
}

void fl_pop_clip() {
    if (g_clip_stack) {
        ClipRect* tmp = g_clip_stack;
        g_clip_stack = g_clip_stack->next;
        free(tmp);
    }
}

void fl_restore_clip() {
    while (g_clip_stack) {
        fl_pop_clip();
    }
}

void fl_clip_box(int x, int y, int w, int h, int& cx, int& cy, int& cw, int& ch) {
    if (!g_clip_stack) {
        cx = x; cy = y; cw = w; ch = h;
        return;
    }
    int x1 = x > g_clip_stack->x ? x : g_clip_stack->x;
    int y1 = y > g_clip_stack->y ? y : g_clip_stack->y;
    int x2 = (x + w) < (g_clip_stack->x + g_clip_stack->w) ? (x + w) : (g_clip_stack->x + g_clip_stack->w);
    int y2 = (y + h) < (g_clip_stack->y + g_clip_stack->h) ? (y + h) : (g_clip_stack->y + g_clip_stack->h);
    if (x2 > x1 && y2 > y1) {
        cx = x1; cy = y1; cw = x2 - x1; ch = y2 - y1;
    } else {
        cx = x; cy = y; cw = 0; ch = 0;
    }
}

int fl_not_clipped(int x, int y, int w, int h) {
    if (!g_clip_stack) return 1;
    if (x + w <= g_clip_stack->x || x >= g_clip_stack->x + g_clip_stack->w ||
        y + h <= g_clip_stack->y || y >= g_clip_stack->y + g_clip_stack->h)
        return 0;
    return 1;
}

static inline void draw_pixel_raw(uint32_t* buf, int win_w, int win_h, int px, int py, uint32_t color) {
    if (px < 0 || px >= win_w || py < 0 || py >= win_h) return;
    if (g_clip_stack) {
        if (px < g_clip_stack->x || px >= g_clip_stack->x + g_clip_stack->w ||
            py < g_clip_stack->y || py >= g_clip_stack->y + g_clip_stack->h)
            return;
    }
    buf[py * win_w + px] = color | 0xFF000000;
}

void fl_rectf(int x, int y, int w, int h, Fl_Color c) {
    Fl_Window* win = Fl::first_window();
    if (!win || !win->buffer()) return;
    uint32_t* buf = win->buffer();
    int win_w = win->w();
    int win_h = win->h();

    uint32_t argb = fl_color_to_rgb(c) | 0xFF000000;
    for (int r = y; r < y + h; r++) {
        for (int col = x; col < x + w; col++) {
            draw_pixel_raw(buf, win_w, win_h, col, r, argb);
        }
    }
}

void fl_rectf(int x, int y, int w, int h) { fl_rectf(x, y, w, h, g_current_color); }
void fl_rectf(int x, int y, int w, int h, unsigned char r, unsigned char g, unsigned char b) {
    fl_rectf(x, y, w, h, fl_rgb_color(r, g, b));
}

void fl_rect(int x, int y, int w, int h, Fl_Color c) {
    fl_rectf(x, y, w, 1, c);
    fl_rectf(x, y + h - 1, w, 1, c);
    fl_rectf(x, y, 1, h, c);
    fl_rectf(x + w - 1, y, 1, h, c);
}

void fl_rect(int x, int y, int w, int h) { fl_rect(x, y, w, h, g_current_color); }

void fl_line(int x1, int y1, int x2, int y2) {
    int dx = abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
    int dy = -abs(y2 - y1), sy = y1 < y2 ? 1 : -1;
    int err = dx + dy, e2;

    Fl_Window* win = Fl::first_window();
    if (!win || !win->buffer()) return;
    uint32_t* buf = win->buffer();
    int win_w = win->w(), win_h = win->h();
    uint32_t argb = g_current_color | 0xFF000000;

    while (1) {
        draw_pixel_raw(buf, win_w, win_h, x1, y1, argb);
        if (x1 == x2 && y1 == y2) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x1 += sx; }
        if (e2 <= dx) { err += dx; y1 += sy; }
    }
}

void fl_xyline(int x, int y, int x1) { fl_line(x, y, x1, y); }
void fl_xyline(int x, int y, int x1, int y2) { fl_line(x, y, x1, y); fl_line(x1, y, x1, y2); }
void fl_yxline(int x, int y, int y1) { fl_line(x, y, x, y1); }
void fl_yxline(int x, int y, int y1, int x2) { fl_line(x, y, x, y1); fl_line(x, y1, x2, y1); }

void fl_point(int x, int y) {
    Fl_Window* win = Fl::first_window();
    if (win && win->buffer()) draw_pixel_raw(win->buffer(), win->w(), win->h(), x, y, g_current_color);
}

void fl_polygon(int x0, int y0, int x1, int y1, int x2, int y2) {
    fl_line(x0, y0, x1, y1); fl_line(x1, y1, x2, y2); fl_line(x2, y2, x0, y0);
}
void fl_polygon(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3) {
    fl_line(x0, y0, x1, y1); fl_line(x1, y1, x2, y2); fl_line(x2, y2, x3, y3); fl_line(x3, y3, x0, y0);
}
void fl_loop(int x0, int y0, int x1, int y1, int x2, int y2) { fl_polygon(x0, y0, x1, y1, x2, y2); }
void fl_loop(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3) { fl_polygon(x0, y0, x1, y1, x2, y2, x3, y3); }
void fl_arc(int x, int y, int w, int h, double, double) { fl_rect(x, y, w, h); }
void fl_pie(int x, int y, int w, int h, double, double) { fl_rectf(x, y, w, h); }

void fl_draw(const char* str, int x, int y) {
    if (!str) return;
    fl_draw(str, strlen(str), x, y);
}

void fl_draw(const char* str, int n, int x, int y) {
    if (!str || n <= 0) return;
    Fl_Window* win = Fl::first_window();
    if (!win || !win->buffer()) return;
    uint32_t* buf = win->buffer();
    int win_w = win->w(), win_h = win->h();
    uint32_t real_color = fl_color_to_rgb(g_current_color);
    uint32_t color_argb = real_color | 0xFF000000;

    load_font_if_needed();
    if (g_font_loaded) {
        float scale = stbtt_ScaleForPixelHeight(&g_font_info, (float)g_current_fontsize);
        int ascent = 0, descent = 0, linegap = 0;
        stbtt_GetFontVMetrics(&g_font_info, &ascent, &descent, &linegap);
        int baseline = (int)(ascent * scale);

        int cur_x = x;
        int cur_y = y - baseline;

        for (int i = 0; i < n; i++) {
            int c = (unsigned char)str[i];
            int c_x1, c_y1, c_x2, c_y2;
            stbtt_GetCodepointBitmapBox(&g_font_info, c, scale, scale, &c_x1, &c_y1, &c_x2, &c_y2);

            int width = c_x2 - c_x1;
            int height = c_y2 - c_y1;

            if (width > 0 && height > 0) {
                unsigned char* bmp = (unsigned char*)malloc(width * height);
                stbtt_MakeCodepointBitmap(&g_font_info, bmp, width, height, width, scale, scale, c);

                for (int row = 0; row < height; row++) {
                    for (int col = 0; col < width; col++) {
                        unsigned char alpha = bmp[row * width + col];
                        if (alpha > 0) {
                            int px = cur_x + c_x1 + col;
                            int py = cur_y + baseline + c_y1 + row;
                            if (alpha == 255) {
                                draw_pixel_raw(buf, win_w, win_h, px, py, color_argb);
                            } else {
                                unsigned char r = (real_color >> 16) & 0xFF;
                                unsigned char g = (real_color >> 8) & 0xFF;
                                unsigned char b = real_color & 0xFF;
                                uint32_t blended = ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;
                                draw_pixel_raw(buf, win_w, win_h, px, py, blended);
                            }
                        }
                    }
                }
                free(bmp);
            }

            int advance = 0, lsb = 0;
            stbtt_GetCodepointHMetrics(&g_font_info, c, &advance, &lsb);
            cur_x += (int)(advance * scale);
        }
    }
}

void fl_draw(const char* str, int x, int y, int w, int h, Fl_Align align, Fl_Image*, int) {
    if (!str) return;
    int tw = (int)fl_width(str);
    int th = fl_height();
    int tx = x, ty = y + (h + th) / 2 - 2;

    if (align & FL_ALIGN_RIGHT) tx = x + w - tw;
    else if (align & FL_ALIGN_CENTER) tx = x + (w - tw) / 2;

    fl_draw(str, strlen(str), tx, ty);
}

void fl_draw_image(const unsigned char* buf_data, int X, int Y, int W, int H, int D, int L) {
    Fl_Window* win = Fl::first_window();
    if (!win || !win->buffer() || !buf_data) return;
    uint32_t* win_buf = win->buffer();
    int win_w = win->w(), win_h = win->h();
    int stride = L > 0 ? L : W * D;

    for (int r = 0; r < H; r++) {
        const unsigned char* row_ptr = buf_data + r * stride;
        for (int col = 0; col < W; col++) {
            unsigned char red = row_ptr[col * D];
            unsigned char green = row_ptr[col * D + 1];
            unsigned char blue = row_ptr[col * D + 2];
            uint32_t argb = ((uint32_t)red << 16) | ((uint32_t)green << 8) | (uint32_t)blue;
            draw_pixel_raw(win_buf, win_w, win_h, X + col, Y + r, argb);
        }
    }
}

void fl_draw_image_mono(const unsigned char* buf, int X, int Y, int W, int H, int D, int L) {
    fl_draw_image(buf, X, Y, W, H, D, L);
}

void fl_measure(const char* str, int& w, int& h, int) {
    w = (int)fl_width(str);
    h = fl_height();
}

void fl_draw_box(Fl_Boxtype b, int x, int y, int w, int h, Fl_Color c) {
    if (b == FL_NO_BOX) return;

    fl_rectf(x, y, w, h, c);

    if (b == FL_UP_BOX || b == FL_THIN_UP_BOX) {
        fl_rectf(x, y, w, 1, FL_WHITE);
        fl_rectf(x, y, 1, h, FL_WHITE);
        fl_rectf(x, y + h - 1, w, 1, FL_DARKGRAY);
        fl_rectf(x + w - 1, y, 1, h, FL_DARKGRAY);
    } else if (b == FL_DOWN_BOX || b == FL_THIN_DOWN_BOX) {
        fl_rectf(x, y, w, 1, FL_DARKGRAY);
        fl_rectf(x, y, 1, h, FL_DARKGRAY);
        fl_rectf(x, y + h - 1, w, 1, FL_WHITE);
        fl_rectf(x + w - 1, y, 1, h, FL_WHITE);
    } else if (b == FL_BORDER_BOX || b == FL_FRAME_BOX) {
        fl_rect(x, y, w, h, FL_BLACK);
    }
}

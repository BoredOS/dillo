#ifndef FL_DRAW_H
#define FL_DRAW_H

#include "Enumerations.H"
#include "Fl_Image.H"
#include "fl_utf8.h"

class Fl_Widget;

#define FL_NUM_RED 256
#define FL_NUM_GREEN 256
#define FL_NUM_BLUE 256

extern int fl_draw_shortcut;

void fl_color(Fl_Color c);
Fl_Color fl_color();

void fl_font(Fl_Font face, Fl_Fontsize size);
Fl_Font fl_font();
Fl_Fontsize fl_size();

int fl_height();
int fl_height(int font, int size);
int fl_descent();
double fl_width(const char* text, int n = -1);
double fl_width(char c);

void fl_text_extents(const char* c, int& dx, int& dy, int& w, int& h);
void fl_restore_clip();
inline Fl_Color fl_color_cube(int r, int g, int b) { return fl_rgb_color(r, g, b); }
inline Fl_Color fl_contrast(Fl_Color c1, Fl_Color) { return c1; }
inline Fl_Color fl_lighter(Fl_Color c) { return c; }

void fl_push_clip(int x, int y, int w, int h);
void fl_pop_clip();
void fl_clip_box(int x, int y, int w, int h, int& cx, int& cy, int& cw, int& ch);
int fl_not_clipped(int x, int y, int w, int h);

void fl_point(int x, int y);
void fl_line(int x1, int y1, int x2, int y2);
void fl_xyline(int x, int y, int x1);
void fl_xyline(int x, int y, int x1, int y2);
void fl_yxline(int x, int y, int y1);
void fl_yxline(int x, int y, int y1, int x2);

void fl_rect(int x, int y, int w, int h);
void fl_rect(int x, int y, int w, int h, Fl_Color c);
void fl_rectf(int x, int y, int w, int h);
void fl_rectf(int x, int y, int w, int h, Fl_Color c);
void fl_rectf(int x, int y, int w, int h, unsigned char r, unsigned char g, unsigned char b);

void fl_polygon(int x0, int y0, int x1, int y1, int x2, int y2);
void fl_polygon(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3);

void fl_loop(int x0, int y0, int x1, int y1, int x2, int y2);
void fl_loop(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3);

void fl_arc(int x, int y, int w, int h, double a1, double a2);
void fl_pie(int x, int y, int w, int h, double a1, double a2);

void fl_draw(const char* str, int x, int y);
void fl_draw(const char* str, int n, int x, int y);
void fl_draw(const char* str, int x, int y, int w, int h, Fl_Align align, Fl_Image* img = 0, int draw_symbols = 1);

void fl_draw_image(const unsigned char* buf, int X, int Y, int W, int H, int D = 3, int L = 0);
void fl_draw_image_mono(const unsigned char* buf, int X, int Y, int W, int H, int D = 1, int L = 0);

void fl_measure(const char* str, int& w, int& h, int draw_symbols = 1);

void fl_draw_box(Fl_Boxtype b, int x, int y, int w, int h, Fl_Color c);

inline void fl_begin_offscreen(Fl_Offscreen) {}
inline void fl_end_offscreen() {}
inline void fl_push_matrix() {}
inline void fl_pop_matrix() {}
inline void fl_copy_offscreen(int, int, int, int, Fl_Offscreen, int, int) {}
inline void fl_delete_offscreen(Fl_Offscreen) {}
inline Fl_Offscreen fl_create_offscreen(int, int) { return 0; }
inline void fl_scroll(int, int, int, int, int, int, void(*)(void*,int,int,int,int), void*) {}
inline void fl_cursor(Fl_Cursor) {}
inline void fl_line_style(int, int = 0, char* = 0) {}
inline void fl_begin_polygon() {}
inline void fl_begin_complex_polygon() {}
inline void fl_begin_loop() {}
inline void fl_vertex(double, double) {}
inline void fl_end_polygon() {}
inline void fl_end_complex_polygon() {}
inline void fl_end_loop() {}
inline void draw_outside_label(const Fl_Widget&) {}

#endif // FL_DRAW_H

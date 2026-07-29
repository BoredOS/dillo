#ifndef FL_H
#define FL_H

#include "Enumerations.H"
#include <stddef.h>

class Fl_Widget;
class Fl_Window;

typedef void (*Fl_Timeout_Handler)(void* data);
typedef void (*Fl_FD_Handler)(int fd, void* data);
typedef void (*Fl_Idle_Handler)(void* data);
typedef void (*Fl_Callback)(Fl_Widget* w, void* data);

class Fl {
public:
    static int run();
    static int check();
    static int wait(double time = 10.0);
    static void lock();
    static void unlock();
    static void awake(void* msg = NULL);
    static Fl_Widget* pushed();
    static void pushed(Fl_Widget* w);

    static void add_timeout(double time, Fl_Timeout_Handler cb, void* data = NULL);
    static void repeat_timeout(double time, Fl_Timeout_Handler cb, void* data = NULL);
    static void remove_timeout(Fl_Timeout_Handler cb, void* data = NULL);
    static int has_timeout(Fl_Timeout_Handler cb, void* data = NULL);

    static void add_idle(Fl_Idle_Handler cb, void* data = NULL);
    static void remove_idle(Fl_Idle_Handler cb, void* data = NULL);
    static int has_idle(Fl_Idle_Handler cb, void* data = NULL);

    static void add_fd(int fd, int when, Fl_FD_Handler cb, void* data = NULL);
    static void add_fd(int fd, Fl_FD_Handler cb, void* data = NULL);
    static void remove_fd(int fd, int when = -1);

    static int event();
    static int event_x();
    static int event_y();
    static int event_x_root();
    static int event_y_root();
    static int event_dx();
    static int event_dy();
    static int event_button();
    static int event_button1();
    static int event_button2();
    static int event_button3();
    static int event_key();
    static int event_state();
    static int event_state(int mask) { return Fl::event_state() & mask; }
    static int event_shift() { return Fl::event_state() & FL_SHIFT; }
    static int event_ctrl() { return Fl::event_state() & FL_CTRL; }
    static int event_clicks();
    static void event_clicks(int i);
    static int event_is_click();
    static const char* event_text();
    static int event_length();

    static Fl_Widget* focus();
    static void focus(Fl_Widget* w);
    static Fl_Widget* belowmouse();
    static void belowmouse(Fl_Widget* w);
    static Fl_Widget* grabbed();
    static void grabbed(Fl_Widget* w);

    static Fl_Window* first_window();
    static Fl_Window* next_window(const Fl_Window* win);
    static void redraw();
    static void flush();

    static void scheme(const char* s);
    static const char* scheme();
    enum { OPTION_SHOW_TOOLTIPS = 1 };
    static void background(unsigned char r, unsigned char g, unsigned char b);
    static void foreground(unsigned char r, unsigned char g, unsigned char b);
    static void set_color(Fl_Color i, unsigned char r, unsigned char g, unsigned char b);
    static void set_color(Fl_Color i, uint32_t c) { (void)i; (void)c; }
    static Fl_Color get_color(Fl_Color i);

    static int get_font_sizes(Fl_Font f, int*& sizep);
    static const char* get_font_name(Fl_Font f, int* attributes = NULL);
    static void set_font(Fl_Font f, const char* name);
    static void set_font(Fl_Font f, int size) { (void)f; (void)size; }
    static Fl_Font set_fonts(const char* xstarname = NULL);

    static void option(int opt, int val);

    static int visible_focus() { return 1; }
    static void visible_focus(int) {}
    static int event_inside(const Fl_Widget* w);

    static int scrollbar_size() { return 16; }
    static void scrollbar_size(int) {}
    static int w() { return 1024; }
    static int h() { return 768; }

    static void get_mouse(int& x, int& y) { x = Fl::event_x(); y = Fl::event_y(); }
    static int box_dx(Fl_Boxtype) { return 2; }
    static int box_dy(Fl_Boxtype) { return 2; }
    static int box_dw(Fl_Boxtype) { return 4; }
    static int box_dh(Fl_Boxtype) { return 4; }
    static Fl_Boxtype get_boxtype(Fl_Boxtype b) { return b; }
    static void set_boxtype(Fl_Boxtype, Fl_Boxtype, int, int, int, int) {}
    static void delete_widget(Fl_Widget* w);
    static void paste(Fl_Widget&, int) {}
    static void screen_dpi(float& h, float& v) { h = 96.0f; v = 96.0f; }
    static void screen_xywh(int& x, int& y, int& w, int& h) { x = 0; y = 0; w = 1024; h = 768; }
    static void copy(const char*, int, int) {}
    static void dnd_text_ops(int) {}
    static int api_version() { return 10300; }

    static void set_labeltype(int, Fl_Label_Draw_F, Fl_Label_Measure_F) {}
};

#endif // FL_H

#ifndef FL_MENU_WINDOW_H
#define FL_MENU_WINDOW_H
#include "Fl_Window.H"
class Fl_Menu_Window : public Fl_Window {
public:
    Fl_Menu_Window(int w, int h, const char* title = 0) : Fl_Window(w, h, title) {}
    Fl_Menu_Window(int x, int y, int w, int h, const char* title = 0) : Fl_Window(x, y, w, h, title) {}
    void set_override() {}
};
#endif // FL_MENU_WINDOW_H

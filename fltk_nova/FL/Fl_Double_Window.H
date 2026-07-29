#ifndef FL_DOUBLE_WINDOW_H
#define FL_DOUBLE_WINDOW_H
#include "Fl_Window.H"
class Fl_Double_Window : public Fl_Window {
public:
    Fl_Double_Window(int w, int h, const char* title = 0) : Fl_Window(w, h, title) {}
    Fl_Double_Window(int x, int y, int w, int h, const char* title = 0) : Fl_Window(x, y, w, h, title) {}
};
#endif // FL_DOUBLE_WINDOW_H

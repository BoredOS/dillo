#ifndef FL_RETURN_BUTTON_H
#define FL_RETURN_BUTTON_H
#include "Fl_Button.H"
class Fl_Return_Button : public Fl_Button {
public:
    Fl_Return_Button(int x, int y, int w, int h, const char* label = 0)
        : Fl_Button(x, y, w, h, label) {}
};
#endif // FL_RETURN_BUTTON_H

#ifndef FL_ROUND_BUTTON_H
#define FL_ROUND_BUTTON_H
#include "Fl_Button.H"
class Fl_Round_Button : public Fl_Button {
public:
    Fl_Round_Button(int x, int y, int w, int h, const char* label = 0)
        : Fl_Button(x, y, w, h, label) {}
};
#endif // FL_ROUND_BUTTON_H

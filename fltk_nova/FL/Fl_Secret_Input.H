#ifndef FL_SECRET_INPUT_H
#define FL_SECRET_INPUT_H
#include "Fl_Input.H"
class Fl_Secret_Input : public Fl_Input {
public:
    Fl_Secret_Input(int x, int y, int w, int h, const char* label = 0)
        : Fl_Input(x, y, w, h, label) { type(FL_SECRET_INPUT); }
};
#endif // FL_SECRET_INPUT_H

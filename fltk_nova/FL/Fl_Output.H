#ifndef FL_OUTPUT_H
#define FL_OUTPUT_H
#include "Fl_Input.H"
class Fl_Output : public Fl_Input {
public:
    Fl_Output(int x, int y, int w, int h, const char* label = 0)
        : Fl_Input(x, y, w, h, label) {}
};
#endif // FL_OUTPUT_H

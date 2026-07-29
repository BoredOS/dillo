#ifndef FL_CHECK_BUTTON_H
#define FL_CHECK_BUTTON_H

#include "Fl_Button.H"
#include "fl_draw.H"

class Fl_Check_Button : public Fl_Button {
public:
    Fl_Check_Button(int x, int y, int w, int h, const char* label = 0);
    virtual void draw() override { fl_draw_box(FL_DOWN_BOX, x(), y(), w(), h(), color()); }
    virtual int handle(int) override { return 0; }
};

#endif // FL_CHECK_BUTTON_H

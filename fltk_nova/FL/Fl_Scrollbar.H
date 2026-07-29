#ifndef FL_SCROLLBAR_H
#define FL_SCROLLBAR_H

#include "Fl_Widget.H"
#include "fl_draw.H"

class Fl_Scrollbar : public Fl_Widget {
private:
    int value_;
    int linesize_;

public:
    Fl_Scrollbar(int x, int y, int w, int h, const char* label = 0)
        : Fl_Widget(x, y, w, h, label), value_(0), linesize_(16) {}

    int value() const { return value_; }
    int value(int v, int, int, int) { value_ = v; redraw(); return value_; }
    void linesize(int i) { linesize_ = i; }
    int linesize() const { return linesize_; }

    virtual void draw() override { fl_draw_box(FL_UP_BOX, x(), y(), w(), h(), color()); }
    virtual int handle(int) override { return 0; }
};

#endif // FL_SCROLLBAR_H

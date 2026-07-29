#ifndef FL_WIZARD_H
#define FL_WIZARD_H

#include "Fl_Group.H"

class Fl_Wizard : public Fl_Group {
private:
    Fl_Widget* value_;

public:
    Fl_Wizard(int x, int y, int w, int h, const char* label = 0)
        : Fl_Group(x, y, w, h, label), value_(NULL) {}

    void next() {}
    void prev() {}
    Fl_Widget* value() const { return value_; }
    void value(Fl_Widget* w) { value_ = w; }
};

#endif // FL_WIZARD_H

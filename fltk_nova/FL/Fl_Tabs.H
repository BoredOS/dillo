#ifndef FL_TABS_H
#define FL_TABS_H

#include "Fl_Group.H"

class Fl_Tabs : public Fl_Group {
private:
    Fl_Widget* value_;

public:
    Fl_Tabs(int x, int y, int w, int h, const char* label = 0);
    virtual void draw() override;
    virtual int handle(int event) override;

    Fl_Widget* value() const { return value_; }
    int value(Fl_Widget* w);
    Fl_Widget* push() const { return value_; }
    int push(Fl_Widget* w) { return value(w); }
    Fl_Widget* which(int event_x, int event_y);
};

#endif // FL_TABS_H

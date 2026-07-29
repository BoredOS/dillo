#ifndef FL_BOX_H
#define FL_BOX_H

#include "Fl_Widget.H"

class Fl_Box : public Fl_Widget {
public:
    Fl_Box(int x, int y, int w, int h, const char* label = 0);
    Fl_Box(Fl_Boxtype b, int x, int y, int w, int h, const char* label);
    virtual void draw() override;
};

#endif // FL_BOX_H

#ifndef FL_SCROLL_H
#define FL_SCROLL_H

#include "Fl_Group.H"

class Fl_Scroll : public Fl_Group {
private:
    int xposition_, yposition_;
    int scrollbar_size_;

public:
    enum {
        HORIZONTAL = 1,
        VERTICAL = 2,
        BOTH = 3,
        ALWAYS_ON = 4,
        HORIZONTAL_ALWAYS = 5,
        VERTICAL_ALWAYS = 6,
        BOTH_ALWAYS = 7
    };

    Fl_Scroll(int x, int y, int w, int h, const char* label = 0);
    virtual void draw() override;
    virtual int handle(int event) override;

    int xposition() const { return xposition_; }
    int yposition() const { return yposition_; }
    void scroll_to(int x, int y);
    int scrollbar_size() const { return scrollbar_size_; }
    void scrollbar_size(int size) { scrollbar_size_ = size; }
};

#endif // FL_SCROLL_H

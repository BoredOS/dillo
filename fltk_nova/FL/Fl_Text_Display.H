#ifndef FL_TEXT_DISPLAY_H
#define FL_TEXT_DISPLAY_H

#include "Fl_Group.H"
#include "Fl_Text_Buffer.H"

class Fl_Text_Display : public Fl_Group {
private:
    Fl_Text_Buffer* buffer_;

public:
    enum { WRAP_AT_BOUNDS = 1 };

    Fl_Text_Display(int x, int y, int w, int h, const char* label = 0)
        : Fl_Group(x, y, w, h, label), buffer_(NULL) {}

    Fl_Text_Buffer* buffer() const { return buffer_; }
    void buffer(Fl_Text_Buffer* buf) { buffer_ = buf; }
    void buffer(Fl_Text_Buffer& buf) { buffer_ = &buf; }

    void wrap_mode(int, int) {}
};

#endif // FL_TEXT_DISPLAY_H

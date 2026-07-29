#ifndef FL_PACK_H
#define FL_PACK_H

#include "Fl_Group.H"

class Fl_Pack : public Fl_Group {
private:
    int type_;
    int spacing_;

public:
    enum { NORMAL = 0, HORIZONTAL = 1, VERTICAL = 0 };

    Fl_Pack(int x, int y, int w, int h, const char* label = 0)
        : Fl_Group(x, y, w, h, label), type_(VERTICAL), spacing_(0) {}

    int type() const { return type_; }
    void type(int t) { type_ = t; }

    int spacing() const { return spacing_; }
    void spacing(int s) { spacing_ = s; }

    virtual void draw() override {
        int tx = x();
        int ty = y();
        int is_horizontal = (type_ == HORIZONTAL);

        for (int i = 0; i < children(); i++) {
            Fl_Widget* c = child(i);
            if (!c || !c->visible()) continue;
            if (is_horizontal) {
                c->position(tx, y());
                c->size(c->w(), h());
                tx += c->w() + spacing_;
            } else {
                c->position(x(), ty);
                c->size(w(), c->h());
                ty += c->h() + spacing_;
            }
        }
        Fl_Group::draw();
    }
};

#endif // FL_PACK_H

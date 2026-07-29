#ifndef FL_INPUT_H
#define FL_INPUT_H

#include "Fl_Widget.H"

class Fl_Input : public Fl_Widget {
protected:
    char* value_;
    int size_;
    int capacity_;
    int position_;
    int mark_;
    int shortcut_;

    void expand(int new_cap);

public:
    Fl_Input(int x, int y, int w, int h, const char* label = 0);
    virtual ~Fl_Input();

    const char* value() const { return value_ ? value_ : ""; }
    int value(const char* text);
    int value(const char* text, int len);

    void input_type(int t) { type((unsigned char)t); }
    int input_type() const { return type(); }
    void maximum_size(int) {}
    void cursor_color(Fl_Color) {}

    int size() const { return value_ ? strlen(value_) : 0; }

    int position() const { return position_; }
    int position(int p);
    int position(int p, int m) { position(p); mark_ = m; return 0; }
    int mark() const { return mark_; }
    int mark(int m);
    int select(int p, int m);

    int insert(const char* t, int l = 0);
    int replace(int b, int e, const char* text, int ilen = 0);
    int cut();
    int cut(int n);
    int cut(int a, int b);

    virtual void draw() override;
    virtual int handle(int event) override;
};

#endif // FL_INPUT_H

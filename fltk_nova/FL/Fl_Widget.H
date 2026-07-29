#ifndef FL_WIDGET_H
#define FL_WIDGET_H

#include "Enumerations.H"
#include "Fl.H"
#include <stdlib.h>
#include <string.h>

class Fl_Group;
class Fl_Window;

class Fl_Widget {
private:
    int x_, y_, w_, h_;
    const char* label_;
    const char* tooltip_;
    Fl_Group* parent_;
    Fl_Callback callback_;
    void* user_data_;
    Fl_Boxtype box_;
    Fl_Color color_;
    Fl_Color labelcolor_;
    Fl_Font labelfont_;
    Fl_Fontsize labelsize_;
    Fl_Align align_;
    unsigned char type_;
    unsigned int flags_;

public:
    Fl_Widget(int x, int y, int w, int h, const char* label = 0);
    virtual ~Fl_Widget();

    int x() const { return x_; }
    int y() const { return y_; }
    int w() const { return w_; }
    int h() const { return h_; }

    virtual void resize(int x, int y, int w, int h);
    void position(int x, int y) { resize(x, y, w_, h_); }
    void size(int w, int h) { resize(x_, y_, w, h); }

    const char* label() const { return label_; }
    void label(const char* l);
    void copy_label(const char* l) { label(l); }

    const char* tooltip() const { return tooltip_; }
    void tooltip(const char* t) {
        if (tooltip_) free((void*)tooltip_);
        tooltip_ = t ? strdup(t) : NULL;
    }

    unsigned char type() const { return type_; }
    void type(unsigned char t) { type_ = t; }

    void clear_visible_focus() {}

    Fl_Group* parent() const { return parent_; }
    void parent(Fl_Group* p) { parent_ = p; }

    Fl_Callback callback() const { return callback_; }
    void callback(Fl_Callback c, void* p = 0) { callback_ = c; user_data_ = p; }
    void callback(void (*c)(Fl_Widget*, long), long p = 0) { callback_ = (Fl_Callback)c; user_data_ = (void*)p; }
    void* user_data() const { return user_data_; }
    void user_data(void* p) { user_data_ = p; }
    void shortcut(int) {}
    void do_callback() { if (callback_) callback_(this, user_data_); }
    void do_callback(Fl_Widget* o, void* arg) { if (callback_) callback_(o, arg); }

    Fl_Boxtype box() const { return box_; }
    void box(Fl_Boxtype b) { box_ = b; }

    void draw_box();
    void draw_box(Fl_Boxtype b, Fl_Color c);
    void draw_box(Fl_Boxtype b, int x, int y, int w, int h, Fl_Color c);
    void draw_focus() {}

    Fl_Color color() const { return color_; }
    void color(Fl_Color c) { color_ = c; }

    Fl_Color selection_color() const { return FL_SELECTION_COLOR; }
    void selection_color(Fl_Color) {}

    unsigned char when() const { return FL_WHEN_RELEASE; }
    void when(unsigned char) {}

    void clear_changed() {}
    void set_changed() {}
    int changed() const { return 0; }

    Fl_Color labelcolor() const { return labelcolor_; }
    void labelcolor(Fl_Color c) { labelcolor_ = c; }

    Fl_Color textcolor() const { return labelcolor_; }
    void textcolor(Fl_Color c) { labelcolor(c); }

    Fl_Font labelfont() const { return labelfont_; }
    void labelfont(Fl_Font f) { labelfont_ = f; }

    Fl_Font textfont() const { return labelfont_; }
    void textfont(Fl_Font f) { labelfont(f); }

    Fl_Fontsize labelsize() const { return labelsize_; }
    void labelsize(Fl_Fontsize s) { labelsize_ = s; }

    void labeltype(int t) { (void)t; }
    void clear_flag(int) {}

    Fl_Fontsize textsize() const { return labelsize_; }
    void textsize(Fl_Fontsize s) { labelsize(s); }

    Fl_Align align() const { return align_; }
    void align(Fl_Align a) { align_ = a; }

    virtual void draw() = 0;
    virtual int handle(int event);

    void redraw();
    void damage(unsigned char c);
    void damage(unsigned char c, int, int, int, int) { damage(c); }
    unsigned char damage() const;
    void clear_damage();
    void measure_label(int& w, int& h);

    virtual void show();
    virtual void hide();
    int visible() const;
    int visible_r() const;

    void activate();
    void deactivate();
    int active() const;
    int active_r() const;
    int take_focus() { Fl::focus(this); return 1; }

    Fl_Window* window() const;
    virtual class Fl_Group* as_group() { return 0; }
};

#endif // FL_WIDGET_H

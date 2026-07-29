#ifndef FL_BROWSER_H
#define FL_BROWSER_H

#include "Fl_Group.H"
#include "fl_draw.H"

class Fl_Browser_ {
public:
    enum { VERTICAL_ALWAYS = 1 };
    static int incr_height() { return 16; }
};

class Fl_Browser : public Fl_Group {
public:
    char** items_;
    int num_items_;
    int capacity_;
    int selected_;

    Fl_Browser(int x, int y, int w, int h, const char* label = 0);
    virtual ~Fl_Browser();

    void add(const char* newtext, void* d = 0);
    void insert(int line, const char* newtext, void* d = 0);
    void remove(int line);
    void clear();

    int size() const { return num_items_; }
    const char* text(int line) const;
    void text(int line, const char* newtext);

    int value() const { return selected_; }
    void value(int line);

    int selected(int line) const { return line == selected_; }
    void select(int line, int val = 1) { if (val) selected_ = line; else if (selected_ == line) selected_ = 0; }

    int full_height() const { return num_items_ * 16; }
    int incr_height() const { return 16; }
    void* item_first() const { return num_items_ > 0 ? (void*)items_[0] : NULL; }
    int item_width(void* item) const { return (int)fl_width((const char*)item); }
    void* item_next(void*) const { return NULL; }
    void column_widths(const int*) {}
    void column_char(char) {}
    void topline(int) {}
    void hide(int) {}
    void has_scrollbar(int) {}

    virtual void draw() override;
    virtual int handle(int event) override;
};

class Fl_Hold_Browser : public Fl_Browser {
public:
    Fl_Hold_Browser(int x, int y, int w, int h, const char* label = 0)
        : Fl_Browser(x, y, w, h, label) {}
};

#endif // FL_BROWSER_H

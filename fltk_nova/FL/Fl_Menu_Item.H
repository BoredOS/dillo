#ifndef FL_MENU_ITEM_H
#define FL_MENU_ITEM_H

#include "Fl_Widget.H"

#define FL_MENU_INACTIVE    1
#define FL_MENU_TOGGLE      2
#define FL_MENU_VALUE       4
#define FL_MENU_RADIO       8
#define FL_MENU_INVISIBLE   16
#define FL_SUBMENU_POINTER  32
#define FL_SUBMENU          64
#define FL_MENU_DIVIDER     128
#define FL_MENU_HORIZONTAL  256

struct Fl_Menu_Item {
    const char* text;
    int shortcut_;
    Fl_Callback callback_;
    void* user_data_;
    int flags;
    unsigned char labeltype_;
    Fl_Font labelfont_;
    Fl_Fontsize labelsize_;
    Fl_Color labelcolor_;

    const Fl_Menu_Item* next(int n = 1) const;
    const Fl_Menu_Item* find_shortcut(int* ip = 0, bool require_alt = false) const;
    int size() const;
    int submenu() const { return flags & (FL_SUBMENU | FL_SUBMENU_POINTER); }
    const char* label() const { return text; }
    void label(const char* str) { text = str; }
    void label(int type, const char* str) { (void)type; text = str; }

    void* user_data() const { return user_data_; }
    void user_data(void* p) { user_data_ = p; }

    Fl_Callback callback() const { return callback_; }
    void callback(Fl_Callback c, void* p = 0) { callback_ = c; user_data_ = p; }
    void do_callback(Fl_Widget* w) const { if (callback_) callback_(w, user_data_); }
    const Fl_Menu_Item* popup(int x, int y, const char* = 0, const Fl_Menu_Item* = 0, const class Fl_Menu_* = 0) const { (void)x; (void)y; return this; }

    void show() { flags &= ~FL_MENU_INVISIBLE; }
    void hide() { flags |= FL_MENU_INVISIBLE; }
    int visible() const { return !(flags & FL_MENU_INVISIBLE); }
    void activate() { flags &= ~FL_MENU_INACTIVE; }
    void deactivate() { flags |= FL_MENU_INACTIVE; }
    int active() const { return !(flags & FL_MENU_INACTIVE); }
    int value() const { return (flags & FL_MENU_VALUE) != 0; }
    void set() { flags |= FL_MENU_VALUE; }
    void clear() { flags &= ~FL_MENU_VALUE; }
    void setonly();
};

class Fl_Menu_ : public Fl_Widget {
protected:
    const Fl_Menu_Item* menu_;
    int alloc_;
    int value_;

public:
    Fl_Menu_(int x, int y, int w, int h, const char* label = 0);
    virtual ~Fl_Menu_();

    const Fl_Menu_Item* menu() const { return menu_; }
    void menu(const Fl_Menu_Item* m);

    int add(const char* label, int shortcut, Fl_Callback cb, void* user_data = 0, int flags = 0);
    int add(const char* label, const char* shortcut, Fl_Callback cb, void* user_data = 0, int flags = 0);
    void clear();
    int size() const;

    int value() const { return value_; }
    int value(int i);
    int value(const Fl_Menu_Item* m);
    const Fl_Menu_Item* mvalue() const;
    const char* text() const;
    const char* text(int i) const;

    virtual int handle(int event) override;
};

class Fl_Menu_Bar : public Fl_Menu_ {
public:
    Fl_Menu_Bar(int x, int y, int w, int h, const char* label = 0);
    virtual void draw() override;
    virtual int handle(int event) override;
};

class Fl_Choice : public Fl_Menu_ {
public:
    Fl_Choice(int x, int y, int w, int h, const char* label = 0);
    virtual void draw() override;
    virtual int handle(int event) override;
};

class Fl_Menu_Button : public Fl_Menu_ {
public:
    Fl_Menu_Button(int x, int y, int w, int h, const char* label = 0);
    virtual void draw() override;
    virtual int handle(int event) override;
    const Fl_Menu_Item* popup();
};

#endif // FL_MENU_ITEM_H

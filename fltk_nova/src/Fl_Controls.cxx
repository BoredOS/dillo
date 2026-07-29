#include "../FL/Fl_Box.H"
#include "../FL/Fl_Button.H"
#include "../FL/Fl_Check_Button.H"
#include "../FL/Fl_Return_Button.H"
#include "../FL/Fl_Input.H"
#include "../FL/Fl_Output.H"
#include "../FL/Fl_Secret_Input.H"
#include "../FL/Fl_Image.H"
#include "../FL/Fl_Pixmap.H"
#include "../FL/Fl_Menu_Item.H"
#include "../FL/Fl_Tabs.H"
#include "../FL/Fl_Scroll.H"
#include "../FL/Fl_Browser.H"
#include "../FL/Fl_Tooltip.H"
#include "../FL/Fl_File_Chooser.H"
#include "../FL/Fl_Window.H"
#include "../FL/fl_draw.H"
#include "../FL/Fl.H"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Fl_Box
Fl_Box::Fl_Box(int x, int y, int w, int h, const char* label) : Fl_Widget(x, y, w, h, label) {}
Fl_Box::Fl_Box(Fl_Boxtype b, int x, int y, int w, int h, const char* label) : Fl_Widget(x, y, w, h, label) { box(b); }
void Fl_Box::draw() {
    if (box() != FL_NO_BOX) fl_draw_box(box(), x(), y(), w(), h(), color());
    if (label()) {
        fl_color(labelcolor());
        fl_font(labelfont(), labelsize());
        fl_draw(label(), x(), y(), w(), h(), align());
    }
}

// Fl_Button
Fl_Button::Fl_Button(int x, int y, int w, int h, const char* label)
    : Fl_Widget(x, y, w, h, label), value_(0), down_box_(FL_DOWN_BOX), image_(NULL), deimage_(NULL)
{
    box(FL_UP_BOX);
}

Fl_Button::~Fl_Button() {}

void Fl_Button::draw() {
    Fl_Boxtype b = value_ ? (Fl_Boxtype)down_box_ : box();
    fl_draw_box(b, x(), y(), w(), h(), color());
    if (image()) {
        int ix = x() + (w() - image()->w()) / 2;
        int iy = y() + (h() - image()->h()) / 2;
        image()->draw(ix, iy, image()->w(), image()->h());
    } else if (label()) {
        fl_color(labelcolor());
        fl_font(labelfont(), labelsize());
        fl_draw(label(), x(), y(), w(), h(), align());
    }
}

int Fl_Button::handle(int event) {
    if (event == FL_PUSH) {
        value_ = 1;
        redraw();
        return 1;
    } else if (event == FL_RELEASE) {
        if (value_) {
            value_ = 0;
            redraw();
            do_callback();
        }
        return 1;
    }
    return 0;
}

int Fl_Button::value(int v) {
    if (value_ != v) {
        value_ = v;
        redraw();
    }
    return value_;
}

Fl_Check_Button::Fl_Check_Button(int x, int y, int w, int h, const char* label)
    : Fl_Button(x, y, w, h, label) {}

// Fl_Input
Fl_Input::Fl_Input(int x, int y, int w, int h, const char* label)
    : Fl_Widget(x, y, w, h, label), value_(NULL), size_(0), capacity_(0), position_(0), mark_(0), shortcut_(0)
{
    box(FL_DOWN_BOX);
    color(FL_WHITE);
    expand(32);
}

Fl_Input::~Fl_Input() {
    if (value_) free(value_);
}

void Fl_Input::expand(int new_cap) {
    if (new_cap > capacity_) {
        capacity_ = new_cap * 2;
        value_ = (char*)realloc(value_, capacity_);
        if (size_ == 0 && value_) value_[0] = '\0';
    }
}

int Fl_Input::value(const char* text) {
    return value(text, text ? strlen(text) : 0);
}

int Fl_Input::value(const char* text, int len) {
    if (!text) len = 0;
    expand(len + 1);
    if (len > 0) memcpy(value_, text, len);
    value_[len] = '\0';
    size_ = len;
    position_ = len;
    mark_ = len;
    redraw();
    return 1;
}

int Fl_Input::position(int p) {
    if (p < 0) p = 0;
    if (p > size_) p = size_;
    position_ = p;
    mark_ = p;
    redraw();
    return 1;
}

int Fl_Input::insert(const char* t, int l) {
    if (!t) return 0;
    if (l <= 0) l = strlen(t);
    expand(size_ + l + 1);
    memmove(value_ + position_ + l, value_ + position_, size_ - position_ + 1);
    memcpy(value_ + position_, t, l);
    size_ += l;
    position_ += l;
    mark_ = position_;
    redraw();
    return 1;
}

int Fl_Input::replace(int b, int e, const char* text, int ilen) {
    if (b > e) { int tmp = b; b = e; e = tmp; }
    if (b < 0) b = 0;
    if (e > size_) e = size_;
    if (!text) ilen = 0;
    else if (ilen <= 0) ilen = strlen(text);
    int del = e - b;
    expand(size_ - del + ilen + 1);
    memmove(value_ + b + ilen, value_ + e, size_ - e + 1);
    if (ilen > 0) memcpy(value_ + b, text, ilen);
    size_ = size_ - del + ilen;
    position_ = b + ilen;
    mark_ = position_;
    redraw();
    return 1;
}

int Fl_Input::cut() { return replace(0, size_, NULL, 0); }
int Fl_Input::cut(int n) { return replace(position_, position_ + n, NULL, 0); }
int Fl_Input::cut(int a, int b) { return replace(a, b, NULL, 0); }

void Fl_Input::draw() {
    fl_draw_box(box(), x(), y(), w(), h(), color());
    if (label()) {
        fl_color(labelcolor());
        fl_font(labelfont(), labelsize());
        fl_draw(label(), x() - 60, y(), 55, h(), FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);
    }
    if (value_ && value_[0] != '\0') {
        fl_color(FL_BLACK);
        fl_font(FL_HELVETICA, 12);
        fl_draw(value_, x() + 4, y(), w() - 8, h(), FL_ALIGN_LEFT | FL_ALIGN_INSIDE | FL_ALIGN_CLIP);
    }
}

int Fl_Input::handle(int event) {
    if (event == FL_PUSH) {
        Fl::focus(this);
        return 1;
    } else if (event == FL_KEYDOWN) {
        int key = Fl::event_key();
        const char* txt = Fl::event_text();
        if (key == FL_BackSpace) {
            if (position_ > 0) cut(-1);
            return 1;
        } else if (key == FL_Enter) {
            do_callback();
            return 1;
        } else if (txt && txt[0] >= 32) {
            insert(txt, strlen(txt));
            return 1;
        }
    }
    return 0;
}

// Fl_Image
Fl_Image::Fl_Image(int w, int h, int d) : w_(w), h_(h), d_(d), data_(NULL) {}
Fl_Image::~Fl_Image() {}
void Fl_Image::draw(int X, int Y, int W, int H, int cx, int cy) { ((const Fl_Image*)this)->draw(X, Y, W, H, cx, cy); }
void Fl_Image::draw(int X, int Y, int W, int H, int, int) const { fl_rectf(X, Y, W, H, FL_GRAY); }
Fl_Image* Fl_Image::copy(int W, int H) { return new Fl_Image(W, H, d_); }

// Fl_RGB_Image
Fl_RGB_Image::Fl_RGB_Image(const unsigned char* bits, int W, int H, int D, int)
    : Fl_Image(W, H, D), array_(bits), alloc_array_(0) {}
Fl_RGB_Image::~Fl_RGB_Image() { if (alloc_array_ && array_) free((void*)array_); }
void Fl_RGB_Image::draw(int X, int Y, int W, int H, int, int) const {
    if (array_) fl_draw_image(array_, X, Y, W, H, d_);
}
Fl_Image* Fl_RGB_Image::copy(int W, int H) { return new Fl_RGB_Image(array_, W, H, d_); }

Fl_Pixmap::Fl_Pixmap(const char* const* data) : Fl_Image(16, 16, 4), rgb_data_(NULL) {
    data_ = data;
    if (data_ && data_[0]) {
        int w = 0, h = 0, nc = 0, cpp = 0;
        if (sscanf(data_[0], "%d %d %d %d", &w, &h, &nc, &cpp) == 4 && w > 0 && h > 0) {
            w_ = w;
            h_ = h;
        }
    }
}
Fl_Pixmap::Fl_Pixmap(const char* data) : Fl_Image(16, 16, 4), rgb_data_(NULL) { (void)data; }
Fl_Pixmap::~Fl_Pixmap() { if (rgb_data_) free(rgb_data_); }

void Fl_Pixmap::draw(int X, int Y, int W, int H, int, int) const {
    if (!data_ || !data_[0]) return;
    int w = 0, h = 0, ncolors = 0, cpp = 0;
    if (sscanf(data_[0], "%d %d %d %d", &w, &h, &ncolors, &cpp) < 4) return;
    if (w <= 0 || h <= 0 || ncolors <= 0 || cpp <= 0) return;

    struct XpmColor {
        char key[8];
        uint32_t color;
        bool is_none;
    };
    XpmColor* cmap = (XpmColor*)calloc(ncolors, sizeof(XpmColor));

    for (int i = 0; i < ncolors; i++) {
        const char* line = data_[1 + i];
        strncpy(cmap[i].key, line, cpp);
        cmap[i].key[cpp] = '\0';

        const char* cptr = strstr(line + cpp, "c ");
        if (!cptr) cptr = strstr(line + cpp, "C ");
        if (cptr) {
            cptr += 2;
            while (*cptr == ' ' || *cptr == '\t') cptr++;
            if (strncasecmp(cptr, "none", 4) == 0) {
                cmap[i].is_none = true;
                cmap[i].color = 0;
            } else {
                cmap[i].is_none = false;
                unsigned int r = 0, g = 0, b = 0;
                if (*cptr == '#') {
                    cptr++;
                    int hexlen = 0;
                    while ((cptr[hexlen] >= '0' && cptr[hexlen] <= '9') ||
                           (cptr[hexlen] >= 'a' && cptr[hexlen] <= 'f') ||
                           (cptr[hexlen] >= 'A' && cptr[hexlen] <= 'F')) hexlen++;
                    auto hex_val = [](char c) -> unsigned int {
                        if (c >= '0' && c <= '9') return c - '0';
                        if (c >= 'a' && c <= 'f') return c - 'a' + 10;
                        if (c >= 'A' && c <= 'F') return c - 'A' + 10;
                        return 0;
                    };
                    auto hex_byte = [&](const char* p) -> unsigned int {
                        return (hex_val(p[0]) << 4) | hex_val(p[1]);
                    };
                    if (hexlen == 12) {
                        r = hex_byte(cptr);
                        g = hex_byte(cptr + 4);
                        b = hex_byte(cptr + 8);
                    } else if (hexlen == 6) {
                        r = hex_byte(cptr);
                        g = hex_byte(cptr + 2);
                        b = hex_byte(cptr + 4);
                    } else if (hexlen == 3) {
                        r = hex_val(cptr[0]) * 17;
                        g = hex_val(cptr[1]) * 17;
                        b = hex_val(cptr[2]) * 17;
                    }
                } else {
                    if (strncasecmp(cptr, "red", 3) == 0) { r = 255; g = 0; b = 0; }
                    else if (strncasecmp(cptr, "green", 5) == 0) { r = 0; g = 255; b = 0; }
                    else if (strncasecmp(cptr, "blue", 4) == 0) { r = 0; g = 0; b = 255; }
                    else if (strncasecmp(cptr, "yellow", 6) == 0) { r = 255; g = 255; b = 0; }
                    else if (strncasecmp(cptr, "black", 5) == 0) { r = 0; g = 0; b = 0; }
                    else if (strncasecmp(cptr, "white", 5) == 0) { r = 255; g = 255; b = 255; }
                    else if (strncasecmp(cptr, "gray", 4) == 0 || strncasecmp(cptr, "grey", 4) == 0) { r = 128; g = 128; b = 128; }
                }
                cmap[i].color = ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;
            }
        } else if (strstr(line + cpp, "None") || strstr(line + cpp, "none")) {
            cmap[i].is_none = true;
            cmap[i].color = 0;
        } else {
            cmap[i].is_none = false;
            cmap[i].color = 0x00000000;
        }
    }

    Fl_Window* win = Fl::first_window();
    if (win && win->buffer()) {
        uint32_t* buf = win->buffer();
        int win_w = win->w(), win_h = win->h();

        for (int r = 0; r < h && r < H; r++) {
            const char* row_str = data_[1 + ncolors + r];
            for (int col = 0; col < w && col < W; col++) {
                const char* pkey = row_str + col * cpp;
                for (int i = 0; i < ncolors; i++) {
                    if (strncmp(pkey, cmap[i].key, cpp) == 0) {
                        if (!cmap[i].is_none) {
                            uint32_t argb = cmap[i].color | 0xFF000000;
                            if (X + col >= 0 && X + col < win_w && Y + r >= 0 && Y + r < win_h) {
                                buf[(Y + r) * win_w + (X + col)] = argb;
                            }
                        }
                        break;
                    }
                }
            }
        }
    }

    free(cmap);
}

// Fl_Menu_Item & Fl_Menu_
const Fl_Menu_Item* Fl_Menu_Item::next(int n) const { return this + n; }
const Fl_Menu_Item* Fl_Menu_Item::find_shortcut(int*, bool) const { return NULL; }
int Fl_Menu_Item::size() const { return 1; }
void Fl_Menu_Item::setonly() { set(); }

Fl_Menu_::Fl_Menu_(int x, int y, int w, int h, const char* label)
    : Fl_Widget(x, y, w, h, label), menu_(NULL), alloc_(0), value_(0) {}
Fl_Menu_::~Fl_Menu_() {}
void Fl_Menu_::menu(const Fl_Menu_Item* m) { menu_ = m; }
int Fl_Menu_::add(const char*, int, Fl_Callback, void*, int) { return 0; }
int Fl_Menu_::add(const char*, const char*, Fl_Callback, void*, int) { return 0; }
void Fl_Menu_::clear() { menu_ = NULL; }
int Fl_Menu_::size() const { return 0; }
int Fl_Menu_::value(int i) { value_ = i; return value_; }
int Fl_Menu_::value(const Fl_Menu_Item*) { return value_; }
const Fl_Menu_Item* Fl_Menu_::mvalue() const { return menu_ ? &menu_[value_] : NULL; }
const char* Fl_Menu_::text() const { return menu_ ? menu_[value_].text : NULL; }
const char* Fl_Menu_::text(int i) const { return menu_ ? menu_[i].text : NULL; }
int Fl_Menu_::handle(int) { return 0; }

Fl_Menu_Bar::Fl_Menu_Bar(int x, int y, int w, int h, const char* label) : Fl_Menu_(x, y, w, h, label) { box(FL_UP_BOX); }
void Fl_Menu_Bar::draw() { fl_draw_box(box(), x(), y(), w(), h(), color()); }
int Fl_Menu_Bar::handle(int) { return 0; }

Fl_Choice::Fl_Choice(int x, int y, int w, int h, const char* label) : Fl_Menu_(x, y, w, h, label) { box(FL_DOWN_BOX); }
void Fl_Choice::draw() { fl_draw_box(box(), x(), y(), w(), h(), color()); }
int Fl_Choice::handle(int) { return 0; }

Fl_Menu_Button::Fl_Menu_Button(int x, int y, int w, int h, const char* label) : Fl_Menu_(x, y, w, h, label) {}
void Fl_Menu_Button::draw() { fl_draw_box(FL_UP_BOX, x(), y(), w(), h(), color()); }
int Fl_Menu_Button::handle(int) { return 0; }
const Fl_Menu_Item* Fl_Menu_Button::popup() { return NULL; }

// Fl_Tabs
Fl_Tabs::Fl_Tabs(int x, int y, int w, int h, const char* label) : Fl_Group(x, y, w, h, label), value_(NULL) {}
void Fl_Tabs::draw() { draw_children(); }
int Fl_Tabs::handle(int event) { return Fl_Group::handle(event); }
int Fl_Tabs::value(Fl_Widget* w) { value_ = w; return 1; }
Fl_Widget* Fl_Tabs::which(int, int) { return children() > 0 ? child(0) : NULL; }

// Fl_Scroll
Fl_Scroll::Fl_Scroll(int x, int y, int w, int h, const char* label)
    : Fl_Group(x, y, w, h, label), xposition_(0), yposition_(0), scrollbar_size_(16) {}
void Fl_Scroll::draw() { draw_children(); }
int Fl_Scroll::handle(int event) { return Fl_Group::handle(event); }
void Fl_Scroll::scroll_to(int x, int y) { xposition_ = x; yposition_ = y; redraw(); }

// Fl_Browser
Fl_Browser::Fl_Browser(int x, int y, int w, int h, const char* label)
    : Fl_Group(x, y, w, h, label), items_(NULL), num_items_(0), capacity_(0), selected_(0) { box(FL_DOWN_BOX); color(FL_WHITE); }
Fl_Browser::~Fl_Browser() { clear(); }
void Fl_Browser::add(const char* newtext, void*) {
    if (num_items_ >= capacity_) {
        capacity_ = capacity_ ? capacity_ * 2 : 8;
        items_ = (char**)realloc(items_, capacity_ * sizeof(char*));
    }
    items_[num_items_++] = strdup(newtext ? newtext : "");
    redraw();
}
void Fl_Browser::insert(int line, const char* newtext, void* d) { add(newtext, d); }
void Fl_Browser::remove(int line) {
    if (line >= 1 && line <= num_items_) {
        free(items_[line - 1]);
        for (int k = line - 1; k < num_items_ - 1; k++) items_[k] = items_[k + 1];
        num_items_--;
        redraw();
    }
}
void Fl_Browser::clear() {
    for (int i = 0; i < num_items_; i++) free(items_[i]);
    if (items_) free(items_);
    items_ = NULL; num_items_ = 0; capacity_ = 0; selected_ = 0;
    redraw();
}
const char* Fl_Browser::text(int line) const {
    return (line >= 1 && line <= num_items_) ? items_[line - 1] : NULL;
}
void Fl_Browser::text(int line, const char* newtext) {
    if (line >= 1 && line <= num_items_) {
        free(items_[line - 1]);
        items_[line - 1] = strdup(newtext ? newtext : "");
        redraw();
    }
}
void Fl_Browser::value(int line) { selected_ = line; redraw(); }
void Fl_Browser::draw() {
    fl_draw_box(box(), x(), y(), w(), h(), color());
    int line_h = 16;
    for (int i = 0; i < num_items_; i++) {
        int ly = y() + i * line_h + 2;
        if (ly + line_h > y() + h()) break;
        if (i + 1 == selected_) fl_rectf(x() + 2, ly, w() - 4, line_h, FL_BLUE);
        fl_color((i + 1 == selected_) ? FL_WHITE : FL_BLACK);
        fl_font(FL_HELVETICA, 12);
        fl_draw(items_[i], x() + 6, ly, w() - 12, line_h, FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    }
}
int Fl_Browser::handle(int event) {
    if (event == FL_PUSH) {
        int line_h = 16;
        int clicked_line = (Fl::event_y() - y()) / line_h + 1;
        if (clicked_line >= 1 && clicked_line <= num_items_) {
            value(clicked_line);
            do_callback();
        }
        return 1;
    }
    return 0;
}

// Fl_Tooltip
void Fl_Tooltip::delay(float) {}
float Fl_Tooltip::delay() { return 0.5f; }
void Fl_Tooltip::enable(int) {}
int Fl_Tooltip::enabled() { return 1; }
void Fl_Tooltip::enter(Fl_Widget*) {}
void Fl_Tooltip::exit() {}
void Fl_Tooltip::current(Fl_Widget*) {}
Fl_Widget* Fl_Tooltip::current() { return NULL; }
void Fl_Tooltip::font(Fl_Font) {}
Fl_Font Fl_Tooltip::font() { return FL_HELVETICA; }
void Fl_Tooltip::size(Fl_Fontsize) {}
Fl_Fontsize Fl_Tooltip::size() { return 10; }
void Fl_Tooltip::color(Fl_Color) {}
Fl_Color Fl_Tooltip::color() { return FL_YELLOW; }
void Fl_Tooltip::textcolor(Fl_Color) {}
Fl_Color Fl_Tooltip::textcolor() { return FL_BLACK; }

// Fl_File_Chooser
const char* Fl_File_Chooser::file_chooser(const char*, const char*, const char* filename, int) {
    return filename;
}

#include "../FL/Fl_Widget.H"
#include "../FL/Fl_Group.H"
#include "../FL/Fl_Window.H"
#include "../FL/Fl.H"
#include "../FL/fl_draw.H"
#include <string.h>

void Fl_Widget::draw_box() { fl_draw_box(box(), x(), y(), w(), h(), color()); }
void Fl_Widget::draw_box(Fl_Boxtype b, Fl_Color c) { fl_draw_box(b, x(), y(), w(), h(), c); }
void Fl_Widget::draw_box(Fl_Boxtype b, int x, int y, int w, int h, Fl_Color c) { fl_draw_box(b, x, y, w, h, c); }

Fl_Widget::Fl_Widget(int x, int y, int w, int h, const char* label)
    : x_(x), y_(y), w_(w), h_(h), label_(NULL), parent_(NULL),
      callback_(NULL), user_data_(NULL), box_(FL_NO_BOX), color_(0x00ECECEC),
      labelcolor_(0), labelfont_(FL_HELVETICA), labelsize_(12), align_(FL_ALIGN_CENTER),
      flags_(1 /* visible */ | 2 /* active */)
{
    if (label) label_ = strdup(label);
    if (Fl_Group::current()) Fl_Group::current()->add(this);
}

Fl_Widget::~Fl_Widget() {
    if (label_) free((void*)label_);
    if (parent_) parent_->remove(this);
}

void Fl_Widget::label(const char* l) {
    if (label_) free((void*)label_);
    label_ = l ? strdup(l) : NULL;
    redraw();
}

void Fl_Widget::resize(int x, int y, int w, int h) {
    x_ = x; y_ = y; w_ = w; h_ = h;
    redraw();
}

int Fl_Widget::handle(int) {
    return 0;
}

void Fl_Widget::redraw() {
    damage(FL_DAMAGE_ALL);
}

void Fl_Widget::damage(unsigned char c) {
    for (Fl_Widget* w = this; w; w = w->parent()) {
        w->flags_ |= (c << 8);
    }
}

void Fl_Widget::measure_label(int& w, int& h) {
    fl_measure(label(), w, h);
}

unsigned char Fl_Widget::damage() const {
    return (flags_ >> 8) & 0xFF;
}

void Fl_Widget::clear_damage() {
    flags_ &= 0x00FF;
}

void Fl_Widget::show() {
    flags_ |= 1;
    redraw();
}

void Fl_Widget::hide() {
    flags_ &= ~1;
    redraw();
}

int Fl_Widget::visible() const {
    return (flags_ & 1) != 0;
}

int Fl_Widget::visible_r() const {
    for (const Fl_Widget* w = this; w; w = w->parent()) {
        if (!w->visible()) return 0;
    }
    return 1;
}

void Fl_Widget::activate() {
    flags_ |= 2;
    redraw();
}

void Fl_Widget::deactivate() {
    flags_ &= ~2;
    redraw();
}

int Fl_Widget::active() const {
    return (flags_ & 2) != 0;
}

int Fl_Widget::active_r() const {
    for (const Fl_Widget* w = this; w; w = w->parent()) {
        if (!w->active()) return 0;
    }
    return 1;
}

Fl_Window* Fl_Widget::window() const {
    for (const Fl_Widget* w = this; w; w = w->parent()) {
        if (w->parent() == NULL) return (Fl_Window*)w;
    }
    return NULL;
}

// Fl_Group Implementation
static Fl_Group* g_current_group = NULL;

void Fl_Group::init() {
    array_ = NULL;
    children_ = 0;
    capacity_ = 0;
    resizable_ = NULL;
}

Fl_Group::Fl_Group(int x, int y, int w, int h, const char* label)
    : Fl_Widget(x, y, w, h, label)
{
    init();
    begin();
}

Fl_Group::~Fl_Group() {
    clear();
    if (array_) free(array_);
    if (g_current_group == this) g_current_group = NULL;
}

void Fl_Group::begin() {
    g_current_group = this;
}

void Fl_Group::end() {
    if (g_current_group == this) g_current_group = parent();
}

Fl_Group* Fl_Group::current() { return g_current_group; }
void Fl_Group::current(Fl_Group* g) { g_current_group = g; }

int Fl_Group::find(const Fl_Widget* w) const {
    for (int i = 0; i < children_; i++) {
        if (array_[i] == w) return i;
    }
    return children_;
}

void Fl_Group::add(Fl_Widget* w) {
    if (!w) return;
    if (w->parent()) w->parent()->remove(w);
    if (children_ >= capacity_) {
        capacity_ = capacity_ ? capacity_ * 2 : 4;
        array_ = (Fl_Widget**)realloc(array_, capacity_ * sizeof(Fl_Widget*));
    }
    array_[children_++] = w;
    w->parent(this);
}

void Fl_Group::insert(Fl_Widget& w, int i) {
    add(&w);
}

void Fl_Group::remove(Fl_Widget* w) {
    int idx = find(w);
    if (idx < children_) remove(idx);
}

void Fl_Group::remove(int i) {
    if (i < 0 || i >= children_) return;
    array_[i]->parent(NULL);
    for (int k = i; k < children_ - 1; k++) {
        array_[k] = array_[k + 1];
    }
    children_--;
}

void Fl_Group::clear() {
    while (children_ > 0) {
        Fl_Widget* w = array_[--children_];
        w->parent(NULL);
        delete w;
    }
}

void Fl_Group::draw() {
    draw_children();
}

void Fl_Group::draw_children() {
    for (int i = 0; i < children_; i++) {
        if (array_[i]->visible()) array_[i]->draw();
    }
}

int Fl_Group::handle(int event) {
    if (event == FL_PUSH || event == FL_RELEASE || event == FL_MOVE || event == FL_DRAG) {
        for (int i = children_ - 1; i >= 0; i--) {
            Fl_Widget* c = array_[i];
            if (c && c->visible() && c->active() && Fl::event_inside(c)) {
                if (c->handle(event)) {
                    if (event == FL_PUSH && !Fl::pushed()) {
                        Fl::pushed(c);
                    }
                    return 1;
                }
            }
        }
        return 0;
    }
    for (int i = children_ - 1; i >= 0; i--) {
        Fl_Widget* c = array_[i];
        if (c && c->visible() && c->active()) {
            if (c->handle(event)) return 1;
        }
    }
    return 0;
}

void Fl_Group::resize(int X, int Y, int W, int H) {
    int dx = X - x();
    int dy = Y - y();
    int dw = W - w();
    int dh = H - h();

    Fl_Widget::resize(X, Y, W, H);

    for (int i = 0; i < children_; i++) {
        Fl_Widget* c = array_[i];
        if (c) {
            if (c == resizable_) {
                int nw = c->w() + dw;
                int nh = c->h() + dh;
                c->resize(c->x() + dx, c->y() + dy, nw > 0 ? nw : 0, nh > 0 ? nh : 0);
            } else {
                c->resize(c->x() + dx, c->y() + dy, c->w(), c->h());
            }
        }
    }
}

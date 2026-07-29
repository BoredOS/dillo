#ifndef FL_GROUP_H
#define FL_GROUP_H

#include "Fl_Widget.H"

class Fl_Group : public Fl_Widget {
private:
    Fl_Widget** array_;
    int children_;
    int capacity_;
    Fl_Widget* resizable_;

    void init();

public:
    Fl_Group(int x, int y, int w, int h, const char* label = 0);
    virtual ~Fl_Group();

    virtual Fl_Group* as_group() override { return this; }

    int children() const { return children_; }
    Fl_Widget* child(int n) const { return (n >= 0 && n < children_) ? array_[n] : 0; }
    Fl_Widget* const* array() const { return array_; }
    int find(const Fl_Widget* w) const;
    int contains(const Fl_Widget* w) const {
        for (const Fl_Widget* p = w; p; p = p->parent()) {
            if (p == this) return 1;
        }
        return 0;
    }

    void add(Fl_Widget* w);
    void insert(Fl_Widget& w, int i);
    void insert(Fl_Widget& w, Fl_Widget* before) { add(&w); (void)before; }
    void remove(Fl_Widget* w);
    void remove(int i);
    void clear();

    void begin();
    void end();

    static Fl_Group* current();
    static void current(Fl_Group* g);

    Fl_Widget* resizable() const { return resizable_; }
    void resizable(Fl_Widget* w) { resizable_ = w; }
    void resizable(Fl_Group& g) { resizable_ = &g; }

    void init_sizes() {}

    virtual void draw() override;
    virtual int handle(int event) override;
    virtual void resize(int x, int y, int w, int h) override;

    void draw_child(Fl_Widget& w) { if (w.visible()) w.draw(); }
    void update_child(Fl_Widget& w) { draw_child(w); }
    void draw_children();
};

#endif // FL_GROUP_H

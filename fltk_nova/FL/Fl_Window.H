#ifndef FL_WINDOW_H
#define FL_WINDOW_H

#include "Fl_Group.H"
#include <stdint.h>

class Fl_Window : public Fl_Group {
private:
    int nova_fd_;
    uint32_t surface_id_;
    uint32_t* shm_buffer_;
    size_t shm_size_;
    char shm_path_[256];
    bool shown_;
    bool is_subwindow_;
    char title_[256];
    char icon_path_[256];
    Fl_Window* next_window_;

public:
    Fl_Window(int w, int h, const char* title = 0);
    Fl_Window(int x, int y, int w, int h, const char* title = 0);
    virtual ~Fl_Window();

    static void default_xclass(const char*) {}
    void make_current();
    void set_modal() {}
    int shown() const { return is_shown(); }
    void cursor(Fl_Cursor) {}

    virtual void show() override;
    virtual void hide() override;
    virtual void flush();

    void label(const char* l);
    void label(const char* l1, const char*) { label(l1); }
    const char* label() const { return title_; }

    void icon(const void* ic);

    uint32_t* buffer() const { return shm_buffer_; }
    int nova_fd() const { return nova_fd_; }
    uint32_t surface_id() const { return surface_id_; }
    Fl_Window* next_window() const { return next_window_; }

    virtual void resize(int x, int y, int w, int h) override;
    virtual int handle(int event) override;
    virtual void draw() override;

    void size_range(int minw, int minh, int maxw = 0, int maxh = 0);

    bool is_shown() const { return shown_; }
};

#endif // FL_WINDOW_H

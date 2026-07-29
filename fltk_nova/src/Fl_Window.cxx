#include "../FL/Fl_Window.H"
#include "../FL/Fl.H"
#include "../FL/fl_draw.H"
extern "C" {
#include <novaproto.h>
}
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Fl_Window* g_first_window = NULL;
static Fl_Window* g_current_window = NULL;

void Fl_Window::make_current() {
    g_current_window = this;
}

Fl_Window::Fl_Window(int w, int h, const char* title)
    : Fl_Group(0, 0, w, h, title),
      nova_fd_(-1), surface_id_(0), shm_buffer_(NULL), shm_size_(0),
      shown_(false), is_subwindow_(false), next_window_(NULL)
{
    shm_path_[0] = '\0';
    icon_path_[0] = '\0';
    if (title) strncpy(title_, title, sizeof(title_) - 1);
    else title_[0] = '\0';
}

Fl_Window::Fl_Window(int x, int y, int w, int h, const char* title)
    : Fl_Group(x, y, w, h, title),
      nova_fd_(-1), surface_id_(0), shm_buffer_(NULL), shm_size_(0),
      shown_(false), is_subwindow_(false), next_window_(NULL)
{
    shm_path_[0] = '\0';
    icon_path_[0] = '\0';
    if (title) strncpy(title_, title, sizeof(title_) - 1);
    else title_[0] = '\0';
}

Fl_Window::~Fl_Window() {
    hide();
}

void Fl_Window::show() {
    printf("[Fl_Window::show] Called for window %p, w=%d, h=%d, title='%s'\n", (void*)this, w(), h(), title_);
    if (shown_) return;

    nova_fd_ = nova_connect(NULL);
    printf("[Fl_Window::show] nova_connect returned fd=%d\n", nova_fd_);
    if (nova_fd_ < 0) {
        fprintf(stderr, "[Fl_Window] Failed to connect to Nova display server.\n");
        return;
    }

    uint32_t flags = 0;
    int res = nova_create_surface(nova_fd_, w(), h(), 2, flags, &surface_id_, shm_path_);
    printf("[Fl_Window::show] nova_create_surface res=%d, surf_id=%u, shm_path='%s'\n", res, surface_id_, shm_path_);
    if (res < 0) {
        fprintf(stderr, "[Fl_Window] Failed to create Nova surface.\n");
        close(nova_fd_);
        nova_fd_ = -1;
        return;
    }

    shm_size_ = (size_t)w() * h() * 4;
    int shm_fd = open(shm_path_, O_RDWR);
    printf("[Fl_Window::show] open('%s') returned shm_fd=%d, shm_size=%zu\n", shm_path_, shm_fd, shm_size_);
    if (shm_fd >= 0) {
        shm_buffer_ = (uint32_t*)mmap(NULL, shm_size_, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
        close(shm_fd);
        if (shm_buffer_ == MAP_FAILED) shm_buffer_ = NULL;
    }
    printf("[Fl_Window::show] mmap buffer=%p\n", (void*)shm_buffer_);

    if (!shm_buffer_) {
        fprintf(stderr, "[Fl_Window] Failed to map Nova surface memory for path %s.\n", shm_path_);
        nova_destroy_surface(nova_fd_, surface_id_);
        close(nova_fd_);
        nova_fd_ = -1;
        return;
    }

    if (title_[0] != '\0') {
        nova_set_title(nova_fd_, surface_id_, title_);
    }

    shown_ = true;
    this->parent(NULL);

    next_window_ = g_first_window;
    g_first_window = this;
    g_current_window = this;

    printf("[Fl_Window::show] Window successfully shown! g_first_window=%p\n", (void*)g_first_window);

    draw();
    flush();
}

void Fl_Window::hide() {
    printf("[Fl_Window::hide] Hiding window %p, shown_=%d, g_first_window=%p\n", (void*)this, shown_, (void*)g_first_window);
    if (!shown_) return;

    if (shm_buffer_ && shm_size_ > 0) {
        munmap(shm_buffer_, shm_size_);
        shm_buffer_ = NULL;
        shm_size_ = 0;
    }

    if (nova_fd_ >= 0 && surface_id_ > 0) {
        nova_destroy_surface(nova_fd_, surface_id_);
        close(nova_fd_);
        nova_fd_ = -1;
        surface_id_ = 0;
    }

    shown_ = false;

    if (g_first_window == this) {
        g_first_window = next_window_;
    } else {
        for (Fl_Window* w = g_first_window; w; w = w->next_window_) {
            if (w->next_window_ == this) {
                w->next_window_ = this->next_window_;
                break;
            }
        }
    }
    next_window_ = NULL;
    if (g_current_window == this) g_current_window = g_first_window;
}

void Fl_Window::label(const char* l) {
    if (l) strncpy(title_, l, sizeof(title_) - 1);
    else title_[0] = '\0';
    if (shown_ && nova_fd_ >= 0 && surface_id_ > 0) {
        nova_set_title(nova_fd_, surface_id_, title_);
    }
}

void Fl_Window::icon(const void* ic) {
    if (ic) strncpy(icon_path_, (const char*)ic, sizeof(icon_path_) - 1);
    if (shown_ && nova_fd_ >= 0 && surface_id_ > 0 && icon_path_[0] != '\0') {
        nova_set_icon(nova_fd_, surface_id_, icon_path_);
    }
}

void Fl_Window::resize(int x, int y, int nw, int nh) {
    if (nw == w() && nh == h()) {
        position(x, y);
        return;
    }

    Fl_Group::resize(x, y, nw, nh);

    if (shown_ && nova_fd_ >= 0 && surface_id_ > 0) {
        char new_shm_path[256];
        if (nova_resize_surface(nova_fd_, surface_id_, nw, nh, new_shm_path) == 0) {
            if (shm_buffer_ && shm_size_ > 0) {
                munmap(shm_buffer_, shm_size_);
            }
            shm_size_ = (size_t)nw * nh * 4;
            int shm_fd = open(new_shm_path, O_RDWR);
            if (shm_fd >= 0) {
                shm_buffer_ = (uint32_t*)mmap(NULL, shm_size_, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
                close(shm_fd);
            }
        }
    }

    redraw();
}

void Fl_Window::flush() {
    if (!shown_ || !shm_buffer_ || nova_fd_ < 0) return;

    NovaRect rect;
    rect.x = 0;
    rect.y = 0;
    rect.w = w();
    rect.h = h();
    nova_damage_surface(nova_fd_, surface_id_, 1, &rect);
}

int Fl_Window::handle(int event) {
    if (event == FL_CLOSE) {
        hide();
        return 1;
    }
    if (event == FL_KEYBOARD || event == FL_KEYDOWN || event == FL_KEYUP) {
        if (Fl::focus() && Fl::focus()->handle(event)) return 1;
    }
    if (event == FL_PUSH) {
        Fl::pushed(NULL);
        return Fl_Group::handle(event);
    }
    if (event == FL_RELEASE) {
        Fl_Widget* pw = Fl::pushed();
        if (pw) {
            Fl::pushed(NULL);
            return pw->handle(FL_RELEASE);
        }
        return Fl_Group::handle(event);
    }
    if (event == FL_DRAG) {
        Fl_Widget* pw = Fl::pushed();
        if (pw) {
            return pw->handle(FL_DRAG);
        }
        return Fl_Group::handle(event);
    }
    return Fl_Group::handle(event);
}

void Fl_Window::draw() {
    if (!shm_buffer_) return;
    fl_restore_clip();

    // Fill background with mapped RGB color
    uint32_t bg = fl_color_to_rgb(color() ? color() : FL_BACKGROUND_COLOR);
    size_t total_pixels = (size_t)w() * h();
    for (size_t i = 0; i < total_pixels; i++) {
        shm_buffer_[i] = bg | 0xFF000000;
    }

    Fl_Group::draw();
}

void Fl_Window::size_range(int, int, int, int) {}

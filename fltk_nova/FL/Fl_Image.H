#ifndef FL_IMAGE_H
#define FL_IMAGE_H

#include "Enumerations.H"

class Fl_Image {
protected:
    int w_, h_, d_;
    const char* const* data_;

public:
    Fl_Image(int w, int h, int d);
    virtual ~Fl_Image();

    int w() const { return w_; }
    int h() const { return h_; }
    int d() const { return d_; }

    void desaturate() {}
    void color_average(Fl_Color, float) {}

    virtual void draw(int X, int Y, int W, int H, int cx = 0, int cy = 0);
    void draw(int X, int Y) { draw(X, Y, w_, h_, 0, 0); }
    virtual void draw(int X, int Y, int W, int H, int cx = 0, int cy = 0) const;
    void draw(int X, int Y) const { draw(X, Y, w_, h_, 0, 0); }
    virtual Fl_Image* copy(int W, int H);
    Fl_Image* copy() { return copy(w_, h_); }
};

class Fl_RGB_Image : public Fl_Image {
private:
    const unsigned char* array_;
    int alloc_array_;

public:
    Fl_RGB_Image(const unsigned char* bits, int W, int H, int D = 3, int LD = 0);
    virtual ~Fl_RGB_Image();
    const unsigned char* array() const { return array_; }
    virtual void draw(int X, int Y, int W, int H, int cx = 0, int cy = 0) const override;
    virtual Fl_Image* copy(int W, int H) override;
};

class Fl_Pixmap : public Fl_Image {
private:
    unsigned char* rgb_data_;

public:
    Fl_Pixmap(const char* const* data);
    Fl_Pixmap(const char* data);
    virtual ~Fl_Pixmap();
    virtual void draw(int X, int Y, int W, int H, int cx = 0, int cy = 0) const override;
};

#endif // FL_IMAGE_H

#ifndef FL_BITMAP_H
#define FL_BITMAP_H

#include "Fl_Image.H"

class Fl_Bitmap : public Fl_Image {
public:
    Fl_Bitmap(const unsigned char* bits, int W, int H)
        : Fl_Image(W, H, 1) { (void)bits; }
    Fl_Bitmap(const char* bits, int W, int H)
        : Fl_Image(W, H, 1) { (void)bits; }
};

#endif // FL_BITMAP_H

#ifndef FL_BUTTON_H
#define FL_BUTTON_H

#include "Fl_Widget.H"

class Fl_Button : public Fl_Widget {
private:
    char value_;
    char down_box_;
    const class Fl_Image* image_;
    const class Fl_Image* deimage_;

public:
    Fl_Button(int x, int y, int w, int h, const char* label = 0);
    virtual ~Fl_Button();

    virtual void draw() override;
    virtual int handle(int event) override;

    void simulate_key_action() { do_callback(); }

    int value(int v);
    int value() const { return value_; }

    void down_box(Fl_Boxtype b) { down_box_ = b; }
    Fl_Boxtype down_box() const { return (Fl_Boxtype)down_box_; }

    void image(const class Fl_Image* img) { image_ = img; }
    const class Fl_Image* image() const { return image_; }

    void deimage(const class Fl_Image* img) { deimage_ = img; }
    const class Fl_Image* deimage() const { return deimage_; }
};

#endif // FL_BUTTON_H

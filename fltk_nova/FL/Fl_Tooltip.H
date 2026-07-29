#ifndef FL_TOOLTIP_H
#define FL_TOOLTIP_H

#include "Enumerations.H"

class Fl_Widget;

class Fl_Tooltip {
public:
    static void delay(float f);
    static float delay();
    static void enable(int b = 1);
    static void disable() { enable(0); }
    static int enabled();
    static void enter(Fl_Widget* w);
    static void exit();
    static void current(Fl_Widget* w);
    static Fl_Widget* current();
    static void font(Fl_Font f);
    static Fl_Font font();
    static void size(Fl_Fontsize s);
    static Fl_Fontsize size();
    static void color(Fl_Color c);
    static Fl_Color color();
    static void textcolor(Fl_Color c);
    static Fl_Color textcolor();
};

#endif // FL_TOOLTIP_H

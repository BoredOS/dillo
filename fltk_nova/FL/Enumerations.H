#ifndef FL_ENUMERATIONS_H
#define FL_ENUMERATIONS_H

#include <stdint.h>

typedef unsigned char uchar;
typedef unsigned int Fl_Color;
typedef uint32_t Fl_Offscreen;

#define FL_FOREGROUND_COLOR 0
#define FL_BACKGROUND_COLOR 49
#define FL_BACKGROUND2_COLOR 7
#define FL_INACTIVE_COLOR 8
#define FL_SELECTION_COLOR 15

#define FL_BLACK       0x00000000
#define FL_RED         0x00FF0000
#define FL_GREEN       0x0000FF00
#define FL_YELLOW      0x00FFFF00
#define FL_BLUE        0x000000FF
#define FL_MAGENTA     0x00FF00FF
#define FL_CYAN        0x0000FFFF
#define FL_WHITE       0x00FFFFFF
#define FL_GRAY        0x00C0C0C0
#define FL_LIGHTGRAY   0x00E0E0E0
#define FL_DARKGRAY    0x00808080
#define FL_DARK2       0x00606060
#define FL_DARK3       0x00404040

inline Fl_Color fl_rgb_color(unsigned char r, unsigned char g, unsigned char b) {
    return ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;
}

inline Fl_Color fl_color_average(Fl_Color c1, Fl_Color c2, float weight) {
    unsigned char r1 = (c1 >> 16) & 0xFF, g1 = (c1 >> 8) & 0xFF, b1 = c1 & 0xFF;
    unsigned char r2 = (c2 >> 16) & 0xFF, g2 = (c2 >> 8) & 0xFF, b2 = c2 & 0xFF;
    unsigned char r = (unsigned char)(r1 * weight + r2 * (1.0f - weight));
    unsigned char g = (unsigned char)(g1 * weight + g2 * (1.0f - weight));
    unsigned char b = (unsigned char)(b1 * weight + b2 * (1.0f - weight));
    return fl_rgb_color(r, g, b);
}

extern Fl_Color g_colormap[256];

inline Fl_Color fl_color_to_rgb(Fl_Color c) {
    if (c == 0) return 0x00000000;
    if (c > 255) {
        if ((c & 0xFF000000) == 0) return c;
        if ((c & 0x000000FF) == 0 && c > 0x00FFFFFF) return c >> 8;
        return c & 0x00FFFFFF;
    }
    if (g_colormap[c] != 0) return g_colormap[c];
    if (c == 7) return 0x00FFFFFF;
    if (c == 8) return 0x00808080;
    if (c == 15) return 0x00000080;
    if (c >= 32 && c <= 55) {
        unsigned char g = (unsigned char)(255 - (55 - c) * 5);
        return ((uint32_t)g << 16) | ((uint32_t)g << 8) | (uint32_t)g;
    }
    static const uint32_t std_colors[16] = {
        0x00000000, 0x00800000, 0x00008000, 0x00808000,
        0x00000080, 0x00800080, 0x00008080, 0x00FFFFFF,
        0x00808080, 0x00FF0000, 0x0000FF00, 0x00FFFF00,
        0x000000FF, 0x00FF00FF, 0x0000FFFF, 0x00FFFFFF
    };
    if (c < 16) return std_colors[c];
    return 0x00ECECEC;
}

typedef int Fl_Font;
#define FL_HELVETICA              0
#define FL_HELVETICA_BOLD         1
#define FL_HELVETICA_ITALIC       2
#define FL_HELVETICA_BOLD_ITALIC  3
#define FL_COURIER                4
#define FL_COURIER_BOLD           5
#define FL_COURIER_ITALIC         6
#define FL_COURIER_BOLD_ITALIC    7
#define FL_TIMES                  8
#define FL_TIMES_BOLD             9
#define FL_TIMES_ITALIC           10
#define FL_TIMES_BOLD_ITALIC      11
#define FL_SCREEN                 12
#define FL_BOLD                   1
#define FL_ITALIC                 2

typedef int Fl_Fontsize;

// Event Types
#define FL_NO_EVENT        0
#define FL_PUSH            1
#define FL_RELEASE         2
#define FL_ENTER           3
#define FL_LEAVE           4
#define FL_DRAG            5
#define FL_FOCUS           6
#define FL_UNFOCUS         7
#define FL_KEYDOWN         8
#define FL_KEYUP           9
#define FL_CLOSE          10
#define FL_MOVE           11
#define FL_SHORTCUT       12
#define FL_DEACTIVATE     13
#define FL_ACTIVATE       14
#define FL_HIDE           15
#define FL_SHOW           16
#define FL_PASTE          17
#define FL_SELECTIONCLEAR 18
#define FL_MOUSEWHEEL     19
#define FL_KEYBOARD       8

// Mouse Buttons
#define FL_LEFT_MOUSE     1
#define FL_MIDDLE_MOUSE   2
#define FL_RIGHT_MOUSE    3

// Key Codes
#define FL_Button         0xfee8
#define FL_BackSpace      0xff08
#define FL_Tab            0xff09
#define FL_Enter          0xff0d
#define FL_Pause          0xff13
#define FL_Scroll_Lock    0xff14
#define FL_Escape         0xff1b
#define FL_Home           0xff50
#define FL_Left           0xff51
#define FL_Up             0xff52
#define FL_Right          0xff53
#define FL_Down           0xff54
#define FL_Page_Up        0xff55
#define FL_Page_Down      0xff56
#define FL_End            0xff57
#define FL_Insert         0xff63
#define FL_Delete         0xffff

// Modifiers
#define FL_SHIFT          0x00010000
#define FL_CAPS_LOCK      0x00020000
#define FL_CTRL           0x00040000
#define FL_ALT            0x00080000
#define FL_META           0x00400000

// When flags
#define FL_WHEN_NEVER      0
#define FL_WHEN_CHANGED    1
#define FL_WHEN_RELEASE    4
#define FL_WHEN_ENTER_KEY  8
#define FL_WHEN_ENTER_KEY_ALWAYS 8

// Box Types
typedef enum {
    FL_NO_BOX = 0,
    FL_FLAT_BOX,
    FL_UP_BOX,
    FL_DOWN_BOX,
    FL_BORDER_BOX,
    FL_THIN_UP_BOX,
    FL_THIN_DOWN_BOX,
    FL_FRAME_BOX,
    FL_ROUND_UP_BOX,
    FL_ROUND_DOWN_BOX,
    FL_ROUNDED_BOX = FL_ROUND_UP_BOX,
    FL_RSHADOW_BOX = FL_DOWN_BOX,
    FL_RFLAT_BOX = FL_FLAT_BOX,
    FL_EMBOSSED_BOX = FL_BORDER_BOX
} Fl_Boxtype;

inline Fl_Boxtype fl_down(Fl_Boxtype) { return FL_DOWN_BOX; }

// Alignments
typedef unsigned int Fl_Align;
#define FL_ALIGN_CENTER           0
#define FL_ALIGN_TOP              1
#define FL_ALIGN_BOTTOM           2
#define FL_ALIGN_LEFT             4
#define FL_ALIGN_RIGHT            8
#define FL_ALIGN_INSIDE          16
#define FL_ALIGN_TEXT_OVER_IMAGE 0x0020
#define FL_ALIGN_IMAGE_OVER_TEXT 0x0000

#define FL_ALIGN_TOP_LEFT (FL_ALIGN_TOP | FL_ALIGN_LEFT)
#define FL_ALIGN_TOP_RIGHT (FL_ALIGN_TOP | FL_ALIGN_RIGHT)
#define FL_ALIGN_BOTTOM_LEFT (FL_ALIGN_BOTTOM | FL_ALIGN_LEFT)
#define FL_ALIGN_BOTTOM_RIGHT (FL_ALIGN_BOTTOM | FL_ALIGN_RIGHT)
#define FL_ALIGN_WRAP           128
#define FL_ALIGN_CLIP           64

#define FL_F           0xff00
#define FL_Menu        0xffda
#define FL_Print       0xffdb
#define FL_Back        0xffdc
#define FL_Favorites   0xffdd
#define FL_Forward     0xffde
#define FL_Home_Page   0xffdf

// Damage flags
#define FL_DAMAGE_ALL      0x80
#define FL_DAMAGE_CHILD    0x01
#define FL_DAMAGE_EXPOSE   0x02
#define FL_DAMAGE_SCROLL   0x04
#define FL_DAMAGE_USER1    0x10

#define FL_BUTTON1 1
#define FL_BUTTON2 2
#define FL_BUTTON3 4

#define FL_TOGGLE_BUTTON 1
#define FL_HOLD_BROWSER  1
#define FL_MULTI_BROWSER 2

#define FL_NORMAL_INPUT 0
#define FL_SECRET_INPUT 5

#define FL_HORIZONTAL 0
#define FL_VERTICAL   1

typedef int Fl_Cursor;
#define FL_CURSOR_DEFAULT 0
#define FL_CURSOR_HAND    1
#define FL_CURSOR_TEXT    2
#define FL_TEXT_EDITOR_ANY_STATE 0
#define FL_CURSOR_WAIT    3
#define FL_CURSOR_HELP    4
#define FL_CURSOR_CROSS   1
#define FL_CURSOR_MOVE    1
#define FL_CURSOR_WE      1
#define FL_CURSOR_NESW    1
#define FL_CURSOR_NWSE    1
#define FL_CURSOR_NS      1
#define FL_CURSOR_INSERT  2

#define FL_SOLID 0
#define FL_DASH  1
#define FL_CAP_ROUND 0

// FD Modes
#define FL_READ  1
#define FL_WRITE 4
#define FL_EXCEPT 8

#define FL_NORMAL_LABEL 0
#define FL_FREE_LABELTYPE 16
#define FL_FREE_BOXTYPE 16
#define FL_SHADOW_LABEL 0

#define FL_THIN_DOWN_FRAME FL_THIN_DOWN_BOX
#define FL_THIN_UP_FRAME   FL_THIN_UP_BOX
#define FL_GTK_THIN_UP_BOX FL_THIN_UP_BOX

#define FL_ALIGN_TEXT_NEXT_TO_IMAGE FL_ALIGN_TEXT_OVER_IMAGE
#define SHORTCUT_LABEL 0x1000

#define FL_BACK_MOUSE    4
#define FL_FORWARD_MOUSE 5

#define FL_Mail        0xffee
#define FL_Media_Next  0xffef
#define FL_Media_Play  0xfff0
#define FL_Media_Prev  0xfff1
#define FL_Media_Stop  0xfff2
#define FL_Refresh     0xfff3
#define FL_Search      0xfff4
#define FL_Sleep       0xfff5
#define FL_Stop        0xfff6
#define FL_Volume_Down 0xfff7
#define FL_Volume_Mute 0xfff8
#define FL_Volume_Up   0xfff9

typedef void (*Fl_Label_Draw_F)(const struct Fl_Label* o, int X, int Y, int W, int H, Fl_Align align);
typedef void (*Fl_Label_Measure_F)(const struct Fl_Label* o, int& W, int& H);

struct Fl_Label {
    const char* value;
    class Fl_Image* image;
    Fl_Font font;
    Fl_Fontsize size;
    Fl_Color color;
    unsigned char type;
};

#endif // FL_ENUMERATIONS_H

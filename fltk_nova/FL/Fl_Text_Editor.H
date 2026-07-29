#ifndef FL_TEXT_EDITOR_H
#define FL_TEXT_EDITOR_H

#include "Fl_Text_Display.H"

class Fl_Text_Editor : public Fl_Text_Display {
public:
    typedef int (*Key_Binding_F)(int key, Fl_Text_Editor* editor);

    Fl_Text_Editor(int x, int y, int w, int h, const char* label = 0)
        : Fl_Text_Display(x, y, w, h, label) {}

    void remove_key_binding(int, int) {}
    void add_key_binding(int, int, Key_Binding_F) {}
    static int kf_backspace(int, Fl_Text_Editor*) { return 0; }
    int insert_position() const { return 0; }
    void previous_word() {}
    void show_insert_position() {}
    void cursor_color(Fl_Color) {}
};

#endif // FL_TEXT_EDITOR_H

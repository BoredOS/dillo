#ifndef FL_TEXT_BUFFER_H
#define FL_TEXT_BUFFER_H

#include <stdlib.h>
#include <string.h>

class Fl_Text_Buffer {
private:
    char* text_;
    int length_;
    int capacity_;

public:
    Fl_Text_Buffer(int initial_capacity = 128) {
        capacity_ = initial_capacity > 0 ? initial_capacity : 128;
        text_ = (char*)malloc(capacity_);
        if (text_) text_[0] = '\0';
        length_ = 0;
    }

    ~Fl_Text_Buffer() {
        if (text_) free(text_);
    }

    char* text() { return text_ ? text_ : (char*)""; }
    const char* text() const { return text_ ? text_ : ""; }
    int length() const { return length_; }
    void remove(int, int) {}

    void text(const char* txt) {
        if (!txt) txt = "";
        int len = strlen(txt);
        if (len + 1 > capacity_) {
            capacity_ = (len + 1) * 2;
            text_ = (char*)realloc(text_, capacity_);
        }
        if (text_) {
            memcpy(text_, txt, len + 1);
            length_ = len;
        }
    }

    void append(const char* txt) {
        if (!txt) return;
        int len = strlen(txt);
        if (length_ + len + 1 > capacity_) {
            capacity_ = (length_ + len + 1) * 2;
            text_ = (char*)realloc(text_, capacity_);
        }
        if (text_) {
            memcpy(text_ + length_, txt, len + 1);
            length_ += len;
        }
    }
};

#endif // FL_TEXT_BUFFER_H

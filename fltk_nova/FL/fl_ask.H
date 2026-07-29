#ifndef FL_ASK_H
#define FL_ASK_H

#include "Enumerations.H"

void fl_alert(const char* fmt, ...);
int fl_choice(const char* fmt, const char* b0, const char* b1, const char* b2, ...);
const char* fl_input(const char* fmt, const char* defstr = 0, ...);
const char* fl_password(const char* fmt, const char* defstr = 0, ...);
void fl_message(const char* fmt, ...);
inline void fl_message_title(const char*) {}
inline void fl_message_title_default(const char*) {}

#endif // FL_ASK_H

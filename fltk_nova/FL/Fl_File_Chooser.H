#ifndef FL_FILE_CHOOSER_H
#define FL_FILE_CHOOSER_H

#include "Enumerations.H"

class Fl_File_Chooser {
public:
    static const char* file_chooser(const char* message, const char* pattern, const char* filename, int relative = 0);
};

inline const char* fl_file_chooser(const char* message, const char* pattern, const char* filename, int relative = 0) {
    return Fl_File_Chooser::file_chooser(message, pattern, filename, relative);
}

#endif // FL_FILE_CHOOSER_H

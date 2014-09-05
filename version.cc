//
// version.cc: library version string holder
// -----------------------------------

#include <string>
#include <sstream>
#include "version.h"

namespace sigen
{
    static const char* VER_NUM_MAJ = "2.6";
    static const char* VER_NUM_MIN = "2";
    static const char* VER_DATE = __DATE__ " " __TIME__;

    std::string version()
    {
        return std::string(VER_NUM_MAJ) + VER_NUM_MIN + " (" + VER_DATE + ")";
    }

} // namespace

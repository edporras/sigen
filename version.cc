//
// version.cc: library version string holder
// -----------------------------------

#include <string>
#include <sstream>
#include "version.h"

#define SIGEN_VER_NUM  "2.6.3"
#define SIGEN_VER_DATE "Sat Jun 20 01:10:55 EDT 2015"

namespace sigen
{
    const char* version()
    {
        return (SIGEN_VER_NUM " (" SIGEN_VER_DATE ")");
    }

} // namespace

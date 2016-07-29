//
// version.cc: library version string holder
// -----------------------------------

#include <string>
#include <sstream>
#include "version.h"
#include "config.h"

namespace sigen
{
    const char* version()
    {
        return SIGEN_VERSION;
    }

} // namespace

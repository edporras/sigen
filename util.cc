//
// util.cc: miscellaneous utilities
// -----------------------------------

#include "util.h"

namespace sigen {

    ui32 rbits(bool reserved, ui32 mask) {
        static const ui32 w_r_bits[] = { 0x00000000, 0xffffffff };
        return w_r_bits[ reserved ] & mask;
    }

    // ---------------------------------------
    // Language Code container class
    //

    // constructor
    //
    void LanguageCode::refit()
    {
        if (code.length() > BASE_LEN)
            code = code.substr(0, BASE_LEN);
    }

} // namespace

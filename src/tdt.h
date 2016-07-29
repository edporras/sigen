//
// tdt.h: class definition for the TDT
// -----------------------------------

#pragma once

#include "utc.h"

namespace sigen {

    class TStream;

    //
    // Time and Date Section
    //
    class TDT : public STable
    {
    private:
        UTC utc;

        enum { BASE_LEN = 5,
               MAX_SEC_LEN = STable::BASE_LEN + BASE_LEN,
               TID = 0x70 };

    public:
        enum { PID = 0x14 };

        // default constructor creates a TDT with current time
        TDT() :
            STable(TID, BASE_LEN, MAX_SEC_LEN, false, true, true)
        { }

        // this one takes a UTC instance
        TDT(const UTC &t, bool rsrvd = true) :
            STable(TID, BASE_LEN, MAX_SEC_LEN, false, rsrvd, rsrvd),
            utc(t)
        { }

        // accessors
        const UTC& getUTC() const { return utc; }

        // utility
        virtual void buildSections(TStream &) const;

#ifdef ENABLE_DUMP
        void dump(std::ostream &) const;
#endif
    };

} // namespace

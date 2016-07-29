//
// tot.h: class definition for the TOT
// -----------------------------------

#pragma once

#include <list>
#include "utc.h"
#include "table.h"
#include "util.h"

namespace sigen {

    class TStream;
    class Descriptor;

    //
    // Time Offset Table
    //
    class TOT : public STable
    {
    private:
        UTC utc;

        enum { BASE_LEN = 7, MAX_SEC_LEN = 1024, TID = 0x73 };

        ui16 desc_loop_length;
        std::list<PtrWrapper<Descriptor> > desc_list; // descriptor's list

    public:
        enum { PID = 0x14 };

        // first constructor uses current time.. second uses the specific
        // time provided
        TOT(bool rsrvd = true) :
            STable(TID, BASE_LEN, MAX_SEC_LEN, false, rsrvd, rsrvd),
            desc_loop_length(0)
        { }
        TOT(const UTC &t, bool rsrvd = true) :
            STable(TID, BASE_LEN, MAX_SEC_LEN, false, rsrvd, rsrvd),
            utc(t),
            desc_loop_length(0)
        { }

        // accessors
        virtual ui16 getMaxDataLen() const;
        const UTC& getUTC() const { return utc; }

        // utility
        bool addDesc(Descriptor &);
        virtual void buildSections(TStream &) const;

#ifdef ENABLE_DUMP
        void dump(std::ostream &) const;
#endif
    };

} // namespace

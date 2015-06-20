//
// tdt.cc: class definition for the TDT
// -----------------------------------

#include <iostream>
#include "table.h"
#include "tstream.h"
#include "tdt.h"

namespace sigen
{
    //
    // writes the table to a stream
    //
    void TDT::buildSections(TStream &strm) const
    {
        Section *s = strm.getNewSection( getMaxSectionLen() );

        STable::buildSections(*s);

        s->set16Bits( utc.mjd );
        s->set08Bits( utc.time.getBCDHour() );
        s->set08Bits( utc.time.getBCDMinute() );
        s->set08Bits( utc.time.getBCDSecond() );
    }

    //
    // debug routine
    //
#ifdef ENABLE_DUMP
    void TDT::dump(std::ostream &o) const
    {
        // table header
        dumpHeader( o, TDT_DUMP_S );

        // the UTC data
        identStr(o, UTC_S);
        o << utc << std::endl;
    }
#endif

} // namespace

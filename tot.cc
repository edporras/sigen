//
// tot.cc: class definition for the TOT
// -----------------------------------

#include <iostream>
#include <list>
#include "table.h"
#include "tstream.h"
#include "tot.h"
#include "descriptor.h"

namespace sigen
{
    //
    // max data length is calculated like a PSITable because of the CRC
    //
    ui16 TOT::getMaxDataLen() const
    {
        return getMaxSectionLen() - STable::BASE_LEN - Section::CRC_LEN;
    }

    //
    // debug
#ifdef ENABLE_DUMP
    void TOT::dump(std::ostream &o) const
    {
        // the general table info
        STable::dumpHeader( o, TOT_DUMP_S );

        // the UTC data
        identStr(o, UTC_S);
        o << utc << std::endl;

        // reserved + loop length
        o << std::hex;
        identStr(o, RESERVED_FU_S, rbits(reserved, 0xf));
        identStr(o, DESC_LOOP_LEN_S, desc_loop_length & LEN_MASK, true);
        o << std::endl;

        // descriptors
        dumpDescLoop( desc_list, o );
    }
#endif


    //
    // adds a descriptor to the loop
    //
    bool TOT::addDesc(Descriptor &d)
    {
        ui16 d_len = d.length();

        // make sure we have enough room to add it
        if ( !incLength(d_len) )
            return false;

        desc_list.push_back(&d);

        // increment the table and network_desc_loop lengths
        desc_loop_length += d_len;
        return true;
    }


    //
    // writes the table to a stream
    //
    void TOT::buildSections(TStream &strm) const
    {
        Section *s = strm.getNewSection( getMaxSectionLen() );

        STable::buildSections(*s);

        // UTC data
        s->set16Bits( utc.mjd );
        s->set08Bits( utc.time.getBCDHour() );
        s->set08Bits( utc.time.getBCDMinute() );
        s->set08Bits( utc.time.getBCDSecond() );

        // reserved bits (4) and loop length (12)
        s->set16Bits( rbits(reserved, ~LEN_MASK) | (desc_loop_length & LEN_MASK) );

        // descriptors
        std::list<PtrWrapper<Descriptor> >::const_iterator desc_iter = desc_list.begin();

        while (desc_iter != desc_list.end())
            (*desc_iter++)()->buildSections(*s);

        // crc it
        s->calcCrc();
    }

} // namespace

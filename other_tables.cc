//
// other_tables.cc: class definitions for the Stuffing and
//                  Running Status tables
// -----------------------------------

#include <iostream>
#include <string>
#include <list>
#include "table.h"
#include "tstream.h"
#include "other_tables.h"

namespace sigen
{
    // ------------------------------------
    // the Running Status Table
    //

    //
    // add a transort stream to the list
    bool RST::addXportStream(ui16 xsid, ui16 onid, ui16 sid,
                             ui16 eid, ui8 rs)
    {
        if ( !incLength(XportStream::BASE_LEN) )
            return false;

        // we can fit it, so let's try to add it
        XportStream *xs = new XportStream(xsid, onid, sid, eid, rs);
        xport_stream_list.push_back( PtrWrapper<XportStream>(xs) );
        return true;
    }


    //
    // table builder function
    void RST::buildSections(TStream& strm) const
    {
        Section *s = strm.getNewSection( getMaxSectionLen() );

        STable::buildSections(*s);

        // write the transport stream data
        for ( std::list<PtrWrapper<XportStream> >::const_iterator xs_iter = xport_stream_list.begin();
              xs_iter != xport_stream_list.end();
              xs_iter++ )
        {
            const XportStream& xs = *(*xs_iter);

            s->set16Bits(xs.id);
            s->set16Bits(xs.original_network_id);
            s->set16Bits(xs.service_id);
            s->set16Bits(xs.event_id);
            s->set08Bits( rbits(reserved, 0xf8) | (xs.running_status & 0x7) );
        }
    }


    //
    // debug
#ifdef ENABLE_DUMP
    void RST::dump(std::ostream& o) const
    {
        // table header
        dumpHeader( o, RST_DUMP_S );
        o << std::endl;

        // dump the transport streams
        incOutLevel();
        for ( std::list<PtrWrapper<XportStream> >::const_iterator xs_iter = xport_stream_list.begin();
              xs_iter != xport_stream_list.end();
              xs_iter++ )
        {
            const XportStream& xs = *(*xs_iter);

            headerStr(o, XPORT_STREAM_S, false);

            identStr(o, XPORT_STREAM_ID_S, xs.id);
            identStr(o, ORIG_NETWORK_ID_S, xs.original_network_id);
            identStr(o, SERVICE_ID_S, xs.service_id, true);
            identStr(o, EVENT_ID_S, xs.event_id);
            identStr(o, RUNNING_STATUS_S, (ui16) xs.running_status, true);

            o << std::endl;
        }
        decOutLevel();
        o << std::endl;
    }
#endif



    // ------------------------------------
    // the Stuffing Table
    //

    Stuffing::Stuffing(const std::string& d, bool ssi, bool rsrvd) :
        STable(TID, BASE_LEN, MAX_SEC_LEN, ssi, rsrvd, rsrvd),
        data( d )
    {
        // !!! check to make sure it fits!
    }


    Stuffing::Stuffing(ui8 d, ui16 len, bool ssi, bool rsrvd) :
        STable(TID, BASE_LEN, MAX_SEC_LEN, ssi, rsrvd, rsrvd),
        data( std::string(d, len) )
    {
        // !!! check to make sure it fits!
    }


    //
    // the actual table builder
    //
    void Stuffing::buildSections(TStream& strm) const
    {
        Section *s = strm.getNewSection( getMaxSectionLen() );

        STable::buildSections(*s);

        s->setBits( data );
    }


    //
    // debug
    //
#ifdef ENABLE_DUMP
    void Stuffing::dump(std::ostream& o) const
    {
        dumpHeader( o, ST_DUMP_S );
        o << std::endl;

        dumpData(o, (ui8 *) data.c_str(), getDataLength());
        o << std::endl;
    }
#endif

} // namespace

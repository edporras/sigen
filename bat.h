//
// bat.h: class definition for the BAT
//
// -----------------------------------

#ifndef SIGEN_BAT_H
#define SIGEN_BAT_H

#include <list>
#include <sigen/table.h>
#include <sigen/util.h>

namespace sigen {

    class Descriptor;

    //
    // the BAT class
    //
    class BAT : public PSITable
    {
    private:
        enum { BASE_LEN = 9, TID = 0x4a, MAX_SEC_LEN = 1024 };

        // the transport stream struct
        struct XportStream
        {
            enum { BASE_LEN = 6 };

            ui16 id,                    // transport stream id
                original_network_id,
                desc_length;
            std::list<PtrWrapper<Descriptor> > desc_list;  // descriptors

            // constructor
            XportStream(ui16 tsid, ui16 onid) :
                id(tsid), original_network_id(onid), desc_length(0) { }

            // utility methods
            int length() const { return desc_length + BASE_LEN; }

            XportStream *clone() const { return new XportStream(*this); }
        };

        // BAT members
        ui16 bouquet_desc_length,
            xport_stream_loop_length;

        std::list<PtrWrapper<Descriptor> > bouquet_desc;   // bouquet descriptors
        std::list<PtrWrapper<XportStream> > xport_streams; // transport streams

    public:
        enum { PID = 0x11 };

        // constructor
        BAT(ui16 bouquet_id, ui8 ver, bool cni = true, bool rsrvd = true) :
            PSITable(TID, bouquet_id, BASE_LEN, MAX_SEC_LEN, ver, cni,
                     rsrvd, rsrvd),
            bouquet_desc_length(0),
            xport_stream_loop_length(0)
        { }

        // utility
        bool addBouquetDesc(Descriptor &);
        bool addXportStream(ui16 xs_id, ui16 on_id);
        bool addXportStreamDesc(ui16 xs_id, ui16 on_id, Descriptor &);
        bool addXportStreamDesc(Descriptor &); // adds it to the last stream added

        // dump to stdout routines
#ifdef ENABLE_DUMP
        virtual void dump(std::ostream &) const;
#endif

    protected:
        bool addXportStreamDesc(XportStream& , Descriptor &, ui16);
        virtual bool writeSection(Section& , ui8, ui16 &) const;
        bool writeXportStream(Section& , const XportStream& , ui16 &, ui16 &) const;

#ifdef ENABLE_DUMP
        void dumpXportStreams(std::ostream &) const;
#endif
    };

} // sigen namespace

#endif

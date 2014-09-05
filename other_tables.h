//
// other_tables.h: class definitions for the Running Status
//                 and Stuffing tables
// -----------------------------------

#ifndef SIGEN_OTHER_TABLES_H
#define SIGEN_OTHER_TABLES_H

#include <list>
#include <string>
#include <sigen/table.h>

namespace sigen {

    class TStream;
    class Section;

    //
    // Running Status Table
    //
    class RST : public STable
    {
    private:
        enum { BASE_LEN = 0, MAX_SEC_LEN = 1024, TID = 0x71 };

        // the private transport stream class
        struct XportStream {
            enum { BASE_LEN = 9 };

            ui16 id,
                original_network_id,
                service_id,
                event_id;
            ui8 running_status : 3;

            // constructor
            XportStream(ui16 xsid, ui16 onid, ui16 sid, ui16 eid, ui8 rs) :
                id(xsid), original_network_id(onid),
                service_id(sid), event_id(eid), running_status(rs) {}

            XportStream* clone() const { return new XportStream(*this); }
        };

        // the list of transport streams
        std::list<PtrWrapper<XportStream> > xport_stream_list;

    public:
        enum { PID = 0x13 };

        RST(bool rsrvd = true) :
            STable(TID, BASE_LEN, MAX_SEC_LEN, false, rsrvd, rsrvd) {}

        // utility
        bool addXportStream(ui16 xsid, ui16 onid, ui16 sid,
                            ui16 eid, ui8 rs);
        virtual void buildSections(TStream&) const;

#ifdef ENABLE_DUMP
        void dump(std::ostream&) const;
#endif
    };



    //
    // Stuffing Table
    //
    class Stuffing : public STable
    {
    private:
        std::string data;

        enum { BASE_LEN = 0, MAX_SEC_LEN = 4096, TID = 0x72 };

    public:
        // constructor for NULL terminated data
        Stuffing(const std::string& data, bool ssi = true, bool rsrvd = true);
        // constructor where we replicate the passed data
        Stuffing(ui8 data, ui16 len, bool ssi = true, bool rsrvd = true);

        // accessors
        const std::string& getDataByte() const { return data; }

        // utility
        virtual void buildSections(TStream&) const;

#ifdef ENABLE_DUMP
        void dump(std::ostream&) const;
#endif
    };

} // namespace

#endif

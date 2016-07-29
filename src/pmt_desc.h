//
// pmt_desc.h: class definition for the DVB defined PMT descriptors
// -----------------------------------

#pragma once

#include <list>
#include "descriptor.h"
#include "util.h"

namespace sigen {

    // ---------------------------
    // AC-3 Descriptor (EN 300 468 v1.4.1, Annex E)
    //
    class AC3Desc : public Descriptor
    {
    public:
        enum { TAG = 0x6a, BASE_LEN = 1 };

        // keys for the possible fields that can be set in this descriptor
        enum flag_t {
            AC3TYPE, BSID, MAINID, ASVC,
            VALUE_COUNT // internal use
        };

        struct entry {
            bool is_set;
            ui8 value;

            entry() : is_set(false) { }
        };


        // constructor
        AC3Desc(const std::string info = "", bool rsrvd = true) :
            Descriptor(TAG, BASE_LEN),
            additional_info( incLength(info) ),
            reserved( rsrvd )
        { }

        // utility
        void setValue(flag_t key, ui8 value);

        virtual Descriptor* clone() const {
            return new AC3Desc( *this );
        }

        virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
        virtual void dump(std::ostream& o) const;
#endif

    private:

#ifdef ENABLE_DUMP
        // define the STRIDs to use for the flags & values in dump.[ch] and
        // set the ones to use in pmt_desc.c
        static STRID flag_strid[VALUE_COUNT];
        static STRID value_strid[VALUE_COUNT];
#endif

        entry field[ VALUE_COUNT ];
        std::string additional_info;
        bool reserved;
    };



    // ---------------------------
    // Ancillary Data Descriptor- derived from PrimitiveDatatypeDesc
    // which handles buildingSections
    //
    //  ui8 data represents ancillary_data_identifier
    //
    class AncillaryDataDesc : public PrimitiveDatatypeDesc<ui8>
    {
    public:
        enum { TAG = 0x6b }; // BASE_LEN implictly 1 (ui8)

        // ancillary data identifier values
        enum { DVD_VIDEO = 0x01,
               EXTENDED = 0x02,
               ANNOUNCEMENT_SWITCHING = 0x04,
               DAB = 0x08,
               SCALE_FACTOR_ERROR_CHECK = 0x10,
        };

        // constructor
        AncillaryDataDesc(ui8 ancillary_data_identifier) :
            PrimitiveDatatypeDesc<ui8>(TAG, ancillary_data_identifier) {}

        // utility
        virtual Descriptor* clone() const {
            return new AncillaryDataDesc( *this );
        }

#ifdef ENABLE_DUMP
        virtual void dump(std::ostream& o) const {
            dumpData( o, ANCILLARY_DATA_D_S, ANCILLARY_DATA_IDENT_S );
        }
#endif
    };


    // ---------------------------
    // Data Broadcast Id Descriptor
    //
    class DataBroadcastIdDesc : public Descriptor
    {
    private:
        ui16 data_broadcast_id;
        std::vector<ui8> selector_bytes;

    public:
        enum { TAG = 0x66, BASE_LEN = 2 };

        enum {
            DATA_PIPE = 0x0001,
            ASYNC_DATA_STREAM = 0x0002,
            SYNCRONOUS_DATA_STREAM = 0x0003,
            SYNCHONISED_DATA_STREAM = 0x0004,
            DATA_MULTI_PROTOCOL_ENCAP = 0x0005,
            DATA_CAROUSEL = 0x0006,
            OBJECT_CAROUSEL = 0x0007,
            DVB_ATM_STREAMS = 0x0008,
            HIGHER_PROT_BASED_ON_ASYNC_DATA_STREAMS = 0x0009,
        };

        // constructor
        DataBroadcastIdDesc(ui16 db_id, const std::string& sel_bytes = "") :
            Descriptor(TAG, BASE_LEN),
            data_broadcast_id(db_id)
        {}

        // utility
        virtual Descriptor* clone() const {
            return new DataBroadcastIdDesc( *this );
        }

        // set private data bytes
        bool setSelectorBytes(const std::vector<ui8>& bytes);

        virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
        virtual void dump(std::ostream& o) const;
#endif
    };



    // ---------------------------
    // Service Move Descriptor
    //
    class ServiceMoveDesc : public Descriptor
    {
    private:
        ui16 original_network_id,
            xport_stream_id,
            new_service_id;

    public:
        enum { TAG = 0x60, BASE_LEN = 6 };

        // constructor
        ServiceMoveDesc(ui16 onid, ui16 xsid, ui16 sid) :
            Descriptor(TAG, BASE_LEN),
            original_network_id(onid), xport_stream_id(xsid), new_service_id(sid) {}

        // utility
        virtual Descriptor* clone() const { return new ServiceMoveDesc(*this); }
        virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
        virtual void dump(std::ostream&) const;
#endif
    };



    // ---------------------------
    // Stream Identifier Descriptor- derived from PrimitiveDatatypeDesc
    // which handles buildingSections
    //
    //  ui8 data represents component_tag
    //
    class StreamIdentifierDesc : public PrimitiveDatatypeDesc<ui8>
    {
    public:
        enum { TAG = 0x52 }; // BASE_LEN implictly 1 (ui8)

        // constructor
        StreamIdentifierDesc(ui8 c_tag) : PrimitiveDatatypeDesc<ui8>(TAG, c_tag) {}

        // utility
        virtual Descriptor* clone() const {
            return new StreamIdentifierDesc( *this );
        }

#ifdef ENABLE_DUMP
        virtual void dump(std::ostream& o) const {
            dumpData( o, STREAM_IDENT_D_S, COMPONENT_TAG_S );
        }
#endif
    };


    // ---------------------------
    // Subtitling Descriptor
    //
    class SubtitlingDesc : public Descriptor
    {
    public:
        enum { TAG = 0x59, BASE_LEN = 0 };

        // private subtitling structure
        struct Subtitling
        {
            enum { BASE_LEN = 8 };

            // data
            LanguageCode language_code;
            ui16 composition_page_id,
                ancillary_page_id;
            ui8 type;

            Subtitling( const std::string code, ui8 st, ui16 cpid, ui16 apid ) :
                language_code(code), composition_page_id(cpid),
                ancillary_page_id(apid), type(st) { }
        };


        // constructor
        SubtitlingDesc() : Descriptor(TAG, BASE_LEN) {}

        // utility
        virtual Descriptor* clone() const { return new SubtitlingDesc(*this); }
        bool addSubtitling(const std::string& lang_code, ui8 type, ui16 c_page_id,
                           ui16 a_page_id);
        virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
        virtual void dump(std::ostream&) const;
#endif

    private:
        std::list<Subtitling> subtitling_list;
    };



    // ---------------------------
    // Teletext Descriptor
    //
    class TeletextDesc : public Descriptor
    {
    public:
        enum { TAG = 0x56, BASE_LEN = 0 };

        // private teletext structure
        struct Teletext
        {
            enum { BASE_LEN = 5 };

            // data
            LanguageCode language_code;
            ui8 page_number,
                type : 5,
                magazine_number : 3;

            Teletext(const std::string& code, ui8 t, ui8 mn, ui8 pn) :
                language_code(code), page_number(pn), type(t),
                magazine_number(mn) { }
        };


        // defined teletext types
        enum Type {
            INITIAL = 0x01,
            SUBTITLE = 0x02,
            ADDTNL_INFO = 0x03,
            PROG_SCHED = 0x04
        };

        // contructor
        TeletextDesc() : Descriptor(TAG, BASE_LEN) {}

        // utility
        virtual Descriptor* clone() const { return new TeletextDesc(*this); }
        bool addTeletext(const std::string& lang_code, ui8 type, ui8 mag_num,
                         ui8 page_num);
        virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
        virtual void dump(std::ostream&) const;
#endif

    private:
        std::list<Teletext> teletext_list;
    };

} // sigen namespace

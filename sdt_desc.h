//
// sdt_desc.h: class definition for the SDT descriptors
// -----------------------------------

#ifndef SIGEN_SDT_DESC_H
#define SIGEN_SDT_DESC_H

#include <string>
#include <list>
#include <sigen/util.h>
#include <sigen/descriptor.h>

namespace sigen {

    // ---------------------------
    // Multilingual Service Name Descriptor
    //
    class MultilingualServiceNameDesc : public Descriptor
    {
    public:
        enum { TAG = 0x5d, BASE_LEN = 0 };

        struct Language {
            enum { BASE_LEN = LanguageCode::BASE_LEN + 2 };

            // data
            LanguageCode language_code;
            std::string provider_name,
                service_name;

            // contructor
            Language(const std::string& code, const std::string& pn,
                     const std::string& sn) :
                language_code(code), provider_name(pn), service_name(sn) {}

            // utility
            ui8 length() const { return provider_name.length() +
                    service_name.length() + BASE_LEN; }
        };

        // constructor
        MultilingualServiceNameDesc() : Descriptor(TAG, BASE_LEN) {}

        // utility
        virtual Descriptor *clone() const {
            return new MultilingualServiceNameDesc(*this);
        }
        bool addLanguage(const std::string& code, const std::string& prov_name,
                         const std::string& serv_name);

        virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
        virtual void dump(std::ostream&) const;
#endif

    private:
        // the list of language structs
        std::list<Language> language_list;
    };



    // ---------------------------
    // NVOD Reference Descriptor
    //
    class NVODReferenceDesc : public Descriptor
    {
    public:
        enum { TAG = 0x4b, BASE_LEN = 0 };

        struct Ident {
            enum { BASE_LEN = 6 };

            // data
            ui16 xport_stream_id,
                original_network_id,
                service_id;

            // constructor
            Ident(ui16 xsid, ui16 onid, ui16 sid) :
                xport_stream_id(xsid), original_network_id(onid), service_id(sid) {}
        };

        // constructor
        NVODReferenceDesc() : Descriptor(TAG, BASE_LEN) {}

        // utility
        virtual Descriptor* clone() const { return new  NVODReferenceDesc(*this); }
        bool addIdentifiers(ui16 xsid, ui16 onid, ui16 sid);

        virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
        virtual void dump(std::ostream&) const;
#endif

    private:
        // the list of ident structs
        std::list<Ident> ident_list;
    };



    // ---------------------------
    // Service Descriptor
    //
    class ServiceDesc : public Descriptor
    {
    private:
        std::string provider_name,
            name;
        ui8 type;

    public:
        enum { TAG = 0x48, BASE_LEN = 3 };

        // constructor
        ServiceDesc(ui8 t, const std::string& pn, const std::string& n) :
            Descriptor(TAG, BASE_LEN),
            provider_name( incLength( pn ) ),
            name( incLength(n) ),
            type(t)
        { }

        // utility
        Descriptor *clone() const { return new ServiceDesc(*this); }
        virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
        virtual void dump(std::ostream&) const;
#endif
    };


    // ---------------------------
    // Time Shifted Service Descriptor - derived from
    // PrimitiveDatatypeDesc which handles buildingSections
    //
    //  ui16 data represents reference_service_id
    //
    class TimeShiftedServiceDesc : public PrimitiveDatatypeDesc<ui16>
    {
    public:
        enum { TAG = 0x4c }; // BASE_LEN implicitly is 2 (ui16)

        // constructor
        TimeShiftedServiceDesc(ui16 sid) :
            PrimitiveDatatypeDesc<ui16>(TAG, sid) {}

        // utility
        Descriptor *clone() const { return new TimeShiftedServiceDesc( *this ); }

#ifdef ENABLE_DUMP
        virtual void dump(std::ostream& o) const {
            dumpData( o, TIME_SHIFTED_SERV_D_S, REF_SID_S, true);
        }
#endif
    };

} // sigen namespace

#endif

//
// descriptor.h: base descriptor & stream descriptor classes
// -----------------------------------

#ifndef SIGEN_DESCRIPTOR_H
#define SIGEN_DESCRIPTOR_H

//
// IMPORTANT: note that for efficiency all tables store their
// descriptors in linked-lists of POINTERS (wrapped via PtrWrapper).
// For this reason, when adding descriptors to a table it is necessary
// to 'new' them so they are not lost once execution goes out of the
// current scope
//
// Also, it is NOT necessary to delete them.  Any table that maintains
// a list of descriptors will take care of this itself.
//

#include <list>
#include <string>
#include <sigen/table.h>
#include <sigen/tstream.h>

namespace sigen {

    // ---------------------------
    // abstract base descriptor class
    //
    class Descriptor : public Table
    {
    private:
        ui8 tag;
        ui16 total_length; // 8-bit field but stored wider for computations

    protected:
        enum { BASE_LEN = 2, MAX_LEN = 255, CAPACITY = MAX_LEN + BASE_LEN };

        // protected constructor for derived classes
        Descriptor(ui8 t, ui8 l) :
            tag(t), total_length(l + BASE_LEN) { }

    public:
        virtual ~Descriptor() { }

        // getter methods
        ui16 length() const { return total_length; } // total bytes

        // utility methods
        virtual Descriptor* clone() const = 0;
        virtual void buildSections(Section &) const;

    protected:
        // methods to be used by the derived descriptor classes
#ifdef ENABLE_DUMP
        virtual void dumpHeader(std::ostream &, STRID tag) const;
#endif

        // checks if the given len argument fits
        bool lengthFits(ui16 len) const { return (total_length + len < CAPACITY); }
        // increments the length of the desc based on the given size
        bool incLength(ui8 len);
        // increments the length of the desc based on the given string's length
        std::string incLength(const std::string &str);
    };



    // ---------------------------
    // generic abstract decriptor for descriptors that have one primitive
    // field (ui32, ui16, ui8) as the only member
    //
    template <class T>
    class PrimitiveDatatypeDesc : public Descriptor
    {
    private:
        // the data stored - depending on the derived class, it'll
        // instantiate this to be a ui32, ui16, etc
        T data;

        enum { BASE_LEN = sizeof(T) };

    protected:
        // constructor
        PrimitiveDatatypeDesc(ui8 tag, const T &d) :
            Descriptor(tag, BASE_LEN),
            data(d) {}

        // utility functions
        virtual void buildSections(Section &s) const {
            Descriptor::buildSections(s);
            s.setBits( data );
        }

#ifdef ENABLE_DUMP
        // handles dumping data with correct tags
        void dumpData(std::ostream &o, STRID desc_type, STRID data_type,
                      bool dechex = false) const {
            dumpHeader(o, desc_type);
            identStr( o, data_type, data, dechex );
        }
#endif
    };


    // ---------------------------
    // String Data Descriptor - for descriptors that store data in a
    // variable length sequence of characsters
    //
    class StringDataDesc : public Descriptor
    {
    private:
        enum { BASE_LEN = 0 };
        std::string data;

    protected:
        // constructor
        StringDataDesc(ui8 tag, const std::string &d) :
            Descriptor(tag, BASE_LEN),
            data( incLength(d) ) {
#ifdef SHOW_MEM_MGMT
            cerr << "building: \"" << data << "\"" << endl;
#endif
        }

#ifdef SHOW_MEM_MGMT
        StringDataDesc(const StringDataDesc &d) : data(d.data) {
            cerr << "copying :\"" << data << "\"" << endl; }
        StringDataDesc &operator=(const StringDataDesc &d)  {
            if (this != &d) data = d.data;
            cerr << "assigning :\"" << data << "\"" << endl; return *this; }
        ~StringDataDesc() { cerr << "freeing: \"" << data << "\"" << endl; }
#endif

        // utility functions
        virtual void buildSections(Section &s) const {
            Descriptor::buildSections(s);
            s.setBits( data );
        }

#ifdef ENABLE_DUMP
        void dumpData(std::ostream &o, STRID desc_type, STRID data_type) const {
            dumpHeader( o, desc_type );
            identStr(o, data_type, data );
        }
#endif
    };


    // ---------------------------
    // abstract Multilingual Text Descriptor base class
    //
    class MultilingualTextDesc : public Descriptor
    {
    public:
        // general struct for data in loops
        struct Text {
            enum { BASE_LEN = 4 };

            // data
            LanguageCode code;
            std::string data;

            // constructor
            Text(const LanguageCode& c, const std::string &t) : code(c), data(t) { }

            ui16 length() const { return BASE_LEN + data.length(); }
        };

    protected:
        // descriptor data members begin here
        std::list<Text> ml_text_list;

        // protected constructor - only derived classes can build this
        MultilingualTextDesc(ui8 tag, ui8 base_len) : Descriptor(tag, base_len) {}

#ifdef ENABLE_DUMP
        // make it a pure virtual method to disallow instantiation of this
        // class
        virtual void dump(std::ostream &) const = 0;
        // used by the derived classes
        void dumpData(std::ostream &o, STRID desc, STRID data_type) const {
            dumpHeader(o, desc);
            dumpTextLoop(o, data_type);
        }
        void dumpTextLoop(std::ostream &o, STRID data_type)  const;
#endif

        void buildLoopData(Section &) const;

    public:
        // utility
        bool addLanguage(const LanguageCode& lang, const std::string &code);
        virtual void buildSections(Section &s) const {
            Descriptor::buildSections(s);
            buildLoopData(s);
        }
    };

} // sigen namespace

#endif

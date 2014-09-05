//
// tstream.h: definitions for the transport stream object
// and transport stream section object.
// -----------------------------------

#ifndef SIGEN_TSTREAM_H
#define SIGEN_TSTREAM_H

#include <string>
#include <list>
#include <vector>
#include <sigen/types.h>

namespace sigen {

    class LanguageCode;


    //
    // buffer object for storing the transport stream table
    // sections
    //
    class Section
    {
    private:
        ui8 *pos,
            *data;
        ui32 crc;
        ui16 data_length;
        const ui16 size; // max size of the section (set at construction)

        // checks if len bytes can fit
        bool lengthFits(ui16 len) const { return ((data_length + len) <= size); }

        // prohibit
        Section(const Section &);
        Section &operator=(const Section &);

    public:
        enum { CRC_LEN = 4 };

        // constructor / destructor
        Section(ui16 section_size);
        ~Section() { delete [] data; }

        // accessors
        const ui8 *getBinaryData() const { return data; }
        ui16 length() const { return data_length; }
        ui16 capacity() const { return size; }

        ui8 *getCurDataPosition() const { return pos; }
        ui32 getCRC() const { return crc; }

        // utility
        bool set08Bits(ui8 data);
        bool set16Bits(ui16 data);
        bool set24Bits(const ui8 *data); // copies from an array
        bool set24Bits(ui32 d);          // copies the lower 24 bits (0x00nnnnnn)
        bool set32Bits(ui32 data);

        // overloaded equivalent
        bool setBits(ui8 data)  { return set08Bits(data); }
        bool setBits(ui16 data) { return set16Bits(data); }
        bool setBits(ui32 data) { return set32Bits(data); }
        bool setBits(const std::string &data);
        bool setBits(const LanguageCode &code);
        bool setBits(const std::vector<ui8> &v);

        // sets data without incrementing pointer
        bool set08Bits(ui8 idx, ui8 data);
        bool set16Bits(ui8 *pos, ui16 data);
        bool set16Bits(ui16 idx, ui16 data);

        void write(std::ostream &) const;
        bool calcCrc();

#ifdef ENABLE_DUMP
        void dump(std::ostream &) const;
#endif
    };


    //
    // the output class
    //
    class TStream
    {
    private:
        // prohibit
        TStream(const TStream &);
        TStream &operator=(const TStream &);

    public:
        TStream() { }
        ~TStream();

        // the linked-list of sections
        std::list<Section *> section_list;

        // accessors
        ui16 getNumSections() const { return section_list.size(); }

        // allocates a new section of 'section_size' bytes
        Section *getNewSection(ui16 section_size);

        // create the stream and save it to a file
        void write(const std::string &file_name) const;

#ifdef ENABLE_DUMP
        void dump(std::ostream &) const;
#endif
    };

} // sigen namespace

#endif

// Copyright 1999-2022 Ed Porras
//
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use, copy,
// modify, merge, publish, distribute, sublicense, and/or sell copies
// of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
// BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// tstream.h: definitions for the transport stream object
// and transport stream section object.
// -----------------------------------

#pragma once

#include <string>
#include <list>
#include <vector>
#include "types.h"

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

   public:
      enum { CRC_LEN = 4 };

      // constructor / destructor
      Section(ui16 section_size);
      ~Section() { delete [] data; }
      // prohibit
      Section(const Section &) = delete;
      Section(const Section &&) = delete;
      Section &operator=(const Section &) = delete;
      Section &operator=(const Section &&) = delete;

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

   /*!
    * \brief Stream output class.
    */
   class TStream
   {
   public:
      //! \brief Constructor.
      TStream() { }
      //! \brief Destructor.
      ~TStream();

      // prohibit
      TStream(const TStream &) = delete;
      TStream(const TStream &&) = delete;
      TStream &operator=(const TStream &) = delete;
      TStream &operator=(const TStream &&) = delete;

      // the linked-list of sections
      std::list<Section *> section_list;

      // accessors
      ui16 getNumSections() const { return section_list.size(); }

      // allocates a new section of 'section_size' bytes
      Section *getNewSection(ui16 section_size);

      /*!
       * \brief Write the section data to a file with the specified
       * name.
       * \param file_name File name to write data to.
       */
      void write(const std::string &file_name) const;

#ifdef ENABLE_DUMP
      void dump(std::ostream &) const;
#endif
   };

} // sigen namespace

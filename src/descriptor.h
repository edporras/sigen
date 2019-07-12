// Copyright 1999-2019 Ed Porras
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
// descriptor.h: base descriptor & stream descriptor classes
// -----------------------------------

#pragma once

#include <list>
#include <string>
#include "language_code.h"
#include "table.h"
#include "tstream.h"

namespace sigen {

   /*!
    * \addtogroup abstract
    * @{
    */

   /*!
    * \brief Abstract base class for all descriptor types.
    *
    * \attention Unlike tables, all Descriptor subclasses must be
    * allocated by the caller using the `new` operator and fully
    * populated before adding them to a table instance. Once added,
    * the table claims ownership of the descriptor pointer and will
    * handle deletion.
    *
    * \warning Note that once added, the descriptor must not
    * be modified by the caller (in essence, the pointer is now
    * invalid).
    */
   class Descriptor : public Table
   {
   public:
      virtual ~Descriptor() { }

      //! \brief Returns the total length of the descriptor data.
      ui16 length() const { return total_length; }

      //! \internal
      //! \brief  Write data bytes to the section. Used by the sectionizer.
      virtual void buildSections(Section &s) const;

   private:
      ui16 total_length; // 8-bit field but stored wider for computations

   protected:
      const ui8 tag;

      enum { MAX_LEN = 255, CAPACITY = 257 };

      // protected constructor for derived classes
      Descriptor(ui8 t, ui8 l = 0) :
         Table(l + 2),
         total_length(BASE_LENGTH), tag(t) { }

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
   protected:
      // constructor
      PrimitiveDatatypeDesc(ui8 tag, const T &d) :
         Descriptor(tag, sizeof(T)),
         data(d) {}
      PrimitiveDatatypeDesc() = delete;

      // utility functions
      virtual void buildSections(Section &s) const {
         Descriptor::buildSections(s);
         s.setBits( data );
      }

#ifdef ENABLE_DUMP
      // handles dumping data with correct tags
      void dumpData(std::ostream &o, STRID desc_type, STRID data_type) const {
         dumpHeader(o, desc_type);
         identStr(o, data_type, data);
      }
#endif

   private:
      // the data stored - depending on the derived class, it'll
      // instantiate this to be a ui32, ui16, etc
      T data;
   };


   // ---------------------------
   // String Data Descriptor - for descriptors that store data in a
   // variable length sequence of characsters
   //
   class StringDataDesc : public Descriptor
   {
   protected:
      // constructor
      StringDataDesc(ui8 tag, const std::string &str) :
         Descriptor(tag, 0),
         data( incLength(str) ) {
      }

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

   private:
      std::string data;
   };


   /*!
    * \brief Abstract Multilingual Text Descriptor base class.
    */
   class MultilingualTextDesc : public Descriptor
   {
   public:
      /*!
       * \brief Add a text entry with an associated language code to the descriptor.
       * \param lang Language code as per ISO 639-2.
       * \param text Text to add.
       */
      bool addText(const LanguageCode& lang, const std::string& text);

      [[deprecated("replaced by addText()")]] bool addLanguage(const LanguageCode& lang,
                                                               const std::string& text) {
         return addText(lang, text);
      }

      virtual void buildSections(Section &s) const {
         Descriptor::buildSections(s);
         buildLoopData(s);
      }

   protected:
      // general struct for data in loops
      struct Text : public STable::ListItem {
         enum { BASE_LEN = 4 };

         // data
         LanguageCode code;
         std::string data;

         // constructor
         Text(const LanguageCode& c, const std::string &t) : code(c), data(t) { }
         Text() = delete;

         ui16 length() const { return BASE_LEN + data.length(); }
      };

      // descriptor data members begin here
      std::list<Text> ml_text_list;

      // protected constructor - only derived classes can build this
      MultilingualTextDesc(ui8 tag, ui8 base_len = 0) : Descriptor(tag, base_len) {}

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
   };

   //! @}

} // sigen namespace

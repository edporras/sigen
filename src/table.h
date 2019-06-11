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
// table.h: abstract base definitions for the table classes
// -----------------------------------

#pragma once

#include <memory>
#include <list>
#include "types.h"
#include "util.h"
#include "dump.h"

namespace sigen {

   class TStream;
   class Section;
   class Descriptor;

   //
   // abstract superclass for both tables and descriptors
   struct Table {
      virtual ~Table() { }

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream &) const = 0;
      virtual void dumpHeader(std::ostream &, STRID) const = 0;
#endif
   };

   //
   // abstract stream table class
   //
   class STable : public Table
   {
   private:
      ui16 max_section_length;        // the maximum length of each sub-section
      // (specified by the class' MAX_SEC_LEN)
      ui8 id;
      bool section_syntax_indicator,
         private_bit;                // reserved_future_use in some

      ui16 length;                    // data length (not including CRC and
      // 3-byte header (STable::BASE_LEN))

   protected:
      bool reserved;                   // state of the reserved flags

      enum { BASE_LEN = 3,
             LEN_MASK = 0x0fff,
             MAX_TABLE_LEN = 65536 }; // max size for storage of total table data
      // (pre section split)

      // protected constructors for derived classes
      STable(ui8 tid, ui8 min_len, ui16 max_len, bool ssi, bool data_bit,
             bool rsrvd) :
         max_section_length(max_len),
         id(tid),
         section_syntax_indicator(ssi),
         private_bit(data_bit),
         length(min_len),
         reserved(rsrvd)
      { }

   public:
      // contains a list of descriptors and tracks the data
      // length. Handles taking ownership of the descriptor pointer to
      // auto-delete when table goes out of scope.
      class DescListItem
      {
         ui16 d_length = 0;
         std::list<std::unique_ptr<Descriptor> > d_list;

      public:
         void add(Descriptor& d, ui16 data_len);
         const std::list<std::unique_ptr<Descriptor> >& list() const { return d_list; }
         ui16 loop_length() const { return d_length; }

         bool empty() const { return d_list.empty(); }
         const std::unique_ptr<Descriptor>& front() const { return d_list.front(); }
         std::list<std::unique_ptr<Descriptor> >::const_iterator begin() const { return d_list.begin(); }
         std::list<std::unique_ptr<Descriptor> >::const_iterator end() const { return d_list.end(); }

         // only writes data loop - not length as it depends on the table
         void buildSections(Section &s) const;
      };

      virtual ~STable() {}

      // prohibit
      STable(const STable&) = delete;
      STable(const STable&&) = delete;
      STable& operator=(const STable&) = delete;
      STable& operator=(const STable&&) = delete;

      // accessors
      ui8 getId() const { return id; }
      ui16 getMaxSectionLen() const { return max_section_length; }
      ui16 getDataLength() const { return length; }

      // utility
      // max section length is reduced by the offset set with this
      void setSectionReservedLen(ui8 l) { max_section_length -= l; }
      // override max_section_length with this
      void setMaxSectionLen(ui16 l) { max_section_length = l; }

      virtual void buildSections(TStream &) const = 0;

   protected:
      // used by the derived tables to check for available space for data
      virtual ui16 getMaxDataLen() const {
         return max_section_length - BASE_LEN;
      }

      void buildSections(Section &) const;
      ui16 buildLengthData(ui16) const;

      bool lengthFits(ui32 l) const {
         return (static_cast<ui32>(length) + l < MAX_TABLE_LEN);
      }
      bool incLength(ui32 l);

#ifdef ENABLE_DUMP
      virtual void dumpHeader(std::ostream &o, STRID) const;
      static void dumpDescLoop(const std::list<std::unique_ptr<Descriptor> > &,
                               std::ostream &);
#endif
   };


   //
   // base class for large tables (those that can be split into multiple
   // sections (such as the PSI tables: pat, pmt, cat, nit)
   //
   class PSITable : public STable
   {
   private:
      ui16 table_id_extension;        // id extension for private tables
      ui8 version_number : 5;         // ver_num (5)
      bool current_next_indicator;    // cur_next (1)

   public:
      virtual void buildSections(TStream &) const;

      ui8 getVersionNumber() const { return version_number; }
      ui8 getCurrentNextIndicator() const { return current_next_indicator; }

      void setVersionNumber(ui8 v) { version_number = v; }
      void incVersionNumber() { version_number++; }
      void setCurrentNextIndicator(bool cni) { current_next_indicator = cni; }

   protected:
      PSITable(ui8 tid, ui16 tid_ext, ui8 min_len, ui16 max_sec_len,
               ui8 ver, bool cni, bool data_bit, bool rsrvd) :
         STable(tid, min_len, max_sec_len, true, data_bit, rsrvd),
         table_id_extension(tid_ext),
         version_number(ver),
         current_next_indicator(cni)
      { }
      virtual ~PSITable() { }

      // utility
      virtual ui16 getMaxDataLen() const;

      void writeSectionHeader(Section &) const;
      virtual bool writeSection(Section& , ui8, ui16 &) const = 0;

#ifdef ENABLE_DUMP
      virtual void dumpHeader(std::ostream &o, STRID table_label,
                              STRID ext_label, bool hexdec = false) const;
#endif
   };

} // namespace

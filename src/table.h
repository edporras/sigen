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
// table.h: abstract base definitions for the table classes
// -----------------------------------

#pragma once

#include <memory>
#include <list>
#include "types.h"
#include "dump.h"

namespace sigen {

   class TStream;
   class Section;
   class Descriptor;

   /*!
    * \defgroup abstract Abstract base classes
    * \defgroup table Tables
    * \defgroup descriptor Descriptors
    */

   /*! \ingroup abstract
    *  @{
    */

   /*!
    * \brief Abstract base class for all tables and descriptors.
    */
   struct Table {
#ifdef ENABLE_DUMP
      virtual void dump(std::ostream& o) const = 0;
      virtual void dumpHeader(std::ostream& o, STRID) const = 0;

      friend std::ostream& operator<<(std::ostream& o, const Table& t) {
         t.dump(o);
         return o;
      }
#endif
      // for lists of inner structs
      struct ListItem {
         ListItem() = default;

         ListItem(const ListItem&) = delete;
         ListItem(const ListItem&&) = delete;
         ListItem operator=(const ListItem&) = delete;
         ListItem operator=(const ListItem&&) = delete;
      };

   protected:
      const ui16 BASE_LENGTH;

      Table(ui16 base_len) : BASE_LENGTH(base_len) {}
      virtual ~Table() { }

      // helper to fill reserved bits
      static ui32 rbits(ui32 mask);

      // prohibit
      Table(const Table&) = delete;
      Table(const Table&&) = delete;
      Table& operator=(const Table&) = delete;
      Table& operator=(const Table&&) = delete;
   };

   /*!
    * \brief Abstract class for tables.
    */
   class STable : public Table
   {
   public:
      virtual ~STable() {}

      STable() = delete;

      // accessors
      ui8 getId() const { return id; }
      ui16 getMaxSectionLen() const { return max_section_length; }
      ui16 getDataLength() const { return length; }

      // utility
      // max section length is reduced by the offset set with this
      void setSectionReservedLen(ui8 l) { max_section_length -= l; }
      // override max_section_length with this
      void setMaxSectionLen(ui16 l) { max_section_length = l; }

      /*!
       * \brief Write table data to the specified stream.
       * \param stream Stream to write section data to.
       */
      virtual void buildSections(TStream& stream) const = 0;

   protected:
      enum {
         LEN_MASK = 0x0fff,
         // TODO: technically should be 256 * max_section_length
         MAX_TABLE_LEN = 65536 // max size for storage of total table data (pre section split)
      };

      // protected constructors for derived classes
      STable(ui8 tid, ui8 min_len, ui16 max_len, bool ssi = false, bool data_bit = true) :
         Table(min_len),
         max_section_length(max_len),
         id(tid),
         section_syntax_indicator(ssi),
         private_bit(data_bit),
         length(min_len)
      { }

      // contains a list of descriptors and tracks the data
      // length. Handles taking ownership of the descriptor pointer to
      // auto-delete when table goes out of scope.
      class DescList
      {
      public:
         void add(Descriptor& d, ui16 data_len);
         const std::list<std::unique_ptr<Descriptor> >& list() const { return d_list; }
         ui16 loop_length() const { return d_length; }

         bool empty() const { return d_list.empty(); }
         const std::unique_ptr<Descriptor>& front() const { return d_list.front(); }
         std::list<std::unique_ptr<Descriptor> >::const_iterator begin() const { return d_list.begin(); }
         std::list<std::unique_ptr<Descriptor> >::const_iterator end() const { return d_list.end(); }

         // only writes data loop - not length as it depends on the table
         void buildSections(Section& s) const;
#ifdef ENABLE_DUMP
         // as buildSections, only dumps data loop
         void dump(std::ostream& o) const;
#endif

      private:
         ui16 d_length = 0;
         std::list<std::unique_ptr<Descriptor> > d_list;
      };

      // used by the derived tables to check for available space for data
      virtual ui16 getMaxDataLen() const { return max_section_length - 3; }

      void buildSections(Section& s) const;
      ui16 buildLengthData(ui16) const;

      bool lengthFits(ui32 l) const {
         return (static_cast<ui32>(length) + l < MAX_TABLE_LEN);
      }
      bool incLength(ui32 l);

#ifdef ENABLE_DUMP
      virtual void dumpHeader(std::ostream& o, STRID) const;
#endif

   private:
      ui16 max_section_length;        // the maximum length of each sub-section
                                      // (specified by the class' MAX_SEC_LEN)
      ui8 id;
      bool section_syntax_indicator;
      bool private_bit;               // reserved_future_use in some

      ui16 length;                    // data length (not including CRC and
                                      // 3-byte header)
   };


   /*!
    * \brief Abstract class for the sectionale tables.
    *
    * Base class for large tables (those that can be split into multiple
    * sections (such as the PSI tables: pat, pmt, cat, nit)
    */
   class PSITable : public STable
   {
   public:
      virtual void buildSections(TStream& ts) const;

      ui8 getVersionNumber() const { return version_number; }
      ui8 getCurrentNextIndicator() const { return current_next_indicator; }

      void setVersionNumber(ui8 v) { version_number = v; }
      void incVersionNumber() { version_number++; }
      void setCurrentNextIndicator(bool cni) { current_next_indicator = cni; }

   protected:
      PSITable(ui8 tid, ui16 tid_ext, ui8 min_len, ui16 max_sec_len,
               ui8 ver, bool cni, bool data_bit) :
         STable(tid, min_len, max_sec_len, true, data_bit),
         table_id_extension(tid_ext),
         version_number(ver),
         current_next_indicator(cni)
      { }
      virtual ~PSITable() { }

      // utility
      virtual ui16 getMaxDataLen() const;

      void writeSectionHeader(Section& s) const;
      virtual bool writeSection(Section& s, ui8, ui16& l) const = 0;

#ifdef ENABLE_DUMP
      virtual void dumpHeader(std::ostream& o, STRID table_label, STRID ext_label) const;
#endif

   private:
      ui16 table_id_extension;        // id extension for private tables
      ui8 version_number : 5;         // ver_num (5)
      bool current_next_indicator;    // cur_next (1)
   };

   //
   //
   class ExtPSITable : public PSITable
   {
   public:
      virtual ~ExtPSITable();

   protected:
      ExtPSITable(ui8 size, ui8 tid, ui16 tid_ext, ui8 min_len, ui16 max_sec_len,
                  ui8 ver, bool cni, bool data_bit = true)
         : PSITable(tid, tid_ext, min_len, max_sec_len, ver, cni, data_bit),
           items(size)
      { }

      // for inner class with descriptor lists
      struct ListItem : public STable::ListItem {
         virtual ~ListItem() {}

         DescList descriptors;

         virtual ui16 length() const = 0;
         virtual bool equals(ui16 id) const = 0;

         // controls the state machine for writing the loop's section data
         bool write_section(Section& sec, ui16 max_data_len, ui16& sec_bytes,
                            ui16* item_loop_len = nullptr) const;
         // writes item header bytes, returns num bytes written
         virtual ui8 write_header(Section& sec) const = 0;
         // writes the 2-byte desc loop len
         virtual void write_desc_loop_len(Section& sec, ui8* pos, ui16 len) const;

      protected:
         // section building state tracking
         enum State_t { INIT, WRITE_HEAD, GET_DESC, WRITE_DESC };
         mutable struct Context {
            Context() : op_state(INIT), d(nullptr) {}

            State_t op_state;
            const Descriptor* d;
            std::list<std::unique_ptr<Descriptor> >::const_iterator d_iter;
         } run;
      };

      static bool contains(const std::list<ListItem*>& list, ui16 id) {
         return (nullptr != ExtPSITable::find(list, id));
      }
      static ListItem* find(const std::list<ListItem*>& list, ui16 id);
      bool addItemDesc(std::list<ListItem*>& list, Descriptor& desc);
      bool addItemDesc(std::list<ListItem*>& list, ui16 id, Descriptor& desc);

      std::vector<std::list<ListItem*> > items;
   private:
      bool addItemDesc(ListItem* item, Descriptor& d);
   };

   //! @}

} // namespace

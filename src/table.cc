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
// table.cc: abstract base definitions for the table classes
// -----------------------------------

#include <iostream>
#include <list>
#include "types.h"
#include "table.h"
#include "descriptor.h"
#include "tstream.h"

namespace sigen
{
   // ------------------------------------
   // abstract Table class
   //

   // for setting reserved bit fields
   ui32 Table::rbits(ui32 mask) {
      return 0xffffffff & mask;
   }

   // ------------------------------------
   // abstract STable class
   //

   void STable::DescList::add(Descriptor& d, ui16 d_len)
   {
      // claim ownership of the pointer
      std::unique_ptr<Descriptor> dp;
      dp.reset(&d);
      d_list.push_back( std::move(dp) );
      d_length += d_len;
   }

   void STable::DescList::buildSections(Section &s) const
   {
      for (const std::unique_ptr<Descriptor>& dp : d_list)
         (*dp).buildSections(s);
   }


#ifdef ENABLE_DUMP
   //
   // dumps the contents of a descriptor loop
   //
   void STable::DescList::dump(std::ostream &o) const
   {
      if (empty())
         return;

      incOutLevel();

      for (const std::unique_ptr<Descriptor>& dp : d_list) {
         dp->dump(o);
         o << std::endl;
      }
      o << std::endl;
      decOutLevel();
   }
#endif

   //
   // checks if the data can fit, and if so bumps the length by the
   // amount
   bool STable::incLength(ui32 l)
   {
      if (lengthFits(l)) {
         length += l;
         return true;
      }
      return false;
   }


   //
   // displays table fields
   //
#ifdef ENABLE_DUMP
   void STable::dumpHeader(std::ostream &o, STRID table_label) const
   {
      headerStr(o, table_label, false);

      o << std::hex;
      identStr(o, TID_S, id);
      identStr(o, SECT_SYNTAX_IND_S, section_syntax_indicator);
      identStr(o, RESERVED_FU_S, private_bit);
      identStr(o, RESERVED_S, rbits(0x03));
      identStr(o, TABLE_LEN_S, length, true);
   }
#endif


   //
   // formats the length 16-bit field (common in all tables)
   //
   ui16 STable::buildLengthData(ui16 len) const {
      return (section_syntax_indicator << 15) |
         (private_bit << 14) |
         rbits(0x3000) |
         (len & LEN_MASK);
   }



   //
   // writes the common table info
   //
   void STable::buildSections(Section &s) const
   {
      s.set08Bits(id); // table id

      // derived classes will handle updating length if the table is split
      // into multiple sections
      s.set16Bits( buildLengthData(length) );
   }


   // ------------------------------------
   // the PSI Table abstract base class
   //

   //
   // the size of the section space which will be written by data (does not
   // include CRC)
   ui16 PSITable::getMaxDataLen() const
   {
      return STable::getMaxDataLen() - Section::CRC_LEN;
   }


   //
   // controls the sectionable table building.. calls the virtual function
   // writeSection() which defines how each table is written.. this function
   // simply allocates space, calls for the write, writes the total length
   // of the section (returned by writeSection() and crc's it
   //
   void PSITable::buildSections(TStream &strm) const
   {
      enum State_t { MALLOC_SEC, WRITE_SEC, END_TABLE };

      bool done = false;
      ui8 cur_sec = 0;
      ui16 sec_bytes = 0;
      State_t state = MALLOC_SEC;

      Section *s = nullptr;
      // this table's sections
      std::list<Section *> table_sections;

      // add each field while it still fits in this section
      while (!done)
      {
         switch (state)
         {
           case MALLOC_SEC:
              s = strm.getNewSection(getMaxSectionLen());

              // add it to the temp link list - we will need to
              // adjust some fields once we're all done with all sections
              table_sections.push_back( s );

              state = WRITE_SEC;
              break;

           case WRITE_SEC:
              // write as much data as we can to this section
              if (writeSection(*s, cur_sec, sec_bytes))
                 state = END_TABLE;
              else {
                 // writeSection() returned 'false' which means it is not done
                 // so we increment the section count
                 cur_sec++;
                 state = MALLOC_SEC;
              }
              // update the section's length with the count that was added
              s->set16Bits(1, buildLengthData(sec_bytes) + 4);
              break;

           case END_TABLE:
              // done with the table.. update the last_section field
              // in all the sections
              for (auto sp : table_sections) {
                 sp->set08Bits(7, cur_sec); // save the last_section_number
                 sp->calcCrc();             // crc the section
              }
              done = true;
              break;
         }
      }
   }


   //
   // writes the table_id_extension, and reserved | version | current_next
   // bytes
   void PSITable::writeSectionHeader(Section &s) const
   {
      STable::buildSections(s);

      // private table data
      s.set16Bits( table_id_extension );
      s.set08Bits( rbits(0xc0) |
                   version_number << 1 |
                   current_next_indicator );
   }

   bool PSITable::ListItem::write_section(Section& section, ui16 max_data_len,
                                          ui16& sec_bytes, ui16* loop_len_ptr) const
   {
      ui8 header_len;
      ui8* desc_loop_len_pos = 0;
      ui16 d_len, desc_loop_len = 0;
      bool exit = false, done = false;

      while (!exit)
      {
         switch (run.op_state)
         {
           case INIT:
              // set the descriptor iterator
              run.d_iter = descriptors.begin();
              run.op_state = WRITE_HEAD;

           case WRITE_HEAD:
              header_len = write_header(section) + 2;

              // save the position for the desc loop len.. we'll update it later
              desc_loop_len_pos = section.getCurDataPosition();
              section.set16Bits( 0 );

              // increment the byte count
              sec_bytes += header_len;
              if (loop_len_ptr)
                 *loop_len_ptr += header_len;

              run.op_state = (!run.d ? GET_DESC : WRITE_DESC);
              break;

           case GET_DESC:
              // if we have descriptors available..
              if (run.d_iter != descriptors.end()) {
                 run.d = (*run.d_iter++).get();

                 // make sure we can fit the next one
                 if ( (sec_bytes + run.d->length()) > max_data_len ) {
                    run.op_state = WRITE_HEAD;
                    exit = true;
                    break;
                 }
                 run.op_state = WRITE_DESC;
              }
              else {
                 // no more descriptors.. done writing this xport stream,
                 run = Context();
                 exit = done = true;
                 break;
              }
              break;

           case WRITE_DESC:
              run.d->buildSections(section);

              // increment all byte counts
              d_len = run.d->length();
              sec_bytes += d_len;
              if (loop_len_ptr)
                 *loop_len_ptr += d_len;
              desc_loop_len += d_len;

              // try to get another one
              run.op_state = GET_DESC;
              break;
         }
      }
      // write the desc loop length
      write_desc_loop_len(section, desc_loop_len_pos, desc_loop_len);

      return done;
   }

   //
   // general case. Some tables will define their own (e.g., SDT, EIT)
   void PSITable::ListItem::write_desc_loop_len(Section& section, ui8* pos, ui16 len) const
   {
      section.set16Bits( pos, rbits(~LEN_MASK) | (len & LEN_MASK) );
   }

#ifdef ENABLE_DUMP
   //
   // displays the reserved | version | current_next byte
   void PSITable::dumpHeader(std::ostream &o, STRID table_label, STRID ext_label,
                             bool hexdec) const
   {
      // table header
      STable::dumpHeader(o, table_label);

      o << std::hex;
      identStr(o, ext_label, table_id_extension, hexdec);

      identStr(o, RESERVED_S, rbits(0x03));
      identStr(o, VER_NUM_S, version_number);
      identStr(o, CURR_NEXT_IND_S, current_next_indicator);
   }
#endif

} // namespace

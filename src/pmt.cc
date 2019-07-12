//
// pmt.cc: class definition for the PMT
// -----------------------------------

#include <iostream>
#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <list>
#include "table.h"
#include "pmt.h"
#include "descriptor.h"
#include "tstream.h"

namespace sigen
{
   //
   // add a descriptor to the table
   bool PMT::addProgramDesc(Descriptor &d)
   {
      ui16 d_len = d.length();
      if ( !incLength(d_len) )
         return false;

      prog_desc.add(d, 0);
      program_info_length += d_len;
      return true;
   }

   //
   // add an elementary stream
   bool PMT::addElemStream(ui8 type, ui16 elem_pid)
   {
#ifdef CHECK_DUPLICATES
      if (es_list.end() !=
          std::find_if(es_list.begin(), es_list.end(),
                       [=](auto& s) { return s.equals(elem_pid); })) {
         std::stringstream err;
         err << "Attempt to add duplicate elemtary stream with pid " << std::hex << elem_pid;
         throw std::range_error(err.str());
      }
#endif

      if ( !incLength(ElementaryStream::BASE_LEN) )
         return false;

      es_list.emplace_back(elem_pid, type);
      return true;
   }


   //
   // add a descriptor to the stream specified by elem_pid
   //
   bool PMT::addElemStreamDesc(ui16 elem_pid, Descriptor &d)
   {
      // look for the stream
      auto it = std::find_if(es_list.begin(), es_list.end(),
                             [=](const auto& es) { return es.equals(elem_pid); });
      if (it == es_list.end())
         return false;

      return addElemStreamDesc(*it, d);
   }

   //
   // tries to add a descriptor to the last stream added
   //
   bool PMT::addElemStreamDesc(Descriptor &d)
   {
      if (es_list.empty())
         return false;

      return addElemStreamDesc( es_list.back(), d );
   }

   //
   // actually adds the descriptor to the transport stream
   //
   bool PMT::addElemStreamDesc(ElementaryStream& stream, Descriptor &d)
   {
      ui16 d_len = d.length();
      if ( !incLength( d_len ) )
         return false;

      // take ownership and store it
      stream.descriptors.add(d, 0); // PMT output does not carry a loop length
      return true;
   }

   //
   // writes the data to the stream
   //
   bool PMT::writeSection(Section& section, ui8 cur_sec, ui16 &sec_bytes) const
   {
      ui8 *prog_info_len_pos = 0;
      ui16 d_len, prog_info_len = 0;
      bool done = false, exit = false;

      while (!exit)
      {
         switch (run.op_state)
         {
           case INIT:
              // associate the iterators to the list
              run.pd_iter = prog_desc.begin();
              run.es_iter = es_list.begin();
              run.op_state = WRITE_HEAD;

           case WRITE_HEAD:
              // common data for every section
              // if table's length is > max_section_len, we'll
              // overwrite the section length later on
              writeSectionHeader(section);

              // section count information
              section.set08Bits(cur_sec);

              // we don't know the last section number yet, so
              // for now set it to 0,
              section.set08Bits(0);//last_sec_num);

              section.set16Bits( rbits(0xe000) | pcr_pid );

              // save the position for the network_desc_loop_len
              prog_info_len_pos = section.getCurDataPosition();

              // for now, set it to something.. this is necessary to
              // increment the internal data pointer of the buffer
              section.set16Bits( 0 );

              sec_bytes = BASE_LENGTH; // the minimum section size
              run.op_state = (!run.pd ? GET_PROG_DESC : WRITE_PROG_DESC);
              break;

           case GET_PROG_DESC:
              // update the prog_info_len.. we always do this
              // and if we don't add any, it is set to 0 anyways
              section.set16Bits(prog_info_len_pos,
                                rbits(~LEN_MASK) |
                                (prog_info_len & LEN_MASK) );

              if (!run.d_done) {
                 // fetch the next program descriptor
                 if (run.pd_iter != prog_desc.end()) {
                    run.pd = (*run.pd_iter++).get();

                    // check if we can fit it in this section
                    if (sec_bytes + run.pd->length() > getMaxDataLen()) {
                       // we can't.. return so we can get a new section
                       // we'll add it when we come back
                       run.op_state = WRITE_HEAD;
                       exit = true;
                    }
                    else // we found one to write
                       run.op_state = WRITE_PROG_DESC;
                    break;
                 }
                 else {
                    run.pd = nullptr;
                    run.d_done = true;
                    // fall through
                 }
              }

              // done with all program descriptors.. move on to the
              // transport streams
              run.op_state = (!run.es ? GET_XPORT_STREAM : WRITE_XPORT_STREAM);
              break;

           case WRITE_PROG_DESC:
              // add the network descriptor
              run.pd->buildSections(section);

              d_len = run.pd->length();
              sec_bytes += d_len;
              prog_info_len += d_len;

              // try to add another one
              run.op_state = GET_PROG_DESC;
              break;

           case GET_XPORT_STREAM:
              // fetch a transport stream
              if (run.es_iter != es_list.end()) {
                 run.es = &(*run.es_iter++);

                 // first, check if it has any descriptors.. we'll try to fit
                 // at least one
                 if (!run.es->descriptors.empty()) {
                    const Descriptor *d = run.es->descriptors.front().get();

                    // check the size with the descriptor
                    if ( (sec_bytes + PMT::ElementaryStream::BASE_LEN + d->length()) >
                         getMaxDataLen() ) {
                       // won't fit.. wait until the next section
                       run.op_state = WRITE_HEAD;
                       exit = true;
                       break;
                    }
                 }
                 else {
                    // check if this XS with no descs will fit here
                    if ( (sec_bytes + PMT::ElementaryStream::BASE_LEN) > getMaxDataLen() ) {
                       // nope.. wait also
                       run.op_state = WRITE_HEAD;
                       exit = true;
                       break;
                    }
                 }
                 // all is ok.. add it
                 run.op_state = WRITE_XPORT_STREAM;
              }
              else {
                 // no more network descriptors or transport streams, so
                 // all sections are done!
                 run = Context();
                 exit = done = true;
                 break;
              }
              break;

           case WRITE_XPORT_STREAM:
              // finally write it
              if (!(*run.es).write_section(section, getMaxDataLen(), sec_bytes)) {
                 run.op_state = WRITE_HEAD;
                 exit = true;
                 break;
              }
              run.op_state = GET_XPORT_STREAM;
              break;
         }
      }
      return done;
   }


   //
   // data writers for PMT::ElementaryStream
   //
   ui8 PMT::ElementaryStream::write_header(Section& section) const
   {
      // write the pmt transport stream data
      section.set08Bits(type);
      section.set16Bits( rbits(0xe000) |
                         elementary_pid );

      return ElementaryStream::BASE_LEN - 2;
   }

#ifdef ENABLE_DUMP
   //
   // dumps to an ostream
   void PMT::dump(std::ostream &o) const
   {
      // table header
      dumpHeader(o, PMT_DUMP_S, PROGRAM_NUM_S);

      // pmt-specific
      identStr(o, RESERVED_S, 0x07);
      identStr(o, PCR_PID_S, pcr_pid);
      identStr(o, RESERVED_S, 0x0f);
      identStr(o, PROGRAM_INFO_LEN_S, program_info_length);
      o << std::endl;

      // program desc list
      prog_desc.dump(o);

      // stream list
      incOutLevel(); // indent output
      headerStr(o, STREAM_LIST_S);

      for (const ElementaryStream& stream : es_list) {
         identStr(o, STREAM_TYPE_S, stream.type);
         identStr(o, RESERVED_S, 0x07);
         identStr(o, ELEM_PID_S, stream.elementary_pid);
         identStr(o, RESERVED_S, 0x0f);
         identStr(o, ES_INFO_LEN_S, stream.descriptors.loop_length());

         o << std::endl;

         // output the descriptors
         stream.descriptors.dump(o);
      }
      decOutLevel();
      o << std::endl;
   }
#endif

} // namespace sigen

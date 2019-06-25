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
// nit_bat.cc: class definition for the NIT and BAT
// -----------------------------------

#include <iostream>
#include <utility>
#include <list>
#include <algorithm>
#include "table.h"
#include "descriptor.h"
#include "tstream.h"
#include "nit_bat.h"

namespace sigen
{
   //
   // add a network descriptor to the table
   //
   bool NIT_BAT::addDesc(Descriptor& d)
   {
      ui16 d_len = d.length();

      // make sure we have enough room to add it
      if ( !incLength(d_len) )
         return false;

      descriptors.add(d, d_len);
      return true;
   }


   //
   // creates a new transport stream entry
   //
   bool NIT_BAT::addXportStream(ui16 xport_stream_id, ui16 original_network_id)
   {
      if ( !incLength(XportStream::BASE_LEN) )
         return false;

      // add it to the list
      xport_streams.emplace_back(xport_stream_id, original_network_id);
      xport_stream_loop_length += XportStream::BASE_LEN;
      return true;
   }


   //
   // add descriptors to the xport_stream loop
   //
   bool NIT_BAT::addXportStreamDesc(ui16 xsid, ui16 on_id, Descriptor& d)
   {
      // lookup the transport_stream by the passed id
      auto it = std::find_if(xport_streams.begin(), xport_streams.end(),
                             [=](auto& xs) { return xs.equals(xsid, on_id); });
      if (it == xport_streams.end())
         return false;

      return addXportStreamDesc(*it, d);
   }

   //
   // adds a descriptor to the last transport stream that was added
   //
   bool NIT_BAT::addXportStreamDesc(Descriptor& d)
   {
      if (xport_streams.empty())
         return false;

      return addXportStreamDesc(xport_streams.back(), d);
   }

   //
   // actually adds the descriptor to the transport stream
   //
   bool NIT_BAT::addXportStreamDesc(XportStream& xs, Descriptor& d)
   {
      ui16 d_len = d.length();
      if ( !incLength(d_len) )
         return false;

      xs.descriptors.add(d, d_len);
      xport_stream_loop_length += d_len;
      return true;
   }

   //
   // handles writing the data to the stream. return true if the table
   // is done (all sections are completed)
   //
   bool NIT_BAT::writeSection(Section& section, ui8 cur_sec, ui16& sec_bytes) const
   {
      ui8 *nd_loop_len_pos = 0, *ts_loop_len_pos = 0;
      ui16 d_len, net_desc_len = 0, ts_loop_len = 0;
      bool done = false, exit = false;

      while (!exit)
      {
         switch (run.op_state)
         {
           case INIT:
              // associate the iterators to the lists.. once they reach the
              // end, they'll take care to reset themselves
              run.nd_iter = descriptors.begin();
              run.ts_iter = xport_streams.begin();
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

              // save the position for the descriptors_loop_len
              nd_loop_len_pos = section.getCurDataPosition();

              // for now, save the original value.. this is necessary to
              // increment the internal data pointer of the buffer
              section.set16Bits(descriptors.loop_length());

              sec_bytes = BASE_LENGTH; // the minimum section size
              run.op_state = (!run.nd ? GET_NET_DESC : WRITE_NET_DESC);
              break;

           case GET_NET_DESC:
              // update the descriptors_len.. we always do this
              // and if we don't add any, it is set to 0 anyways
              section.set16Bits(nd_loop_len_pos,
                                rbits(~LEN_MASK) |
                                (net_desc_len & LEN_MASK) );

              if (!run.nd_done) {
                 // fetch the next network descriptor
                 if (run.nd_iter != descriptors.end()) {
                    run.nd = (*run.nd_iter++).get();

                    // check if we can fit it in this section
                    if (sec_bytes + run.nd->length() > getMaxDataLen()) {
                       // holy crap, this is a special case! if the
                       // section is filled with net descriptors, we
                       // must sitll write the XS loop length!
                       ts_loop_len_pos = section.getCurDataPosition();
                       section.set16Bits( 0 ); //xport_stream_loop_length);

                       // we can't.. return so we can get a new section
                       // we'll add it when we come back
                       run.op_state = WRITE_HEAD;
                       exit = true;
                    }
                    else  // found one
                       run.op_state = WRITE_NET_DESC;
                    break;
                 }
                 else {
                    run.nd_done = true;
                    run.nd = nullptr;
                    // fall through
                 }
              }

              // done with all network descriptors.. move on to the
              // transport streams
              run.op_state = WRITE_XPORT_LOOP_LEN;
              break;

           case WRITE_NET_DESC:
              // add the network descriptor
              run.nd->buildSections(section);

              d_len = run.nd->length();
              sec_bytes += d_len;
              net_desc_len += d_len;

              // try to add another one
              run.op_state = GET_NET_DESC;
              break;

           case WRITE_XPORT_LOOP_LEN:
              // save the location for the transport stream loop length
              ts_loop_len_pos = section.getCurDataPosition();
              // again, this will be overwritten, but is necessary to
              // increment the internal buffer pointer

              section.set16Bits( 0 ); //xport_stream_loop_length);

              // if we were looking at one already, don't get a new xport stream
              run.op_state = (!run.ts ? GET_XPORT_STREAM : WRITE_XPORT_STREAM);
              break;

           case GET_XPORT_STREAM:

              // fetch a transport stream
              if (run.ts_iter != xport_streams.end()) {
                 run.ts = &(*run.ts_iter++);

                 // first, check if it has any descriptors.. we'll try to fit
                 // at least one
                 if (!run.ts->descriptors.empty()) {
                    // check the size with the descriptor
                    if ( (sec_bytes + XportStream::BASE_LEN + run.ts->descriptors.front()->length()) >
                         getMaxDataLen() ) {
                       // won't fit.. wait until the next section
                       run.op_state = WRITE_HEAD;
                       exit = true;
                       break;
                    }
                 }
                 else {
                    // check if this XS with no descs will fit here
                    if ( (sec_bytes + XportStream::BASE_LEN) > getMaxDataLen() )
                    {
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
              if (!(*run.ts).write_section(section, getMaxDataLen(), sec_bytes, &ts_loop_len)) {
                 run.op_state = WRITE_HEAD;
                 exit = true;
                 break;
              }
              run.op_state = GET_XPORT_STREAM;
              break;
         }
      }

      // save the xport stream length we've calculated so far
      section.set16Bits( ts_loop_len_pos,
                         rbits(~LEN_MASK) |
                         (ts_loop_len & LEN_MASK) );
      return done;
   }


   //
   // state machine for writing the transport streams
   //
   ui8 NIT_BAT::XportStream::write_header(Section& section) const
   {
      // write the transport stream data
      section.set16Bits(id);
      section.set16Bits(original_network_id);

      return XportStream::BASE_LEN - 2;
   }

   //
   // dumps to stdout
   //
#ifdef ENABLE_DUMP
   void NIT_BAT::dump(std::ostream& o) const
   {
      // dump the table's header
      if (getId() == NIT_BAT::NIT_ACTUAL_TID || getId() == NIT_BAT::NIT_OTHER_TID) {
         dumpHeader( o,
                     ((getId() == NIT_BAT::NIT_ACTUAL_TID) ? NIT_DUMP_ACTUAL_S : NIT_DUMP_OTHER_S),
                     NETWORK_ID_S, true );
      } else {
         // BAT
         dumpHeader( o, BAT_DUMP_S, BOUQUET_ID_S, true );
      }

      // reserved bits
      identStr(o, RESERVED_FU_S, rbits(0xf));

      // descriptors
      identStr(o,
               ((getId() == NIT_BAT::BAT_TID) ? BOUQUET_DESC_LEN_S : NETWORK_DESC_LEN_S),
               descriptors.loop_length(), true);
      o << std::endl;

      descriptors.dump(o);

      // transport streams
      dumpXportStreams(o);
      o << std::endl;
   }


   //
   // dumps the transport stream data to stdout
   //
   void NIT_BAT::dumpXportStreams(std::ostream& o) const
   {
      // loop length
      o << std::hex;
      identStr(o, RESERVED_FU_S, rbits(0xf));
      identStr(o, XS_LOOP_LEN_S, xport_stream_loop_length, true);
      o << std::endl;

      // display each transport stream's data & descriptors
      incOutLevel();
      for (const XportStream& xs : xport_streams)
      {
         headerStr(o, XPORT_STREAM_S, false);

         identStr(o, XPORT_STREAM_ID_S, xs.id);
         identStr(o, ORIG_NETWORK_ID_S, xs.original_network_id, true);
         identStr(o, RESERVED_FU_S, rbits(0xf));
         identStr(o, DESC_LEN_S, xs.descriptors.loop_length(), true);
         o << std::endl;

         // dump the descriptors (inherited method)
         xs.descriptors.dump(o);
      }
      decOutLevel();
   }

#endif

} // namespace sigen

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
// bat.cc: class definition for the BAT
//

#include <iostream>
#include <utility>
#include <list>
#include "table.h"
#include "descriptor.h"
#include "tstream.h"
#include "bat.h"

namespace sigen
{
   //
   // add a bouquet descriptor to the table
   //
   bool BAT::addBouquetDesc(Descriptor &d)
   {
      ui16 d_len = d.length();

      // make sure we have enough room to add it
      if ( !incLength(d_len) )
         return false;

      bouquet_desc.add(d, d_len);
      return true;
   }


   //
   // creates a new transport stream entry
   //
   bool BAT::addXportStream(ui16 xport_stream_id, ui16 original_network_id)
   {
      if ( !incLength(XportStream::BASE_LEN) )
         return false;

      xport_streams.push_back(std::make_unique<XportStream>(xport_stream_id, original_network_id));

      // increment the lengths
      xport_stream_loop_length += XportStream::BASE_LEN;
      return true;
   }


   //
   // add descriptors to the xport_stream loop
   //
   bool BAT::addXportStreamDesc(ui16 tsid, ui16 on_id, Descriptor &d)
   {
      // lookup the transport_stream by the passed id
      for (std::unique_ptr<XportStream>& xsp : xport_streams)
      {
         XportStream& ts = *xsp;
         if ( (ts.id == tsid) && (ts.original_network_id == on_id) )
            return addXportStreamDesc(ts, d);
      }
      return false;
   }

   //
   // adds a descriptor to the last transport stream that was added
   //
   bool BAT::addXportStreamDesc(Descriptor &d)
   {
      if (!xport_streams.empty()) {
         return addXportStreamDesc( *xport_streams.back(), d );
      }
      return false;
   }


   //
   // actually adds the descriptor to the transport stream
   //
   bool BAT::addXportStreamDesc(XportStream& ts, Descriptor &d)
   {
      ui16 d_len = d.length();
      if ( !incLength(d_len) )
         return false;

      ts.descriptors.add(d, d_len);
      xport_stream_loop_length += d_len;
      return true;
   }

   //
   // handles writing the data to the stream. return true if the table
   // is done (all sections are completed)
   //
   bool BAT::writeSection(Section& section, ui8 cur_sec, ui16 &sec_bytes) const
   {
      enum State_t {
         WRITE_HEAD,
         GET_BOUQUET_DESC, WRITE_BOUQUET_DESC,
         WRITE_XPORT_LOOP_LEN,
         GET_XPORT_STREAM, WRITE_XPORT_STREAM
      };

      ui8 *bd_loop_len_pos = 0, *ts_loop_len_pos = 0;
      ui16 d_len, bouquet_desc_len = 0, ts_loop_len = 0;
      bool done, exit;
      static bool bd_done = false;
      static State_t op_state = WRITE_HEAD;
      static const Descriptor *bd = nullptr;
      static const XportStream *ts = nullptr;
      static std::list<std::unique_ptr<Descriptor> >::const_iterator bd_iter = bouquet_desc.begin();
      static std::list<std::unique_ptr<XportStream> >::const_iterator ts_iter = xport_streams.begin();

      // associate the iterators to the lists.. once they reach the
      // end, they'll take care to reset themselves
      if (!bd_done)
         if (!bd)
            bd_iter = bouquet_desc.begin();

      if (!ts)
         ts_iter = xport_streams.begin();

      done = exit = false;

      while (!exit)
      {
         switch (op_state)
         {
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

              // save the position for the bouquet_desc_loop_len
              bd_loop_len_pos = section.getCurDataPosition();

              // for now, save the original value.. this is necessary to
              // increment the internal data pointer of the buffer
              section.set16Bits(bouquet_desc.loop_length());

              sec_bytes = BASE_LEN; // the minimum section size
              op_state = (!bd ? GET_BOUQUET_DESC : WRITE_BOUQUET_DESC);
              break;

           case GET_BOUQUET_DESC:
              // update the network_desc_len.. we always do this
              // and if we don't add any, it is set to 0 anyways
              section.set16Bits( bd_loop_len_pos,
                                 rbits(reserved, ~LEN_MASK) |
                                 (bouquet_desc_len & LEN_MASK) );

              if (!bd_done)
              {
                 // fetch the next network descriptor
                 if (bd_iter != bouquet_desc.end())
                 {
                    bd = (*bd_iter++).get();

                    // check if we can fit it in this section
                    if (sec_bytes + bd->length() > getMaxDataLen())
                    {
                       // special case! if the section is filled with
                       // net descriptors, we must sitll write the XS
                       // loop length! same deal as with NIT
                       ts_loop_len_pos = section.getCurDataPosition();
                       section.set16Bits( 0 ); //xport_stream_loop_length);


                       // we can't.. return so we can get a new section
                       // we'll add it when we come back
                       op_state = WRITE_HEAD;
                       exit = true;
                    }
                    else  // found one
                       op_state = WRITE_BOUQUET_DESC;
                    break;
                 }
                 else
                 {
                    bd_done = true;
                    bd = nullptr;
                    // fall through
                 }
              }

              // done with all bouquet descriptors.. move on to the
              // transport streams
              op_state = WRITE_XPORT_LOOP_LEN;
              break;

           case WRITE_BOUQUET_DESC:
              // add the network descriptor
              bd->buildSections(section);

              d_len = bd->length();
              sec_bytes += d_len;
              bouquet_desc_len += d_len;

              // try to add another one
              op_state = GET_BOUQUET_DESC;
              break;

           case WRITE_XPORT_LOOP_LEN:
              // save the location for the transport stream loop length
              ts_loop_len_pos = section.getCurDataPosition();
              // again, this will be overwritten, but is necessary to
              // increment the internal buffer pointer
              section.set16Bits( 0 ); //xport_stream_loop_length);

              // if we were looking at one already, don't get a new xport stream
              op_state = (!ts ? GET_XPORT_STREAM : WRITE_XPORT_STREAM);
              break;

           case GET_XPORT_STREAM:
              // fetch a transport stream
              if (ts_iter != xport_streams.end())
              {
                 ts = (*ts_iter++).get();

                 // first, check if it has any descriptors.. we'll try to fit
                 // at least one
                 if (!ts->descriptors.empty())
                 {
                    const Descriptor *d = ts->descriptors.front().get();

                    // check the size with the descriptor
                    if ( (sec_bytes + XportStream::BASE_LEN + d->length()) >
                         getMaxDataLen() )
                    {
                       // won't fit.. wait until the next section
                       op_state = WRITE_HEAD;
                       exit = true;
                       break;
                    }
                 }
                 else
                 {
                    // check if this XS with no descs will fit here
                    if ( (sec_bytes + XportStream::BASE_LEN) > getMaxDataLen() )
                    {
                       // nope.. wait also
                       op_state = WRITE_HEAD;
                       exit = true;
                       break;
                    }
                 }
                 // all is ok.. add it
                 op_state = WRITE_XPORT_STREAM;
              }
              else
              {
                 // no more network descriptors or transport streams, so
                 // all sections are done!
                 ts = nullptr;
                 bd_done = false;
                 op_state = WRITE_HEAD;
                 exit = done = true;
                 break;
              }
              break;

           case WRITE_XPORT_STREAM:
              // finally write it
              if (!writeXportStream(section, *ts, sec_bytes, ts_loop_len))
              {
                 op_state = WRITE_HEAD;
                 exit = true;
                 break;
              }
              op_state = GET_XPORT_STREAM;
              break;
         }
      }

      // save the xport stream length we've calculated so far
      section.set16Bits( ts_loop_len_pos,
                         rbits( reserved, ~LEN_MASK) | (ts_loop_len & LEN_MASK) );
      return done;
   }


   //
   // state machine for writing the transport streams
   //
   bool BAT::writeXportStream(Section& section, const XportStream& ts,
                              ui16 &sec_bytes, ui16 &ts_loop_len) const
   {
      enum State_t { WRITE_HEAD, GET_DESC, WRITE_DESC };

      static State_t op_state = WRITE_HEAD;

      ui8 *ts_desc_len_pos = 0;
      ui16 d_len, ts_desc_len = 0;
      bool exit, done;
      static const Descriptor *tsd = nullptr;
      static std::list<std::unique_ptr<Descriptor> >::const_iterator tsd_iter = ts.descriptors.begin();

      exit = done = false;

      // set the descriptor iterator
      if (!tsd)
         tsd_iter = ts.descriptors.begin();

      while (!exit)
      {
         switch (op_state)
         {
           case WRITE_HEAD:
              // write the transport stream data
              section.set16Bits(ts.id);
              section.set16Bits(ts.original_network_id);

              // save the position for the desc loop len.. we'll update it later
              ts_desc_len_pos = section.getCurDataPosition();
              section.set16Bits( 0 );

              // increment the byte count
              sec_bytes += XportStream::BASE_LEN;
              ts_loop_len += XportStream::BASE_LEN;

              op_state = (!tsd ? GET_DESC : WRITE_DESC);
              break;

           case GET_DESC:
              if (tsd_iter != ts.descriptors.end())
              {
                 tsd = (*tsd_iter++).get();

                 // make sure we can fit it
                 if ( (sec_bytes + tsd->length()) > getMaxDataLen() )
                 {
                    op_state = WRITE_HEAD;
                    exit = true;
                    break;
                 }
                 op_state = WRITE_DESC;
              }
              else
              {
                 // no more descriptors.. done writing this xport stream,
                 tsd = nullptr;
                 op_state = WRITE_HEAD;
                 exit = done = true;
                 break;
              }
              break;

           case WRITE_DESC:
              tsd->buildSections(section);

              // increment all byte counts
              d_len = tsd->length();
              sec_bytes += d_len;
              ts_loop_len += d_len;
              ts_desc_len += d_len;

              // try to get another one
              op_state = GET_DESC;
              break;
         }
      }
      // write the desc loop length
      section.set16Bits( ts_desc_len_pos,
                         rbits( reserved, ~LEN_MASK) | (ts_desc_len & LEN_MASK) );
      return done;
   }


   //
   // dumps to stdout
   //
#ifdef ENABLE_DUMP
   void BAT::dump(std::ostream &o) const
   {
      // table header
      dumpHeader( o, BAT_DUMP_S, BOUQUET_ID_S );

      // bat-specific
      identStr(o, RESERVED_FU_S, rbits(reserved, 0xf));

      // bouquet descriptors
      identStr(o, BOUQUET_DESC_LEN_S, bouquet_desc.loop_length(), true);
      o << std::endl;

      bouquet_desc.dump( o );

      // transport streams
      dumpXportStreams(o);
      o << std::endl;
   }


   //
   // dumps the transport stream data to stdout
   //
   void BAT::dumpXportStreams(std::ostream &o) const
   {
      // loop length
      o << std::hex;
      identStr(o, RESERVED_FU_S, rbits( reserved, 0xf));
      identStr(o, XS_LOOP_LEN_S, xport_stream_loop_length, true);
      o << std::endl;

      // display each transport stream's data & descriptors
      incOutLevel();
      for (const std::unique_ptr<XportStream>& xsp : xport_streams)
      {
         const XportStream &ts = *xsp;

         headerStr(o, XPORT_STREAM_S, false);

         identStr(o, XPORT_STREAM_ID_S, ts.id);
         identStr(o, ORIG_NETWORK_ID_S, ts.original_network_id);
         identStr(o, RESERVED_FU_S, rbits(reserved, 0xf));
         identStr(o, DESC_LEN_S, ts.descriptors.loop_length(), true);
         o << std::endl;

         // dump the transport descriptors
         ts.descriptors.dump(o);
      }
      decOutLevel();
   }
#endif

} // namespace

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
// other_tables.cc: class definitions for the Stuffing and
//                  Running Status tables
// -----------------------------------

#include <iostream>
#include <utility>
#include <string>
#include <list>
#include "table.h"
#include "tstream.h"
#include "other_tables.h"

namespace sigen
{
   // ------------------------------------
   // the Running Status Table
   //

   //
   // add a transort stream to the list
   bool RST::addXportStream(ui16 xsid, ui16 onid, ui16 sid,
                            ui16 eid, ui8 rs)
   {
      if ( !incLength(XportStream::BASE_LEN) )
         return false;

      // we can fit it, so let's try to add it
      xport_stream_list.push_back(std::make_unique<XportStream>(xsid, onid, sid, eid, rs));
      return true;
   }


   //
   // table builder function
   void RST::buildSections(TStream& strm) const
   {
      Section *s = strm.getNewSection( getMaxSectionLen() );

      STable::buildSections(*s);

      // write the transport stream data
      for ( const std::unique_ptr<XportStream>& xsp : xport_stream_list)
      {
         const XportStream& xs = *xsp;

         s->set16Bits(xs.id);
         s->set16Bits(xs.original_network_id);
         s->set16Bits(xs.service_id);
         s->set16Bits(xs.event_id);
         s->set08Bits( rbits(0xf8) | (xs.running_status & 0x7) );
      }
   }


   //
   // debug
#ifdef ENABLE_DUMP
   void RST::dump(std::ostream& o) const
   {
      // table header
      dumpHeader( o, RST_DUMP_S );
      o << std::endl;

      // dump the transport streams
      incOutLevel();
      for ( const std::unique_ptr<XportStream>& xsp : xport_stream_list)
      {
         const XportStream& xs = *xsp;

         headerStr(o, XPORT_STREAM_S, false);

         identStr(o, XPORT_STREAM_ID_S, xs.id);
         identStr(o, ORIG_NETWORK_ID_S, xs.original_network_id);
         identStr(o, SERVICE_ID_S, xs.service_id, true);
         identStr(o, EVENT_ID_S, xs.event_id);
         identStr(o, RUNNING_STATUS_S, static_cast<ui16>(xs.running_status), true);

         o << std::endl;
      }
      decOutLevel();
      o << std::endl;
   }
#endif



   // ------------------------------------
   // the Stuffing Table
   //

   Stuffing::Stuffing(const std::string& d, bool ssi) :
      STable(TID, 0, MAX_SEC_LEN, ssi, true),
      data( d )
   {
      // !!! check to make sure it fits!
   }


   Stuffing::Stuffing(ui8 d, ui16 len, bool ssi) :
      STable(TID, 0, MAX_SEC_LEN, ssi, true),
      data( std::string(d, len) )
   {
      // !!! check to make sure it fits!
   }


   //
   // the actual table builder
   //
   void Stuffing::buildSections(TStream& strm) const
   {
      Section *s = strm.getNewSection( getMaxSectionLen() );

      STable::buildSections(*s);

      s->setBits( data );
   }


   //
   // debug
   //
#ifdef ENABLE_DUMP
   void Stuffing::dump(std::ostream& o) const
   {
      dumpHeader( o, ST_DUMP_S );
      o << std::endl;

      dumpData(o, reinterpret_cast<const ui8 *>(data.c_str()), getDataLength());
      o << std::endl;
   }
#endif

} // namespace

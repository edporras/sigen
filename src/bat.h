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
// bat.h: class definition for the BAT
//
// -----------------------------------

#pragma once

#include <memory>
#include <list>
#include "table.h"
#include "util.h"

namespace sigen {

   class Descriptor;

   //
   // the BAT class
   //
   class BAT : public PSITable
   {
   public:
      enum { PID = 0x11 };

      // constructor
      BAT(ui16 bouquet_id, ui8 ver, bool cni = true) :
         PSITable(TID, bouquet_id, BASE_LEN, MAX_SEC_LEN, ver, cni),
         xport_stream_loop_length(0)
      { }
      BAT() = delete;

      // utility
      bool addBouquetDesc(Descriptor &);
      bool addXportStream(ui16 xs_id, ui16 on_id);
      bool addXportStreamDesc(ui16 xs_id, ui16 on_id, Descriptor &);
      bool addXportStreamDesc(Descriptor &); // adds it to the last stream added

      // dump to stdout routines
#ifdef ENABLE_DUMP
      virtual void dump(std::ostream &) const;
#endif

   private:
      enum { BASE_LEN = 9, TID = 0x4a, MAX_SEC_LEN = 1024 };

      // the transport stream struct
      struct XportStream
      {
         enum { BASE_LEN = 6 };

         ui16 id;                  // transport stream id
         ui16 original_network_id;
         DescList descriptors;

         // constructor
         XportStream(ui16 tsid, ui16 onid) :
            id(tsid), original_network_id(onid)
         { }

         // utility methods
         int length() const { return descriptors.loop_length() + BASE_LEN; }
      };

      // BAT members
      ui16 xport_stream_loop_length;
      DescList bouquet_desc;
      std::list<std::unique_ptr<XportStream> > xport_streams; // transport streams

   protected:
      bool addXportStreamDesc(XportStream& , Descriptor &);
      virtual bool writeSection(Section& , ui8, ui16 &) const;
      bool writeXportStream(Section& , const XportStream& , ui16 &, ui16 &) const;

#ifdef ENABLE_DUMP
      void dumpXportStreams(std::ostream &) const;
#endif
   };

} // sigen namespace

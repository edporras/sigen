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
// nit.h: class definition for the NIT
// -----------------------------------

#pragma once

#include <memory>
#include <list>
#include "table.h"

namespace sigen {

   class Descriptor;

   //
   // the NIT
   //
   class NIT : public PSITable
   {
   public:
      enum Type { ACTUAL = 0x40, OTHER = 0x41 };
      enum { PID = 0x10 };

      NIT() = delete;

      bool addNetworkDesc(Descriptor &);                             // add a network descriptor
      bool addXportStream(ui16 xs_id, ui16 on_id);                   // add a transport stream
      bool addXportStreamDesc(ui16 xs_id, ui16 on_id, Descriptor &); // add a descriptor to the indicate transport stream
      bool addXportStreamDesc(Descriptor &);                         // adds it to the last stream added

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream &) const;
#endif

   private:
      enum { BASE_LEN = 9, MAX_SEC_LEN = 1024 };

      // the transport stream struct
      struct XportStream
      {
         enum { BASE_LEN = 6 };

         ui16 id,                    // transport stream id
            original_network_id;
         DescList descriptors;

         // constructor
         XportStream(ui16 tsid, ui16 onid) :
            id(tsid), original_network_id(onid) { }

         int length() const { return descriptors.loop_length() + BASE_LEN; }
      };

      // NIT members
      ui16 xport_stream_loop_length;

      DescList network_desc;
      std::list<std::unique_ptr<XportStream> > xport_streams;

   protected:
      // protected constructor - type refers to ACTUAL or OTHER,
      // reserved is the state of reserved bits
      NIT(ui16 network_id, NIT::Type type, ui8 ver, bool cni = true,
          bool rsrvd = true) :
         PSITable((ui8) type, network_id, BASE_LEN, MAX_SEC_LEN, ver, cni, rsrvd, rsrvd),
         xport_stream_loop_length(0)
      { }

      bool addXportStreamDesc(XportStream& , Descriptor &);
      virtual bool writeSection(Section& , ui8, ui16 &) const;
      bool writeXportStream(Section& , const XportStream& , ui16 &, ui16 &) const;

#ifdef ENABLE_DUMP
      void dumpXportStreams(std::ostream &) const;
#endif
   };

   // public interface to create NIT tables
   struct NITActual : public NIT
   {
      NITActual(ui16 network_id, ui8 ver, bool cni = true, bool rsrvd = true) :
         NIT(network_id, NIT::ACTUAL, ver, cni, rsrvd)
      { }
   };

   struct NITOther : public NIT
   {
      NITOther(ui16 network_id, ui8 ver, bool cni = true, bool rsrvd = true) :
         NIT(network_id, NIT::OTHER, ver, cni, rsrvd)
      { }
   };

} // sigen namespace

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

      // the transport stream struct - public as the BAT uses it too
      struct XportStream
      {
         enum { BASE_LEN = 6 };

         ui16 id;                  // transport stream id
         ui16 original_network_id;
         DescList descriptors;

         // constructor
         XportStream(ui16 tsid, ui16 onid) :
            id(tsid), original_network_id(onid) { }

         XportStream(const XportStream&) = delete;
         XportStream(const XportStream&&) = delete;
         XportStream operator=(const XportStream&) = delete;
         XportStream operator=(const XportStream&&) = delete;
         
         int length() const { return descriptors.loop_length() + BASE_LEN; }
         bool equals(ui16 tsid, ui16 onid) const { return (tsid == id && onid == original_network_id); }

         bool writeSection(Section& , ui16, ui16 &, ui16 &) const;

      private:
         // section building state tracking
         enum State_t { INIT, WRITE_HEAD, GET_DESC, WRITE_DESC };
         mutable struct Context {
            Context() : op_state(INIT), tsd(nullptr) {}

            State_t op_state;
            const Descriptor *tsd;
            std::list<std::unique_ptr<Descriptor> >::const_iterator tsd_iter;
         } run;
      };

   private:
      enum { MAX_SEC_LEN = 1024 };

      // NIT members
      ui16 xport_stream_loop_length;
      DescList network_desc;
      std::list<XportStream> xport_streams;

      // private methods
      bool addXportStreamDesc(XportStream& , Descriptor &);
      virtual bool writeSection(Section& , ui8, ui16 &) const;

#ifdef ENABLE_DUMP
      void dumpXportStreams(std::ostream &) const;
#endif

      // section building state tracking
      enum State_t { INIT, WRITE_HEAD, GET_NET_DESC, WRITE_NET_DESC, WRITE_XPORT_LOOP_LEN,
                     GET_XPORT_STREAM, WRITE_XPORT_STREAM };
      mutable struct Context {
         Context() :
            nd_done(false), op_state(INIT), nd(nullptr), ts(nullptr)
         {}

         bool nd_done;
         State_t op_state;

         const Descriptor *nd;
         const XportStream *ts;
         std::list<std::unique_ptr<Descriptor> >::const_iterator nd_iter;
         std::list<XportStream>::const_iterator ts_iter;
      } run;

   protected:
      // protected constructor - type refers to ACTUAL or OTHER,
      // reserved is the state of reserved bits
      NIT(ui16 network_id, NIT::Type type, ui8 ver, bool cni = true) :
         PSITable(static_cast<ui8>(type), network_id, 9, MAX_SEC_LEN, ver, cni),
         xport_stream_loop_length(0)
      { }
   };

   // public interface to create NIT tables
   struct NITActual : public NIT
   {
      NITActual(ui16 network_id, ui8 ver, bool cni = true) :
         NIT(network_id, NIT::ACTUAL, ver, cni)
      { }
   };

   struct NITOther : public NIT
   {
      NITOther(ui16 network_id, ui8 ver, bool cni = true) :
         NIT(network_id, NIT::OTHER, ver, cni)
      { }
   };

} // sigen namespace

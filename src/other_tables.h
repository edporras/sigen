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
// other_tables.h: class definitions for the Running Status
//                 and Stuffing tables
// -----------------------------------

#pragma once

#include <memory>
#include <list>
#include <string>
#include "table.h"

namespace sigen {

   class TStream;
   class Section;

   //
   // Running Status Table
   //
   class RST : public STable
   {
   public:
      enum { PID = 0x13 };

      RST(bool rsrvd = true) :
         STable(TID, BASE_LEN, MAX_SEC_LEN, false, rsrvd, rsrvd) {}

      // utility
      bool addXportStream(ui16 xsid, ui16 onid, ui16 sid,
                          ui16 eid, ui8 rs);
      virtual void buildSections(TStream&) const;

#ifdef ENABLE_DUMP
      void dump(std::ostream&) const;
#endif

   private:
      enum { BASE_LEN = 0, MAX_SEC_LEN = 1024, TID = 0x71 };

      // the private transport stream class
      struct XportStream {
         enum { BASE_LEN = 9 };

         ui16 id,
            original_network_id,
            service_id,
            event_id;
         ui8 running_status : 3;

         // constructor
         XportStream(ui16 xsid, ui16 onid, ui16 sid, ui16 eid, ui8 rs) :
            id(xsid), original_network_id(onid),
            service_id(sid), event_id(eid), running_status(rs) {}
      };

      // the list of transport streams
      std::list<std::unique_ptr<XportStream> > xport_stream_list;
   };


   //
   // Stuffing Table
   //
   class Stuffing : public STable
   {
   public:
      // constructor for NULL terminated data
      Stuffing(const std::string& data, bool ssi = true, bool rsrvd = true);
      // constructor where we replicate the passed data
      Stuffing(ui8 data, ui16 len, bool ssi = true, bool rsrvd = true);

      // accessors
      const std::string& getDataByte() const { return data; }

      // utility
      virtual void buildSections(TStream&) const;

#ifdef ENABLE_DUMP
      void dump(std::ostream&) const;
#endif

   private:
      std::string data;

      enum { BASE_LEN = 0, MAX_SEC_LEN = 4096, TID = 0x72 };
   };
} // namespace

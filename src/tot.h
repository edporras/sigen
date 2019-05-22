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
// tot.h: class definition for the TOT
// -----------------------------------

#pragma once

#include <memory>
#include <list>
#include "utc.h"
#include "table.h"
#include "util.h"

namespace sigen {

   class TStream;
   class Descriptor;

   //
   // Time Offset Table
   //
   class TOT : public STable
   {
   private:
      UTC utc;

      enum { BASE_LEN = 7, MAX_SEC_LEN = 1024, TID = 0x73 };

      ui16 desc_loop_length;
      std::list<std::unique_ptr<Descriptor> > desc_list; // descriptor's list

   public:
      enum { PID = 0x14 };

      // first constructor uses current time.. second uses the specific
      // time provided
      TOT(bool rsrvd = true) :
         STable(TID, BASE_LEN, MAX_SEC_LEN, false, rsrvd, rsrvd),
         desc_loop_length(0)
      { }
      TOT(const UTC &t, bool rsrvd = true) :
         STable(TID, BASE_LEN, MAX_SEC_LEN, false, rsrvd, rsrvd),
         utc(t),
         desc_loop_length(0)
      { }

      // accessors
      virtual ui16 getMaxDataLen() const;
      const UTC& getUTC() const { return utc; }

      // utility
      bool addDesc(Descriptor &);
      virtual void buildSections(TStream &) const;

#ifdef ENABLE_DUMP
      void dump(std::ostream &) const;
#endif
   };

} // namespace

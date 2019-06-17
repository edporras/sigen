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

#include "utc.h"
#include "table.h"

namespace sigen {

   class TStream;
   class Descriptor;

   //
   // Time Offset Table
   //
   class TOT : public STable
   {
   public:
      enum { PID = 0x14 };

      // first constructor uses current time.. second uses the specific
      // time provided
      TOT() : TOT(UTC()) { }
      TOT(const UTC &t) :
         STable(TID, 7, MAX_SEC_LEN),
         utc(t)
      { }

      // accessors
      virtual ui16 getMaxDataLen() const;
      const UTC& getUTC() const { return utc; }

      // utility
      bool addDesc(Descriptor &);

      // section data writer
      virtual void buildSections(TStream &) const;

#ifdef ENABLE_DUMP
      void dump(std::ostream &) const;
#endif

   private:
      UTC utc;

      enum { MAX_SEC_LEN = 1024, TID = 0x73 };

      DescList descriptors;
   };
} // namespace

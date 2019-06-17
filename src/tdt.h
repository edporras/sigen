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
// tdt.h: class definition for the TDT
// -----------------------------------

#pragma once

#include "table.h"
#include "utc.h"

namespace sigen
{
   class TStream;

   //
   // Time and Date Section
   //
   class TDT : public STable
   {
   public:
      enum { PID = 0x14 };

      // default constructor creates a TDT with current time
      TDT() : TDT(UTC()) { }

      // this one takes a UTC instance
      TDT(const UTC &t) :
         STable(TID, 5, MAX_SEC_LEN),
         utc(t)
      { }

      // accessors
      const UTC& getUTC() const { return utc; }

      // section data writer
      virtual void buildSections(TStream &) const;

#ifdef ENABLE_DUMP
      void dump(std::ostream &) const;
#endif

   private:
      UTC utc;

      enum { MAX_SEC_LEN = 8, TID = 0x70 };
   };
} // namespace

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
// tdt.cc: class definition for the TDT
// -----------------------------------

#include <iostream>
#include "descriptor.h"
#include "table.h"
#include "tstream.h"
#include "tdt.h"

namespace sigen
{
   //
   // writes the table to a stream
   //
   void TDT::buildSections(TStream &strm) const
   {
      Section *s = strm.getNewSection( getMaxSectionLen() );

      STable::buildSections(*s);

      s->set16Bits( utc.mjd );
      s->set08Bits( utc.time.getBCDHour() );
      s->set08Bits( utc.time.getBCDMinute() );
      s->set08Bits( utc.time.getBCDSecond() );
   }

   //
   // debug routine
   //
#ifdef ENABLE_DUMP
   void TDT::dump(std::ostream &o) const
   {
      dumpHeader(o, TDT_DUMP_S);
      identStr(o, UTC_S, utc);
   }
#endif

} // namespace

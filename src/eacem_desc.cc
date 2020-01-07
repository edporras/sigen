// Copyright 1999-2020 Ed Porras
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
// eacem_desc.cc
// -----------------------------------

//#include <utility>
#include <list>
#include <numeric>
#include "descriptor.h"
#include "eacem_desc.h"
#include "tstream.h"

namespace sigen
{
   namespace EACEM
   {
      //
      // Logical Channel Descriptor
      // ---------------------------------------

      //
      // adds a content entry to the loop
      bool LogicalChannelDesc::addLogicalChan(ui16 service_id, ui16 LCN, bool visible_service_flag)
      {
         // check if we can add another to the loop
         if ( !incLength( LogicalChan::BASE_LEN ) )
            return false;

         // create an entry and add it
         channel_list.emplace_back(service_id, visible_service_flag, LCN);
         return true;
      }

      //
      // write the binary data
      //
      void LogicalChannelDesc::buildSections(Section &s) const
      {
         Descriptor::buildSections(s);

         for (const auto &channel : channel_list) {
            s.set16Bits( channel.sid );
            s.set16Bits( (channel.visible_service_flag << 15) |
                         (rbits(0x1f) << 10) |
                         (channel.logical_channel_number & 0x03ff) );
         }
      }

#ifdef ENABLE_DUMP
      //
      // debug
      void LogicalChannelDesc::dump(std::ostream &o) const
      {
         dumpHeader(o, LOGICAL_CHAN_D_S);

         incOutLevel();
         for (const auto &channel : channel_list) {
            identStr(o, SERVICE_ID_S, channel.sid);
            identStr(o, VISIBLE_SERV_F_S, channel.visible_service_flag);
            identStr(o, RESERVED_S, 0x1f);
            identStr(o, LOGICAL_CHAN_NUM_S, channel.logical_channel_number);
         }
         decOutLevel();
      }
#endif

   }
} // namespace sigen

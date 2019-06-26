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
// packetizer.h: definition of mpeg packetizer class
// -----------------------------------

#pragma once

#include <string>
#include "types.h"

namespace sigen {

   class Section;

   //
   // mpeg packetizer class
   //
   class MpgPacketizer
   {
   public:
      enum ScramblingControl_t { NOT_SCRAMBLED = 0x00 }; // 0x01-0x11 user def'd
      enum AdaptationField_t {
         RESERVED                     = 0x0,
         NO_ADAPTATION_FIELD          = 0x1, // default
         ADAPTATION_FIELD_ONLY        = 0x2,
         ADAPTATION_FIELD_AND_PAYLOAD = 0x4
      };

      // constructor
      MpgPacketizer(const std::string &out_file, ui8 cont_count);
      // prohibit
      MpgPacketizer() = delete;
      MpgPacketizer(const MpgPacketizer &) = delete;
      MpgPacketizer(const MpgPacketizer &&) = delete;
      MpgPacketizer &operator=(const MpgPacketizer &) = delete;
      MpgPacketizer &operator=(const MpgPacketizer &&) = delete;

      void setFlags(bool transport_err_ind, bool transport_pri) {
         transport_error_indicator = transport_err_ind;
         transport_priority = transport_pri;
      }

      int packetize(const Section &section, ui16 pid);

   protected:
      void getHeader(ui8 *packet, const ui8 *section_data,
                     bool payload_unit_start_indicator, ui16 pid);

   private:
      enum {
         SYNC_BYTE     = 0x47,
         HEADER_SIZE   = 4,
         PKT_DATA_SIZE = 184,
         PACKET_SIZE   = PKT_DATA_SIZE + 4,
      };

      // data
      std::string filename;

      bool transport_error_indicator,
           transport_priority;
      ui8 continuity_count,
          transport_scrambling_control : 2,
          adaptation_field_control : 2;
   };

} // sigen namespace

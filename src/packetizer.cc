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
// packetizer.cc: class definition for mpeg packetizer
// -----------------------------------

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include "types.h"
#include "packetizer.h"
#include "tstream.h"

namespace sigen
{
   // --------------------------------
   // mpeg packetizer class
   //
   //
   MpgPacketizer::MpgPacketizer(const std::string &out_file, ui8 cont_count) :
      filename(out_file),
      transport_error_indicator(false),
      transport_priority(false),
      continuity_count(cont_count),
      transport_scrambling_control(MpgPacketizer::NOT_SCRAMBLED),
      adaptation_field_control(MpgPacketizer::NO_ADAPTATION_FIELD)
   {
      // open the file to create it (empty) - we'll append to it below
      std::ofstream tfile( filename.c_str(), std::ios::out );
   }


   //
   // this is the main packetizing function
   //
   int MpgPacketizer::packetize(const Section &section, ui16 pid)
   {
      bool payload_unit_start_indicator = true;
      ui16 cur_section_size, this_section_size;
      ui8 header_size, packet_size;
      ui8 packet[PACKET_SIZE], *tptr;
      std::ofstream packetfile(filename.c_str(), std::ios::app);

      const ui8 *section_data = section.getBinaryData();

      cur_section_size = this_section_size = section.length();

      header_size = HEADER_SIZE;
      packet_size = PACKET_SIZE - header_size;

      // start
      while (cur_section_size > 0)
      {
         // fill packet with pad val for short packets
         for (int i = 0; i < PACKET_SIZE; i++)
            packet[i] = 0xff;

         // set the packet ptr to point to this packet
         if (header_size != 0)
         {
            getHeader(&packet[0], nullptr, // section_data,
                      payload_unit_start_indicator, pid);
            tptr = &packet[header_size];
         }
         else
            tptr = &packet[0];

         // move packet data in
         for (int i = 0; i < PKT_DATA_SIZE; i++)
         {
            if (payload_unit_start_indicator && (i == 0))
            {
               *(tptr++) = (char) 0;
            }
            else
            {
               if (cur_section_size > 0)
               {
                  *(tptr++) = section_data[ this_section_size - cur_section_size ];
                  cur_section_size--;
               }
               else
                  break;
            }
         }
         // clear unit start so only first packet of section is unit start
         payload_unit_start_indicator = false;

         for (int i = 0; i < PACKET_SIZE; i++)
            packetfile << packet[i];
      }
      return continuity_count;
   }

   // builds the header
   //
   void MpgPacketizer::getHeader(ui8 *packet,
                                 const ui8 *section_data,
                                 bool payload_unit_start_indicator,
                                 ui16 pid)
   {
      *(packet++) = SYNC_BYTE;

      if (!section_data)
      {
         std::cout << "byte: " << std::hex << (ui16) ( (transport_error_indicator << 7) |
                                                       (payload_unit_start_indicator << 6) |
                                                       (transport_priority << 5) |
                                                       ((pid >> 8) & 0x1f) ) << std::endl;

         *(packet++) = (ui8) ( (transport_error_indicator << 7) |
                               (payload_unit_start_indicator << 6) |
                               (transport_priority << 5) |
                               ((pid >> 8) & 0x1f) );
         *(packet++) = (ui8) (pid & 0xff);
         *(packet) =   (ui8) ( (transport_scrambling_control << 6) |
                               (adaptation_field_control << 4) |
                               (continuity_count & 0xf) );

         std::cerr << "tsc: " << (ui16) transport_scrambling_control << ", afc: " << (ui16) adaptation_field_control << ", cc: " << (ui16) continuity_count << std::endl;

         // only increment CC based on value of AFC
         if ( (adaptation_field_control != MpgPacketizer::RESERVED) &&
              (adaptation_field_control != MpgPacketizer::ADAPTATION_FIELD_ONLY) )
         {
            continuity_count++;
         }
      }
      else
      {
         *(packet++) = (ui8) ( (0 << 7) |
                               (0 << 6) |
                               (0 << 5) |
                               (0x1f) );
         *(packet++) = (ui8) (0xff);
         *(packet) =   (ui8) ( (0 << 6) |
                               (1 << 4) |
                               0 );
      }
   }

} // namespace

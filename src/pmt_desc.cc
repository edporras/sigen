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
// pmt_desc.cc: class definition for the DVB defined PMT descriptors
// -----------------------------------

#include <iostream>
#include <utility>
#include <list>
#include <string>
#include "descriptor.h"
#include "pmt_desc.h"
#include "tstream.h"
#include "util.h"

namespace sigen
{
   //
   // AC3 Descriptor
   // ---------------------------------------

#ifdef ENABLE_DUMP
   STRID AC3Desc::flag_strid[VALUE_COUNT] = {
      AC3TYPE_FLAG_S,
      BSID_FLAG_S,
      MAINID_FLAG_S,
      ASVC_FLAG_S,
   };

   STRID AC3Desc::value_strid[VALUE_COUNT] = {
      AC3TYPE_S,
      BSID_S,
      MAINID_S,
      ASVC_S,
   };
#endif

   //
   // sets the value for the given key
   void AC3Desc::setValue(flag_t key, ui8 value)
   {
      // if this is the first time the field is set, bump the
      // descriptor's length by one..
      if (!field[ key ].is_set)
         incLength(1);

      // now set the value and mark it as set
      field[ key ].value = value;
      field[ key ].is_set = true;
   }

   //
   // binary data formatter
   void AC3Desc::buildSections(Section& s) const
   {
      Descriptor::buildSections(s);

      ui8 flags = 0;
      ui32 i;

      // first, figure out the 4-bit flag field.. check if it's set and
      // if so, set the appropriate bit to on
      for (i = 0; i < VALUE_COUNT; i++) {
         flags |= field[ i ].is_set << (7 - i);
      }

      // reserved bits follow, so set them
      for (; i < 8; i++) {
         flags |= rbits(0x1) << (7 - i);
      }

      s.set08Bits( flags );

      // now set the actual values if they exist
      for (i = 0; i < VALUE_COUNT; i++) {
         if ( field[ i ].is_set ) {
            s.set08Bits( field[ i ].value );
         }
      }

      // and finally the reserved-future use bytes
      s.setBits( additional_info );
   }

#ifdef ENABLE_DUMP
   //
   // output formatter
   void AC3Desc::dump(std::ostream& o) const
   {
      dumpHeader(o, AC3_D_S);

      int i;
      for (i = 0; i < VALUE_COUNT; i++) {
         identStr( o, flag_strid[ i ], field[ i ].is_set);
      }

      identStr(o, RESERVED_S, rbits(0xff));

      for (i = 0; i < VALUE_COUNT; i++)
      {
         if (field[ i ].is_set) {
            identStr( o, value_strid[ i ], field[ i ].value );
         }
      }

      identStr(o, ADDITIONAL_INFO_S, additional_info);
   }
#endif



   //
   // Data Broadcast Id Descriptor
   // ---------------------------------------
   bool DataBroadcastIdDesc::setSelectorBytes(const std::vector<ui8>& bytes)
   {
      // check if we can fit it
      if ( !incLength( bytes.size()) )
         return false;

      // yep, add it then
      selector_bytes = bytes;
      return true;
   }

   void DataBroadcastIdDesc::buildSections(Section& s) const
   {
      Descriptor::buildSections(s);

      s.set16Bits( data_broadcast_id );
      s.setBits( selector_bytes );
   }

#ifdef ENABLE_DUMP
   void DataBroadcastIdDesc::dump(std::ostream& o) const
   {
      dumpHeader(o, DATA_BCAST_ID_D_S);

      identStr(o, DATA_BCAST_ID_S, data_broadcast_id, true);
      identStr(o, SELECTOR_S, selector_bytes);
   }
#endif



   //
   // Service Move Descriptor
   // ---------------------------------------
   void ServiceMoveDesc::buildSections(Section &s) const
   {
      Descriptor::buildSections(s);

      s.set16Bits(original_network_id);
      s.set16Bits(xport_stream_id);
      s.set16Bits(new_service_id);
   }

#ifdef ENABLE_DUMP
   //
   //
   void ServiceMoveDesc::dump(std::ostream &o) const
   {
      dumpHeader(o, SERV_MOVE_D_S);

      identStr(o, ORIG_NETWORK_ID_S, original_network_id);
      identStr(o, XPORT_STREAM_ID_S, xport_stream_id);
      identStr(o, NEW_SERV_ID_S, new_service_id);
   }
#endif




   //
   // Subtitling Descriptor
   // ---------------------------------------

   //
   // add a subtitling element
   bool SubtitlingDesc::addSubtitling(const std::string &lang_code, ui8 type,
                                      ui16 c_page_id, ui16 a_page_id)
   {
      // check if we can fit it
      if ( !incLength( Subtitling::BASE_LEN) )
         return false;

      // yep, add it then
      subtitling_list.push_back( std::make_unique<Subtitling>(lang_code, type, c_page_id, a_page_id) );
      return true;
   }

   //
   // write the section data
   void SubtitlingDesc::buildSections(Section &s) const
   {
      Descriptor::buildSections(s);

      // iterate through the list and write the data
      for (const auto &subt : subtitling_list)
      {
         s.setBits( subt->language_code );
         s.set08Bits( subt->type );
         s.set16Bits( subt->composition_page_id );
         s.set16Bits( subt->ancillary_page_id );
      }
   }

#ifdef ENABLE_DUMP
   //
   // dump to stdout
   void SubtitlingDesc::dump(std::ostream &o) const
   {
      dumpHeader(o, SUBTITLING_D_S);

      // iterate through the list and write the data
      incOutLevel();
      for (const auto &subt : subtitling_list)
      {
         identStr(o, LANGUAGE_CODE_S, subt->language_code);
         identStr(o, TYPE_S, subt->type);
         identStr(o, COMPOSITION_PAGE_ID_S, subt->composition_page_id);
         identStr(o, ANCILLARY_PAGE_ID_S, subt->ancillary_page_id);
         o << std::endl;
      }
      decOutLevel();
   }
#endif


   //
   // Teletext Descriptor
   // ---------------------------------------

   //
   // add a teletext element
   bool TeletextDesc::addTeletext(const std::string &lang_code, ui8 type, ui8 mag_num,
                                  ui8 page_num)
   {
      // see if we can fit it
      if ( !incLength( Teletext::BASE_LEN) )
         return false;

      // add it, then
      teletext_list.push_back( std::make_unique<Teletext>(lang_code, type, mag_num, page_num) );
      return true;
   }


   //
   // write the section data
   void TeletextDesc::buildSections(Section &s) const
   {
      Descriptor::buildSections(s);

      // iterate through the list and write the data
      for (const auto &teletext : teletext_list)
      {
         s.setBits( teletext->language_code );
         s.set08Bits( (teletext->type << 3) | (teletext->magazine_number) );
         s.set08Bits( teletext->page_number );
      }
   }

#ifdef ENABLE_DUMP
   //
   // dump to stdout
   void TeletextDesc::dump(std::ostream &o) const
   {
      dumpHeader(o, TELETEXT_D_S);

      // iterate through the list and write the data
      incOutLevel();
      for (const auto &teletext : teletext_list)
      {
         identStr(o, LANGUAGE_CODE_S, teletext->language_code);
         identStr(o, TYPE_S, teletext->type);
         identStr(o, MAGAZINE_NUM_S, teletext->magazine_number);
         identStr(o, PAGE_NUM_S, teletext->page_number);
         o << std::endl;
      }
      decOutLevel();
   }
#endif

} // namespace

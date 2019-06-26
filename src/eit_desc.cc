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
// eit_desc.cc: class definition for the DVB defined EIT descriptors
// -----------------------------------

#include <utility>
#include <list>
#include <string>
#include "descriptor.h"
#include "eit_desc.h"
#include "tstream.h"

namespace sigen
{
   //
   // Component Descriptor
   // ---------------------------------------
   ComponentDesc::ComponentDesc(ui8 sc, ui8 ctype, ui8 ctag, const std::string &code,
                                const std::string &t) :
      Descriptor(TAG, 6),
      text( incLength(t) ),  // size the string
      language_code(code),
      component_type(ctype),
      component_tag(ctag),
      stream_content(sc)
   {
   }

   //
   // write the data to the stream
   void ComponentDesc::buildSections(Section &s) const
   {
      Descriptor::buildSections(s);

      s.set08Bits( rbits(0xf0) | stream_content );
      s.set08Bits( component_type );
      s.set08Bits( component_tag );
      s.setBits( language_code );
      s.setBits( text );
   }

#ifdef ENABLE_DUMP
   //
   // dump to stdout
   void ComponentDesc::dump(std::ostream &o) const
   {
      dumpHeader(o, COMPONENT_D_S);

      rsrvdStr(o, RESERVED_FU_S, 0x0f);
      identStr(o, STREAM_CONTENT_S, static_cast<ui16>(stream_content));
      identStr(o, COMPONENT_TYPE_S, static_cast<ui16>(component_type));
      identStr(o, COMPONENT_TAG_S, static_cast<ui16>(component_tag));
      identStr(o, CODE_S, language_code);
      identStr(o, TEXT_S, text);
   }
#endif



   //
   // Content Descriptor
   // ---------------------------------------

   //
   // adds a content entry to the loop
   bool ContentDesc::addContent(ui8 nl1, ui8 nl2, ui8 un1, ui8 un2)
   {
      // check if we can add another to the loop
      if ( !incLength( Content::BASE_LEN ) )
         return false;

      // create an entry and add it
      content_list.emplace_back(nl1, nl2, un1, un2);
      return true;
   }


   //
   // write the binary data
   //
   void ContentDesc::buildSections(Section &s) const
   {
      Descriptor::buildSections(s);

      for (const auto &content : content_list) {
         s.set08Bits( (content.nibble_level_1 << 4) | content.nibble_level_2 );
         s.set08Bits( (content.user_nibble_1 << 4) | content.user_nibble_2 );
      }
   }


#ifdef ENABLE_DUMP
   //
   // debug
   void ContentDesc::dump(std::ostream &o) const
   {
      dumpHeader(o, CONTENT_D_S);

      incOutLevel();
      for (const auto &content : content_list) {
         identStr(o, CONTENT_NL_1, content.nibble_level_1);
         identStr(o, CONTENT_NL_2, content.nibble_level_2);
         identStr(o, USER_NL_1, content.user_nibble_1);
         identStr(o, USER_NL_2, content.user_nibble_2);
      }
      decOutLevel();
   }
#endif



   //
   // Extended Event Descriptor
   // ---------------------------------------

   //
   // adds an item to the loop
   //
   bool ExtendedEventDesc::addItem(const std::string& desc, const std::string& name)
   {
      // this descriptor is kind of funky because we need to store more
      // data than the usual 255 as it can span across multiple extended
      // event descriptors sent.  however, each item's total length can't
      // exceed 255 either.. so to simplify things, if an item exceeds
      // the 255 limit, we reject it off the bat..
      std::unique_ptr<Item> item(new Item(desc, name));

      if ( !incLength( item->length() ) )
         return false;

      item_list.push_back( std::move(item) );
      return true;
   }


   //
   // computes the total size of the items in the list
   ui8 ExtendedEventDesc::itemListSize() const
   {
      ui8 size = 0;

      for (const auto& i : item_list) 
         size += i->length();

      return size;
   }


   //
   // builds the binary data
   //
   void ExtendedEventDesc::buildSections(Section &s) const
   {
      Descriptor::buildSections(s);

      s.set08Bits( (descriptor_number << 4) | last_descriptor_number );
      s.setBits( language_code );
      s.set08Bits( itemListSize() );

      // write the loop data
      for (const auto& item : item_list) {
         // the loop's description field
         s.set08Bits( item->description.length() );
         s.setBits( item->description );

         // the loop's item field
         s.set08Bits( item->name.length() );
         s.setBits( item->name );
      }

      // the descriptor's text field
      s.set08Bits( text.length() );
      s.setBits( text );
   }

#ifdef ENABLE_DUMP
   //
   // debug
   void ExtendedEventDesc::dump(std::ostream &o) const
   {
      dumpHeader(o, EXTENDED_EVENT_D_S);

      identStr(o, DESC_NUM_S, descriptor_number);
      identStr(o, LAST_DESC_NUM_S, last_descriptor_number);
      identStr(o, CODE_S, language_code);
      identStr(o, LENGTH_OF_ITEMS_S, itemListSize(), true);

      // the loop of items
      incOutLevel();

      for (const auto& item : item_list) {
         identStr(o, DESCRPTN_LEN_S, item->description.length(), true);
         identStr(o, DESCRPTN_S, item->description);
         identStr(o, ITEM_LEN_S, item->name.length(), true);
         identStr(o, ITEM_S, item->name);
      }
      decOutLevel();

      // finally the text
      identStr(o, TEXT_LEN_S, text.length(), true);
      identStr(o, TEXT_S, text);
   }
#endif


   //
   // Multilingual Component Descriptor
   // ---------------------------------------

   // derived from MultilingualTextDesc so it inherits most of its
   // functionality.. this class just defines the TAG and BASE_LEN
   // (in the header), defines how the debug should de displayed
   // and how the data should be built since it adds the
   // component_tag field


   //
   // build the section data
   void MultilingualComponentDesc::buildSections(Section &s) const
   {
      Descriptor::buildSections(s);
      s.set08Bits(component_tag);
      buildLoopData(s);
   }


#ifdef ENABLE_DUMP
   //
   // to stdout
   void MultilingualComponentDesc::dump(std::ostream &o) const
   {
      dumpHeader(o, MULTILING_COMPONENT_D_S);
      identStr(o, COMPONENT_TAG_S, static_cast<ui16>(component_tag));
      dumpTextLoop(o, TEXT_S );
   }
#endif



   //
   // Parental Rating Descriptor
   // ---------------------------------------

   //
   // adds a rating to the loop
   bool ParentalRatingDesc::addRating(const std::string &code, ui8 rating)
   {
      // can we add it?
      if ( !incLength( Rating::BASE_LEN ) )
         return false;

      // add it
      rating_list.emplace_back(code, rating);
      return true;
   }


   //
   // builds the section data
   void ParentalRatingDesc::buildSections(Section &s) const
   {
      Descriptor::buildSections(s);

      for (const auto &rating : rating_list) {
         s.setBits( rating.country_code );
         s.set08Bits( rating.value );
      }
   }

#ifdef ENABLE_DUMP
   //
   // debug
   void ParentalRatingDesc::dump(std::ostream &o) const
   {
      dumpHeader(o, PARENTAL_RATING_D_S);

      incOutLevel();
      for (const auto &rating : rating_list) {
         identStr(o, CODE_S, rating.country_code);
         identStr(o, RATING_S, static_cast<ui16>(rating.value));
      }
      decOutLevel();
   }
#endif




   //
   // PDC Descriptor
   // ---------------------------------------
   void PDCDesc::buildSections(Section& s) const
   {
      Descriptor::buildSections(s);

      s.setBits( rbits(0xf00000) |
                 programme_identification_label );
   }

#ifdef ENABLE_DUMP
   void PDCDesc::dump(std::ostream& o) const
   {
      dumpHeader(o, PDC_D_S);

      rsrvdStr(o, RESERVED_FU_S, 0xf);
      identStr(o, PROG_IDENT_LABEL_S, programme_identification_label);
   }
#endif


   //
   // Short Event Descriptor
   // ---------------------------------------
   ShortEventDesc::ShortEventDesc(const std::string &code, const std::string &ev_name,
                                  const std::string &ev_text) :
      Descriptor(TAG, 5),
      language_code(code), name( incLength(ev_name) ),
      text( incLength(ev_text) )
   {
   }


   //
   // build the secion data
   //
   void ShortEventDesc::buildSections(Section &s) const
   {
      Descriptor::buildSections(s);

      s.setBits( language_code );

      s.set08Bits( name.length() );
      s.setBits( name );

      s.set08Bits( text.length() );
      s.setBits( text );
   }


#ifdef ENABLE_DUMP
   //
   // debug
   void ShortEventDesc::dump(std::ostream &o) const
   {
      dumpHeader(o, SHORT_EVENT_D_S);

      identStr(o, CODE_S, language_code);
      identStr(o, EVENT_NAME_LEN_S, name.length(), true);
      identStr(o, EVENT_NAME_S, name);
      identStr(o, TEXT_LEN_S, text.length(), true);
      identStr(o, TEXT_S, text);
   }
#endif



   //
   // Short Soothing Buffer Descriptor
   // ---------------------------------------

   // constructor / destructor
   ShortSmoothingBufferDesc::ShortSmoothingBufferDesc(ui8 size, ui8 leak_rate,
                                                      const std::string &dvb_rsrvd) :
      Descriptor(TAG, 1),
      DVB_reserved( incLength(dvb_rsrvd) ),  // size the string
      sb_size(size), sb_leak_rate(leak_rate)
   {
   }


   //
   // builds the binary data
   //
   void ShortSmoothingBufferDesc::buildSections(Section &s) const
   {
      Descriptor::buildSections(s);

      s.set08Bits( (sb_size << 6) | sb_leak_rate );
      s.setBits( DVB_reserved );
   }

#ifdef ENABLE_DUMP
   //
   // debug
   void ShortSmoothingBufferDesc::dump(std::ostream &o) const
   {
      dumpHeader(o, SHORT_SMTHNG_BUF_D_S);

      identStr(o, SB_SIZE_S, static_cast<ui16>(sb_size));
      identStr(o, SB_LEAK_RATE_S, static_cast<ui16>(sb_leak_rate));
      identStr(o, RESERVED_S, DVB_reserved);
   }
#endif




   //
   // Time Shifted Event Descriptor
   // ---------------------------------------
   void TimeShiftedEventDesc::buildSections(Section &s) const
   {
      Descriptor::buildSections(s);
      s.set16Bits(ref_service_id);
      s.set16Bits(ref_event_id);
   }

#ifdef ENABLE_DUMP
   void TimeShiftedEventDesc::dump(std::ostream &o) const
   {
      dumpHeader(o, TIME_SHIFTED_EVENT_D_S);

      identStr(o, REF_SID_S, ref_service_id, true);
      identStr(o, REF_EVENT_ID_S, ref_event_id, true);
   }
#endif

} // namespace sigen

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
// descriptor.c: base descriptor & stream descriptor classes
// -----------------------------------

#include <iostream>
#include <utility>
#include <list>
#include <string>
#include "descriptor.h"

namespace sigen
{
   // ============================================
   // base descriptor class
   // --------------------------------------------
   void Descriptor::buildSections(Section& s) const
   {
      s.set08Bits(tag);
      s.set08Bits(total_length - BASE_LEN);
   }


#ifdef ENABLE_DUMP
   //
   // dumps to stdout
   void Descriptor::dumpHeader(std::ostream& o, STRID id_str) const
   {
      headerStr(o, id_str);

      o << std::hex;
      identStr(o, TAG_S, tag);
      identStr(o, LEN_S, total_length - BASE_LEN, true);
   }
#endif

   //
   // increments the length by the given ccount (if there's room
   // available)
   //
   bool Descriptor::incLength(ui8 len)
   {
      if (lengthFits(len))
      {
         total_length += len;
         return true;
      }
      return false;
   }

   //
   // tests if the string can be added to the descriptor.. if not, it
   // resizes the string, increments the descriptor length by the size
   // and returns the resulting string
   //
   std::string Descriptor::incLength(const std::string& str)
   {
      std::string new_str(str);
      ui16 len = new_str.length();

      // determine if the current string can fit
      if ( !lengthFits(len) )
      {
         // nope.. truncate it to the max size
         ui16 free_space = CAPACITY - total_length;

         new_str = new_str.substr( 0, free_space );
         len = free_space;
      }

      // increment the descriptor's length
      total_length += len;
      return new_str;
   }


   // ---------------------------------------
   // abstract multilingual text descriptor base class
   //

   //
   // adds a new language to the specified network
   bool MultilingualTextDesc::addLanguage(const LanguageCode& code,
                                          const std::string& text)
   {
      // check if we can even fit a lang code & text length byte
      if (!incLength(Text::BASE_LEN))
         return false;

      // try to add the length of the text
      std::string t = incLength( text );

      // the string has been resized if it didn't fit so add the language
      // data to the list
      ml_text_list.push_back( std::make_unique<Text>(code, t) );
      return true;
   }


   //
   // writes the loop's data to the section
   void MultilingualTextDesc::buildLoopData(Section& s) const
   {
      for (const auto& text : ml_text_list)
      {
         // write its data
         s.setBits( text->code ); // lang code
         s.set08Bits( static_cast<ui8>(text->data.length()) );  // text length
         s.setBits( text->data );                               // text
      }
   }


#ifdef ENABLE_DUMP
   //
   // dumps the text loop
   void MultilingualTextDesc::dumpTextLoop(std::ostream& o, STRID data_type) const
   {
      incOutLevel();

      for (const auto& text : ml_text_list)
      {
         identStr(o, CODE_S, text->code);
         identStr(o, NAME_LEN_S, text->data.length(), true);
         identStr(o, data_type, text->data);
      }

      decOutLevel();
   }
#endif

} // namespace sigen

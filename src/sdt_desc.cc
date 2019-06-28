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
// sdt_desc.cc: class definition for the SDT descriptors
// -----------------------------------

#include <iostream>
#include <list>
#include <string>
#include "descriptor.h"
#include "sdt_desc.h"
#include "tstream.h"

namespace sigen
{
   //
   // Multilingual Service Name Descriptor
   // ---------------------------------------

   //
   // add a language to the list
   bool MultilingualServiceNameDesc::addLanguage(const std::string &code,
                                                 const std::string &prov_name,
                                                 const std::string &name)
   {
      if (!incLength(Language::expected_length(prov_name, name)))
         return false;

      // now create a language entry and add it
      language_list.emplace_back(code, prov_name, name);
      return true;
   }

   //
   // writes to the stream
   void MultilingualServiceNameDesc::buildSections(Section &s) const
   {
      Descriptor::buildSections(s);

      for (const auto &lang : language_list) {
         // write the code
         s.setBits( lang.language_code );

         // provider name
         s.set08Bits( lang.provider_name.length() );
         s.setBits( lang.provider_name );

         // service name
         s.set08Bits( lang.service_name.length() );
         s.setBits( lang.service_name );
      }
   }

   //
   // dump to stdout
#ifdef ENABLE_DUMP
   void MultilingualServiceNameDesc::dump(std::ostream &o) const
   {
      dumpHeader(o, MULTILING_SERV_NAME_D_S);

      incOutLevel();
      for (const auto &lang : language_list) {
         identStr(o, CODE_S, lang.language_code);
         identStr(o, PROV_NAME_LEN_S, lang.provider_name.length());
         identStr(o, PROV_NAME_S, lang.provider_name);
         identStr(o, SERVICE_NAME_LEN_S, lang.service_name.length());
         identStr(o, SERVICE_NAME_S, lang.service_name);
      }
      decOutLevel();
   }
#endif


   //
   // NVOD Reference Descriptor
   // ---------------------------------------

   //
   // adds a set of identifiers to the list
   bool NVODReferenceDesc::addIdentifiers(ui16 xsid, ui16 onid, ui16 sid)
   {
      if (!incLength( Ident::BASE_LEN ))
         return false;

      ident_list.emplace_back(xsid, onid, sid);
      return true;
   }


   //
   // write to a stream
   void NVODReferenceDesc::buildSections(Section &s) const
   {
      Descriptor::buildSections(s);

      // cycle through the list and write the bytes
      for (const auto &ident : ident_list) {
         s.set16Bits( ident.xport_stream_id );
         s.set16Bits( ident.original_network_id );
         s.set16Bits( ident.service_id );
      }
   }

#ifdef ENABLE_DUMP
   //
   // dump to stdout
   void NVODReferenceDesc::dump(std::ostream &o) const
   {
      dumpHeader(o, NVOD_REF_D_S);

      // dump the descriptor's data
      incOutLevel();
      for (const auto &ident : ident_list) {
         identStr(o, XPORT_STREAM_ID_S, ident.xport_stream_id);
         identStr(o, ORIG_NETWORK_ID_S, ident.original_network_id);
         identStr(o, SERVICE_ID_S, ident.service_id);
         o << std::endl;
      }
      decOutLevel();
   }
#endif



   //
   // Service Descriptor
   // ---------------------------------------

   //
   // writes to the stream
   //
   void ServiceDesc::buildSections(Section &s) const
   {
      Descriptor::buildSections(s);

      s.set08Bits( type );

      s.set08Bits( provider_name.length() );
      s.setBits( provider_name );

      s.set08Bits( name.length() );
      s.setBits( name );
   }


#ifdef ENABLE_DUMP
   //
   // to stdout
   //
   void ServiceDesc::dump(std::ostream &o) const
   {
      dumpHeader(o, SERV_D_S);

      // dump the descriptor's data
      identStr(o, TYPE_S, type);
      identStr(o, PROV_NAME_LEN_S, provider_name.length());
      identStr(o, PROV_NAME_S, provider_name);
      identStr(o, SERVICE_NAME_LEN_S, name.length());
      identStr(o, SERVICE_NAME_S, name);
   }
#endif

} // namespace

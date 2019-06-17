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
// sdt_desc.h: class definition for the SDT descriptors
// -----------------------------------

#pragma once

#include <memory>
#include <string>
#include <list>
#include "util.h"
#include "descriptor.h"

namespace sigen {

   // ---------------------------
   // Multilingual Service Name Descriptor
   //
   class MultilingualServiceNameDesc : public Descriptor
   {
   public:
      enum { TAG = 0x5d };

      // constructor
      MultilingualServiceNameDesc() : Descriptor(TAG) {}

      // utility
      bool addLanguage(const std::string& code, const std::string& prov_name,
                       const std::string& serv_name);

      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif

   private:
      struct Language {
         enum { BASE_LEN = LanguageCode::BASE_LEN + 2 };

         LanguageCode language_code;
         std::string provider_name;
         std::string service_name;

         // contructor
         Language(const std::string& code, const std::string& pn,
                  const std::string& sn) :
            language_code(code), provider_name(pn), service_name(sn) {}

         // utility
         ui8 length() const { return provider_name.length() +
               service_name.length() + BASE_LEN; }
      };

      // the list of language structs
      std::list<std::unique_ptr<Language> > language_list;
   };



   // ---------------------------
   // NVOD Reference Descriptor
   //
   class NVODReferenceDesc : public Descriptor
   {
   public:
      enum { TAG = 0x4b };

      // constructor
      NVODReferenceDesc() : Descriptor(TAG) {}

      // utility
      bool addIdentifiers(ui16 xsid, ui16 onid, ui16 sid);

      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif

   private:
      struct Ident {
         enum { BASE_LEN = 6 };

         ui16 xport_stream_id;
         ui16 original_network_id;
         ui16 service_id;

         // constructor
         Ident(ui16 xsid, ui16 onid, ui16 sid) :
            xport_stream_id(xsid), original_network_id(onid), service_id(sid) {}
      };

      // the list of ident structs
      std::list<std::unique_ptr<Ident> > ident_list;
   };



   // ---------------------------
   // Service Descriptor
   //
   class ServiceDesc : public Descriptor
   {
   public:
      enum { TAG = 0x48 };

      // constructor
      ServiceDesc(ui8 t, const std::string& pn, const std::string& n) :
         Descriptor(TAG, 3),
         provider_name( incLength( pn ) ),
         name( incLength(n) ),
         type(t)
      { }
      ServiceDesc() = delete;

      // utility
      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif

   private:
      std::string provider_name;
      std::string name;
      ui8 type;
   };


   // ---------------------------
   // Time Shifted Service Descriptor - derived from
   // PrimitiveDatatypeDesc which handles buildingSections
   //
   //  ui16 data represents reference_service_id
   //
   struct TimeShiftedServiceDesc : public PrimitiveDatatypeDesc<ui16>
   {
      enum { TAG = 0x4c };

      // constructor
      TimeShiftedServiceDesc(ui16 sid) :
         PrimitiveDatatypeDesc<ui16>(TAG, sid)
      {}

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream& o) const {
         dumpData( o, TIME_SHIFTED_SERV_D_S, REF_SID_S, true);
      }
#endif
   };

} // sigen namespace

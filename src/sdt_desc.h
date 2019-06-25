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

#include <string>
#include <list>
#include "descriptor.h"

namespace sigen {

   /*! \addtogroup descriptor
    *  @{
    */

   /*!
    * \brief Multilingual Service Name Descriptor.
    */
   class MultilingualServiceNameDesc : public Descriptor
   {
   public:
      enum { TAG = 0x5d };

      //! \brief Constructor.
      MultilingualServiceNameDesc() : Descriptor(TAG) {}

      /*!
       * \brief Add a language entry to the descriptor loop.
       * \param code ISO 639-2 language code of the fields.
       * \param prov_name Name of the service provider.
       * \param serv_name Name of the service.
       */
      bool addLanguage(const std::string& code, const std::string& prov_name,
                       const std::string& serv_name);

      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif

   private:
      struct Language {
         enum { BASE_LEN = LanguageCode::ISO_639_2_CODE_LENGTH + 2 };

         LanguageCode language_code;
         std::string provider_name;
         std::string service_name;

         // contructor
         Language(const std::string& code, const std::string& pn,
                  const std::string& sn) :
            language_code(code), provider_name(pn), service_name(sn) {}
         Language() = delete;

         // utility
         ui8 length() const { return expected_length(provider_name, service_name); }
         static ui8 expected_length(const std::string& prov_name, const std::string& serv_name) {
            return BASE_LEN + prov_name.length() + serv_name.length();
         }
      };

      // the list of language structs
      std::list<Language> language_list;
   };

   /*!
    * \brief NVOD Reference Descriptor.
    */
   class NVODReferenceDesc : public Descriptor
   {
   public:
      enum { TAG = 0x4b };

      //! \brief Constructor.
      NVODReferenceDesc() : Descriptor(TAG) {}

      /*!
       * \brief Add a language entry to the descriptor loop.
       * \param xs_id Id uniquely identifying the transport stream.
       * \param onid Id of the originating network.
       * \param sid Id uniquely udentifying the service.
       */
      bool addIdentifiers(ui16 xs_id, ui16 onid, ui16 sid);

      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif

   private:
      struct Ident : public STable::ListItem {
         enum { BASE_LEN = 6 };

         ui16 xport_stream_id;
         ui16 original_network_id;
         ui16 service_id;

         // constructor
         Ident(ui16 xsid, ui16 onid, ui16 sid) :
            xport_stream_id(xsid), original_network_id(onid), service_id(sid) {}
         Ident() = delete;
      };

      // the list of ident structs
      std::list<Ident> ident_list;
   };


   /*!
    * \brief Service Descriptor.
    */
   class ServiceDesc : public Descriptor
   {
   public:
      enum { TAG = 0x48 };

      /*!
       * \brief Constructor.
       * \param serv_type Type of the service, as per Dvb::ServiceType_t.
       * \param prov_name Name of the service provider.
       * \param serv_name Name of the service.
       */
      ServiceDesc(ui8 serv_type, const std::string& prov_name, const std::string& serv_name) :
         Descriptor(TAG, 3),
         provider_name( incLength( prov_name ) ),
         name( incLength(serv_name) ),
         type(serv_type)
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


   /*!
    * \brief Time Shifted Service Descriptor.
    */
   struct TimeShiftedServiceDesc : public PrimitiveDatatypeDesc<ui16>
   {
      enum { TAG = 0x4c };

      /*!
       * \brief Constructor.
       * \param ref_sid Id uniquely identifying the reference NVOD service.
       */
      TimeShiftedServiceDesc(ui16 ref_sid) :
         PrimitiveDatatypeDesc<ui16>(TAG, ref_sid)
      {}

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream& o) const {
         dumpData( o, TIME_SHIFTED_SERV_D_S, REF_SID_S, true);
      }
#endif
   };
   //! @}
} // sigen namespace

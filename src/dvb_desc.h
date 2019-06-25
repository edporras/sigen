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
// dvb_desc.h: class definitions for general DVB descriptors
// -----------------------------------

#pragma once

#include <list>
#include <string>
#include "descriptor.h"
#include "utc.h"

namespace sigen {

   class Section;
   class LanguageCode;

   /*! \addtogroup descriptor
    *  @{
    */

   /*!
    * \brief Bouquet Name Descriptor.
    */
   struct BouquetNameDesc : public StringDataDesc
   {
      enum { TAG = 0x47 };

      /*!
       * \brief Constructor.
       * \param name Name of the bouquet.
       */
      BouquetNameDesc(const std::string& name)
         : StringDataDesc(TAG, name) { }
      BouquetNameDesc() = delete;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream& o) const {
         dumpData( o, BOUQUET_NAME_D_S, DATA_S);
      }
#endif
   };


   /*!
    * \brief CA Identifier Descriptor.
    */
   class CAIdentifierDesc : public Descriptor
   {
   public:
      enum { TAG = 0x53 };

      //! \brief Constructor.
      CAIdentifierDesc() : Descriptor(TAG) {}

      /*!
       * \brief Add a CA System ID.
       * \param CA_system_id Allocated CA system ID as per ETSI TS 101 162.
       */
      bool addSystemId(ui16 CA_system_id);

      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif

   private:
      std::list<ui16> id_list;
   };


   /*!
    * \brief Country Availability Descriptor.
    */
   class CountryAvailabilityDesc : public Descriptor
   {
   public:
      enum { TAG = 0x49 };

      /*!
       * \brief Constructor.
       * \param country_availability_f `true`: Service reception intended for include country codes; `false`: otherwise.
       */
      CountryAvailabilityDesc(bool country_availability_f)
         : Descriptor(TAG, 1),
         country_availability_flag(country_availability_f)
      {}
      CountryAvailabilityDesc() = delete;

      /*!
       * \brief Add a country country code to the list.
       * \param country_code Country code as per ISO/IEC 8859-1 or ETSI TS 101 162.
       */
      bool addCountry(const std::string& country_code);

      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif

   private:
      bool country_availability_flag;

      std::list<LanguageCode> country_list;
   };


   /*!
    * \brief Data Broadcast Descriptor.
    */
   class DataBroadcastDesc : public Descriptor
   {
   public:
      enum { TAG = 0x64 };

      /*!
       * \brief Constructor using a component tag.
       * \param db_id Data broadcast id as per ETSI TS 101 162.
       * \param ctag Component tag.
       * \param sel_byte Selector field.
       * \param lang_code ISO 639-2 language code of the text fields.
       * \param txt Text description of the data component.
       */
      DataBroadcastDesc(ui16 db_id, ui8 ctag, const std::string& sel_byte,
                        const std::string& lang_code, const std::string& txt)
         : Descriptor(TAG, 8),
         selector_byte( incLength(sel_byte) ), text( incLength(txt) ),
         code(lang_code), data_broadcast_id(db_id), component_tag(ctag)
      { }
      /*!
       * \brief Constructor without a component tag.
       * \param db_id Data broadcast id as per ETSI TS 101 162.
       * \param sel_byte Selector field.
       * \param lang_code ISO 639-2 language code of the text fields.
       * \param txt Text description of the data component.
       */
      DataBroadcastDesc(ui16 db_id, const std::string& sel_byte,
                        const std::string& lang_code, const std::string& txt)
         : DataBroadcastDesc(db_id, 0, sel_byte, lang_code, txt) {}
      DataBroadcastDesc() = delete;

      // utility
      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif

   private:
      std::string selector_byte;
      std::string text;
      LanguageCode code;
      ui16 data_broadcast_id;
      ui8 component_tag;
   };


   /*!
    * \brief DSNG Descriptor, as per 300 468 1.4.1.
    */
   struct DSNGDesc : public StringDataDesc
   {
      enum { TAG = 0x68 };

      /*!
       * \brief Constructor.
       * \param data ASCII codes for "CONA" as per ETSI EN 301 210.
       */
      DSNGDesc(const std::string& data) : StringDataDesc(TAG, data) { }
      DSNGDesc() = delete;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream& o) const { dumpData( o, DSNG_D_S, DATA_S); }
#endif
   };


   /*!
    * \brief Local Time Offset Descriptor.
    */
   class LocalTimeOffsetDesc : public Descriptor
   {
   public:
      enum { TAG = 0x58 };

      //! \brief Constructor.
      LocalTimeOffsetDesc() : Descriptor(TAG) {}

      /*!
       * \brief Add a time offset entry to the data loop.
       * \param code Country code, as per ISO 3166.
       * \param country_region_id Id identifying a zone.
       * \param offset_polarity Local time offset polarity. `false`: positive, `true`: negative.
       * \param lt_offset Offset time from UTC.
       * \param time_of_change MJD date and time when the change takes place.
       * \param next_time_offset Offset from UTC when current time is => `time_of_change`
       */
      bool addTimeOffset(const LanguageCode& code, ui8 country_region_id,
                         bool offset_polarity, ui16 lt_offset,
                         const UTC& time_of_change, ui16 next_time_offset);

      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif

   private:
      // loop data
      struct TimeOffset : public STable::ListItem {
         enum { BASE_LEN = 13 };

         // offset data
         LanguageCode country_code;
         UTC time_of_change;
         ui16 local_time_offset;
         ui16 next_time_offset;
         ui8 country_region_id : 6;
         bool local_time_offset_polarity;

         // contructor
         TimeOffset(const LanguageCode& cc, ui8 crid, bool ltop, ui16 lto,
                    const UTC& toc, ui16 nto) :
            country_code(cc), time_of_change(toc), local_time_offset(lto),
            next_time_offset(nto), country_region_id(crid),
            local_time_offset_polarity(ltop)
         { }
         TimeOffset() = delete;
      };

      std::list<TimeOffset> time_offset_list;
   };


   // ---------------------------
   // Mosaic Descriptor
   //

   // NOT YET IMPLEMENTED
   class MosaicDesc : public Descriptor
   {
   public:
      enum { TAG = 0x51 };

      // constructor
      MosaicDesc(bool mep, ui8 num_hor_cells, ui8 num_ver_cells);

      // utility functions
      virtual void buildSections(Section&) const = 0; // not implemented

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const = 0; // not implemented
#endif

   private:
      ui8 num_horizontal_elem_cells : 3,
          num_vertical_elem_cells : 3,
          mosaic_entry_point : 1;
   };


   /*!
    * \brief Multilingual Bouquet Name Descriptor.
    */
   struct MultilingualBouquetNameDesc : public MultilingualTextDesc
   {
      enum { TAG = 0x5c };

      //! \brief Constructor.
      MultilingualBouquetNameDesc() : MultilingualTextDesc(TAG) {}

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream& o) const {
         dumpData(o, MULTILING_BOUQUET_NAME_D_S, BOUQUET_NAME_S );
      }
#endif
   };


   /*!
    * \brief Partial Transport Stream Descriptor.
    */
   class PartialTransportStreamDesc : public Descriptor
   {
   public:
      enum { TAG = 0x63 };

      /*!
       * \brief Constructor.
       * \param pk_rate Max. momentary tansport packet rate.
       * \param min_osr  Min. smoothing buffer leak rate for the overall transport stream.
       * \param max_osb Max. smoothing buffer size for the overall transport stream.
       */
      PartialTransportStreamDesc(ui32 pk_rate, ui32 min_osr, ui16 max_osb)
         : Descriptor(TAG, 8),
         peak_rate( pk_rate ),
         min_overall_smoothing_rate( min_osr ),
         max_overall_smoothing_buffer( max_osb )
      { }
      PartialTransportStreamDesc() = delete;

      // utility
      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif

   private:
      ui32 peak_rate : 22;
      ui32 min_overall_smoothing_rate : 22;
      ui16 max_overall_smoothing_buffer : 14;
   };


   /*!
    * \brief Private Data Specifier Descriptor.
    */
   struct PrivateDataSpecifierDesc : public PrimitiveDatatypeDesc<ui32>
   {
      enum { TAG = 0x5f };

      /*!
       * \brief Constructor.
       * \param private_data Private data specifier.
       */
      PrivateDataSpecifierDesc(ui32 private_data)
         : PrimitiveDatatypeDesc<ui32>(TAG, private_data) {}

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream& o) const {
         dumpData( o, PVT_DATA_SPEC_D_S, DATA_S );
      }
#endif
   };


   /*!
    * \brief Service List Descriptor.
    */
   class ServiceListDesc : public Descriptor
   {
   public:
      enum { TAG = 0x41 };

      //! \brief Constructor.
      ServiceListDesc() : Descriptor(TAG) { }

      /*!
       * \brief Add s service to the data loop.
       * \param service_id Unique id of the service within the TS.
       * \param service_type DVB service type code as per ::sigen::Dvb::ServiceType_t.
       */
      bool addService(ui16 service_id, ui8 service_type);

      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif

   private:
      // service structs for the list
      struct Service : public STable::ListItem {
         enum { BASE_LEN = 3 };

         ui16 id;
         ui8 type;

         // constructors
         Service(ui16 sid, ui8 stype) : id(sid), type(stype) {}
         Service() = delete;
      };

      std::list<Service> service_list;
   };


   /*!
    * \brief Stuffing Descriptor.
    */
   struct StuffingDesc : public StringDataDesc
   {
      enum { TAG = 0x42 };

      /*!
       * \brief Constructor.
       * \param byte Byte to build data from.
       * \param len Length of stuffing data.
       */
      StuffingDesc(ui8 byte, ui8 len = 1)
         : StuffingDesc(std::string(len, byte)) {}
      /*!
       * \brief Constructor.
       * \param data Stuffing data byte sequence as a `std::string`.
       */
      StuffingDesc(const std::string& data)
         : StringDataDesc( TAG, data ) {}
      StuffingDesc() = delete;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream& o) const {
         dumpData( o, STUFFING_D_S, DATA_S );
      }
#endif
   };


   /*!
    * \brief Transport Stream Descriptor.
    */
   struct TransportStreamDesc : public StringDataDesc
   {
      enum { TAG = 0x67 };

      /*!
       * \brief Constructor to identify DVB streams.
       */
      TransportStreamDesc() : StringDataDesc( TAG, "DVB" ) {}
      /*!
       * \brief Constructor to identifies others.
       * \param data bytes identifying the stream.
       */
      TransportStreamDesc(const std::string& data)
         : StringDataDesc( TAG, data ) {}

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream& o) const {
         dumpData( o, XPORT_STREAM_D_S, DATA_S );
      }
#endif
   };


   /*!
    * \brief Telephone Descriptor.
    */
   class TelephoneDesc : public Descriptor
   {
   public:
      enum { TAG = 0x57 };

      /*!
       * \brief Constructor.
       * \param foreign_avail `true`: can be called from outside the country specified by `country_prefix`
       * \param connection_type Connection type.
       * \param country_prefix Country prefix code, as per ISO/IEC 8859-1.
       * \param intl_area_code International area code, as per ISO/IEC 8859-1.
       * \param operator_code Operator code, as per ISO/IEC 8859-1.
       * \param natl_area_code National area code, as per ISO/IEC 8859-1.
       * \param number Core number, as per ISO/IEC 8859-1.
       */
      TelephoneDesc(bool foreign_avail, ui8  connection_type,
                    const std::string& country_prefix,
                    const std::string& intl_area_code,
                    const std::string& operator_code,
                    const std::string& natl_area_code,
                    const std::string& number);
      TelephoneDesc() = delete;

      // utility functions
      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif

   private:
      enum {
         MAX_CP_LEN  = 3,
         MAX_IAC_LEN = 7,
         MAX_OC_LEN  = 3,
         MAX_NAC_LEN = 7,
         MAX_CN_LEN  = 15
      };

      // telephone data
      std::string country_prefix;
      std::string international_area_code;
      std::string operator_code;
      std::string national_area_code;
      std::string core_number;
      ui8  connection_type : 5;
      bool foreign_availability;

   protected:
      std::string set_code(const std::string& src_c, ui8 max);
   };

   //! @}

} // sigen namespace

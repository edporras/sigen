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


   // ---------------------------
   // Bouquet Name Descriptor
   //
   struct BouquetNameDesc : public StringDataDesc
   {
      enum { TAG = 0x47 };

      // constructor
      BouquetNameDesc(const std::string& name)
         : StringDataDesc(TAG, name) { }
      BouquetNameDesc() = delete;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream& o) const {
         dumpData( o, BOUQUET_NAME_D_S, DATA_S);
      }
#endif
   };


   // ---------------------------
   // CA Identifier Descriptor
   //
   class CAIdentifierDesc : public Descriptor
   {
   public:
      enum { TAG = 0x53 };

      // constructor
      CAIdentifierDesc() : Descriptor(TAG) {}

      // utility methods
      bool addSystemId(ui16 CA_system_id);

      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif

   private:
      std::list<ui16> id_list;
   };


   // ---------------------------
   // Country Availability Descriptor
   //
   class CountryAvailabilityDesc : public Descriptor
   {
   public:
      enum { TAG = 0x49 };

      // constructor
      CountryAvailabilityDesc(bool country_availability_f)
         : Descriptor(TAG, 1),
         country_availability_flag(country_availability_f)
      {}
      CountryAvailabilityDesc() = delete;

      // utility
      bool addCountry(const std::string& country_code);

      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif

   private:
      bool country_availability_flag;

      std::list<LanguageCode> country_list;
   };



   // ---------------------------
   // Data Broadcast Descriptor
   //
   class DataBroadcastDesc : public Descriptor
   {
   public:
      enum { TAG = 0x64 };

      // constructor - takes selector data as string instance
      DataBroadcastDesc(ui16 db_id, ui8 ctag, const std::string& sel_byte,
                        const std::string& lang_code, const std::string& txt)
         : Descriptor(TAG, 8),
         selector_byte( incLength(sel_byte) ), text( incLength(txt) ),
         code(lang_code), data_broadcast_id(db_id), component_tag(ctag)
      { }
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



   // ---------------------------
   // DSNG Descriptor (300 468 1.4.1)
   //
   struct DSNGDesc : public StringDataDesc
   {
      enum { TAG = 0x68 };

      // constructor
      DSNGDesc(const std::string& data) : StringDataDesc(TAG, data) { }
      DSNGDesc() = delete;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream& o) const { dumpData( o, DSNG_D_S, DATA_S); }
#endif
   };


   // ---------------------------
   // Local Time Offset Descriptor
   //
   class LocalTimeOffsetDesc : public Descriptor
   {
   public:
      enum { TAG = 0x58 };

      // constructor
      LocalTimeOffsetDesc() : Descriptor(TAG) {}
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


   // ---------------------------
   // Multilingual Bouquet Name Descriptor
   // - derived from MultilingualTextDesc (descriptor.h)
   //
   struct MultilingualBouquetNameDesc : public MultilingualTextDesc
   {
      enum { TAG = 0x5c };

      // constructor
      MultilingualBouquetNameDesc() : MultilingualTextDesc(TAG) {}

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream& o) const {
         dumpData(o, MULTILING_BOUQUET_NAME_D_S, BOUQUET_NAME_S );
      }
#endif
   };


   // ---------------------------
   // Partial Transport Stream Descriptor
   //
   class PartialTransportStreamDesc : public Descriptor
   {
   public:
      enum { TAG = 0x63 };

      // constructor
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


   // ---------------------------
   // Private Data Specifier Descriptor - derived from
   // PrimitiveDatatypeDesc which handles building sections
   //
   //  ui32 data represents private_data
   //
   struct PrivateDataSpecifierDesc : public PrimitiveDatatypeDesc<ui32>
   {
      enum { TAG = 0x5f };

      // constructor
      PrivateDataSpecifierDesc(ui32 private_data)
         : PrimitiveDatatypeDesc<ui32>(TAG, private_data) {}

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream& o) const {
         dumpData( o, PVT_DATA_SPEC_D_S, DATA_S );
      }
#endif
   };


   // ---------------------------
   // Service List Descriptor
   //
   class ServiceListDesc : public Descriptor
   {
   public:
      enum { TAG = 0x41 };

      // constructor
      ServiceListDesc() : Descriptor(TAG) { }

      // utility
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


   // ---------------------------
   // Stuffing Descriptor
   //
   struct StuffingDesc : public StringDataDesc
   {
      enum { TAG = 0x42 };

      // constructor
      StuffingDesc(ui8 byte, ui8 len = 1)
         : StuffingDesc(std::string(len, byte)) {}
      StuffingDesc(const std::string& data)
         : StringDataDesc( TAG, data ) {}
      StuffingDesc() = delete;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream& o) const {
         dumpData( o, STUFFING_D_S, DATA_S );
      }
#endif
   };


   // ---------------------------
   // Transport Stream Descriptor
   //
   struct TransportStreamDesc : public StringDataDesc
   {
      enum { TAG = 0x67 };

      // default constructor identifies DVB streams
      // constructor for other types
      TransportStreamDesc() : StringDataDesc( TAG, "DVB" ) {}
      TransportStreamDesc(const std::string& data)
         : StringDataDesc( TAG, data ) {}

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream& o) const {
         dumpData( o, XPORT_STREAM_D_S, DATA_S );
      }
#endif
   };


   // ---------------------------
   // Telephone Descriptor
   //
   class TelephoneDesc : public Descriptor
   {
   public:
      enum { TAG = 0x57 };

      // constructor
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

} // sigen namespace

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
   class BouquetNameDesc : public StringDataDesc
   {
   public:
      enum { TAG = 0x47 }; // BASE_LEN is implicitly defined by StringDataDesc

      // constructor
      BouquetNameDesc(const std::string& data) : StringDataDesc(TAG, data) { }

      // utility
      virtual Descriptor *clone() const { return new BouquetNameDesc(*this); }

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
   private:
      std::list<ui16> id_list;

   public:
      enum { TAG = 0x53, BASE_LEN = 0 };

      // constructor
      CAIdentifierDesc() : Descriptor(TAG, BASE_LEN) {}

      // utility methods
      virtual Descriptor *clone() const { return new CAIdentifierDesc(*this); }

      bool addSystemId(ui16);
      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif
   };


   // ---------------------------
   // Country Availability Descriptor
   //
   class CountryAvailabilityDesc : public Descriptor
   {
   private:
      bool country_availability_flag,
         reserved;

      std::list<LanguageCode> country_list;

   public:
      enum { TAG = 0x49, BASE_LEN = 1 };

      // constructor
      CountryAvailabilityDesc(bool caf, bool rsrvd = true) :
         Descriptor(TAG, BASE_LEN),
         country_availability_flag(caf), reserved(rsrvd) {}

      // utility
      virtual Descriptor *clone() const {
         return new CountryAvailabilityDesc(*this);
      }
      bool addCountry(const std::string&);

      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif
   };



   // ---------------------------
   // Data Broadcast Descriptor
   //
   class DataBroadcastDesc : public Descriptor
   {
   private:
      std::string selector_byte,
         text;
      LanguageCode code;
      ui16 data_broadcast_id;
      ui8 component_tag;

   public:
      enum { TAG = 0x64, BASE_LEN = 8 };

      // constructor - takes selector data as string instance
      DataBroadcastDesc(ui16 db_id, ui8 ctag, const std::string& sel_byte,
                        const std::string& lang_code, const std::string& txt) :
         Descriptor(TAG, BASE_LEN),
         selector_byte( incLength(sel_byte) ), text( incLength(txt) ),
         code(lang_code), data_broadcast_id(db_id), component_tag(ctag) { }

      // utility - commented for now to prevent instances of this desc
      virtual Descriptor *clone() const { return new DataBroadcastDesc(*this); }
      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif
   };



   // ---------------------------
   // DSNG Descriptor (300 468 1.4.1)
   //
   class DSNGDesc : public StringDataDesc
   {
   public:
      enum { TAG = 0x68 }; // BASE_LEN is implicitly defined by StringDataDesc

      // constructor
      DSNGDesc(const std::string& data) : StringDataDesc(TAG, data) { }

      // utility
      virtual Descriptor *clone() const { return new DSNGDesc(*this); }

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
      enum { TAG = 0x58, BASE_LEN = 0 };

      // loop data
      struct TimeOffset {
         enum { BASE_LEN = 13 };

         // offset data
         LanguageCode country_code;
         UTC time_of_change;
         ui16 local_time_offset,
            next_time_offset;
         ui8 country_region_id : 6;
         bool local_time_offset_polarity,
            reserved;

         // contructor
         TimeOffset(const LanguageCode& cc, ui8 crid, bool ltop, ui16 lto,
                    const UTC& toc, ui16 nto, bool rsrvd = true) :
            country_code(cc), time_of_change(toc), local_time_offset(lto),
            next_time_offset(nto), country_region_id(crid),
            local_time_offset_polarity(ltop),
            reserved(rsrvd)
         { }
      };


      // constructor
      LocalTimeOffsetDesc() :
         Descriptor(TAG, BASE_LEN) {}

      // utility
      virtual Descriptor *clone() const { return new LocalTimeOffsetDesc(*this); }
      bool addTimeOffset(const LanguageCode& code, ui8 country_region_id,
                         bool offset_polarity, ui16 lt_offset,
                         const UTC& time_of_change, ui16 next_time_offset);

      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif

   private:
      // data
      std::list<TimeOffset> time_offset_list;
   };


   // ---------------------------
   // Mosaic Descriptor
   //

   // NOT YET IMPLEMENTED
   class MosaicDesc : public Descriptor
   {
   private:
      ui8 num_horizontal_elem_cells : 3,
      num_vertical_elem_cells : 3,
      mosaic_entry_point : 1;
      bool reserved;

   public:
      enum { TAG = 0x51, BASE_LEN = 1 };

      // constructor
      MosaicDesc(bool mep, ui8 num_hor_cells, ui8 num_ver_cells,
                 bool rsrvd = true);

      // utility functions
      virtual void buildSections(Section&) const = 0; // not implemented

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const = 0; // not implemented
#endif
   };


   // ---------------------------
   // Multilingual Bouquet Name Descriptor
   // - derived from MultilingualTextDesc (descriptor.h)
   //
   class MultilingualBouquetNameDesc : public MultilingualTextDesc
   {
   public:
      enum { TAG = 0x5c, BASE_LEN = 0 };

      // constructor
      MultilingualBouquetNameDesc() : MultilingualTextDesc(TAG, BASE_LEN) {}

      // utility methods
      virtual Descriptor* clone() const {
         return new MultilingualBouquetNameDesc(*this);
      }

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
   private:
      ui32 peak_rate : 22;
      ui32 min_overall_smoothing_rate : 22;
      ui16 max_overall_smoothing_buffer : 14;
      bool reserved;

   public:
      enum { TAG = 0x63, BASE_LEN = 8 };

      // constructor
      PartialTransportStreamDesc(ui32 pk_rate, ui32 min_osr, ui16 max_osb,
                                 bool rsrvd = true) :
         Descriptor(TAG, BASE_LEN),
         peak_rate( pk_rate ),
         min_overall_smoothing_rate( min_osr ),
         max_overall_smoothing_buffer( max_osb )
      { }

      virtual Descriptor* clone() const {
         return new PartialTransportStreamDesc(*this);
      }

      // utility functions
      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif
   };


   // ---------------------------
   // Private Data Specifier Descriptor - derived from
   // PrimitiveDatatypeDesc which handles building sections
   //
   //  ui32 data represents private_data
   //
   class PrivateDataSpecifierDesc : public PrimitiveDatatypeDesc<ui32>
   {
   public:
      enum { TAG = 0x5f }; // BASE_LEN is implicitly 4 (ui32)

      // constructor
      PrivateDataSpecifierDesc(ui32 d) : PrimitiveDatatypeDesc<ui32>(TAG, d) {}

      // utility functions
      virtual Descriptor *clone() const {
         return new PrivateDataSpecifierDesc( *this );
      }

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
      enum { TAG = 0x41, BASE_LEN = 0 };

      // service structs for the list
      struct Service
      {
         enum { BASE_LEN = 3 };

         ui16 id;
         ui8 type;

         // constructors
         Service(ui16 sid, ui8 stype) : id(sid), type(stype) {}
      };


      // constructor
      ServiceListDesc() : Descriptor(TAG, BASE_LEN) { }

      // utility functions
      virtual Descriptor* clone() const { return new ServiceListDesc(*this); }

      bool addService(ui16 service_id, ui8 service_type);
      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif

   private:
      std::list<Service> service_list;
   };


   // ---------------------------
   // Stuffing Descriptor
   //
   class StuffingDesc : public StringDataDesc
   {
   public:
      enum { TAG = 0x42 };

      // constructor
      StuffingDesc(char c, ui8 len = 1) :
         StringDataDesc( TAG, std::string(len, c) ) { }
      StuffingDesc(const std::string& data) : StringDataDesc( TAG, data ) {}

      // utility
      virtual Descriptor* clone() const { return new StuffingDesc(*this); }

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream& o) const {
         dumpData( o, STUFFING_D_S, DATA_S );
      }
#endif
   };


   // ---------------------------
   // Transport Stream Descriptor
   //
   class TransportStreamDesc : public StringDataDesc
   {
   public:
      enum { TAG = 0x67 };

      // default constructor identifies DVB streams
      TransportStreamDesc() :
         StringDataDesc( TAG, "DVB" ) {}
      // constructor for other types
      TransportStreamDesc(const std::string& data) :
         StringDataDesc( TAG, data ) {}

      // utility
      virtual Descriptor* clone() const { return new TransportStreamDesc(*this); }

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
   private:
      enum {
         MAX_CP_LEN  = 3,
         MAX_IAC_LEN = 7,
         MAX_OC_LEN  = 3,
         MAX_NAC_LEN = 7,
         MAX_CN_LEN  = 15
      };

      // telephone data
      std::string country_prefix,
         international_area_code,
         operator_code,
         national_area_code,
         core_number;
      ui8 connection_type : 5;
      bool foreign_availability,
         reserved;

   public:
      enum { TAG = 0x57, BASE_LEN = 3 };

      // constructor
      TelephoneDesc(bool fa,
                    ui8 ct,
                    const std::string& cp,
                    const std::string& iac,
                    const std::string& oc,
                    const std::string& nac,
                    const std::string& cn,
                    bool rsrvd = true);

      // utility functions
      virtual Descriptor *clone() const { return new TelephoneDesc(*this); }
      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif

   protected:
      void setCode(std::string& dest_c, const std::string& src_c, ui8 max);
   };

} // sigen namespace

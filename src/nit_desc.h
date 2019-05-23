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
// nit_desc.h: class definition for the NIT descriptors
// -----------------------------------

#pragma once

#include <string>
#include <list>
#include "descriptor.h"
#include "dvb_defs.h"

namespace sigen {

   // ---------------------------
   // Base delivery class for the three delivery descriptors
   class DeliveryDesc : public Descriptor
   {
   protected:
      DeliveryDesc(ui8 tag, ui8 len) :
         Descriptor(tag, len)
      { }
      DeliveryDesc() = delete;
   };


   // ---------------------------
   // Cable Delivery System Descriptor
   //
   class CableDeliverySystemDesc : public DeliveryDesc
   {
   private:
      ui32 _frequency,
         _symbol_rate : 28;
      ui8 _modulation,
         _fec_outer : 4,
         _fec_inner : 4;
      bool _reserved;

   public:
      enum { TAG = 0x44, BASE_LEN = 11 };

      // constructor
      CableDeliverySystemDesc(ui32 freq,
                              ui32 sym_rate,
                              ui8 fec_o = Dvb::Cable::UNDEF_FECO,
                              ui8 mod = Dvb::Cable::UNDEF_MOD,
                              ui8 fec_i = Dvb::UNDEF_FECI,
                              bool rsrvd = true) :
         DeliveryDesc(TAG, BASE_LEN),
         _frequency(freq),
         _symbol_rate(sym_rate),
         _modulation(mod),
         _fec_outer(fec_o),
         _fec_inner(fec_i),
         _reserved(rsrvd)
      { }

      // utility methods
      ui32 frequency() const { return _frequency; }
      ui32 symbolRate() const { return _symbol_rate; }
      ui8 modulation() const { return _modulation; }
      ui8 fecInner() const { return _fec_inner; }
      ui8 fecOuter() const { return _fec_outer; }

      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif
   };


   // ---------------------------
   // Satellite Delivery System Descriptor
   //
   class SatelliteDeliverySystemDesc : public DeliveryDesc
   {
   private:
      ui32 _frequency,
         _symbol_rate : 28;
      ui16 _orbital_position;
      ui8 _modulation : 5,
         _polarisation : 2,
         _fec_inner : 4;
      bool _west_east;

   public:
      enum { TAG = 0x43, BASE_LEN = 11 };

      // constructor
      SatelliteDeliverySystemDesc(ui32 freq, ui16 orb_pos, ui32 sym_rate,
                                  bool wef = false,
                                  ui8 pol = Dvb::Sat::LINEAR_HOR_POL,
                                  ui8 mod = Dvb::Sat::UNDEF_MOD,
                                  ui8 fec_i = Dvb::UNDEF_FECI) :
         DeliveryDesc(TAG, BASE_LEN),
         _frequency(freq),
         _symbol_rate(sym_rate),
         _orbital_position(orb_pos),
         _modulation(mod),
         _polarisation(pol),
         _fec_inner(fec_i),
         _west_east(wef)
      { }

      // utility methods
      ui32 frequency() const { return _frequency; }
      ui32 symbolRate() const { return _symbol_rate; }
      ui16 orbitalPosition() const { return _orbital_position; }
      ui8 modulation() const { return _modulation; }
      ui8 polarisation() const { return _polarisation; }
      ui8 fecInner() const { return _fec_inner; }
      bool westEastFlag() const { return _west_east; }

      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif
   };


   // ---------------------------
   // Terrestrial Delivery System Descriptor
   //
   class TerrestrialDeliverySystemDesc : public Descriptor
   {
   private:
      ui32 _ctr_frequency;
      ui8 _bandwidth : 3,
         _constellation : 2,
         _hierarchy_info : 3,
         _cr_HP_stream : 3,
         _cr_LP_stream : 3,
         _guard_interval : 2,
         _transmission_mode : 2;
      bool _other_freq_flag;
      bool _reserved;

   public:
      enum { TAG = 0x5a, BASE_LEN = 11 };

      // constructor
      TerrestrialDeliverySystemDesc(ui32 cfreq, ui8 bandw, ui8 constel,
                                    ui8 hier_i, ui8 cr_HP, ui8 cr_LP,
                                    ui8 guard_i, ui8 trans_m, bool other_f_f,
                                    bool rsrvd = true) :
         Descriptor(TAG, BASE_LEN),
         _ctr_frequency(cfreq),
         _bandwidth(bandw),
         _constellation(constel),
         _hierarchy_info(hier_i),
         _cr_HP_stream(cr_HP),
         _cr_LP_stream(cr_LP),
         _guard_interval(guard_i),
         _transmission_mode(trans_m),
         _other_freq_flag(other_f_f),
         _reserved(rsrvd)
      { }

      // utility methods
      ui32 frequency() const { return _ctr_frequency; }
      ui8 bandwidth() const { return _bandwidth; }
      ui8 constellation() const { return _constellation; }
      ui8 hierarchyInfo() const { return _hierarchy_info; }
      ui8 codeRateHPStream() const { return _cr_HP_stream; }
      ui8 codeRateLPStream() const { return _cr_LP_stream; }
      ui8 guardInterval() const { return _guard_interval; }
      bool otherFrequencyFlag() const { return _other_freq_flag; }

      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif
   };



   // other NIT descriptors

   // ---------------------------
   // Announcement Support Descriptor
   //
   class AnnouncementSupportDesc : public Descriptor
   {
   public:
      enum { TAG = 0x6e, BASE_LEN = 2 };

      // announcement support values
      enum {
         EMERGENCY_ALARM_AS        = 0x0001,
         ROAD_TRAFFIC_FLASH_AS     = 0x0002,
         PUBLIC_TRANSPORT_FLASH_AS = 0x0004,
         WARNING_MESSAGE_AS        = 0x0008,
         NEWS_FLASH_AS             = 0x0010,
         WEATHER_FLASH_AS          = 0x0020,
         EVENT_ANNOUNCEMENT_AS     = 0x0040,
         PERSONAL_CALL_AS          = 0x0080,
      };

      // announcement types
      enum {
         EMERGENCY_ALARM,
         ROAD_TRAFFIC_FLASH,
         PUBLIC_TRANSPORT_FLASH,
         WARNING_MESSAGE,
         NEWS_FLASH,
         WEATHER_FLASH,
         EVENT_ANNOUNCEMENT,
         PERSONAL_CALL,
      };

      // reference types
      enum {
         SERVICE_AUDIO_STREAM,
         SERVICE_SEPARATE_AUDIO_STREAM,
         DIFFERENT_SERVICE,
         DIFFERENT_SERVICE_AND_TRANSPORT_STREAM,

         // internal use
         NUM_DEFINED_REF_TYPES
      };

      // announcement data struct
      struct Announcement {
         enum { BASE_LEN = 1 };

         ui8 type : 4,
            reference_type : 3;
         ui16 original_network_id,
            xport_stream_id,
            service_id;
         ui8 component_tag;
         bool reserved;

         // constructor
         Announcement(ui8 announcement_type, ui8 ref_type,
                      ui16 onid, ui16 tsid, ui16 sid, ui8 c_tag,
                      bool rsrvd) :
            type( announcement_type ),
            reference_type( ref_type ),
            original_network_id( onid ),
            xport_stream_id( tsid ),
            service_id( sid ),
            component_tag( c_tag ),
            reserved( rsrvd )
         { }

         ui8 length() const { return BASE_LEN +
               (reference_type < NUM_DEFINED_REF_TYPES ?
				7 : 0); }
      };

      // constructor
      AnnouncementSupportDesc(ui16 announcement_support_ind,
                              bool reserved = true) :
         Descriptor( TAG, BASE_LEN ),
         announcement_support_indicator(announcement_support_ind)
      { }
      AnnouncementSupportDesc() = delete;

      // utility methods
      bool addAnnouncement(ui8 type, ui8 reference_type,
                           ui16 onid = 0, ui16 tsid = 0, ui16 sid = 0,
                           ui8 c_tag = 0, bool rsrvd = true);

      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif

   private:
      // descriptor data members
      ui16 announcement_support_indicator;
      std::list<Announcement> announcement_list;
   };



   // ---------------------------
   // Cell Frequency Link Descriptor
   //
   class CellFrequencyLinkDesc : public Descriptor
   {
   public:
      enum { TAG = 0x6d, BASE_LEN = 0 };

      // funky descriptor with a loop within a loop
      struct Link {

         struct SubCell {
            enum { BASE_LEN = 5 };

            ui8 cell_id_extension;
            ui32 transposer_frequency;

            // constructor
            SubCell( ui8 cid_ext, ui32 xposer_freq ) :
               cell_id_extension( cid_ext ),
               transposer_frequency( xposer_freq ) { }
         };

         // Link members
         enum { BASE_LEN = 7 };

         ui16 cell_id;
         ui32 frequency;
         std::list<SubCell> subcell_list;

         // constructor
         Link(ui16 c_id, ui32 freq) :
            cell_id( c_id ),
            frequency( freq )
         { }

         ui8 length() const { return BASE_LEN +
               subcell_list.size() * SubCell::BASE_LEN; }
      };


      // constructor
      CellFrequencyLinkDesc() : Descriptor( TAG, BASE_LEN ) { }

      // utility methods
      bool addLink(ui16 cell_id, ui32 frequency);
      bool addLinkSubCell(ui16 cell_id, ui32 frequency, ui8 cid_ext,
                          ui32 xposer_freq);
      // adds to the most recently added link
      bool addLinkSubCell(ui8 cid_ext, ui32 xposer_freq);

      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif

   private:
      // descriptor data members
      std::list<Link> cflink_list;

   protected:
      bool addLinkSubCell(Link& l, ui8 cid_ext, ui32 xposer_freq);
   };


   // ---------------------------
   // Cell List Descriptor
   //
   class CellListDesc : public Descriptor
   {
   public:
      enum { TAG = 0x6c, BASE_LEN = 0 };

      // another funky descriptor with a loop within a loop
      struct Cell {

         struct SubCell {
            enum { BASE_LEN = 8 };

            ui8 cell_id_extension;
            ui16 latitude;
            ui16 longitude;
            ui16 extend_of_latitude : 12;
            ui16 extend_of_longitude : 12;

            // constructor
            SubCell( ui8 cid_ext, ui16 lat, ui16 lon,
                     ui16 ext_lat, ui16 ext_lon) :
               cell_id_extension( cid_ext ),
               latitude( lat ),
               longitude( lon ),
               extend_of_latitude( ext_lat ),
               extend_of_longitude( ext_lon )
            { }
         };

         // Cell members
         enum { BASE_LEN = 10 };

         ui16 id;
         ui16 latitude;
         ui16 longitude;
         ui32 extend_of_latitude : 12,
            extend_of_longitude : 12;

         std::list<SubCell> subcell_list;

         // constructor
         Cell(ui16 c_id, ui16 lat, ui16 lon, ui16 ext_lat, ui16 ext_lon) :
            id( c_id ),
            latitude( lat ),
            longitude( lon ),
            extend_of_latitude( ext_lat ),
            extend_of_longitude( ext_lon )
         { }

         ui8 length() const { return BASE_LEN +
               subcell_list.size() * SubCell::BASE_LEN; }
      };


      // constructor
      CellListDesc() : Descriptor( TAG, BASE_LEN ) { }

      // utility methods
      bool addCell(ui16 id, ui16 lat, ui16 lon, ui16 ext_lat, ui16 ext_lon);
      bool addCellSubCell(ui16 id, ui8 cid_ext, ui16 sc_lat, ui16 sc_lon,
                          ui16 sc_ext_lat, ui16 sc_ext_lon);

      // adds the subcell to the most recently added cell
      bool addCellSubCell(ui8 cid_ext, ui16 sc_lat, ui16 sc_lon,
                          ui16 sc_ext_lat, ui16 sc_ext_lon);

      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif

   private:
      // descriptor data members
      std::list<Cell> cell_list;

   protected:
      bool addCellSubCell(Cell& cell, ui8 cid_ext, ui16 sc_lat, ui16 sc_lon,
                          ui16 sc_ext_lat, ui16 sc_ext_lon);
   };



   // ---------------------------
   // Frequency List Descriptor
   //
   class FrequencyListDesc : public Descriptor
   {
   private:
      std::list<ui32> frequency_list;
      bool reserved;
      ui8 coding_type : 2;

   public:
      enum { TAG = 0x62, BASE_LEN = 1 };

      // coding type values
      enum Coding {
         UNDEFINED   = 0x0,
         SATELLITE   = 0x1,
         CABLE       = 0x2,
         TERRESTRIAL = 0x3,
      };

      // constructor
      FrequencyListDesc(ui8 ct, bool rsrvd = true) :
         Descriptor(TAG, BASE_LEN),
         reserved(rsrvd),
         coding_type(ct) { }
      FrequencyListDesc() = delete;

      // utility functions
      bool addFrequency(ui32 f);
      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif
   };


   // ---------------------------
   // Multilingual Network Name Descriptor
   // - derived from MultilingualTextDesc (descriptor.h)
   //
   class MultilingualNetworkNameDesc : public MultilingualTextDesc
   {
   public:
      enum { TAG = 0x5b, BASE_LEN = 0 };

      // constructor
      MultilingualNetworkNameDesc() : MultilingualTextDesc(TAG, BASE_LEN) {}

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream& o) const {
         dumpData(o, MULTILING_NET_NAME_D_S, NETWORK_NAME_S );
      }
#endif
   };


   // ---------------------------
   // Network Name Descriptor
   //
   class NetworkNameDesc : public StringDataDesc
   {
   public:
      enum { TAG = 0x40 }; // BASE_LEN implicitly defined by StringDataDesc

      // constructor
      NetworkNameDesc(const std::string& name) : StringDataDesc(TAG, name) { }
      NetworkNameDesc() = delete;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream& o) const {
         dumpData( o, NETWORK_NAME_D_S, NETWORK_NAME_S );
      }
#endif
   };

} // sigen namespace


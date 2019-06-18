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
   public:
      enum { TAG = 0x44 };

      // constructor
      CableDeliverySystemDesc(ui32 freq,
                              ui32 sym_rate,
                              ui8 fec_o = Dvb::Cable::UNDEF_FECO,
                              ui8 mod = Dvb::Cable::UNDEF_MOD,
                              ui8 fec_i = Dvb::UNDEF_FECI) :
         DeliveryDesc(TAG, 11),
         frequency(freq),
         symbol_rate(sym_rate),
         modulation(mod),
         fec_outer(fec_o),
         fec_inner(fec_i)
      { }

      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif

   private:
      ui32 frequency;
      ui32 symbol_rate : 28;
      ui8  modulation;
      ui8  fec_outer : 4,
           fec_inner : 4;
   };


   // ---------------------------
   // Satellite Delivery System Descriptor
   //
   class SatelliteDeliverySystemDesc : public DeliveryDesc
   {
   public:
      enum { TAG = 0x43 };

      // constructor
      SatelliteDeliverySystemDesc(ui32 freq, ui16 orb_pos, ui32 sym_rate,
                                  bool wef = false,
                                  ui8 pol = Dvb::Sat::LINEAR_HOR_POL,
                                  ui8 mod = Dvb::Sat::UNDEF_MOD,
                                  ui8 fec_i = Dvb::UNDEF_FECI) :
         DeliveryDesc(TAG, 11),
         frequency(freq),
         symbol_rate(sym_rate),
         orbital_position(orb_pos),
         modulation(mod),
         polarisation(pol),
         fec_inner(fec_i),
         west_east(wef)
      { }

      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif

   private:
      ui32 frequency;
      ui32 symbol_rate : 28;
      ui16 orbital_position;
      ui8  modulation : 5,
           polarisation : 2,
           fec_inner : 4;
      bool west_east;
   };


   // ---------------------------
   // Terrestrial Delivery System Descriptor
   //
   class TerrestrialDeliverySystemDesc : public Descriptor
   {
   public:
      enum { TAG = 0x5a };

      // constructor
      TerrestrialDeliverySystemDesc(ui32 cfreq, ui8 bandw, ui8 constel,
                                    ui8 hier_i, ui8 cr_HP, ui8 cr_LP,
                                    ui8 guard_i, ui8 trans_m, bool other_f_f) :
         Descriptor(TAG, 11),
         ctr_frequency(cfreq),
         bandwidth(bandw),
         constellation(constel),
         hierarchy_info(hier_i),
         cr_HP_stream(cr_HP),
         cr_LP_stream(cr_LP),
         guard_interval(guard_i),
         transmission_mode(trans_m),
         other_freq_flag(other_f_f)
      { }

      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif

   private:
      ui32 ctr_frequency;
      ui8  bandwidth : 3,
           constellation : 2,
           hierarchy_info : 3,
           cr_HP_stream : 3,
           cr_LP_stream : 3,
           guard_interval : 2,
           transmission_mode : 2;
      bool other_freq_flag;
   };



   // other NIT descriptors

   // ---------------------------
   // Announcement Support Descriptor
   //
   class AnnouncementSupportDesc : public Descriptor
   {
   public:
      enum { TAG = 0x6e };

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
      struct Announcement : public STable::ListItem {
         // present if ref_type is 0x01, 0x02, or 0x03
         struct ActualServInfo {
            ActualServInfo(ui16 onid, ui16 tsid, ui16 sid, ui8 c_tag) :
               original_network_id(onid), xport_stream_id(tsid), service_id(sid), component_tag(c_tag) {}

            ui16 original_network_id,
                 xport_stream_id,
                 service_id;
            ui8  component_tag;
         };

         ui8 type : 4,
             reference_type : 3;
         ActualServInfo* actual_serv_info;

         // constructor
         Announcement(ui8 announcement_type, ui8 ref_type,
                      ui16 onid, ui16 tsid, ui16 sid, ui8 c_tag) :
            type( announcement_type ),
            reference_type( ref_type ),
            actual_serv_info((ref_type < DIFFERENT_SERVICE_AND_TRANSPORT_STREAM) ?
                             new ActualServInfo(onid, tsid, sid, c_tag) :
                             nullptr)
         {}
         Announcement() = delete;
         ~Announcement() { delete actual_serv_info; }

         ui8 length() const { return expected_length(reference_type); }
         static ui8 expected_length(ui8 ref_type) { return (ref_type < NUM_DEFINED_REF_TYPES ? 8 : 1); }
      };

      // constructor
      AnnouncementSupportDesc(ui16 announcement_support_ind) :
         Descriptor( TAG, 2 ),
         announcement_support_indicator(announcement_support_ind)
      { }
      AnnouncementSupportDesc() = delete;

      // populate announcement loop - this method is for ref_types 0x01, 0x02, 0x03
      bool addAnnouncement(ui8 type, ui8 reference_type,
                           ui16 onid, ui16 tsid, ui16 sid,
                           ui8 c_tag);
      // for ref_type 0x04
      bool addAnnouncement(ui8 type) {
         return addAnnouncement(type, DIFFERENT_SERVICE_AND_TRANSPORT_STREAM, 0, 0, 0, 0);
      }

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
      enum { TAG = 0x6d };

      // funky descriptor with a loop within a loop
      struct Link : public STable::ListItem {
         struct SubCell : public STable::ListItem {
            enum { BASE_LEN = 5 };

            ui8 cell_id_extension;
            ui32 transposer_frequency;

            // constructor
            SubCell( ui8 cid_ext, ui32 xposer_freq ) :
               cell_id_extension( cid_ext ),
               transposer_frequency( xposer_freq ) { }
            SubCell() = delete;
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
         Link() = delete;

         ui8 length() const { return BASE_LEN + subcell_list.size() * SubCell::BASE_LEN; }
      };


      // constructor
      CellFrequencyLinkDesc() : Descriptor(TAG) { }

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
      enum { TAG = 0x6c };

      // another funky descriptor with a loop within a loop
      struct Cell : public STable::ListItem {
         struct SubCell : public STable::ListItem {
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
            SubCell() = delete;
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
         Cell() = delete;

         ui8 length() const { return BASE_LEN + subcell_list.size() * SubCell::BASE_LEN; }
      };


      // constructor
      CellListDesc() : Descriptor(TAG) { }

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
   public:
      enum { TAG = 0x62 };

      // coding type values
      enum Coding {
         UNDEFINED   = 0x0,
         SATELLITE   = 0x1,
         CABLE       = 0x2,
         TERRESTRIAL = 0x3,
      };

      // constructor
      FrequencyListDesc(ui8 ct) :
         Descriptor(TAG, 1),
         coding_type(ct) { }
      FrequencyListDesc() = delete;

      // utility functions
      bool addFrequency(ui32 f);
      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif

   private:
      std::list<ui32> frequency_list;
      ui8 coding_type : 2;
   };


   // ---------------------------
   // Multilingual Network Name Descriptor
   // - derived from MultilingualTextDesc (descriptor.h)
   //
   struct MultilingualNetworkNameDesc : public MultilingualTextDesc
   {
      enum { TAG = 0x5b };

      // constructor
      MultilingualNetworkNameDesc() : MultilingualTextDesc(TAG) {}

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream& o) const {
         dumpData(o, MULTILING_NET_NAME_D_S, NETWORK_NAME_S );
      }
#endif
   };


   // ---------------------------
   // Network Name Descriptor
   //
   struct NetworkNameDesc : public StringDataDesc
   {
      enum { TAG = 0x40 };

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


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
      DeliveryDesc(ui8 tag, ui8 len) : Descriptor(tag, len) { }
      DeliveryDesc() = delete;
   };

   /*! \addtogroup descriptor
    *  @{
    */

   /*!
    * \defgroup delivery_d Delivery Descriptors
    *  @{
    */

   /*!
    * \brief Cable Delivery System Descriptor.
    */
   class CableDeliverySystemDesc : public DeliveryDesc
   {
   public:
      enum { TAG = 0x44 };

      /*!
       * \brief Constructor
       * \param freq BCD frequency.
       * \param sym_rate Symbol rate in Msymbol/s.
       * \param fec_o Outer Forward Error Correction scheme, as per sigen::Dvb::Cable::FecOuter_t.
       * \param mod Modulation scheme, as per sigen::Dvb::Cable::Mod_t.
       * \param fec_i Inner Foward Error Correction scheme, as per sigen::Dvb::FecInner_t.
       */
      CableDeliverySystemDesc(ui32 freq, ui32 sym_rate, ui8 fec_o, ui8 mod, ui8 fec_i)
         : DeliveryDesc(TAG, 11),
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


   /*!
    * \brief Satellite Delivery System Descriptor, as per ETSI EN 300 468 1.15.1 (2016-03).
    */
   class SatelliteDeliverySystemDesc : public DeliveryDesc
   {
   public:
      enum { TAG = 0x43 };

      /*!
       * \brief Constructor for DVB-S modulation systems.
       * \param freq BCD frequency.
       * \param orb_pos BCD orbital position.
       * \param sym_rate Symbol rate in Msymbol/s.
       * \param wef West-East flag. `false`: western position, `true`: eastern position.
       * \param pol Polarization of the transmitted signal, as per sigen::Dvb::Sat::Pol_t.
       * \param mod_type Modulation type, as per sigen::Dvb::Sat::Mod_t.
       * \param fec_i Inner Foward Error Correction scheme, as per sigen::Dvb::FecInner_t.
       */
      SatelliteDeliverySystemDesc(ui32 freq, ui16 orb_pos, ui32 sym_rate, bool wef,
                                  ui8 pol, ui8 mod_type, ui8 fec_i)
         : SatelliteDeliverySystemDesc(freq, orb_pos, sym_rate, wef, pol, Dvb::Sat::MODSYS_DVB_S,
                                       mod_type, 0, fec_i)
      { }
      /*!
       * \brief Constructor for DVB-S2 modulation systems.
       * \param freq BCD frequency.
       * \param orb_pos BCD orbital position.
       * \param sym_rate Symbol rate in Msymbol/s.
       * \param wef West-East flag. `false`: western position, `true`: eastern position.
       * \param pol Polarization of the transmitted signal, as per sigen::Dvb::Sat::Pol_t.
       * \param mod_type Modulation type, as per sigen::Dvb::Sat::Mod_t.
       * \param fec_i Inner Foward Error Correction scheme, as per sigen::Dvb::FecInner_t.
       * \param rof Roll-off factor, as per sigen::Dvb::Sat::RolloffFactor_t.
       */
      SatelliteDeliverySystemDesc(ui32 freq, ui16 orb_pos, ui32 sym_rate, bool wef,
                                  ui8 pol, ui8 mod_type, ui8 fec_i, ui8 rof)
         : SatelliteDeliverySystemDesc(freq, orb_pos, sym_rate, wef, pol, Dvb::Sat::MODSYS_DVB_S2,
                                       mod_type, rof, fec_i)
      { }

      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif

   private:
      ui32 frequency;
      ui32 symbol_rate : 28;
      ui16 orbital_position;
      ui8  modulation_system : 1,
           modulation_type : 2,
           roll_off : 2;
      ui8  polarisation : 2,
           fec_inner : 4;
      bool west_east;

      SatelliteDeliverySystemDesc(ui32 freq, ui16 orb_pos, ui32 sym_rate, bool wef,
                                  ui8 pol, ui8 mod_sys, ui8 mod_type, ui8 ro, ui8 fec_i)
         : DeliveryDesc(TAG, 11),
           frequency(freq),
           symbol_rate(sym_rate),
           orbital_position(orb_pos),
           modulation_system(mod_sys),
           modulation_type(mod_type),
           roll_off(ro),
           polarisation(pol),
           fec_inner(fec_i),
           west_east(wef)
      { }

   };


   /*!
    * \brief Terrestrial Delivery System Descriptor.
    */
   class TerrestrialDeliverySystemDesc : public Descriptor
   {
   public:
      enum { TAG = 0x5a };

      /*!
       * \brief Constructor
       * \param cfreq Center frequency.
       * \param bandw Bandwidth.
       * \param constel Constellation pattern.
       * \param hier_i Hierarchy information specifying the α value.
       * \param cr_HP Code Rate HP stream.
       * \param cr_LP Code Rate LP stream.
       * \param guard_i Guard interval.
       * \param trans_m Transmission mode.
       * \param other_f_f `false`: no other freq. in use. `true`: One or more other freqs. in use.
       */
      TerrestrialDeliverySystemDesc(ui32 cfreq, ui8 bandw, ui8 constel,
                                    ui8 hier_i, ui8 cr_HP, ui8 cr_LP,
                                    ui8 guard_i, ui8 trans_m, bool other_f_f)
         : Descriptor(TAG, 11),
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
   //! @}


   // other NIT descriptors

   /*!
    * \brief Announcement Support Descriptor.
    */
   class AnnouncementSupportDesc : public Descriptor
   {
   public:
      enum { TAG = 0x6e };

      // announcement support values
      enum AnnouncementSupportIndicator {
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
      enum AnnouncementType {
         EMERGENCY_ALARM_AT,
         ROAD_TRAFFIC_FLASH_AT,
         PUBLIC_TRANSPORT_FLASH_AT,
         WARNING_MESSAGE_AT,
         NEWS_FLASH_AT,
         WEATHER_FLASH_AT,
         EVENT_ANNOUNCEMENT_AT,
         PERSONAL_CALL_AT,
      };

      // reference types
      enum ReferenceType {
         SERVICE_AUDIO_STREAM_RT                    = 0,
         SERVICE_SEPARATE_AUDIO_STREAM_RT           = 0x01,
         DIFFERENT_SERVICE_RT                       = 0x02,
         DIFFERENT_SERVICE_AND_TRANSPORT_STREAM_RT  = 0x03
      };

      /*!
       * \brief Constructor.
       * \param announcement_support_ind Announcement support indicator.
       */
      AnnouncementSupportDesc(ui16 announcement_support_ind)
         : Descriptor( TAG, 2 ),
         announcement_support_indicator(announcement_support_ind)
      { }
      AnnouncementSupportDesc() = delete;

      /*!
       * \brief Add an announcement to the data loop for ref types 0x01, 0x02, 0x03.
       * \param type Announcement type.
       * \param reference_type Transport method.
       * \param onid Id of the originating network.
       * \param ts_id Unique id of the transport stream.
       * \param sid Unique id of the service containing the announcements indicated.
       * \param c_tag Component tag.
       */
      bool addAnnouncement(ui8 type, ui8 reference_type,
                           ui16 onid, ui16 ts_id, ui16 sid,
                           ui8 c_tag);
      /*!
       * \brief Add an announcement to the data loop for ref types 0 (SERVICE_AUDIO_STREAM).
       * \param type Announcement type.
       */
      bool addAnnouncement(ui8 type) {
         return addAnnouncement(type, SERVICE_AUDIO_STREAM_RT, 0, 0, 0, 0);
      }

      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif

   private:
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
            actual_serv_info(carries_service_info(ref_type) ?
                             new ActualServInfo(onid, tsid, sid, c_tag) :
                             nullptr)
         {}
         Announcement() = delete;
         ~Announcement() { delete actual_serv_info; }

         ui8 length() const { return expected_length(reference_type); }
         static bool carries_service_info(ui8 ref_type) {
            return (ref_type == SERVICE_SEPARATE_AUDIO_STREAM_RT ||
                    ref_type == DIFFERENT_SERVICE_RT ||
                    ref_type == DIFFERENT_SERVICE_AND_TRANSPORT_STREAM_RT);
         }
         static ui8 expected_length(ui8 ref_type) { return (carries_service_info(ref_type) ? 8 : 1); }
      };

      // descriptor data members
      ui16 announcement_support_indicator;
      std::list<Announcement> announcement_list;
   };


   /*!
    * \brief Cell Frequency Link Descriptor.
    */
   class CellFrequencyLinkDesc : public Descriptor
   {
   public:
      enum { TAG = 0x6d };

      //! \brief Constructor
      CellFrequencyLinkDesc() : Descriptor(TAG) { }

      /*!
       * \brief Add a cell to the descriptor data loop.
       * \param cell_id Unique id of the cell.
       * \param frequency Main frequency used in the indicated cell.
       */
      bool addCell(ui16 cell_id, ui32 frequency);
      /*!
       * \brief Add a cell id extension to the last added cell.
       * \param cid_ext If of the subcell within the cell.
       * \param xposer_freq Transposer frequency in the subcell.
       */
      bool addSubCell(ui8 cid_ext, ui32 xposer_freq);
      /*!
       * \brief Add a cell id extension to the specified cell.
       * \param cell_id Id of the cell entry already in the descriptor.
       * \param cid_ext If of the subcell within the cell.
       * \param xposer_freq Transposer frequency in the subcell.
       */
      bool addSubCell(ui16 cell_id, ui8 cid_ext, ui32 xposer_freq);

      [[deprecated("replaced by addCell()")]] bool addLink(ui16 cid, ui32 f) { return addCell(cid, f); }
      [[deprecated("replaced by addSubCell()")]] bool addLinkSubCell(ui8 cid_ext, ui32 f) {
          return addSubCell(cid_ext, f);
       }
      [[deprecated("replaced by addSubCell()")]] bool addLinkSubCell(ui16 cid, ui32 f, ui16 cid_ext, ui32 xf) {
         return addSubCell(cid, cid_ext, xf);
      }

      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif

   private:
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

      // descriptor data members
      std::list<Link> cflink_list;

   protected:
      bool add_subcell(Link& l, ui8 cid_ext, ui32 xposer_freq);
   };


   /*!
    * \brief Cell List Descriptor.
    */
   class CellListDesc : public Descriptor
   {
   public:
      enum { TAG = 0x6c };

      //! \brief Constructor
      CellListDesc() : Descriptor(TAG) { }

      /*!
       * \brief Add a cell to the descriptor data loop.
       * \param id Unique id of the cell.
       * \param lat Cell latitude.
       * \param lon Cell longitude.
       * \param ext_lat Cell extent of latitude.
       * \param ext_lon Cell extent of longitude.
       */
      bool addCell(ui16 id, ui16 lat, ui16 lon, ui16 ext_lat, ui16 ext_lon);
      /*!
       * \brief Add a sub cell to the last added cell.
       * \param cid_ext Id of subcell within the cell.
       * \param sc_lat Subcell latitude.
       * \param sc_lon Subcell longitude.
       * \param sc_ext_lat Subcell extent of latitude.
       * \param sc_ext_lon Subcell extent of longitude.
       */
      bool addSubCell(ui8 cid_ext, ui16 sc_lat, ui16 sc_lon,
                      ui16 sc_ext_lat, ui16 sc_ext_lon);
      /*!
       * \brief Add a sub cell to the specified cell.
       * \param id Id of cell already added to the descriptor.
       * \param cid_ext Id of subcell within the cell.
       * \param sc_lat Subcell latitude.
       * \param sc_lon Subcell longitude.
       * \param sc_ext_lat Subcell extent of latitude.
       * \param sc_ext_lon Subcell extent of longitude.
       */
      bool addSubCell(ui16 id, ui8 cid_ext, ui16 sc_lat, ui16 sc_lon,
                      ui16 sc_ext_lat, ui16 sc_ext_lon);

      [[deprecated("replaced by addSubCell()")]] bool addCellSubCell(ui8 cid_ext, ui16 sc_lat, ui16 sc_lon,
                                                                     ui16 sc_ext_lat, ui16 sc_ext_lon) {
         return addSubCell(cid_ext, sc_lat, sc_lon, sc_ext_lat, sc_ext_lon);
      }
      [[deprecated("replaced by addSubCell()")]] bool addCellSubCell(ui16 id, ui8 cid_ext, ui16 sc_lat, ui16 sc_lon,
                                                                     ui16 sc_ext_lat, ui16 sc_ext_lon) {
         return addSubCell(id, cid_ext, sc_lat, sc_lon, sc_ext_lat, sc_ext_lon);
      }

      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif

   private:
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

      // descriptor data members
      std::list<Cell> cell_list;

   protected:
      bool add_subcell(Cell& cell, ui8 cid_ext, ui16 sc_lat, ui16 sc_lon,
                       ui16 sc_ext_lat, ui16 sc_ext_lon);
   };


   /*!
    * \brief Frequency List Descriptor.
    */
   class FrequencyListDesc : public Descriptor
   {
   public:
      enum { TAG = 0x62 };

      /*!
       * \enum  Coding
       *
       * \brief Coding type values
       */
      enum Coding {
         UNDEFINED   = 0x0,
         SATELLITE   = 0x1,
         CABLE       = 0x2,
         TERRESTRIAL = 0x3,
      };

      /*!
       * \brief Constructor.
       * \param ct Coding type, as per FrequencyListDesc::Coding.
       */
      FrequencyListDesc(ui8 ct)
         : Descriptor(TAG, 1),
         coding_type(ct) { }
      FrequencyListDesc() = delete;

      /*!
       * \brief Add a frequency to the data loop.
       * \param centre_freq Frequency as defined in the delivery system descriptor indicated.
       */
      bool addFrequency(ui32 centre_freq);

      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif

   private:
      std::list<ui32> frequency_list;
      ui8 coding_type : 2;
   };


   /*!
    * \brief Multilingual Network Name Descriptor.
    */
   struct MultilingualNetworkNameDesc : public MultilingualTextDesc
   {
      enum { TAG = 0x5b };

      //! \brief Constructor.
      MultilingualNetworkNameDesc() : MultilingualTextDesc(TAG) {}

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream& o) const {
         dumpData(o, MULTILING_NET_NAME_D_S, NETWORK_NAME_S );
      }
#endif
   };


   /*!
    * \brief Network Name Descriptor.
    */
   struct NetworkNameDesc : public StringDataDesc
   {
      enum { TAG = 0x40 };

      /*!
       * \brief Constructor.
       * \param name Name of the delivery system informed by the NIT.
       */
      NetworkNameDesc(const std::string& name) : StringDataDesc(TAG, name) { }
      NetworkNameDesc() = delete;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream& o) const {
         dumpData( o, NETWORK_NAME_D_S, NETWORK_NAME_S );
      }
#endif
   };
   //! @}
} // sigen namespace

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
// nit_desc.cc: class definitions for the NIT descriptors
// -----------------------------------

#include <iostream>
#include <list>
#include <algorithm>
#include "descriptor.h"
#include "tstream.h"
#include "nit_desc.h"

namespace sigen
{
   // ---------------------------------------
   // announcement support descriptor
   //

   //
   // adds an announcement to the data loop
   bool AnnouncementSupportDesc::addAnnouncement(ui8 type, ui8 reference_type,
                                                 ui16 onid, ui16 tsid,
                                                 ui16 sid, ui8 c_tag)
   {
      // try to increment the descriptor's length
      if (!incLength( Announcement::expected_length(reference_type) ))
         return false; // can't do it - full

      // add it to the list
      announcement_list.emplace_back(type, reference_type, onid, tsid, sid, c_tag);
      return true;
   }

   //
   // data writer
   void AnnouncementSupportDesc::buildSections(Section& s) const
   {
      Descriptor::buildSections(s);

      s.set16Bits( announcement_support_indicator );

      // iterate through the list and write the data
      for (const auto& ann : announcement_list) {
         s.set08Bits( (ann.type << 4) |
                      rbits(0x08) |
                      ann.reference_type );

         if (ann.actual_serv_info) {
            s.set16Bits( ann.actual_serv_info->original_network_id );
            s.set16Bits( ann.actual_serv_info->xport_stream_id );
            s.set16Bits( ann.actual_serv_info->service_id );
            s.set08Bits( ann.actual_serv_info->component_tag );
         }
      }
   }


#ifdef ENABLE_DUMP
   void AnnouncementSupportDesc::dump(std::ostream& o) const
   {
      dumpHeader(o, ANNOUNCEMENT_SUPPORT_D_S);

      // dump the descriptor's data
      identStr( o, ANNOUNCEMENT_SUPPORT_IND_S, announcement_support_indicator );

      incOutLevel();
      for (const auto& ann : announcement_list) {
         identStr( o, ANNOUNCEMENT_TYPE_S, ann.type );
         identStr( o, RESERVED_FU_S, true );
         identStr( o, REF_TYPE_S, ann.reference_type );

         if (ann.actual_serv_info) {
            identStr( o, ORIG_NETWORK_ID_S, ann.actual_serv_info->original_network_id );
            identStr( o, XPORT_STREAM_ID_S, ann.actual_serv_info->xport_stream_id );
            identStr( o, SERVICE_ID_S, ann.actual_serv_info->service_id, true );
            identStr( o, COMPONENT_TAG_S, ann.actual_serv_info->component_tag );
         }
         o << std::endl;
      }
      decOutLevel();
   }
#endif


   // ---------------------------------------
   // cable delivery system descriptor
   //

   //
   // writes the desc to a dvb section
   //
   void CableDeliverySystemDesc::buildSections(Section &s) const
   {
      Descriptor::buildSections(s);

      s.set32Bits( frequency );
      s.set16Bits( rbits(0xfff0) | fec_outer );
      s.set08Bits( modulation );
      s.set32Bits( (symbol_rate << 4) | fec_inner );
   }


#ifdef ENABLE_DUMP
   //
   // to stdout
   void CableDeliverySystemDesc::dump(std::ostream &o) const
   {
      dumpHeader( o, CABLE_DEL_SYS_D_S );

      identStr(o, FREQ_S, frequency);
      identStr(o, RESERVED_FU_S, rbits(0xfff));
      identStr(o, FEC_O_S, fec_outer);
      identStr(o, MOD_S, modulation);
      identStr(o, SYM_RATE_S, symbol_rate);
      identStr(o, FEC_I_S, fec_inner);
   }
#endif


   // ---------------------------------------
   // cell frequency link descriptor
   //
   bool CellFrequencyLinkDesc::addLink(ui16 cell_id, ui32 frequency)
   {
      if (!incLength(Link::BASE_LEN))
         return false;

      cflink_list.emplace_back(cell_id, frequency);
      return true;
   }


   //
   // helper method to add a subcell to a link in the list
   bool CellFrequencyLinkDesc::addLinkSubCell(Link& link, ui8 cid_ext, ui32 xposer_freq)
   {
      // check if it fits
      if ( !incLength( Link::SubCell::BASE_LEN ) )
         return false;

      // add it
      link.subcell_list.emplace_back(cid_ext, xposer_freq);
      return true;
   }

   bool CellFrequencyLinkDesc::addLinkSubCell(ui16 cell_id, ui32 frequency, ui8 cid_ext, ui32 xposer_freq)
   {
      // look for a matching c/f link
      auto it = std::find_if(cflink_list.begin(), cflink_list.end(),
                             [=](const auto& l) { return (l.cell_id == cell_id &&
                                                          l.frequency == frequency); });
      if (it == cflink_list.end())
         return false;

      return addLinkSubCell(*it, cid_ext, xposer_freq);
   }

   bool CellFrequencyLinkDesc::addLinkSubCell(ui8 cid_ext, ui32 xposer_freq)
   {
      // if any have been added, try to add teh subcell to the one in the
      // end
      if (cflink_list.empty())
         return false;

      return addLinkSubCell( cflink_list.back(), cid_ext, xposer_freq );
   }

   //
   // format the binary data
   void CellFrequencyLinkDesc::buildSections(Section& s) const
   {
      Descriptor::buildSections(s);

      for (const auto& link : cflink_list) {
         s.set16Bits( link.cell_id );
         s.set32Bits( link.frequency );
         s.set08Bits( link.subcell_list.size() * Link::SubCell::BASE_LEN );

         for (const auto &subcell : link.subcell_list) {
            s.set08Bits( subcell.cell_id_extension );
            s.set32Bits( subcell.transposer_frequency );
         }
      }
   }

#ifdef ENABLE_DUMP
   void CellFrequencyLinkDesc::dump(std::ostream& o) const
   {
      dumpHeader( o, CELL_FREQ_LINK_D_S );

      for (const auto& link : cflink_list) {
         identStr(o, CELL_ID_S, link.cell_id );
         identStr(o, FREQ_S, link.frequency );
         identStr(o, SUBCELL_INFO_LOOP_LEN_S, link.subcell_list.size() * Link::SubCell::BASE_LEN );

         for (const auto& subcell : link.subcell_list) {
            identStr(o, CELL_ID_EXT_S, subcell.cell_id_extension );
            identStr(o, XPOSER_FREQ_S, subcell.transposer_frequency );
         }
      }
   }
#endif


   // ---------------------------------------
   // cell list descriptor
   //

   //
   // add a cell to the list
   bool CellListDesc::addCell(ui16 id, ui16 lat, ui16 lon,  ui16 ext_lat, ui16 ext_lon)
   {
      if ( !incLength(Cell::BASE_LEN) )
         return false;

      cell_list.emplace_back(id, lat, lon, ext_lat, ext_lon);
      return true;
   }

   bool CellListDesc::add_subcell(Cell& cell, ui8 cid_ext, ui16 sc_lat, ui16 sc_lon,
                                  ui16 sc_ext_lat, ui16 sc_ext_lon)
   {
      if ( !incLength( Cell::SubCell::BASE_LEN ) )
         return false;

      cell.subcell_list.emplace_back(cid_ext, sc_lat, sc_lon, sc_ext_lat, sc_ext_lon);
      return true;
   }


   bool CellListDesc::addSubCell(ui16 cell_id, ui8 cid_ext,
                                 ui16 sc_lat, ui16 sc_lon,
                                 ui16 sc_ext_lat, ui16 sc_ext_lon)
   {
      auto it = std::find_if(cell_list.begin(), cell_list.end(),
                             [=](const auto& c) { return c.id == cell_id; });
      if (it == cell_list.end())
         return false;

      return add_subcell(*it, cid_ext, sc_lat, sc_lon, sc_ext_lat, sc_ext_lon);
   }

   // adds the subcell to the most recently added cell
   bool CellListDesc::addSubCell(ui8 cid_ext, ui16 sc_lat, ui16 sc_lon,
                                 ui16 sc_ext_lat, ui16 sc_ext_lon)
   {
      if (cell_list.empty())
         return false;

      return add_subcell( cell_list.back(), cid_ext, sc_lat, sc_lon,
                          sc_ext_lat, sc_ext_lon );
   }

   //
   // format the binary data
   void CellListDesc::buildSections(Section& s) const
   {
      Descriptor::buildSections(s);

      for (const auto& cell : cell_list) {
         s.set16Bits( cell.id );
         s.set16Bits( cell.latitude );
         s.set16Bits( cell.longitude );
         s.set16Bits( cell.extend_of_latitude );
         s.set16Bits( cell.extend_of_longitude );

         for (const auto& subcell : cell.subcell_list) {
            s.set08Bits( subcell.cell_id_extension );
            s.set16Bits( subcell.latitude );
            s.set16Bits( subcell.longitude );
            s.set24Bits( (subcell.extend_of_latitude << 12) |
                         subcell.extend_of_longitude );
         }
      }
   }

#ifdef ENABLE_DUMP
   void CellListDesc::dump(std::ostream& o) const
   {
      dumpHeader(o, CELL_LIST_D_S);

      for (const auto& cell : cell_list) {
         identStr( o, CELL_ID_S, cell.id );
         identStr( o, CELL_LATITUDE_S, cell.latitude );
         identStr( o, CELL_LONGITUDE_S, cell.longitude );
         identStr( o, CELL_EXT_LAT_S, cell.extend_of_latitude );
         identStr( o, CELL_EXT_LON_S, cell.extend_of_longitude );

         for (const auto& subcell : cell.subcell_list) {
            identStr( o, CELL_ID_EXT_S, subcell.cell_id_extension );
            identStr( o, SUBCELL_LAT_S, subcell.latitude );
            identStr( o, SUBCELL_LON_S, subcell.longitude );
            identStr( o, SUBCELL_EXT_LAT_S, subcell.extend_of_latitude );
            identStr( o, SUBCELL_EXT_LON_S, subcell.extend_of_longitude );
         }
      }
   }
#endif


   // ---------------------------------------
   // frequency list descriptor
   //

   //
   // add a frequency to the loop
   bool FrequencyListDesc::addFrequency(ui32 f)
   {
      // can we add it?
      if ( !incLength(sizeof(f)) )
         return false;

      // ok, then add it
      frequency_list.push_back( f );
      return true;
   }


   //
   // build the section data
   void FrequencyListDesc::buildSections(Section &s) const
   {
      Descriptor::buildSections(s);

      s.set08Bits( rbits(0xfc) | coding_type );
      for (ui32 f : frequency_list) {
         s.set32Bits(f);
      }
   }


#ifdef ENABLE_DUMP
   //
   // dump to stdout
   void FrequencyListDesc::dump(std::ostream &o) const
   {
      dumpHeader( o, FREQ_LIST_D_S );

      identStr( o, RESERVED_FU_S, rbits(0x3f) );
      identStr( o, CODING_TYPE_S, coding_type);

      incOutLevel();
      for (ui32 f : frequency_list) {
         identStr( o, FREQ_S, f );
      }
      decOutLevel();
   }
#endif


   //
   // Satellite Delivery System Descriptor
   // ---------------------------------------

   //
   // write to a section
   void SatelliteDeliverySystemDesc::buildSections(Section &s) const
   {
      Descriptor::buildSections(s);

      s.set32Bits( frequency );
      s.set16Bits( orbital_position );
      s.set08Bits( (west_east << 7) | (polarisation << 5) | modulation );
      s.set32Bits( (symbol_rate << 4) | fec_inner );
   }


#ifdef ENABLE_DUMP
   void SatelliteDeliverySystemDesc::dump(std::ostream &o) const
   {
      dumpHeader( o, SAT_DEL_SYS_D_S );

      identStr(o, FREQ_S, frequency);
      identStr(o, ORB_POS_S, orbital_position);
      identStr(o, WEST_EAST_S, west_east);
      identStr(o, POL_S, polarisation);
      identStr(o, MOD_S, modulation);
      identStr(o, SYM_RATE_S, symbol_rate);
      identStr(o, FEC_I_S, fec_inner);
   }
#endif



   //
   // Terrestrial Delivery System Descriptor
   // ---------------------------------------

   //
   // write to a section
   void TerrestrialDeliverySystemDesc::buildSections(Section &s) const
   {
      Descriptor::buildSections(s);

      s.set32Bits( ctr_frequency );
      s.set08Bits( (bandwidth << 5) |
                   rbits(0x1f) );
      s.set08Bits( (constellation << 6) |
                   (hierarchy_info << 3) |
                   cr_HP_stream );
      s.set08Bits( (cr_LP_stream << 5) |
                   (guard_interval << 3) |
                   (transmission_mode << 1) |
                   other_freq_flag );

      s.set32Bits( rbits(0xffffffff) );
   }


#ifdef ENABLE_DUMP
   void TerrestrialDeliverySystemDesc::dump(std::ostream &o) const
   {
      dumpHeader( o, TER_DEL_SYS_D_S );

      identStr( o, FREQ_S, ctr_frequency);

      identStr( o, BANDWIDTH_S, bandwidth );
      identStr( o, RESERVED_FU_S, rbits(0x1f) );
      identStr( o, CONSTELLATION_S, constellation );
      identStr( o, H_INFO_S, hierarchy_info );
      identStr( o, CR_HP_STREAM_S, cr_HP_stream );
      identStr( o, CR_LP_STREAM_S, cr_LP_stream );
      identStr( o, GUARD_INTERV_S, guard_interval );
      identStr( o, TRANS_MODE_S, transmission_mode );
      identStr( o, OTHER_FREQ_FLAG_S, other_freq_flag );

      identStr( o, RESERVED_S, rbits(0xffffffff) );
   }
#endif

} // namespace

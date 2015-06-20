//
// nit_desc.cc: class definitions for the NIT descriptors
// -----------------------------------

#include <iostream>
#include <list>
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
                                                  ui16 sid, ui8 c_tag,
                                                  bool rsrvd)
    {
        Announcement ann(type, reference_type, onid, tsid, sid, c_tag, rsrvd);

        // try to increment the descriptor's length
        if (!incLength( ann.length() ))
            return false; // can't do it - full

        // add it to the list
        announcement_list.push_back( ann );
        return true;
    }

    //
    // data writer
    void AnnouncementSupportDesc::buildSections(Section& s) const
    {
        Descriptor::buildSections(s);

        s.set16Bits( announcement_support_indicator );

        // iterate through the list and write the data
        for ( std::list<Announcement>::const_iterator a_iter = announcement_list.begin();
              a_iter != announcement_list.end();
              a_iter++ )
        {
            const Announcement& ann = *a_iter;

            s.set08Bits( (ann.type << 4) |
                         rbits(ann.reserved, 0x08) |
                         ann.reference_type );

            if (ann.reference_type < NUM_DEFINED_REF_TYPES)
            {
                s.set16Bits( ann.original_network_id );
                s.set16Bits( ann.xport_stream_id );
                s.set16Bits( ann.service_id );
                s.set08Bits( ann.component_tag );
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
        for ( std::list<Announcement>::const_iterator a_iter = announcement_list.begin();
              a_iter != announcement_list.end();
              a_iter++ )
        {
            const Announcement &ann = *a_iter;

            identStr( o, ANNOUNCEMENT_TYPE_S, ann.type );
            identStr( o, RESERVED_FU_S, ann.reserved );
            identStr( o, REF_TYPE_S, ann.reference_type );

            if (ann.reference_type < NUM_DEFINED_REF_TYPES)
            {
                identStr( o, ORIG_NETWORK_ID_S, ann.original_network_id );
                identStr( o, XPORT_STREAM_ID_S, ann.xport_stream_id );
                identStr( o, SERVICE_ID_S, ann.service_id, true );
                identStr( o, COMPONENT_TAG_S, ann.component_tag );
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

        s.set32Bits( _frequency );
        s.set16Bits( rbits(_reserved, 0xfff0) | _fec_outer );
        s.set08Bits( _modulation );
        s.set32Bits( (_symbol_rate << 4) | _fec_inner );
    }


#ifdef ENABLE_DUMP
    //
    // to stdout
    void CableDeliverySystemDesc::dump(std::ostream &o) const
    {
        dumpHeader( o, CABLE_DEL_SYS_D_S );

        identStr(o, FREQ_S, _frequency);
        identStr(o, RESERVED_FU_S, rbits(_reserved, 0xfff));
        identStr(o, FEC_O_S, _fec_outer);
        identStr(o, MOD_S, _modulation);
        identStr(o, SYM_RATE_S, _symbol_rate);
        identStr(o, FEC_I_S, _fec_inner);
    }
#endif


    // ---------------------------------------
    // cell frequency link descriptor
    //
    bool CellFrequencyLinkDesc::addLink(ui16 cell_id, ui32 frequency)
    {
        Link cflink(cell_id, frequency);

        if (!incLength(cflink.length()))
            return false;

        cflink_list.push_back( cflink );
        return true;
    }


    //
    // helper method to add a subcell to a link in the list
    bool CellFrequencyLinkDesc::addLinkSubCell(Link& link, ui8 cid_ext,
                                               ui32 xposer_freq)
    {
        Link::SubCell subcell(cid_ext, xposer_freq);

        // check if it fits
        if ( !incLength( subcell.BASE_LEN ) )
            return false;

        // add it
        link.subcell_list.push_back(subcell);
        return true;
    }

    bool CellFrequencyLinkDesc::addLinkSubCell(ui16 cell_id, ui32 frequency,
                                               ui8 cid_ext, ui32 xposer_freq)
    {
        // look for a matching c/f link
        for ( std::list<Link>::iterator l_iter = cflink_list.begin();
              l_iter != cflink_list.end();
              l_iter++ )
        {
            Link& link = *l_iter;

            // if found, add a subcell
            if ( link.cell_id == cell_id && link.frequency == frequency )
                return addLinkSubCell(link, cid_ext, xposer_freq);
        }

        return false;
    }

    bool CellFrequencyLinkDesc::addLinkSubCell(ui8 cid_ext, ui32 xposer_freq)
    {
        // if any have been added, try to add teh subcell to the one in the
        // end
        if (cflink_list.size() > 0)
            return addLinkSubCell( cflink_list.back(), cid_ext, xposer_freq );

        return false;
    }

    //
    // format the binary data
    void CellFrequencyLinkDesc::buildSections(Section& s) const
    {
        Descriptor::buildSections(s);

        for ( std::list<Link>::const_iterator l_iter = cflink_list.begin();
              l_iter != cflink_list.end();
              l_iter++ )
        {
            const Link& link = *l_iter;

            s.set16Bits( link.cell_id );
            s.set32Bits( link.frequency );
            s.set08Bits( link.subcell_list.size() * Link::SubCell::BASE_LEN );

            for ( std::list<Link::SubCell>::const_iterator sc_iter = link.subcell_list.begin();
                  sc_iter != link.subcell_list.end();
                  sc_iter++ )
            {
                const Link::SubCell &subcell = *sc_iter;

                s.set08Bits( subcell.cell_id_extension );
                s.set32Bits( subcell.transposer_frequency );
            }

        }
    }

#ifdef ENABLE_DUMP
    void CellFrequencyLinkDesc::dump(std::ostream& o) const
    {
        dumpHeader( o, CELL_FREQ_LINK_D_S );

        for ( std::list<Link>::const_iterator l_iter = cflink_list.begin();
              l_iter != cflink_list.end();
              l_iter++ )
        {
            const Link& link = *l_iter;

            identStr(o, CELL_ID_S, link.cell_id );
            identStr(o, FREQ_S, link.frequency );
            identStr(o, SUBCELL_INFO_LOOP_LEN_S, link.subcell_list.size() * Link::SubCell::BASE_LEN );

            for ( std::list<Link::SubCell>::const_iterator sc_iter = link.subcell_list.begin();
                  sc_iter != link.subcell_list.end();
                  sc_iter++ )
            {
                const Link::SubCell &subcell = *sc_iter;

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
    bool CellListDesc::addCell(ui16 id, ui16 lat, ui16 lon,
                               ui16 ext_lat, ui16 ext_lon)
    {
        Cell cell(id, lat, lon, ext_lat, ext_lon);

        if ( !incLength(Cell::BASE_LEN) )
            return false;

        cell_list.push_back( cell );
        return true;
    }

    bool CellListDesc::addCellSubCell(Cell& cell, ui8 cid_ext,
                                      ui16 sc_lat, ui16 sc_lon,
                                      ui16 sc_ext_lat, ui16 sc_ext_lon)
    {
        Cell::SubCell subcell(cid_ext, sc_lat, sc_lon, sc_ext_lat, sc_ext_lon);

        if ( !incLength( Cell::SubCell::BASE_LEN ) )
            return false;

        cell.subcell_list.push_back( subcell );
        return true;
    }


    bool CellListDesc::addCellSubCell(ui16 cell_id, ui8 cid_ext,
                                      ui16 sc_lat, ui16 sc_lon,
                                      ui16 sc_ext_lat, ui16 sc_ext_lon)
    {
        for ( std::list<Cell>::iterator c_iter = cell_list.begin();
              c_iter != cell_list.end();
              c_iter++ )
        {
            Cell& cell = *c_iter;

            if (cell.id == cell_id)
                return addCellSubCell(cell, cid_ext, sc_lat, sc_lon,
                                      sc_ext_lat, sc_ext_lon);
        }
        return false;
    }

    // adds the subcell to the most recently added cell
    bool CellListDesc::addCellSubCell(ui8 cid_ext, ui16 sc_lat, ui16 sc_lon,
                                      ui16 sc_ext_lat, ui16 sc_ext_lon)
    {
        if (cell_list.size() == 0)
            return false;

        return addCellSubCell( cell_list.back(), cid_ext, sc_lat, sc_lon,
                               sc_ext_lat, sc_ext_lon );
    }

    //
    // format the binary data
    void CellListDesc::buildSections(Section& s) const
    {
        Descriptor::buildSections(s);

        for ( std::list<Cell>::const_iterator c_iter = cell_list.begin();
              c_iter != cell_list.end();
              c_iter++ )
        {
            const Cell& cell = *c_iter;

            s.set16Bits( cell.id );
            s.set16Bits( cell.latitude );
            s.set16Bits( cell.longitude );
            s.set16Bits( cell.extend_of_latitude );
            s.set16Bits( cell.extend_of_longitude );

            for ( std::list<Cell::SubCell>::const_iterator sc_iter = cell.subcell_list.begin();
                  sc_iter != cell.subcell_list.end();
                  sc_iter++ )
            {
                const Cell::SubCell& subcell = *sc_iter;

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

        for ( std::list<Cell>::const_iterator c_iter = cell_list.begin();
              c_iter != cell_list.end();
              c_iter++ )
        {
            const Cell& cell = *c_iter;

            identStr( o, CELL_ID_S, cell.id );
            identStr( o, CELL_LATITUDE_S, cell.latitude );
            identStr( o, CELL_LONGITUDE_S, cell.longitude );
            identStr( o, CELL_EXT_LAT_S, cell.extend_of_latitude );
            identStr( o, CELL_EXT_LON_S, cell.extend_of_longitude );

            for ( std::list<Cell::SubCell>::const_iterator sc_iter = cell.subcell_list.begin();
                  sc_iter != cell.subcell_list.end();
                  sc_iter++ )
            {
                const Cell::SubCell& subcell = *sc_iter;

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

        s.set08Bits( rbits(reserved, 0xfc) | coding_type );

        std::list<ui32>::const_iterator f_iter = frequency_list.begin();

        while (f_iter != frequency_list.end())
            s.set32Bits( *f_iter++ );
    }


#ifdef ENABLE_DUMP
    //
    // dump to stdout
    void FrequencyListDesc::dump(std::ostream &o) const
    {
        dumpHeader( o, FREQ_LIST_D_S );

        identStr( o, RESERVED_FU_S, rbits(reserved, 0x3f) );
        identStr( o, CODING_TYPE_S, coding_type);

        std::list<ui32>::const_iterator f_iter = frequency_list.begin();

        incOutLevel();
        while (f_iter != frequency_list.end())
            identStr( o, FREQ_S, *f_iter++ );
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

        s.set32Bits( _frequency );
        s.set16Bits( _orbital_position );
        s.set08Bits( (_west_east << 7) | (_polarisation << 5) | _modulation );
        s.set32Bits( (_symbol_rate << 4) | _fec_inner );
    }


#ifdef ENABLE_DUMP
    void SatelliteDeliverySystemDesc::dump(std::ostream &o) const
    {
        dumpHeader( o, SAT_DEL_SYS_D_S );

        identStr(o, FREQ_S, _frequency);
        identStr(o, ORB_POS_S, _orbital_position);
        identStr(o, WEST_EAST_S, _west_east);
        identStr(o, POL_S, _polarisation);
        identStr(o, MOD_S, _modulation);
        identStr(o, SYM_RATE_S, _symbol_rate);
        identStr(o, FEC_I_S, _fec_inner);
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

        s.set32Bits( _ctr_frequency );
        s.set08Bits( (_bandwidth << 5) |
                     rbits(_reserved, 0x1f) );
        s.set08Bits( (_constellation << 6) |
                     (_hierarchy_info << 3) |
                     _cr_HP_stream );
        s.set08Bits( (_cr_LP_stream << 5) |
                     (_guard_interval << 3) |
                     (_transmission_mode << 1) |
                     _other_freq_flag );

        s.set32Bits( rbits(_reserved, 0xffffffff) );
    }


#ifdef ENABLE_DUMP
    void TerrestrialDeliverySystemDesc::dump(std::ostream &o) const
    {
        dumpHeader( o, TER_DEL_SYS_D_S );

        identStr( o, FREQ_S, _ctr_frequency);

        identStr( o, BANDWIDTH_S, _bandwidth );
        identStr( o, RESERVED_FU_S, rbits(_reserved, 0x1f) );
        identStr( o, CONSTELLATION_S, _constellation );
        identStr( o, H_INFO_S, _hierarchy_info );
        identStr( o, CR_HP_STREAM_S, _cr_HP_stream );
        identStr( o, CR_LP_STREAM_S, _cr_LP_stream );
        identStr( o, GUARD_INTERV_S, _guard_interval );
        identStr( o, TRANS_MODE_S, _transmission_mode );
        identStr( o, OTHER_FREQ_FLAG_S, _other_freq_flag );

        identStr( o, RESERVED_S, rbits(_reserved, 0xffffffff) );
    }
#endif

} // namespace

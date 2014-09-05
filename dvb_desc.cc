//
// dvb_desc.cc: class definitions for general DVB descriptors
// -----------------------------------

#include <iostream>
#include <string>
#include <list>
#include "descriptor.h"
#include "tstream.h"
#include "dvb_desc.h"
#include "util.h"

namespace sigen
{
    // ============================================
    // CA Identifier Descriptor
    // --------------------------------------------

    //
    // add a system id to the descriptor
    bool CAIdentifierDesc::addSystemId(ui16 id)
    {
        if ( !incLength( sizeof(id) ) )
            return false;

        // add the id to the list
        id_list.push_back(id);
        return true;
    }

    //
    // writes to the stream
    void CAIdentifierDesc::buildSections(Section &s) const
    {
        Descriptor::buildSections(s);

        std::list<ui16>::const_iterator il_iter = id_list.begin();

        while (il_iter != id_list.end())
            s.set16Bits( *il_iter++ );
    }


#ifdef ENABLE_DUMP
    void CAIdentifierDesc::dump(std::ostream &o) const
    {
        dumpHeader( o, CA_IDENT_D_S );

        std::list<ui16>::const_iterator il_iter = id_list.begin();

        incOutLevel();
        while (il_iter != id_list.end())
            identStr(o, CA_SYSTEM_ID_S, *il_iter++);
        decOutLevel();
    }
#endif



    // ============================================
    // Country Availability Descriptor
    // --------------------------------------------

    //
    // adds a country to the list
    bool CountryAvailabilityDesc::addCountry(const std::string &code)
    {
        // make sure we can add this entry
        if ( !incLength( LanguageCode::BASE_LEN) )
            return false;

        // and add it
        country_list.push_back( code );
        return true;
    }

    //
    // write to the stream
    void CountryAvailabilityDesc::buildSections(Section &s) const
    {
        Descriptor::buildSections(s);

        s.set08Bits( country_availability_flag << 7 | rbits(reserved, 0x7f) );

        std::list<LanguageCode>::const_iterator c_iter = country_list.begin();

        while (c_iter != country_list.end())
            s.setBits( *c_iter++ ); // get the language code and write it
    }

#ifdef ENABLE_DUMP
    //
    // dump the data
    void CountryAvailabilityDesc::dump(std::ostream &o) const
    {
        dumpHeader(o, COUNTRY_AVAIL_D_S);

        identStr(o, COUNTRY_AVAIL_FLAG_S, country_availability_flag);
        identStr(o, RESERVED_FU_S, rbits(reserved, 0x7f));

        std::list<LanguageCode>::const_iterator c_iter = country_list.begin();

        incOutLevel();
        while (c_iter != country_list.end())
            identStr(o, COUNTRY_CODE_S, (*c_iter++));
        decOutLevel();
    }
#endif


    // ============================================
    // Data Broadcast Descriptor
    // --------------------------------------------

    void DataBroadcastDesc::buildSections(Section& s) const
    {
        Descriptor::buildSections(s);

        s.set16Bits( data_broadcast_id );
        s.set08Bits( component_tag );

        s.set08Bits( selector_byte.length() );
        s.setBits( selector_byte );

        s.setBits( code );

        s.set08Bits( text.length() );
        s.setBits( text );
    }

#ifdef ENABLE_DUMP
    void DataBroadcastDesc::dump(std::ostream& o) const
    {
        dumpHeader(o, DATA_BCAST_D_S);

        identStr(o, DATA_BCAST_ID_S, data_broadcast_id);
        identStr(o, COMPONENT_TAG_S, component_tag);

        identStr(o, SELECTOR_LEN_S, selector_byte.length(), true);
        identStr(o, SELECTOR_S, selector_byte);

        identStr(o, LANGUAGE_CODE_S, code);

        identStr(o, TEXT_LEN_S, text.length(), true);
        identStr(o, TEXT_S, text );
    }
#endif


    // ============================================
    // Local Time Offset Descriptor
    // --------------------------------------------

    //
    // adds an entry to the time offset list
    //
    bool LocalTimeOffsetDesc::addTimeOffset(const LanguageCode& code,
                                            ui8 country_region_id,
                                            bool offset_polarity,
                                            ui16 lt_offset,
                                            const UTC &time_of_change,
                                            ui16 next_time_offset)
    {
        // see if we can fit it
        if (!incLength( TimeOffset::BASE_LEN ) )
            return false;

        // ok, so create on to add
        TimeOffset to(code, country_region_id, offset_polarity, lt_offset,
                      time_of_change, next_time_offset);

        time_offset_list.push_back( to );
        return true;
    }


    //
    // build the descriptor sectyion data
    //
    void LocalTimeOffsetDesc::buildSections(Section &s) const
    {
        Descriptor::buildSections(s);

        // write the offset loop
        for ( std::list<TimeOffset>::const_iterator to_iter = time_offset_list.begin();
              to_iter != time_offset_list.end();
              to_iter++ )
        {
            // fetch the current time offset struct
            const TimeOffset &offset = *to_iter;

            // now set the data
            s.setBits( offset.country_code );
            s.set08Bits( (offset.country_region_id << 2) |
                         (rbits(offset.reserved, 0x01) << 1) |
                         (offset.local_time_offset_polarity) );
            s.set16Bits( offset.local_time_offset );

            s.set16Bits( offset.time_of_change.mjd );
            s.set08Bits( offset.time_of_change.time.getBCDHour() );
            s.set08Bits( offset.time_of_change.time.getBCDMinute() );
            s.set08Bits( offset.time_of_change.time.getBCDSecond() );

            s.set16Bits( offset.next_time_offset );
        }
    }


    //
    // dump to stdout
    //
#ifdef ENABLE_DUMP
    void LocalTimeOffsetDesc::dump(std::ostream &o) const
    {
        dumpHeader(o, LOCAL_TIME_OFFSET_D_S);

        // write the offset loop
        incOutLevel();
        for ( std::list<TimeOffset>::const_iterator to_iter = time_offset_list.begin();
              to_iter != time_offset_list.end();
              to_iter++ )
        {
            const TimeOffset &offset = *to_iter;

            o << std::endl;

            identStr(o, COUNTRY_CODE_S, offset.country_code);
            identStr(o, COUNTRY_REGION_ID_S, offset.country_region_id);
            identStr(o, RESERVED_S, rbits(offset.reserved, 0x01));
            identStr(o, LTO_POLARITY_S, offset.local_time_offset_polarity);

            identStr(o, LOCAL_TIME_OFFSET_S, offset.local_time_offset);

            identStr(o, UTC_S);
            o << offset.time_of_change << std::endl;

            o << std::hex;
            identStr(o, NEXT_TIME_OFFSET_S, offset.next_time_offset);
        }
        decOutLevel();
    }
#endif


    // ============================================
    // Mosaic Descriptor
    // --------------------------------------------

#if 0
    // NOT YET IMPLEMENTED
    MosaicDesc::MosaicDesc(bool mep, ui8 num_hor_cells, ui8 num_ver_cells, bool rsrvd) :
        Descriptor(TAG, BASE_LEN),
        num_horizontal_elem_cells(num_hor_cells),
        num_vertical_elem_cells(num_ver_cells),
        mosaic_entry_point(mep),
        reserved(rsrvd)
    {
    }
#endif



    // ============================================
    // Partial Transport Stream Descriptor
    // --------------------------------------------

    //
    // binary data formatter
    void PartialTransportStreamDesc::buildSections(Section& s) const
    {
        Descriptor::buildSections(s);

        s.set24Bits( rbits(reserved, 0xc00000) |
                     peak_rate );
        s.set24Bits( rbits(reserved, 0xc00000) |
                     min_overall_smoothing_rate );
        s.set16Bits( rbits(reserved, 0xc000) |
                     max_overall_smoothing_buffer );
    }

#ifdef ENABLE_DUMP
    void PartialTransportStreamDesc::dump(std::ostream& o) const
    {
        dumpHeader( o, PARTIAL_TS_D_S );

        identStr( o, RESERVED_FU_S, rbits(reserved, 0x3 ) );
        identStr( o, PEAK_RATE_S, peak_rate );
        identStr( o, RESERVED_FU_S, rbits(reserved, 0x3 ) );
        identStr( o, MIN_OS_RATE_S, min_overall_smoothing_rate );
        identStr( o, RESERVED_FU_S, rbits(reserved, 0x3 ) );
        identStr( o, MAX_OS_BUFFER_S, max_overall_smoothing_buffer );
    }
#endif


    // ============================================
    // Service List Descriptor
    // --------------------------------------------

    //
    // adds a service to the descriptor
    bool ServiceListDesc::addService(ui16 id, ui8 type)
    {
        // make sure we have room to add is
        if ( !incLength( Service::BASE_LEN ) )
            return false;

        // and then add it
        service_list.push_back( Service(id, type) );
        return true;
    }


    //
    // write contents to a section
    void ServiceListDesc::buildSections(Section &s) const
    {
        Descriptor::buildSections(s);

        for ( std::list<Service>::const_iterator s_iter = service_list.begin();
              s_iter != service_list.end();
              s_iter++ )
        {
            const Service &service = *s_iter;
            s.set16Bits( service.id );
            s.set08Bits( service.type );
        }
    }


#ifdef ENABLE_DUMP
    void ServiceListDesc::dump(std::ostream &o) const
    {
        dumpHeader( o, SERV_LIST_D_S );

        // dump the descriptor's data
        incOutLevel();

        for ( std::list<Service>::const_iterator s_iter = service_list.begin();
              s_iter != service_list.end();
              s_iter++ )
        {
            const Service &service = *s_iter;

            identStr(o, SERVICE_ID_S, service.id, true);
            identStr(o, TYPE_S, service.type);
        }
        decOutLevel();
    }
#endif


    // ============================================
    // Telephone Descriptor
    // --------------------------------------------
    TelephoneDesc::TelephoneDesc(bool fa, ui8 ct,
                                 const std::string &cp, const std::string &iac,
                                 const std::string &oc, const std::string &nac,
                                 const std::string &cn, bool rsrvd) :
        Descriptor(TAG, BASE_LEN),
        country_prefix(cp),
        international_area_code(iac),
        operator_code(oc),
        national_area_code(nac),
        core_number(cn),
        connection_type(ct),
        foreign_availability(fa),
        reserved(rsrvd)
    {
        // add the length of the strings making sure each string is not
        // exceeding its max length
        setCode(country_prefix, cp, MAX_CP_LEN);
        setCode(international_area_code, iac, MAX_IAC_LEN);
        setCode(operator_code, oc, MAX_OC_LEN);
        setCode(national_area_code, nac, MAX_NAC_LEN);
        setCode(core_number, cn, MAX_CN_LEN);
    }


    //
    // checks the string size to make sure they can fit.. if not, it
    // resizes the string
    void TelephoneDesc::setCode(std::string &dest_c, const std::string &c, ui8 max)
    {
        ui8 len = c.length();

        if (len)
        {
            // set it to the max if it exceeds it
            if (len > max)
            {
                len = max;
                dest_c = c.substr(0, len);
            }
            // increase the size of the descriptor by the length
            incLength(len);
        }
    }


    //
    // writes the data to the stream
    //
    void TelephoneDesc::buildSections(Section &s) const
    {
        Descriptor::buildSections(s);

        s.set08Bits( rbits(reserved, 0xc0) |
                     (foreign_availability << 5) |
                     connection_type );
        s.set08Bits( rbits(reserved, 0x80) |
                     (country_prefix.length() << 5) |
                     (international_area_code.length() << 2) |
                     operator_code.length());
        s.set08Bits( rbits(reserved, 0x80) |
                     (national_area_code.length() << 4) |
                     core_number.length());

        // now set the strings
        s.setBits( country_prefix );
        s.setBits( international_area_code );
        s.setBits( operator_code );
        s.setBits( national_area_code );
        s.setBits( core_number );
    }

#ifdef ENABLE_DUMP
    //
    // debug
    void TelephoneDesc::dump(std::ostream &o) const
    {
        dumpHeader(o, TELEPHONE_D_S);

        identStr(o, RESERVED_FU_S, rbits(reserved, 0x03));
        identStr(o, FOREIGN_AVAIL_S, foreign_availability);
        identStr(o, CONNECTION_TYPE_S, connection_type);
        identStr(o, RESERVED_FU_S, rbits(reserved, 0x01));
        identStr(o, COUNTRY_PREFIX_LEN_S, country_prefix.length(), true);
        identStr(o, INTNL_AC_LEN_S, international_area_code.length(), true);
        identStr(o, OPERATOR_CODE_LEN_S, operator_code.length(), true);
        identStr(o, RESERVED_FU_S, rbits(reserved, 0x01));
        identStr(o, NTNL_AC_LEN_S, national_area_code.length(), true);
        identStr(o, CORE_NUM_LEN_S, core_number.length(), true);
        identStr(o, COUNTRY_PREFIX_S, country_prefix);
        identStr(o, INTNL_AC_S, international_area_code);
        identStr(o, OPERATOR_CODE_S, operator_code);
        identStr(o, NTNL_AC_S, national_area_code);
        identStr(o, CORE_NUM_S, core_number);
    }
#endif

} // namespace sigen

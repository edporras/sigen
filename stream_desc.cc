//
// stream_desc.cc: class definitions for the mpeg stream descriptors
// -----------------------------------

#include <string>
#include <list>
#include "descriptor.h"
#include "stream_desc.h"
#include "tstream.h"
#include "util.h"

namespace sigen
{
    //
    // Audio Stream Descriptor
    // ---------------------------------------

    //
    // write data to the sections
    void AudioStreamDesc::buildSections(Section &s) const
    {
        Descriptor::buildSections(s);

        s.set08Bits( (free_format_flag << 7) |
                     (id << 6) |
                     (layer << 4) |
                     rbits(reserved, 0xf) );
    }


    //
    // debug
#ifdef ENABLE_DUMP
    void AudioStreamDesc::dump(std::ostream &o) const
    {
        dumpHeader(o, AUDIO_STREAM_D_S);

        identStr(o, FREE_FRMT_FLAG_S, free_format_flag);
        identStr(o, ID_S, id);
        identStr(o, LAYER_S, layer);
        identStr(o, RESERVED_S, rbits(reserved, 0xf) );
    }
#endif



    //
    // CA Descriptor
    // ---------------------------------------

    //
    // write data to the sections
    void CADesc::buildSections(Section &s) const
    {
        Descriptor::buildSections(s);

        s.set16Bits( CA_system_id );
        s.set16Bits( rbits(reserved, 0xe000) | CA_pid );

        s.setBits( private_data );
    }


    //
    // debug
#ifdef ENABLE_DUMP
    void CADesc::dump(std::ostream &o) const
    {
        dumpHeader(o, CA_D_S);

        identStr(o, CA_SYSTEM_ID_S, CA_system_id);
        identStr(o, RESERVED_S, rbits(reserved, 0x7) );
        identStr(o, CA_PID_S, CA_pid);
        identStr(o, PVT_DATA_S, private_data);
    }
#endif


    //
    // Copyright Descriptor
    // ---------------------------------------
    void CopyrightDesc::buildSections(Section &s) const
    {
        Descriptor::buildSections(s);

        s.set32Bits( identifier );
        s.setBits( info );
    }

    //
    //
#ifdef ENABLE_DUMP
    void CopyrightDesc::dump(std::ostream &o) const
    {
        dumpHeader(o, COPYRIGHT_D_S);

        identStr(o, COPYRIGHT_IDENT_S, identifier);
        identStr(o, COPYRIGHT_INFO_S, info);
    }
#endif


    //
    // Hierarchy Descriptor
    // ---------------------------------------

    //
    // write data to the sections
    void HierarchyDesc::buildSections(Section &s) const
    {
        Descriptor::buildSections(s);

        s.set08Bits( rbits(reserved, 0xf0) | type );
        s.set08Bits( rbits(reserved, 0xc0) | layer_index );
        s.set08Bits( rbits(reserved, 0xc0) | embedded_layer );
        s.set08Bits( rbits(reserved, 0xc0) | priority );
    }


    //
    // debug
#ifdef ENABLE_DUMP
    void HierarchyDesc::dump(std::ostream &o) const
    {
        dumpHeader(o, HIERARCHY_D_S);

        identStr(o, RESERVED_S, rbits(reserved, 0x0f) );
        identStr(o, H_TYPE_S, type);
        identStr(o, RESERVED_S, rbits(reserved, 0x03) );
        identStr(o, H_LAYER_IDX_S, layer_index);
        identStr(o, RESERVED_S, rbits(reserved, 0x03) );
        identStr(o, H_EMBEDDED_LAYER_S, embedded_layer);
        identStr(o, RESERVED_S, rbits(reserved, 0x03) );
        identStr(o, H_PRIORITY_S, priority);
    }
#endif


    //
    // IBP Descriptor
    // ---------------------------------------

    void IBPDesc::buildSections(Section &s) const
    {
        Descriptor::buildSections( s );

        s.set16Bits( (closed_gop_flag << 15) |
                     (identical_gop_flag << 14) |
                     max_gop_len );
    }

#ifdef ENABLE_DUMP
    void IBPDesc::dump(std::ostream &o) const
    {
        dumpHeader( o, IBP_D_S );

        identStr( o, CLOSED_GOP_F_S, closed_gop_flag );
        identStr( o, IDENTICAL_GOP_F_S, identical_gop_flag );
        identStr( o, MAX_GOP_LEN_S, max_gop_len, true );
    }
#endif



    //
    // ISO 639 Language Descriptor
    // ---------------------------------------

    //
    // add a language to the descriptor
    bool ISO639LanguageDesc::addLanguage(const std::string &code, ui8 audio_type)
    {
        // check if we can fit it
        if ( !incLength( Language::BASE_LEN ) )
            return false;

        // add it to the list (passing the string invokes the LanguageCode
        // single-arg convert constructor automatically)
        language_list.push_back( Language(code, audio_type) );
        return true;
    }


    //
    // writes to the stream
    void ISO639LanguageDesc::buildSections(Section &s) const
    {
        Descriptor::buildSections(s);

        // write the language codes
        for ( std::list<Language>::const_iterator l_iter = language_list.begin();
              l_iter != language_list.end();
              l_iter++ )
        {
            const Language &lang = *l_iter;

            s.setBits( lang.code );
            s.set08Bits( lang.audio_type );
        }
    }


#ifdef ENABLE_DUMP
    //
    //
    void ISO639LanguageDesc::dump(std::ostream &o) const
    {
        dumpHeader(o, ISO_639_LANG_D_S);

        incOutLevel();
        for ( std::list<Language>::const_iterator l_iter = language_list.begin();
              l_iter != language_list.end();
              l_iter++ )
        {
            const Language &lang = *l_iter;

            identStr( o, LANGUAGE_CODE_S, lang.code );
            identStr( o, AUDIO_TYPE_S, lang.audio_type );
        }
        decOutLevel();
    }
#endif


    //
    // Maximum Bitrate Descriptor
    // ---------------------------------------

    //
    // write data to the sections
    void MaximumBitrateDesc::buildSections(Section &s) const
    {
        Descriptor::buildSections(s);
        s.set24Bits( (rbits(reserved, 0x03) << 22) | maximum_bitrate );
    }


    //
    // debug
#ifdef ENABLE_DUMP
    void MaximumBitrateDesc::dump(std::ostream &o) const
    {
        dumpHeader(o, MAXIMUM_BITRATE_D_S);

        identStr(o, RESERVED_S, rbits(reserved, 0x03) );
        identStr(o, MAX_BITRATE_S, maximum_bitrate);
    }
#endif



    //
    // Multiplex Buffer Utilization Descriptor
    // ---------------------------------------

    //
    // write data to the sections
    void MultiplexBufferUtilizationDesc::buildSections(Section &s) const
    {
        Descriptor::buildSections(s);

        s.set16Bits( (bound_valid_flag << 15) |
                     LTW_offset_lb );
        s.set16Bits( rbits(reserved, 0x8000) |
                     LTW_offset_ub );
    }


    //
    // debug
#ifdef ENABLE_DUMP
    void MultiplexBufferUtilizationDesc::dump(std::ostream &o) const
    {
        dumpHeader(o, MULTIPLEX_BUF_UTIL_D_S);

        identStr(o, BOUND_VALID_F_S, bound_valid_flag);
        identStr(o, LTW_OFFSET_LB_S, LTW_offset_lb);
        identStr(o, RESERVED_S, rbits(reserved, 0x1) );
        identStr(o, LTW_OFFSET_UB_S, LTW_offset_ub);
    }
#endif


    //
    // Registration Descriptor
    // ---------------------------------------
    void RegistrationDesc::buildSections(Section &s) const
    {
        Descriptor::buildSections(s);

        s.set32Bits( identifier );
        s.setBits( info );
    }

    //
    //
#ifdef ENABLE_DUMP
    void RegistrationDesc::dump(std::ostream &o) const
    {
        dumpHeader(o, REGISTRATION_D_S);

        identStr(o, FORMAT_IDENT_S, identifier);
        identStr(o, IDENT_INFO_S, info);
    }
#endif



    //
    // Smoothing Buffer Descriptor
    // ---------------------------------------

    void SmoothingBufferDesc::buildSections(Section &s) const
    {
        Descriptor::buildSections( s );

        s.set24Bits( (rbits(reserved, 0xc0) << 16) | sb_leak_rate );
        s.set24Bits( (rbits(reserved, 0xc0) << 16) | sb_size );
    }

#ifdef ENABLE_DUMP
    void SmoothingBufferDesc::dump(std::ostream &o) const
    {
        dumpHeader( o, SMOOTHING_BUF_D_S );

        identStr( o, RESERVED_S, rbits(reserved,  0x3 ) );
        identStr( o, SB_LEAK_RATE_S, sb_leak_rate );
        identStr( o, RESERVED_S, rbits(reserved,  0x3 ) );
        identStr( o, SB_SIZE_S, sb_size );
    }
#endif

    //
    // STD Descriptor
    // ---------------------------------------

    //
    // write data to the sections
    void STDDesc::buildSections(Section &s) const
    {
        Descriptor::buildSections(s);

        s.set08Bits( rbits(reserved, 0xe) | leak_valid_flag );
    }


    //
    // debug
#ifdef ENABLE_DUMP
    void STDDesc::dump(std::ostream &o) const
    {
        dumpHeader(o, STD_D_S);

        identStr(o, RESERVED_S, rbits(reserved, 0x7f) );
        identStr(o, LEAK_VALID_FLAG_S, leak_valid_flag);
    }
#endif




    //
    // System Clock Descriptor
    // ---------------------------------------

    //
    // write data to the sections
    void SystemClockDesc::buildSections(Section &s) const
    {
        Descriptor::buildSections(s);

        s.set08Bits( (external_clock_reference_indicator << 7) |
                     rbits(reserved, 0x70) |
                     clock_accuracy_integer );
        s.set08Bits( (clock_accuracy_exponent << 5) |
                     rbits(reserved, 0x01f) );
    }


    //
    // debug
#ifdef ENABLE_DUMP
    void SystemClockDesc::dump(std::ostream &o) const
    {
        dumpHeader(o, SYSTEM_CLOCK_D_S);

        identStr(o, EXT_CLOCK_REF_IND_S, external_clock_reference_indicator);
        identStr(o, RESERVED_S, rbits(reserved, 0x01) );
        identStr(o, CLOCK_ACCURACY_INT_S, clock_accuracy_integer);
        identStr(o, CLOCK_ACCURACY_EXP_S, clock_accuracy_exponent);
        identStr(o, RESERVED_S, rbits(reserved, 0x1f) );
    }
#endif



    //
    // Target Background Grid Descriptor
    // ---------------------------------------

    //
    // write data to the sections
    void TargetBackgroundGridDesc::buildSections(Section &s) const
    {
        Descriptor::buildSections(s);

        s.set32Bits( (horizontal_size << 18) |
                     (vertical_size << 4) |
                     pel_aspect_ratio );
    }


    //
    // debug
#ifdef ENABLE_DUMP
    void TargetBackgroundGridDesc::dump(std::ostream &o) const
    {
        dumpHeader(o, TARGET_BACKGROUND_GRID_D_S);

        identStr(o, HORIZ_SIZE_S, horizontal_size);
        identStr(o, VERT_SIZE_S, vertical_size);
        identStr(o, PEL_ASPECT_R_S, pel_aspect_ratio);
    }
#endif



    //
    // Video Stream Descriptor
    // ---------------------------------------

    //
    // write data to the sections
    void VideoStreamDesc::buildSections(Section &s) const
    {
        Descriptor::buildSections(s);

        s.set08Bits( (multiple_frame_rate_flag << 7) |
                     (frame_rate_code << 3) |
                     (MPEG_2_flag << 2) |
                     (constrained_parameter_flag << 1) |
                     (still_picture_flag) );

        if (MPEG_2_flag)
        {
            s.set08Bits( profile_and_level_indication );
            s.set08Bits( (chroma_format << 6) |
                         (frame_rate_extension_flag << 5) |
                         rbits(reserved, 0x1f) );
        }
    }


    //
    // debug
#ifdef ENABLE_DUMP
    void VideoStreamDesc::dump(std::ostream &o) const
    {
        dumpHeader(o, VIDEO_STREAM_D_S);

        identStr(o, MULT_FRM_RATE_FLAG_S, multiple_frame_rate_flag);
        identStr(o, FRM_RATE_CODE_S, frame_rate_code);
        identStr(o, MPEG_2_FLAG_S, MPEG_2_flag);
        identStr(o, CONSTR_PARAM_FLAG_S, constrained_parameter_flag);
        identStr(o, STILL_PICT_FLAG_S, still_picture_flag);

        if (MPEG_2_flag)
        {
            identStr(o, PROF_LVL_IND_S, profile_and_level_indication);
            identStr(o, CHROMA_FORMAT_S, chroma_format);
            identStr(o, FRM_RATE_EXT_FLAG_S, frame_rate_extension_flag);
            identStr(o, RESERVED_S, rbits(reserved, 0x1f) );
        }
    }
#endif


    //
    // Video Window Descriptor
    // ---------------------------------------

    //
    // write data to the sections
    void VideoWindowDesc::buildSections(Section &s) const
    {
        Descriptor::buildSections(s);

        s.set32Bits( (horizontal_offset << 18) |
                     (vertical_offset << 4) |
                     window_priority );
    }


    //
    // debug
#ifdef ENABLE_DUMP
    void VideoWindowDesc::dump(std::ostream &o) const
    {
        dumpHeader(o, VIDEO_WINDOW_D_S);

        identStr(o, HORIZ_OFFSET_S, horizontal_offset);
        identStr(o, VERT_OFFSET_S, vertical_offset);
        identStr(o, WIN_PRIORITY_S, window_priority);
    }
#endif

} // namespace sigen

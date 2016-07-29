//
// stream_desc.h: class definitions for the mpeg stream descriptors
// -----------------------------------

#pragma once

#include <list>
#include "descriptor.h"

namespace sigen {

    class Section;
    class LanguageCode;


    // ---------------------------
    // Audio Stream Descriptor
    //
    class AudioStreamDesc : public Descriptor
    {
    private:
        bool free_format_flag,
            id,
            reserved;
        ui8 layer : 2;

    public:
        enum { TAG = 3, BASE_LEN = 1 };

        // constructor
        AudioStreamDesc(bool fff, bool ID, ui8 l, bool rsrvd = true) :
            Descriptor(TAG, BASE_LEN),
            free_format_flag(fff), id(ID), reserved(rsrvd), layer(l)  { }

        // utility
        virtual Descriptor *clone() const { return new AudioStreamDesc(*this); }
        virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
        virtual void dump(std::ostream&) const;
#endif
    };


    // ---------------------------
    // CA Descriptor
    //
    class CADesc : public Descriptor
    {
    private:
        // data
        ui16 CA_system_id,
            CA_pid : 13;
        bool reserved;
        std::string private_data;

    public:
        enum { TAG = 9, BASE_LEN = 4 };

        // constructor
        CADesc(ui16 casid, ui16 cap, const std::string& data, bool rsrvd = true) :
            Descriptor(TAG, BASE_LEN),
            CA_system_id(casid), CA_pid(cap), reserved(rsrvd),
            private_data( incLength(data) ) { }

        // utility
        virtual Descriptor *clone() const { return new CADesc(*this); }
        virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
        virtual void dump(std::ostream&) const;
#endif
    };



    // ---------------------------
    // Copyright Descriptor
    //
    class CopyrightDesc : public Descriptor
    {
    private:
        // data
        ui32 identifier;
        std::string info;

    public:
        enum { TAG = 13, BASE_LEN = 4 };

        // constructor
        CopyrightDesc(ui32 ci, const std::string& data, bool rsrvd = true) :
            Descriptor(TAG, BASE_LEN),
            identifier(ci), info( incLength(data) ) { }

        // utility
        virtual Descriptor *clone() const { return new CopyrightDesc(*this); }
        virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
        virtual void dump(std::ostream&) const;
#endif
    };


    // ---------------------------
    // Data Stream Alignment Descriptor - derived from
    // PrimitiveDatatypeDesc which handles building sections
    //
    //  ui8 data represents alignment_type
    //
    class DataStreamAlignmentDesc : public PrimitiveDatatypeDesc<ui8>
    {
    public:
        enum { TAG = 6 }; // BASE_LEN implicitly 1 (ui8)

        enum VS_Alignment_t {
            SLICE = 0x01,
            PICTURE = 0x02,
            GOP = 0x03,
            SEQ = 0x04
        };

        enum AS_Alignment_t {
            AUDIO_FRAME = 0x01
        };

        // constructor
        DataStreamAlignmentDesc(ui8 type) :
            PrimitiveDatatypeDesc<ui8>(TAG, type) { }

        // utility
        virtual Descriptor *clone() const {
            return new DataStreamAlignmentDesc(*this);
        }

#ifdef ENABLE_DUMP
        virtual void dump(std::ostream& o) const {
            dumpData( o, DATA_STREAM_ALIGNMENT_D_S, ALIGNMENT_TYPE_S );
        }
#endif
    };


    // ---------------------------
    // Hierarchy Descriptor
    //
    class HierarchyDesc : public Descriptor
    {
    private:
        ui16 type : 4,             // hierarchy type
        layer_index : 6,
        embedded_layer : 6,
        priority : 6;
        bool reserved;

    public:
        enum { TAG = 4, BASE_LEN = 4 };

        enum { SPATIAL_SCALABILITY = 1,
               SNR_SCALABILITY,
               TEMPORAL_SCALABILITY,
               DATA_PARTITIONING,
               EXTENSION_BITSTREAM };

        // constructor
        HierarchyDesc(ui8 h_type, ui8 h_layer_index, ui8 h_embedded_layer,
                      ui8 h_priority, bool rsrvd = true) :
            Descriptor(TAG, BASE_LEN),
            type(h_type), layer_index(h_layer_index),
            embedded_layer(h_embedded_layer), priority(h_priority),
            reserved(rsrvd) { }

        // utility
        virtual Descriptor* clone() const { return new HierarchyDesc(*this); }
        virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
        virtual void dump(std::ostream&) const;
#endif
    };



    // ---------------------------
    // IBP Descriptor
    //
    class IBPDesc : public Descriptor
    {
    private:
        bool closed_gop_flag,
            identical_gop_flag;
        ui16 max_gop_len : 14;

    public:
        enum { TAG = 18, BASE_LEN = 2 };

        // constructor
        IBPDesc(bool cgf, bool igf, ui16 mgl) :
            Descriptor(TAG, BASE_LEN),
            closed_gop_flag(cgf), identical_gop_flag(igf),
            max_gop_len(mgl) { }

        // utility
        virtual Descriptor* clone() const { return new IBPDesc(*this); }
        virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
        virtual void dump(std::ostream&) const;
#endif
    };


    // ---------------------------
    // ISO 639 Language Descriptor - ISO/IEC 13818-1 (1996)
    //
    class ISO639LanguageDesc : public Descriptor
    {
    public:
        enum { TAG = 10, BASE_LEN = 0 };

        struct Language {
            enum { BASE_LEN = LanguageCode::BASE_LEN + 1 };

            LanguageCode code;
            ui8 audio_type;

            Language( const std::string& lc, ui8 at ) : code(lc), audio_type(at) { }
        };


        // audio types
        enum AudioType_t {
            CLEAN_EFFECTS = 0x01,
            HEARING_IMPAIRED = 0x02,
            VISUAL_IMPAIRED_COMM = 0x03
        };

        // constructor
        ISO639LanguageDesc() : Descriptor(TAG, BASE_LEN) {}

        // utility
        virtual Descriptor *clone() const { return new ISO639LanguageDesc(*this); }

        bool addLanguage(const std::string& code, ui8 audio_type);
        virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
        virtual void dump(std::ostream&) const;
#endif

    private:
        // data
        std::list<Language> language_list;
    };



    // ---------------------------
    // Maximum Bitrate Descriptor
    //
    class MaximumBitrateDesc : public Descriptor
    {
    private:
        ui32 maximum_bitrate : 22;
        bool reserved;

    public:
        enum { TAG = 14, BASE_LEN = 3 };

        // constructor
        MaximumBitrateDesc(ui32 max_br, bool rsrvd = true) :
            Descriptor(TAG, BASE_LEN),
            maximum_bitrate(max_br), reserved(rsrvd) { }

        // utility
        virtual Descriptor *clone() const { return new MaximumBitrateDesc(*this); }
        virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
        virtual void dump(std::ostream&) const;
#endif
    };


    // ---------------------------
    // Multiplex Buffer Utilization Descriptor (ISO/IEC 13818-1 1996)
    //
    class MultiplexBufferUtilizationDesc : public Descriptor
    {
    private:
        bool bound_valid_flag,
            reserved;
        ui16 LTW_offset_lb : 15,
            LTW_offset_ub : 14;

    public:
        enum { TAG = 12, BASE_LEN = 4 };
        enum Strategy_t { EARLY = 1, LATE, MIDDLE };

        // constructor
        MultiplexBufferUtilizationDesc(bool bvf, ui16 ltw_o_lb, ui16 ltw_o_ub,
                                       bool rsrvd = true) :
            Descriptor(TAG, BASE_LEN),
            bound_valid_flag(bvf), reserved(rsrvd),
            LTW_offset_lb(ltw_o_lb), LTW_offset_ub(ltw_o_ub) { }

        // utility
        virtual Descriptor *clone() const {
            return new MultiplexBufferUtilizationDesc(*this);
        }
        virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
        virtual void dump(std::ostream&) const;
#endif
    };


    // ---------------------------
    // Private Data Indicator Descriptor - derived from
    // PrimitiveDatatypeDesc which handles buildingSections
    //
    //  ui32 data represents private_data_indicator
    //
    class PrivateDataIndicatorDesc : public PrimitiveDatatypeDesc<ui32>
    {
    public:
        enum { TAG = 15 }; // BASE_LEN is implicitly 4 (ui32)

        // constructor
        PrivateDataIndicatorDesc(ui32 pdi) :
            PrimitiveDatatypeDesc<ui32>(TAG, pdi) { }

        // utility
        virtual Descriptor *clone() const {
            return new PrivateDataIndicatorDesc( *this );
        }

#ifdef ENABLE_DUMP
        virtual void dump(std::ostream& o) const {
            dumpData( o, PVT_DATA_IND_D_S, PVT_DATA_IND_S );
        }
#endif
    };


    // ---------------------------
    // Registration Descriptor
    //
    class RegistrationDesc : public Descriptor
    {
    private:
        // data
        ui32 identifier;
        std::string info;

    public:
        enum { TAG = 5, BASE_LEN = 4 };

        // constructor
        RegistrationDesc(ui32 fi, const std::string& data, bool rsrvd = true) :
            Descriptor(TAG, BASE_LEN),
            identifier(fi), info( incLength(data) ) { }

        // utility
        virtual Descriptor *clone() const { return new RegistrationDesc(*this); }
        virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
        virtual void dump(std::ostream&) const;
#endif
    };



    // ---------------------------
    // Smoothing Buffer Descriptor
    //
    class SmoothingBufferDesc : public Descriptor
    {
    private:
        ui32 sb_leak_rate : 22,
        sb_size : 22;
        bool reserved;

    public:
        enum { TAG = 16, BASE_LEN = 6 };

        // constructor
        SmoothingBufferDesc(ui32 sblr, ui32 size, bool rsrvd = true) :
            Descriptor(TAG, BASE_LEN),
            sb_leak_rate(sblr), sb_size(size), reserved(rsrvd)  { }

        // utility
        virtual Descriptor *clone() const { return new SmoothingBufferDesc(*this); }
        virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
        virtual void dump(std::ostream&) const;
#endif
    };



    // ---------------------------
    // STD Descriptor (what were they thinking?)
    //
    class STDDesc : public Descriptor
    {
    private:
        bool leak_valid_flag,
            reserved;

    public:
        enum { TAG = 17, BASE_LEN = 1 };

        // constructor
        STDDesc(bool lvf, bool rsrvd = true) :
            Descriptor(TAG, BASE_LEN),
            leak_valid_flag(lvf), reserved(rsrvd)  { }

        // utility
        virtual Descriptor *clone() const { return new STDDesc(*this); }
        virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
        virtual void dump(std::ostream&) const;
#endif
    };



    // ---------------------------
    // System Clock Descriptor
    //
    class SystemClockDesc : public Descriptor
    {
    private:
        ui16 clock_accuracy_integer : 6,
        clock_accuracy_exponent : 3;
        bool external_clock_reference_indicator,
            reserved;

    public:
        enum { TAG = 11, BASE_LEN = 2 };

        // constructor
        SystemClockDesc(bool ecri, ui8 cai, ui8 cae, bool rsrvd = true) :
            Descriptor(TAG, BASE_LEN),
            clock_accuracy_integer(cai),
            clock_accuracy_exponent(cae),
            external_clock_reference_indicator(ecri),
            reserved(rsrvd) { }

        // utility
        virtual Descriptor *clone() const { return new SystemClockDesc(*this); }
        virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
        virtual void dump(std::ostream&) const;
#endif
    };



    // ---------------------------
    // Target Background Grid Descriptor
    //
    class TargetBackgroundGridDesc : public Descriptor
    {
    private:
        ui32 horizontal_size : 14,
	vertical_size : 14,
	pel_aspect_ratio : 4;

    public:
        enum { TAG = 7, BASE_LEN = 4 };

        // constructor
        TargetBackgroundGridDesc(ui16 hs, ui16 vs, ui8 par) :
            Descriptor(TAG, BASE_LEN),
            horizontal_size(hs), vertical_size(vs), pel_aspect_ratio(par) { }

        // utility
        virtual Descriptor *clone() const {
            return new TargetBackgroundGridDesc(*this);
        }
        virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
        virtual void dump(std::ostream&) const;
#endif
    };



    // ---------------------------
    // Video Stream Descriptor
    //
    class VideoStreamDesc : public Descriptor
    {
    private:
        bool multiple_frame_rate_flag,
            MPEG_2_flag,
            constrained_parameter_flag,
            still_picture_flag,
            frame_rate_extension_flag;

        // if MPEG_2_flag = 1, the following fields are required
        ui8 profile_and_level_indication,
            frame_rate_code : 4,
            chroma_format : 2;
        bool reserved;

    public:
        enum { TAG = 2, BASE_LEN = 1 };

        // Use this constructor is MPEG_2_flag is 0
        VideoStreamDesc(bool mfrf, ui8 frc, bool cpf, bool spf) :
            Descriptor(TAG, BASE_LEN),
            multiple_frame_rate_flag(mfrf), MPEG_2_flag(false),
            constrained_parameter_flag(cpf), still_picture_flag(spf),
            frame_rate_code(frc) { }
        // Use this one is MPEG_2_flag is 1
        VideoStreamDesc(bool mfrf, ui8 frc, bool cpf, bool spf,
                        ui8 pali, ui8 cf, bool fref, bool rsrvd = true) :
            Descriptor(TAG, BASE_LEN + 2), // extra data needs 2 bytes
            multiple_frame_rate_flag(mfrf), MPEG_2_flag(true),
            constrained_parameter_flag(cpf), still_picture_flag(spf),
            frame_rate_extension_flag(fref), profile_and_level_indication(pali),
            frame_rate_code(frc), chroma_format(cf), reserved(rsrvd) { }

        // utility
        virtual Descriptor *clone() const { return new VideoStreamDesc(*this); }
        virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
        virtual void dump(std::ostream&) const;
#endif
    };




    // ---------------------------
    // Video Window Descriptor
    //
    class VideoWindowDesc : public Descriptor
    {
    private:
        ui32 horizontal_offset : 14,
	vertical_offset : 14,
	window_priority : 4;

    public:
        enum { TAG = 8, BASE_LEN = 4 };

        // constructor
        VideoWindowDesc(ui16 ho, ui16 vo, ui8 wp) :
            Descriptor(TAG, BASE_LEN),
            horizontal_offset(ho), vertical_offset(vo), window_priority(wp) { }

        // utility
        virtual Descriptor *clone() const { return new VideoWindowDesc(*this); }
        virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
        virtual void dump(std::ostream&) const;
#endif
    };

} // namespace

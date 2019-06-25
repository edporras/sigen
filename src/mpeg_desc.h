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
// mpeg_desc.h: class definitions for the mpeg stream descriptors
// -----------------------------------

#pragma once

#include <list>
#include "descriptor.h"

namespace sigen {

   class Section;
   class LanguageCode;

   /*! \addtogroup descriptor
    *  @{
    */

   /*!
    * \defgroup mpeg_d MPEG Descriptors
    *  @{
    */

   /*!
    * \brief Audio Stream Descriptor.
    */
   class AudioStreamDesc : public Descriptor
   {
   public:
      enum { TAG = 3 };

      /*!
       * \brief Constructor.
       * \param free_fmt_f Free format flag.
       * \param ID `true`: ID field is `1` in each audio frame in the stream; `false`: otherwise.
       * \param lyr Layer field as per ISO/IEC 13818-3).
       * \param var_rate_indic `false`: Bit rate shall not change in consecutive audio frames.
       */
      AudioStreamDesc(bool free_fmt_f, bool ID, ui8 lyr, bool var_rate_indic = true)
         : Descriptor(TAG, 1),
           free_format_flag(free_fmt_f), id(ID), layer(lyr), variable_rate_indicator(var_rate_indic)
      { }
      AudioStreamDesc() = delete;

      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif

   private:
      bool free_format_flag;
      bool id;
      bool variable_rate_indicator;
      ui8 layer : 2;
   };

   /*!
    * \brief CA Descriptor.
    */
   class CADesc : public Descriptor
   {
   public:
      enum { TAG = 9 };

      /*!
       * \brief Constructor.
       * \param casid CA System ID.
       * \param ca_pid PID of the transport stream packets carrying ECM and/or EMM streams.
       * \param data Private bytes.
       */
      CADesc(ui16 casid, ui16 ca_pid, const std::string& data)
         : Descriptor(TAG, 4),
         CA_system_id(casid), CA_pid(ca_pid),
         private_data( incLength(data) )
      { }
      CADesc() = delete;

      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif

   private:
      // data
      ui16 CA_system_id;
      ui16 CA_pid : 13;
      std::string private_data;
   };

   /*!
    * \brief Copyright Descriptor.
    */
   class CopyrightDesc : public Descriptor
   {
   public:
      enum { TAG = 13 };

      /*!
       * \brief Constructor.
       * \param cr_identifier Identifier from the Registration Authority.
       * \param data Additional copyright information.
       */
      CopyrightDesc(ui32 cr_identifier, const std::string& data = "")
         : Descriptor(TAG, 4),
         identifier(cr_identifier), info( incLength(data) )
      { }
      CopyrightDesc() = delete;

      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif

   private:
      ui32 identifier;
      std::string info;
   };


   /*!
    * \brief Data Stream Alignment Descriptor.
    */
   struct DataStreamAlignmentDesc : public PrimitiveDatatypeDesc<ui8>
   {
      enum { TAG = 6 };

      enum VS_Alignment_t {
         SLICE   = 0x01,
         PICTURE = 0x02,
         GOP     = 0x03,
         SEQ     = 0x04,
      };

      enum AS_Alignment_t {
         SYNC_WORD = 0x01,
      };

      /*!
       * \brief Constructor.
       * \param type Alignment type.
       */
      DataStreamAlignmentDesc(ui8 type) : PrimitiveDatatypeDesc<ui8>(TAG, type) { }

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream& o) const {
         dumpData( o, DATA_STREAM_ALIGNMENT_D_S, ALIGNMENT_TYPE_S );
      }
#endif
   };

   /*!
    * \brief Hierarchy Descriptor.
    */
   class HierarchyDesc : public Descriptor
   {
   public:
      enum { TAG = 4 };

      enum HierarcyType {
         SPATIAL_SCALABILITY = 1,
         SNR_SCALABILITY,
         TEMPORAL_SCALABILITY,
         DATA_PARTITIONING,
         EXTENSION_BITSTREAM,
         PRIVATE_STREAM,
         MULTI_VIEW_PROFILE,
      };

      /*!
       * \brief Constructor.
       * \param h_type Hierarchy type.
       * \param h_layer_index Index uniquely identifying the associated program element.
       * \param h_embedded_layer Hierarchy embedded layer index.
       * \param h_channel Intended channel number.
       */
      HierarchyDesc(ui8 h_type, ui8 h_layer_index, ui8 h_embedded_layer,
                    ui8 h_channel)
         : Descriptor(TAG, 4),
           type(h_type), layer_index(h_layer_index),
           embedded_layer(h_embedded_layer), priority(h_channel)
      { }
      HierarchyDesc() = delete;

      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif

   private:
      ui16 type : 4,             // hierarchy type
           layer_index : 6,
           embedded_layer : 6,
           priority : 6;
   };


   /*!
    * \brief IBP Descriptor.
    */
   class IBPDesc : public Descriptor
   {
   public:
      enum { TAG = 18 };

      /*!
       * \brief Constructor.
       * \param closed_gop_f Closed gop flag.
       * \param ident_gop_f Idential gop flag.
       * \param max_gop_len Maximum gop len.
       */
      IBPDesc(bool closed_gop_f, bool ident_gop_f, ui16 max_gop_len)
         : Descriptor(TAG, 2),
         closed_gop_flag(closed_gop_f), identical_gop_flag(ident_gop_f), max_gop_len(max_gop_len)
      { }
      IBPDesc() = delete;

      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif

   private:
      bool closed_gop_flag;
      bool identical_gop_flag;
      ui16 max_gop_len : 14;
   };


   /*!
    * \brief ISO 639 Language Descriptor - ISO/IEC 13818-1 (1996).
    */
   class ISO639LanguageDesc : public Descriptor
   {
   public:
      enum { TAG = 10 };

      /*! \enum AudioType_t
       *
       * \brief Audio Type values.
       */
      enum AudioType_t {
         CLEAN_EFFECTS        = 0x01, //!< Clean effects.
         HEARING_IMPAIRED     = 0x02, //!< Hearing impaired.
         VISUAL_IMPAIRED_COMM = 0x03, //!< Visual impaired commentary.
      };

      //! \brief Constructor.
      ISO639LanguageDesc() : Descriptor(TAG) {}

      /*!
       * \brief Add a language to the descriptor loop.
       * \param code ISO-639 language code of the program element.
       * \param audio_type Type of stream as per ISO639LanguageDesc::AudioType_t.
       */
      bool addLanguage(const LanguageCode& code, ui8 audio_type);

      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif

   private:
      struct Language : public STable::ListItem {
         enum { BASE_LEN = LanguageCode::ISO_639_2_CODE_LENGTH + 1 };

         LanguageCode code;
         ui8 audio_type;

         Language( const LanguageCode& lc, ui8 at ) : code(lc), audio_type(at) { }
         Language() = delete;
      };

      std::list<Language> language_list;
   };

   /*!
    * \brief Maximum Bitrate Descriptor.
    */
   class MaximumBitrateDesc : public Descriptor
   {
   public:
      enum { TAG = 14 };

      /*!
       * \brief Constructor.
       * \param max_br Maximum bitrate that will be encountered on thir program or service.
       */
      MaximumBitrateDesc(ui32 max_br)
         : Descriptor(TAG, 3),
         maximum_bitrate(max_br)
      { }
      MaximumBitrateDesc() = delete;

      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif

   private:
      ui32 maximum_bitrate : 22;
   };


   /*!
    * \brief Multiplex Buffer Utilization Descriptor (ISO/IEC 13818-1 1996).
    */
   class MultiplexBufferUtilizationDesc : public Descriptor
   {
   public:
      enum { TAG = 12 };
      enum Strategy_t { EARLY = 1, LATE, MIDDLE };

      /*!
       * \brief Constructor.
       * \param bound_valid_f `true`: both bound values are valid; `false`: otherwise.
       * \param ltw_offset_lb LTW-offset lower-bound.
       * \param ltw_offset_ub LTW-offset upper-bound.
       */
      MultiplexBufferUtilizationDesc(bool bound_valid_f, ui16 ltw_offset_lb, ui16 ltw_offset_ub)
         : Descriptor(TAG, 4),
           bound_valid_flag(bound_valid_f),
           LTW_offset_lb(ltw_offset_lb), LTW_offset_ub(ltw_offset_ub)
      { }
      MultiplexBufferUtilizationDesc() = delete;

      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif

   private:
      bool bound_valid_flag;
      ui16 LTW_offset_lb : 15;
      ui16 LTW_offset_ub : 14;
   };

   /*!
    * \brief Private Data Indicator Descriptor.
    */
   struct PrivateDataIndicatorDesc : public PrimitiveDatatypeDesc<ui32>
   {
      enum { TAG = 15 };

      /*!
       * \brief Constructor.
       * \param pdi Private data indicator.
       */
      PrivateDataIndicatorDesc(ui32 pdi) : PrimitiveDatatypeDesc<ui32>(TAG, pdi) { }

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream& o) const {
         dumpData( o, PVT_DATA_IND_D_S, PVT_DATA_IND_S );
      }
#endif
   };

   /*!
    * \brief Registration Descriptor.
    */
   class RegistrationDesc : public Descriptor
   {
   public:
      enum { TAG = 5 };

      /*!
       * \brief Constructor.
       * \param format_ident Format identifier as per ISO/IEC JTC 1/SC 29.
       * \param data Additional identification information.
       */
      RegistrationDesc(ui32 format_ident, const std::string& data)
         : Descriptor(TAG, 4),
         identifier(format_ident), info( incLength(data) )
      { }
      RegistrationDesc() = delete;

      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif

   private:
      ui32 identifier;
      std::string info;
   };

   /*!
    * \brief Smoothing Buffer Descriptor.
    */
   class SmoothingBufferDesc : public Descriptor
   {
   public:
      enum { TAG = 16 };

      /*!
       * \brief Constructor.
       * \param sb_lk_rate Smoothing buffer leak rate.
       * \param size Smoothing buffer size
       */
      SmoothingBufferDesc(ui32 sb_lk_rate, ui32 size)
         : Descriptor(TAG, 6),
         sb_leak_rate(sb_lk_rate), sb_size(size)
      { }
      SmoothingBufferDesc() = delete;

      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif

   private:
      ui32 sb_leak_rate : 22;
      ui32 sb_size : 22;
   };

   /*!
    * \brief STD Descriptor.
    */
   class STDDesc : public Descriptor
   {
   public:
      enum { TAG = 17 };

      /*!
       * \brief Constructor.
       * \param lvf Leak-valid flag.
       */
      STDDesc(bool lvf)
         : Descriptor(TAG, 1),
         leak_valid_flag(lvf)
      { }
      STDDesc() = delete;

      // utility
      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif

   private:
      bool leak_valid_flag;
   };

   /*!
    * \brief System Clock Descriptor.
    */
   class SystemClockDesc : public Descriptor
   {
   public:
      enum { TAG = 11 };

      /*!
       * \brief Constructor.
       * \param ext_clk_ref_indic `true`: System clock is derived from external frequency reference.
       * \param clk_accuracy_int 6-bit clock accuracy integer.
       * \param clk_accuracy_exp 3-bit clock accuracy exponent.
       */
      SystemClockDesc(bool ext_clk_ref_indic, ui8 clk_accuracy_int, ui8 clk_accuracy_exp)
         : Descriptor(TAG, 2),
           external_clock_reference_indicator(ext_clk_ref_indic),
           clock_accuracy_integer(clk_accuracy_int),
           clock_accuracy_exponent(clk_accuracy_exp)
      { }
      SystemClockDesc() = delete;

      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif

   private:
      bool external_clock_reference_indicator;
      ui16 clock_accuracy_integer : 6,
           clock_accuracy_exponent : 3;
   };

   /*!
    * \brief Target Background Grid Descriptor.
    */
   class TargetBackgroundGridDesc : public Descriptor
   {
   public:
      enum { TAG = 7 };

      /*!
       * \brief Constructor.
       * \param horiz_size Horizontal size of background grid in pixels.
       * \param vert_size Vertical size of background grid in pixels.
       * \param a_r_info Aspect ratio information as per ITU-T Rec. H.262 | ISO/IEC 13818-2.
       */
      TargetBackgroundGridDesc(ui16 horiz_size, ui16 vert_size, ui8 a_r_info)
         : Descriptor(TAG, 4),
         horizontal_size(horiz_size), vertical_size(vert_size), pel_aspect_ratio(a_r_info)
      { }
      TargetBackgroundGridDesc() = delete;

      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif

   private:
      ui32 horizontal_size : 14,
           vertical_size : 14,
           pel_aspect_ratio : 4;
   };

   /*!
    * \brief Video Stream Descriptor.
    */
   class VideoStreamDesc : public Descriptor
   {
   public:
      enum { TAG = 2 };

      /*!
       * \brief Constructor for descriptors where MPEG_1_only_flag must be set to `1`.
       * \param mult_frame_rate_f Multiple frame rate flag.
       * \param frame_rate_code Frame rate code as per ITU-T Rec. H.262 | ISO/IEC 13818-2.
       * \param constr_param_f `false`: stream may contain constrained and unconstrained video streams; `true`: does not contain unconstrained streams.
       * \param still_pic_f `false`: stream may contain still and moving picture data; `true`: stream contains only still pictures.
       */
      VideoStreamDesc(bool mult_frame_rate_f, ui8 frame_rate_code, bool constr_param_f, bool still_pic_f)
         : VideoStreamDesc(mult_frame_rate_f, frame_rate_code, constr_param_f, still_pic_f, true)
      { }
      /*!
       * \brief Constructor for descriptors where MPEG_1_only_flag must be set to `0`.
       * \param mult_frame_rate_f Multiple frame rate flag.
       * \param frame_rate_code Frame rate code as per ITU-T Rec. H.262 | ISO/IEC 13818-2.
       * \param constr_param_f `false`: stream may contain constrained and unconstrained video streams; `true`: does not contain unconstrained streams.
       * \param still_pic_f `false`: stream may contain still and moving picture data; `true`: stream contains only still pictures.
       * \param profile_and_level_indic Profile and level indication fields as per ITU-T Rec. H.262 | ISO/IEC 13818-2.
       * \param chroma_fmt Chroma format as per ITU-T Rec. H.262 | ISO/IEC 13818-2.
       * \param frame_rate_ext_f `true`: indicates frame extension flags are non-zero.
       */
      VideoStreamDesc(bool mult_frame_rate_f, ui8 frame_rate_code, bool constr_param_f, bool still_pic_f,
                      ui8 profile_and_level_indic, ui8 chroma_fmt, bool frame_rate_ext_f)
         : VideoStreamDesc(mult_frame_rate_f, frame_rate_code, constr_param_f, still_pic_f, false,
                           profile_and_level_indic, chroma_fmt, frame_rate_ext_f)
      { }
      VideoStreamDesc() = delete;

      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif

   private:
      bool multiple_frame_rate_flag;
      bool MPEG_1_only_flag;
      bool constrained_parameter_flag;
      bool still_picture_flag;
      bool frame_rate_extension_flag;

      // if MPEG_1_only_flag = 0, the following fields are required
      ui8 profile_and_level_indication;
      ui8 frame_rate_code : 4,
          chroma_format : 2;

      VideoStreamDesc(bool mult_frame_rate_f, ui8 frame_rate_code, bool constr_param_f, bool still_pic_f,
                      bool mpeg_1_only_f, ui8 profile_and_level_indic = 0, ui8 chroma_fmt = 0,
                      bool frame_rate_ext_f = false)
         : Descriptor(TAG, (mpeg_1_only_f ? 1 : 3)), // extra data needs 2 bytes
           multiple_frame_rate_flag(mult_frame_rate_f), MPEG_1_only_flag(mpeg_1_only_f),
           constrained_parameter_flag(constr_param_f), still_picture_flag(still_pic_f),
           frame_rate_extension_flag(frame_rate_ext_f),
           profile_and_level_indication(profile_and_level_indic),
           frame_rate_code(frame_rate_code), chroma_format(chroma_fmt)
      { }

   };

   /*!
    * \brief Video Window Descriptor.
    */
   class VideoWindowDesc : public Descriptor
   {
   public:
      enum { TAG = 8 };

      /*!
       * \brief Constructor.
       * \param horiz_offset Horizontal position of the display top-left.
       * \param vert_offset Vertical position of the display top-left.
       * \param win_priority Window overlap priority.
       */
      VideoWindowDesc(ui16 horiz_offset, ui16 vert_offset, ui8 win_priority)
         : Descriptor(TAG, 4),
         horizontal_offset(horiz_offset), vertical_offset(vert_offset), window_priority(win_priority)
      { }
      VideoWindowDesc() = delete;

      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif

   private:
      ui32 horizontal_offset : 14,
           vertical_offset : 14,
           window_priority : 4;
   };
   //! @}
   //! @}
} // namespace

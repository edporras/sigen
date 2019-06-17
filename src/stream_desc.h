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
// stream_desc.h: class definitions for the mpeg stream descriptors
// -----------------------------------

#pragma once

#include <memory>
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
   public:
      enum { TAG = 3 };

      // constructor
      AudioStreamDesc(bool fff, bool ID, ui8 l) :
         Descriptor(TAG, 1),
         free_format_flag(fff), id(ID), layer(l)
      { }
      AudioStreamDesc() = delete;

      // utility
      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif

   private:
      bool free_format_flag;
      bool id;
      ui8 layer : 2;
   };


   // ---------------------------
   // CA Descriptor
   //
   class CADesc : public Descriptor
   {
   public:
      enum { TAG = 9 };

      // constructor
      CADesc(ui16 casid, ui16 cap, const std::string& data) :
         Descriptor(TAG, 4),
         CA_system_id(casid), CA_pid(cap),
         private_data( incLength(data) )
      { }
      CADesc() = delete;

      // utility
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



   // ---------------------------
   // Copyright Descriptor
   //
   class CopyrightDesc : public Descriptor
   {
   public:
      enum { TAG = 13 };

      // constructor
      CopyrightDesc(ui32 ci, const std::string& data) :
         Descriptor(TAG, 4),
         identifier(ci), info( incLength(data) )
      { }
      CopyrightDesc() = delete;

      // utility
      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif

   private:
      ui32 identifier;
      std::string info;
   };


   // ---------------------------
   // Data Stream Alignment Descriptor - derived from
   // PrimitiveDatatypeDesc which handles building sections
   //
   //  ui8 data represents alignment_type
   //
   struct DataStreamAlignmentDesc : public PrimitiveDatatypeDesc<ui8>
   {
      enum { TAG = 6 };

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
         PrimitiveDatatypeDesc<ui8>(TAG, type)
      { }

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
   public:
      enum { TAG = 4 };

      enum { SPATIAL_SCALABILITY = 1,
             SNR_SCALABILITY,
             TEMPORAL_SCALABILITY,
             DATA_PARTITIONING,
             EXTENSION_BITSTREAM };

      // constructor
      HierarchyDesc(ui8 h_type, ui8 h_layer_index, ui8 h_embedded_layer,
                    ui8 h_priority) :
         Descriptor(TAG, 4),
         type(h_type), layer_index(h_layer_index),
         embedded_layer(h_embedded_layer), priority(h_priority)
      { }
      HierarchyDesc() = delete;

      // utility
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



   // ---------------------------
   // IBP Descriptor
   //
   class IBPDesc : public Descriptor
   {
   public:
      enum { TAG = 18 };

      // constructor
      IBPDesc(bool cgf, bool igf, ui16 mgl) :
         Descriptor(TAG, 2),
         closed_gop_flag(cgf), identical_gop_flag(igf),
         max_gop_len(mgl)
      { }
      IBPDesc() = delete;

      // utility
      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif

   private:
      bool closed_gop_flag;
      bool identical_gop_flag;
      ui16 max_gop_len : 14;
   };


   // ---------------------------
   // ISO 639 Language Descriptor - ISO/IEC 13818-1 (1996)
   //
   class ISO639LanguageDesc : public Descriptor
   {
   public:
      enum { TAG = 10 };

      // audio types
      enum AudioType_t {
         CLEAN_EFFECTS = 0x01,
         HEARING_IMPAIRED = 0x02,
         VISUAL_IMPAIRED_COMM = 0x03
      };

      // constructor
      ISO639LanguageDesc() : Descriptor(TAG) {}

      // utility
      bool addLanguage(const std::string& code, ui8 audio_type);
      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif

   private:
      struct Language {
         enum { BASE_LEN = LanguageCode::BASE_LEN + 1 };

         LanguageCode code;
         ui8 audio_type;

         Language( const std::string& lc, ui8 at ) : code(lc), audio_type(at) { }
      };

      std::list<std::unique_ptr<Language> > language_list;
   };



   // ---------------------------
   // Maximum Bitrate Descriptor
   //
   class MaximumBitrateDesc : public Descriptor
   {
   public:
      enum { TAG = 14 };

      // constructor
      MaximumBitrateDesc(ui32 max_br) :
         Descriptor(TAG, 3),
         maximum_bitrate(max_br)
      { }
      MaximumBitrateDesc() = delete;

      // utility
      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif

   private:
      ui32 maximum_bitrate : 22;
   };


   // ---------------------------
   // Multiplex Buffer Utilization Descriptor (ISO/IEC 13818-1 1996)
   //
   class MultiplexBufferUtilizationDesc : public Descriptor
   {
   public:
      enum { TAG = 12 };
      enum Strategy_t { EARLY = 1, LATE, MIDDLE };

      // constructor
      MultiplexBufferUtilizationDesc(bool bvf, ui16 ltw_o_lb, ui16 ltw_o_ub) :
         Descriptor(TAG, 4),
         bound_valid_flag(bvf),
         LTW_offset_lb(ltw_o_lb), LTW_offset_ub(ltw_o_ub)
      { }
      MultiplexBufferUtilizationDesc() = delete;

      // utility
      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif

   private:
      bool bound_valid_flag;
      ui16 LTW_offset_lb : 15;
      ui16 LTW_offset_ub : 14;
   };


   // ---------------------------
   // Private Data Indicator Descriptor - derived from
   // PrimitiveDatatypeDesc which handles buildingSections
   //
   //  ui32 data represents private_data_indicator
   //
   struct PrivateDataIndicatorDesc : public PrimitiveDatatypeDesc<ui32>
   {
      enum { TAG = 15 };

      // constructor
      PrivateDataIndicatorDesc(ui32 pdi) :
         PrimitiveDatatypeDesc<ui32>(TAG, pdi)
      { }

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
   public:
      enum { TAG = 5 };

      // constructor
      RegistrationDesc(ui32 fi, const std::string& data) :
         Descriptor(TAG, 4),
         identifier(fi), info( incLength(data) )
      { }
      RegistrationDesc() = delete;

      // utility
      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif

   private:
      ui32 identifier;
      std::string info;
   };



   // ---------------------------
   // Smoothing Buffer Descriptor
   //
   class SmoothingBufferDesc : public Descriptor
   {
   public:
      enum { TAG = 16 };

      // constructor
      SmoothingBufferDesc(ui32 sblr, ui32 size) :
         Descriptor(TAG, 6),
         sb_leak_rate(sblr), sb_size(size)
      { }
      SmoothingBufferDesc() = delete;

      // utility
      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif

   private:
      ui32 sb_leak_rate : 22;
      ui32 sb_size : 22;
   };



   // ---------------------------
   // STD Descriptor
   //
   class STDDesc : public Descriptor
   {
   public:
      enum { TAG = 17 };

      // constructor
      STDDesc(bool lvf) :
         Descriptor(TAG, 1),
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


   // ---------------------------
   // System Clock Descriptor
   //
   class SystemClockDesc : public Descriptor
   {
   public:
      enum { TAG = 11 };

      // constructor
      SystemClockDesc(bool ecri, ui8 cai, ui8 cae) :
         Descriptor(TAG, 2),
         clock_accuracy_integer(cai),
         clock_accuracy_exponent(cae),
         external_clock_reference_indicator(ecri)
      { }
      SystemClockDesc() = delete;

      // utility
      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif

   private:
      ui16 clock_accuracy_integer : 6,
           clock_accuracy_exponent : 3;
      bool external_clock_reference_indicator;
   };


   // ---------------------------
   // Target Background Grid Descriptor
   //
   class TargetBackgroundGridDesc : public Descriptor
   {
   public:
      enum { TAG = 7 };

      // constructor
      TargetBackgroundGridDesc(ui16 hs, ui16 vs, ui8 par) :
         Descriptor(TAG, 4),
         horizontal_size(hs), vertical_size(vs), pel_aspect_ratio(par)
      { }
      TargetBackgroundGridDesc() = delete;

      // utility
      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif

   private:
      ui32 horizontal_size : 14,
           vertical_size : 14,
           pel_aspect_ratio : 4;
   };



   // ---------------------------
   // Video Stream Descriptor
   //
   class VideoStreamDesc : public Descriptor
   {
   public:
      enum { TAG = 2 };

      // Use this constructor if MPEG_2_flag is 0
      VideoStreamDesc(bool mfrf, ui8 frc, bool cpf, bool spf) :
         Descriptor(TAG, 1),
         multiple_frame_rate_flag(mfrf), MPEG_2_flag(false),
         constrained_parameter_flag(cpf), still_picture_flag(spf),
         frame_rate_code(frc)
      { }
      // Use this one if MPEG_2_flag is 1
      VideoStreamDesc(bool mfrf, ui8 frc, bool cpf, bool spf,
                      ui8 pali, ui8 cf, bool fref) :
         Descriptor(TAG, 3), // extra data needs 2 bytes
         multiple_frame_rate_flag(mfrf), MPEG_2_flag(true),
         constrained_parameter_flag(cpf), still_picture_flag(spf),
         frame_rate_extension_flag(fref), profile_and_level_indication(pali),
         frame_rate_code(frc), chroma_format(cf)
      { }
      VideoStreamDesc() = delete;

      // utility
      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif

   private:
      bool multiple_frame_rate_flag;
      bool MPEG_2_flag;
      bool constrained_parameter_flag;
      bool still_picture_flag;
      bool frame_rate_extension_flag;

      // if MPEG_2_flag = 1, the following fields are required
      ui8 profile_and_level_indication;
      ui8 frame_rate_code : 4,
          chroma_format : 2;
   };




   // ---------------------------
   // Video Window Descriptor
   //
   class VideoWindowDesc : public Descriptor
   {
   public:
      enum { TAG = 8 };

      // constructor
      VideoWindowDesc(ui16 ho, ui16 vo, ui8 wp) :
         Descriptor(TAG, 4),
         horizontal_offset(ho), vertical_offset(vo), window_priority(wp)
      { }
      VideoWindowDesc() = delete;

      // utility
      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif

   private:
      ui32 horizontal_offset : 14,
           vertical_offset : 14,
           window_priority : 4;
   };

} // namespace

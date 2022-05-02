// Copyright 1999-2022 Ed Porras
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
// pmt_desc.h: class definition for the DVB defined PMT descriptors
// -----------------------------------

#pragma once

#include <list>
#include <vector>
#include "descriptor.h"

namespace sigen {
   /*! \addtogroup descriptor
    *  @{
    */

   /*!
    * \brief Adaptation Field Data Descriptor.
    */
   class AdaptationFieldDataDesc : public PrimitiveDatatypeDesc<ui8>
   {
   public:
      enum { TAG = 0x70 };

      /*!
       * \enum  AdaptationField
       *
       * \brief Adaptation field data identifier coding.
       */
      enum AdaptationField {
         ANNOUNCEMENT_SWITCHING        = 0x01,  //!< Announcement switching data field, as per ETSI TS 101 154 [9].
         AU_INFO                       = 0x02,  //!< AU information data field, as per ETSI TS 101 154 [9].
         PVR_ASSIST                    = 0x04,  //!< PVR assist information data field, as per ETSI TS 101 154 [9].
         TSAP_TIMELINE                 = 0x08,  //!< TSPA timeline, as per DVB Blue Book A167-2 [i.10].
      };

      /*!
       * \brief Constructor.
       * \param identifier Adaptation field data identifier, as per AdaptationField.
       */
      AdaptationFieldDataDesc(ui8 identifier) : PrimitiveDatatypeDesc<ui8>(TAG, identifier) {}

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream& o) const {
         dumpData(o, ADAPTATION_FIELD_DATA_S, ADAP_FLD_DATA_IDENT_S);
      }
#endif
   };


   class _AC3Desc : public Descriptor
   {
   public:
      // prohibit - use the derived classes for AC3Desc or
      // ExtendedAC3Desc
      _AC3Desc() = delete;

      /*!
       * \brief set the component type field.
       * \param c_type Type of audio carried in the AC-3 stream.
       */
      void setComponentType(ui8 c_type) { set_value(COMPONENT_TYPE, c_type); }
      /*!
       * \brief set the bsid field.
       * \param bsid AC-3 coding version as set in the elementary stream.
       */
      void setBSID(ui8 bsid) { set_value(BSID, bsid); }
      /*!
       * \brief set the mainid field.
       * \param mainid Id identifying the main audio service.
       */
      void setMainid(ui8 mainid) { set_value(MAINID, mainid); }
      /*!
       * \brief set the asvc field.
       * \param asvc Identifies main services this is associated with.
       */
      void setASVC(ui8 asvc) { set_value(ASVC, asvc); }

      /*!
       * \brief set the additional info bytes.
       * \param addl_info_bytes ui8 byte vector.
       */
      void setAdditionalInfo(const std::vector<ui8>& addl_info_bytes);

      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream& o) const;
#endif

   protected:
      // values of the possible flags
      enum flag_t {
         COMPONENT_TYPE,
         BSID,
         MAINID,
         ASVC,
         MIXINFO_EXISTS,
         SUBSTREAM_1,
         SUBSTREAM_2,
         SUBSTREAM_3,
      };

   private:
      struct entry {
         bool on;
         ui8 val;

         entry() : on(false) {}
         void set(ui8 v) { val = v; on = true; }
      };
      std::vector<entry> fields;
      std::vector<ui8> additional_info;

   protected:
      _AC3Desc(ui8 tag, ui8 num_fields) : Descriptor(tag, 1), fields(num_fields) {}

      void set_value(flag_t key, ui8 value);
   };

   /*!
    * \brief AC-3 Descriptor (EN 300 468 v1.15.1, Annex D)
    */
   struct AC3Desc : public _AC3Desc
   {
      enum { TAG = 0x6a };

      //! \brief Constructor.
      AC3Desc() : _AC3Desc(TAG, 4) {}
   };

   /*!
    * \brief Extended AC-3 Descriptor (EN 300 468 v1.15.1, Annex D)
    */
   struct ExtendedAC3Desc : public _AC3Desc
   {
      enum { TAG = 0x7a };

      //! \brief Constructor.
      ExtendedAC3Desc() : _AC3Desc(TAG, 8) {}

      //! \brief set the mixinfo-exists flag.
      void setMixinfoExists() { set_value(MIXINFO_EXISTS, 0); } // value is ignored
      /*!
       * \brief set the substream1 field.
       * \param substream Type of audio carried in the independent substream 1.
       */
      void setSubstream1(ui8 substream) { set_value(SUBSTREAM_1, substream); }
      /*!
       * \brief set the substream2 field.
       * \param substream Type of audio carried in the independent substream 2.
       */
      void setSubstream2(ui8 substream) { set_value(SUBSTREAM_2, substream); }
      /*!
       * \brief set the substream3 field.
       * \param substream Type of audio carried in the independent substream 3.
       */
      void setSubstream3(ui8 substream) { set_value(SUBSTREAM_3, substream); }
   };

   /*!
    * \brief Ancillary Data Descriptor.
    */
   class AncillaryDataDesc : public PrimitiveDatatypeDesc<ui8>
   {
   public:
      enum { TAG = 0x6b };

      /*!
       * \enum  DataCoding
       *
       * \brief Ancillary data identifier coding, as per ETSI TS 101 154 [9].
       */
      enum DataCoding {
         DVD_VIDEO                 = 0x01,  //!< DVD-Video ancillary data.
         EXTENDED                  = 0x02,  //!< Extended ancillary data.
         ANNOUNCEMENT_SWITCHING    = 0x04,  //!< Announcement switching data.
         DAB                       = 0x08,  //!< DAB ancillary data.
         SCALE_FACTOR_ERROR_CHECK  = 0x10,  //!< Scale factor error check.
         MPEG_4                    = 0x20,  //!< MPEG-4 ancillary data.
         RDS_UECP                  = 0x40,  //!< RDS via UECP.
      };

      /*!
       * \brief Constructor.
       * \param ancillary_data_identifier Ancillary data coded in the audio elementary stream using the AncillaryDataDesc::DataCoding bit values.
       */
      AncillaryDataDesc(ui8 ancillary_data_identifier)
         : PrimitiveDatatypeDesc<ui8>(TAG, ancillary_data_identifier)
      {}

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream& o) const {
         dumpData(o, ANCILLARY_DATA_D_S, ANCILLARY_DATA_IDENT_S);
      }
#endif
   };

   class _DataBroadcastIdDesc : public Descriptor
   {
   public:
      enum { TAG = 0x66 };

      enum {
         DATA_PIPE                               = 0x0001,
         ASYNC_DATA_STREAM                       = 0x0002,
         SYNCRONOUS_DATA_STREAM                  = 0x0003,
         SYNCHONISED_DATA_STREAM                 = 0x0004,
         DATA_MULTI_PROTOCOL_ENCAP               = 0x0005,
         DATA_CAROUSEL                           = 0x0006,
         OBJECT_CAROUSEL                         = 0x0007,
         DVB_ATM_STREAMS                         = 0x0008,
         HIGHER_PROT_BASED_ON_ASYNC_DATA_STREAMS = 0x0009,
         SSU                                     = 0x000A,
         IPMAC_NOTIFICATION                      = 0x000B,

         MHP_OBJECT_CAROUSEL                     = 0x00F0,
         MHP_MULTIPROTOCOL_ENCAPSULATION         = 0x00F1,
         MHP_APPLICATION_PRESENCE                = 0x00F2,
      };

      _DataBroadcastIdDesc() = delete;

      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream& o) const;
#endif

   protected:
      const ui16 data_broadcast_id;

      // Also used by derived classes to pass the broadcast id and
      // base length of their data
      _DataBroadcastIdDesc(ui16 db_id, ui8 base_len = 0)
         : Descriptor(TAG, base_len + 2),
           data_broadcast_id(db_id)
      {}
   };

   /*!
    * \brief Data Broadcast Id Descriptor.
    */
   class DataBroadcastIdDesc : public _DataBroadcastIdDesc
   {
   public:
      /*!
       * \brief Constructor.
       * \param db_id Data broadcast specification as per ETSI TS 101 162.
       */
      DataBroadcastIdDesc(ui16 db_id) : _DataBroadcastIdDesc(db_id) {}

      /*!
       * \brief Set the selector bytes field.
       * \param bytes Vector with the byte sequence.
       */
      bool setSelectorBytes(const std::vector<ui8>& bytes);

      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream& o) const;
#endif

   private:
      std::vector<ui8> selector_bytes;
   };


   /*!
    * \brief Service Move Descriptor.
    */
   class ServiceMoveDesc : public Descriptor
   {
   public:
      enum { TAG = 0x60 };

      /*!
       * \brief Constructor.
       * \param new_onid Id identifying the new originating network.
       * \param new_xs_id Id uniquely identifying the new transport stream.
       * \param new_sid Id uniquely identifying the service after the move.
       */
      ServiceMoveDesc(ui16 new_onid, ui16 new_xs_id, ui16 new_sid)
         : Descriptor(TAG, 6),
         original_network_id(new_onid), xport_stream_id(new_xs_id), new_service_id(new_sid)
      {}
      ServiceMoveDesc() = delete;

      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif

   private:
      ui16 original_network_id;
      ui16 xport_stream_id;
      ui16 new_service_id;
   };


   /*!
    * \brief Stream Identifier Descriptor.
    */
   struct StreamIdentifierDesc : public PrimitiveDatatypeDesc<ui8>
   {
      enum { TAG = 0x52 };

      /*!
       * \brief Constructor.
       * \param c_tag Component tag associated with a description given in a component descriptor.
       */
      StreamIdentifierDesc(ui8 c_tag) : PrimitiveDatatypeDesc<ui8>(TAG, c_tag) {}

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream& o) const {
         dumpData(o, STREAM_IDENT_D_S, COMPONENT_TAG_S);
      }
#endif
   };


   /*!
    * \brief Subtitling Descriptor.
    */
   class SubtitlingDesc : public Descriptor
   {
   public:
      enum { TAG = 0x59 };

      //! \brief Constructor.
      SubtitlingDesc() : Descriptor(TAG) {}

      /*!
       * \brief Adds a subtitling entry to the descriptor.
       * \param lang_code ISO-639-2 code of the subtitle language.
       * \param type Subtitling type.
       * \param c_page_id Id of the composition page.
       * \param a_page_id Id of the optional ancillary page.
       */
      bool addSubtitling(const std::string& lang_code, ui8 type, ui16 c_page_id,
                         ui16 a_page_id);

      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif

   private:
      struct Subtitling : public STable::ListItem
      {
         enum { BASE_LEN = 8 };

         // data
         LanguageCode language_code;
         ui16 composition_page_id;
         ui16 ancillary_page_id;
         ui8 type;

         Subtitling( const std::string code, ui8 st, ui16 cpid, ui16 apid ) :
            language_code(code), composition_page_id(cpid),
            ancillary_page_id(apid), type(st) { }
         Subtitling() = delete;
      };

      std::list<Subtitling> subtitling_list;
   };

   /*!
    * \brief Teletext Descriptor.
    */
   class TeletextDesc : public Descriptor
   {
   public:
      enum { TAG = 0x56 };

      /*!
       * \enum  Type
       *
       * \brief Teletext type coding.
       */
      enum Type {
         INITIAL     = 0x01, //!< Initial Teletext page.
         SUBTITLE    = 0x02, //!< Teletext subtitle page.
         ADDTNL_INFO = 0x03, //!< Additional information page.
         PROG_SCHED  = 0x04, //!< Program schedule page.
         HEARING_IMP = 0x05, //!< Teletext subtitle page for hearing impaired.
      };

      //! \brief Constructor.
      TeletextDesc() : Descriptor(TAG) {}

      /*!
       * \brief Adds a teletext entry to the descriptor.
       * \param lang_code ISO-639-2 code of the teletext language.
       * \param type Teletext type, as per TeletextDesc::Type.
       * \param mag_num Teletext magazine number, as per ETSI EN 300 706.
       * \param page_num Teletext page number, as per ETSI EN 300 706.
       */
      bool addTeletext(const std::string& lang_code, ui8 type, ui8 mag_num,
                       ui8 page_num);

      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif

   private:
      struct Teletext : public STable::ListItem
      {
         enum { BASE_LEN = 5 };

         // data
         LanguageCode language_code;
         ui8 page_number;
         ui8 type : 5,
             magazine_number : 3;

         Teletext(const std::string& code, ui8 t, ui8 mn, ui8 pn) :
            language_code(code), page_number(pn), type(t),
            magazine_number(mn) { }
         Teletext() = delete;
      };

      std::list<Teletext> teletext_list;
   };
   //! @}
} // sigen namespace

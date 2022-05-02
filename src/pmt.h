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
// pmt.h: class definition for the PMT
// -----------------------------------

#pragma once

#include <memory>
#include <list>
#include "table.h"

namespace sigen {

   class Descriptor;

   /*! \addtogroup table
    *  @{
    */

   /*! \addtogroup MPEG
    *  @{
    */

   /*!
    * \brief Program Map %Table, as per ISO 13818-1.
    */
   class PMT : public ExtPSITable
   {
   public:
      /*!
       * \enum  esTypes
       *
       * \brief Elementary stream types, as per ISO/IEC 13818-1.
       */
      enum esTypes {
         ES_RESERVED                                          = 0x00, //!< ITU-T | ISO/IEC reserved.
         ES_ISO_IEC_11172_VIDEO                               = 0x01, //!< ISO/IEC 11172 video.
         ES_ISO_IEC_13818_2_VIDEO                             = 0x02, //!< ITU-T Rec. H.262 | ISO/IEC 13818-2 video or ISO/IEC 11172-2 constrained param. video stream.
         ES_ISO_IEC_11172_AUDIO                               = 0x03, //!< ISO/IEC 11172 audio.
         ES_ISO_IEC_13818_3_AUDIO                             = 0x04, //!< ISO/IEC 13818-3 audio.
         ES_ISO_IEC_13818_1_PRIVATE_SECTIONS                  = 0x05, //!< ITU-T Rec. H.222.0 | ISO/IEC 13818-1 priv. sections.
         ES_ISO_IEC_13818_1_PES_PRIVATE_DATA                  = 0x06, //!< ITU-T Rec. H.222.0 | ISO/IEC 13818-1 PES packets containing priv. data.
         ES_ISO_IEC_13522_MHEG                                = 0x07, //!< ISO/IEC 13522 MHEG.
         ES_ISO_IEC_13818_1_ANNEX_A_DSM_CC                    = 0x08, //!< ITU-T Rec. H.222.0 | ISO/IEC 13818-1 annex A DSM-CC.
         ES_ITU_T_REC_H_222_1                                 = 0x09, //!< ITU-T Rec. H.222.1.
         ES_ISO_IEC_13818_6_TYPE_A                            = 0x0A, //!< ISO/IEC 13818-6 type A.
         ES_ISO_IEC_13818_6_TYPE_B                            = 0x0B, //!< ISO/IEC 13818-6 type B.
         ES_ISO_IEC_13818_6_TYPE_C                            = 0x0C, //!< ISO/IEC 13818-6 type C.
         ES_ISO_IEC_13818_6_TYPE_D                            = 0x0D, //!< ISO/IEC 13818-6 type D.
         ES_ISO_IEC_13818_1_AUXILIARY                         = 0x0E, //!< ITU-T Rec. H.222.0 | ISO/IEC 13818-1 auxiliary.
         ES_ISO_IEC_13818_7_AUDIO_ADTS_TS_SYNTAX              = 0x0F, //!< ISO/IEC 13818-7 audio w/ ADTS transport syntax.
         ES_ISO_IEC_14496_2_VISUAL                            = 0x10, //!< ISO/IEC 14496-2 visual.
         ES_ISO_IEC_14496_3_AUDIO_LATM                        = 0x11, //!< ISO/IEC 14496-3 audio w/ LATM transport syntax as defined in ISO/IEC 14496-3 / AMD 1.
         ES_ISO_IEC_14496_1_STREAM_IN_PES_PACKETS             = 0x12, //!< ISO/IEC 14496-1 SL-packetized stream or FlexMux stream carried in PES packets.
         ES_ISO_IEC_14496_1_STREAM_IN_ISO_IEC_144496_SECTIONS = 0x13, //!< ISO/IEC 14496-1 SL-packetized stream or FlexMux stream carried in ISO/IEC 14496 sections.
         ES_ISO_IEC_13818_6_SYNCHRONIZED_DOWNLOAD_PROTOCOL    = 0x14, //!< ISO/IEC 13818-6 synchronized download protocol.
      };

      /*!
       * \brief Constructor.
       * \param program_number Program to which the PID is applicable
       * \param PCR_PID PID of the transport stream packets.
       * \param version_number
       * \param current_next_indicator `true`: version curently applicable, `false`: next applicable.
       */
      PMT(ui16 program_number, ui16 PCR_PID, ui8 version_number, bool current_next_indicator = true)
         : ExtPSITable(1, TID, program_number, 9, MAX_SEC_LEN, version_number, current_next_indicator, D_BIT),
           program_info_length(0),
           pcr_pid(PCR_PID),
           es_list(items[0])
      { }

      /*!
       * \brief Add a Descriptor to the Program Descriptors loop.
       * \param desc Descriptor to add.
       */
      bool addProgramDesc(Descriptor& desc);
      /*!
       * \brief Add an elementary stream to table.
       * \param type Stream type. See PMT::esTypes.
       * \param elem_pid PID to carry the stream packets.
       */
      bool addElemStream(ui8 type, ui16 elem_pid);
      /*!
       * \brief Add a Descriptor to the last added elementary stream.
       * \param desc Descriptor to add.
       */
      bool addElemStreamDesc(Descriptor& desc) { return addItemDesc(es_list, desc); }
      /*!
       * \brief Add a Descriptor to the elementary stream specified.
       * \param elem_pid PID identifying the lementary stream.
       * \param desc Descriptor to add.
       */
      bool addElemStreamDesc(ui16 elem_pid, Descriptor& desc) { return addItemDesc(es_list, elem_pid, desc); }

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream &) const;
#endif

   private:
      enum { D_BIT = 0,
             TID = 0x02,
             MAX_SEC_LEN = 1024 };

      // the stream holder struct - private to the pmt
      struct ElementaryStream : public ExtPSITable::ListItem {
         enum { BASE_LEN = 5 };

         ui16 elementary_pid : 13;
         ui8 type;

         // constructor
         ElementaryStream(ui16 epid, ui8 t) :
            elementary_pid(epid), type(t)
         { }
         ElementaryStream() = delete;

         virtual ui16 length() const { return 5; }
         virtual bool equals(ui16 pid) const { return pid == elementary_pid; }

         // writes item header bytes, returns num bytes written
         virtual ui8 write_header(Section& sec) const;
      };

      // instance variables
      ui16 program_info_length;
      ui16 pcr_pid : 13;
      DescList prog_desc;
      std::list<ListItem*>& es_list;

      enum State_t { INIT, WRITE_HEAD, GET_PROG_DESC, WRITE_PROG_DESC,
                     GET_XPORT_STREAM, WRITE_XPORT_STREAM };
      mutable struct Context {
         Context() : d_done(false), op_state(INIT), pd(nullptr), es(nullptr) {}

         bool d_done;
         State_t op_state;
         const Descriptor* pd;
         const ListItem* es;
         std::list<std::unique_ptr<Descriptor> >::const_iterator pd_iter;
         std::list<ListItem*>::const_iterator es_iter;
      } run;

   protected:
      virtual bool writeSection(Section&, ui8, ui16 &) const;
   };
   //! @}
   //! @}
}


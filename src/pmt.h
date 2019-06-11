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
// pmt.h: class definition for the PMT
// -----------------------------------

#pragma once

#include <memory>
#include <list>
#include "table.h"
#include "util.h"

namespace sigen {

   class Descriptor;

   //
   // the pmt class
   //
   class PMT : public PSITable
   {
   private:
      enum { D_BIT = 0,
             TID = 0x02,
             BASE_LEN = 9, MAX_SEC_LEN = 1024 };

      // the stream holder struct - private to the pmt
      struct ElementaryStream {
         enum { BASE_LEN = 5 };

         ui16 es_info_length,
            elementary_pid : 13;
         ui8 type;
         DescListItem descriptors;

         // constructor
         ElementaryStream(ui16 epid, ui8 t) :
            es_info_length(0), elementary_pid(epid), type(t)
         { }
      };

      // instance variables
      ui16 program_info_length,
         pcr_pid : 13;
      DescListItem prog_desc;                                // program descriptors
      std::list<std::unique_ptr<ElementaryStream> > es_list; // the list of streams

   public:

      // Elementary stream types
      enum esTypes {
         ES_RESERVED                                          = 0x00,
         ES_ISO_IEC_11172_VIDEO                               = 0x01,
         ES_ISO_IEC_13818_2_VIDEO                             = 0x02,
         ES_ISO_IEC_11172_AUDIO                               = 0x03,
         ES_ISO_IEC_13818_3_AUDIO                             = 0x04,
         ES_ISO_IEC_13818_1_PRIVATE_SECTIONS                  = 0x05,
         ES_ISO_IEC_13818_1_PES_PRIVATE_DATA                  = 0x06,
         ES_ISO_IEC_13522_MHEG                                = 0x07,
         ES_ISO_IEC_13818_1_ANNEX_A_DSM_CC                    = 0x08,
         ES_ITU_T_REC_H_222_1                                 = 0x09,
         ES_ISO_IEC_13818_6_TYPE_A                            = 0x0A,
         ES_ISO_IEC_13818_6_TYPE_B                            = 0x0B,
         ES_ISO_IEC_13818_6_TYPE_C                            = 0x0C,
         ES_ISO_IEC_13818_6_TYPE_D                            = 0x0D,
         ES_ISO_IEC_13818_1_AUXILIARY                         = 0x0E,
         ES_ISO_IEC_13818_7_AUDIO_ADTS_TS_SYNTAX              = 0x0F,
         ES_ISO_IEC_14496_2_VISUAL                            = 0x10,
         ES_ISO_IEC_14496_3_AUDIO_LATM                        = 0x11,
         ES_ISO_IEC_14496_1_STREAM_IN_PES_PACKETS             = 0x12,
         ES_ISO_IEC_14496_1_STREAM_IN_ISO_IEC_144496_SECTIONS = 0x13,
         ES_ISO_IEC_13818_6_SYNCHRONIZED_DOWNLOAD_PROTOCOL    = 0x14,
      };

      // constructor
      PMT(ui16 prog_num, ui16 pcrpid, ui8 ver, bool cni = true,
          bool rsrvd = true) :
         PSITable(TID, prog_num, BASE_LEN, MAX_SEC_LEN, ver, cni, D_BIT, rsrvd),
         program_info_length(0),
         pcr_pid(pcrpid)
      { }
      PMT() = delete;

      // utility
      bool addProgramDesc(Descriptor &);
      bool addElemStream(ui8 type, ui16 elem_pid);
      bool addElemStreamDesc(ui16 elem_pid, Descriptor &);
      bool addElemStreamDesc(Descriptor &); // adds it to the last stream added

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream &) const;
#endif

   protected:
      bool addElemStreamDesc(ElementaryStream&, Descriptor &, ui16);
      virtual bool writeSection(Section&, ui8, ui16 &) const;
      bool writeStream(Section&, const ElementaryStream& , ui16 &) const;
   };

} // sigen namespace


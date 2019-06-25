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
// dvb_defs.h: general DVB spec definitions - provider for application
// use
// -----------------------------------

#pragma once

namespace sigen {

   // general dvb defs
   namespace Dvb
   {
      enum FecInner_t {
         UNDEF_FECI,
         CR_1_2_FECI,
         CR_2_3_FECI,
         CR_3_4_FECI,
         CR_5_6_FECI,
         CR_7_8_FECI,
         CR_8_9_FECI,
         CR_3_5_FECI,
         CR_4_5_FECI,
         CR_9_10_FECI,
         NO_FECI = 0xffff,
      };

      // service / event
      enum RunningStatus_t {
         UNDEF_RS        = 0x0,
         NOT_RUNNING_RS  = 0x01,
         FEW_SEC_RS      = 0x02,
         PAUSING_RS      = 0x03,
         RUNNING_RS      = 0x04,
         SERV_OFF_AIR_RS = 0x05,
      };

      // the service types
      enum ServiceType_t {
         RESERVED_ST            = 0x00,
         DIGITAL_TV_ST          = 0x01,
         DIGITAL_RADIO_ST       = 0x02,
         TELETEXT_ST            = 0x03,
         NVOD_REFERENCE_ST      = 0x04,
         NVOD_TIME_SHIFTED_ST   = 0x05,
         MOSAIC_ST              = 0x06,
         FM_RADIO_ST            = 0x07,
         DVB_SRM_ST             = 0x08,
         // 0x09 - reserved
         ADV_CODEC_DIGITAL_RADIO_ST = 0x0A,
         H264AVC_MOSAIC_ST      = 0x0B,
         DATA_BROADCAST_SERVICE = 0x0C,
         COMMON_INTERFACE_USAGE = 0x0D,
         RCS_MAP                = 0x0E,
         RCS_FLS                = 0x0F,
         DVB_MHP_SERVICE        = 0x10,
         MPG2_HD_DIGITALTV_ST   = 0x11,
         // 0x12-0x15 - reserved
         H264_AVC_SD_ST          = 0x16,
         H264_AVC_SD_NVOD_TS_ST  = 0x17,
         H264_AVC_SD_NVOD_REF_ST = 0x18,
         H264_AVC_HD_ST          = 0x19,
         H264_AVC_HD_NVOD_TS_ST  = 0x1A,
         H264_AVC_HD_NVOD_REF_ST = 0x1B,
      };

      // satellite-specific
      namespace Sat
      {
         enum Pol_t {
            LINEAR_HOR_POL,
            LINEAR_VER_POL,
            CIRCULAR_LEFT_POL,
            CIRCULAR_RIGHT_POL,
         };
         enum Mod_t {
            AUTO_MOD,
            QPSK_MOD,
            EIGHT_PSK_MOD,
            SIXTEEN_QAM_MOD,
         };
      }

      // cable-specific
      namespace Cable
      {
         enum FecOuter_t {
            UNDEF_FECO,
            NO_FECO,
            RS_FECO,
         };
         enum Mod_t {
            UNDEF_MOD,
            QAM_16,
            QAM_32,
            QAM_64,
            QAM_128,
            QAM_256,
         };
      }

      // TODO: add
      namespace Terrestrial
      {

      }
   }
}

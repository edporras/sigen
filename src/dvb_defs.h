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
   /*!
    * \brief ETSI-EN-300-468 definitions for DVB constants applicable
    * to several tables and descriptors.
    */
   namespace Dvb
   {
      /*!
       *  \enum  FecInner_t
       *
       *  \brief Inner FEC Scheme.
       */
      enum FecInner_t {
         UNDEF_FECI,                         //!< Undefined.
         CR_1_2_FECI,                        //!< 1/2 conv. CR
         CR_2_3_FECI,                        //!< 2/3 conv. CR
         CR_3_4_FECI,                        //!< 3/4 conv. CR
         CR_5_6_FECI,                        //!< 5/6 conv. CR
         CR_7_8_FECI,                        //!< 7/8 conv. CR
         CR_8_9_FECI,                        //!< 8/9 conv. CR
         CR_3_5_FECI,                        //!< 3/5 conv. CR
         CR_4_5_FECI,                        //!< 4/5 conv. CR
         CR_9_10_FECI,                       //!< 9/10 conv. CR
         NO_FECI = 0xffff,
      };

      /*!
       *  \enum  RunningStatus_t
       *
       *  \brief Running Status of a service or event.
       */
      enum RunningStatus_t {
         UNDEF_RS        = 0x0,  //!< undefined.
         NOT_RUNNING_RS  = 0x01, //!< not running.
         FEW_SEC_RS      = 0x02, //!< starts in a few seconds (e.g., for video recording)
         PAUSING_RS      = 0x03, //!< pausing.
         RUNNING_RS      = 0x04, //!< running.
         SERV_OFF_AIR_RS = 0x05, //!< service off-air.
      };

      /*!
       *  \enum  ServiceType_t
       *
       *  \brief Code describing a DVB service type.
       */
      enum ServiceType_t {
         RESERVED_ST            = 0x00, //!< Reserved, future use.
         DIGITAL_TV_ST          = 0x01, //!< Digital television.
         DIGITAL_RADIO_ST       = 0x02, //!< Digital radio sound.
         TELETEXT_ST            = 0x03, //!< Teletext.
         NVOD_REFERENCE_ST      = 0x04, //!< NVOD reference.
         NVOD_TIME_SHIFTED_ST   = 0x05, //!< NVOD time-shifted.
         MOSAIC_ST              = 0x06, //!< Mosaic.
         FM_RADIO_ST            = 0x07, //!< FM radio.
         DVB_SRM_ST             = 0x08, //!< DVB SRM.
         // 0x09 - reserved
         ADV_CODEC_DIGITAL_RADIO_ST = 0x0A, //!< Advanced coded digital radio.
         H264AVC_MOSAIC_ST      = 0x0B, //!< H.264/AVC mosaic.
         DATA_BROADCAST_SERVICE = 0x0C, //!< Data broadcast.
         COMMON_INTERFACE_USAGE = 0x0D, //!< CIU (reserved).
         RCS_MAP                = 0x0E, //!< RCS Map.
         RCS_FLS                = 0x0F, //!< RCS FLS.
         DVB_MHP_SERVICE        = 0x10, //!< DVB MHP.
         MPG2_HD_DIGITALTV_ST   = 0x11, //!< MPEG-2 HD digital TV.
         // 0x12-0x15 - reserved
         H264_AVC_SD_ST          = 0x16, //!< H.264/AVC SD digital TV.
         H264_AVC_SD_NVOD_TS_ST  = 0x17, //!< H.264/AVC SD NVOD time-shifted.
         H264_AVC_SD_NVOD_REF_ST = 0x18, //!< H.264/AVC SD NVOD reference.
         H264_AVC_HD_ST          = 0x19, //!< H.264/AVC HD digital TV.
         H264_AVC_HD_NVOD_TS_ST  = 0x1A, //!< H.264/AVC HD NVOD time-shifted.
         H264_AVC_HD_NVOD_REF_ST = 0x1B, //!< H.264/AVC HD NVOD reference.
      };

      /*!
       * \brief Definitions specific to satellite systems.
       */
      namespace Sat
      {
         /*!
          *  \enum  Pol_t
          *
          *  \brief Satellite modulation scheme values.
          */
         enum Pol_t {
            LINEAR_HOR_POL,                  //!< Linear-horizontal.
            LINEAR_VER_POL,                  //!< Linear-vertical.
            CIRCULAR_LEFT_POL,               //!< Circular-left.
            CIRCULAR_RIGHT_POL,              //!< Circular-right.
         };
         /*!
          *  \enum  Mod_t
          *
          *  \brief Satellite modulation scheme values.
          */
         enum Mod_t {
            AUTO_MOD,                        //!< Auto.
            QPSK_MOD,                        //!< QPSK.
            EIGHT_PSK_MOD,                   //!< 8PSK.
            SIXTEEN_QAM_MOD,                 //!< 16-QAM.
         };
      }

      /*!
       * \brief Definitions specific to cable systems.
       */
      namespace Cable
      {
         /*!
          *  \enum  FecOuter_t
          *
          *  \brief Cable Forward Error Correction scheme.
          */
         enum FecOuter_t {
            UNDEF_FECO,                      //!< Undefined.
            NO_FECO,                         //!< No coding.
            RS_FECO,                         //!< RS(204/188).
         };
         /*!
          *  \enum  Mod_t
          *
          *  \brief Cable modulation scheme values.
          */
         enum Mod_t {
            UNDEF_MOD,                        //!< Undefined.
            QAM_16,                           //!< 16-QAM.
            QAM_32,                           //!< 32-QAM.
            QAM_64,                           //!< 64-QAM.
            QAM_128,                          //!< 128-QAM.
            QAM_256,                          //!< 256-QAM.
         };
      }

      // TODO: add
      namespace Terrestrial
      {

      }
   }
}

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
         HALF_FECI,
         TWO_THIRDS_FECI,
         THREE_FOURTHS_FECI,
         FIVE_SIXTHS_FECI,
         SEVEN_EIGHTHS_FECI,
         NO_FECI = 0xffff,
      };
      // service / event
      enum RunningStatus_t {
         UNDEF_RS       = 0x0,
         NOT_RUNNING_RS = 0x01,
         FEW_SEC_RS     = 0x02,
         PAUSING_RS     = 0x03,
         RUNNING_RS     = 0x04,
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
         PAL_CODED_SIGNAL_ST    = 0x07,
         SECAM_CODED_SIGNAL_ST  = 0x08,
         DD2_MAC_ST             = 0x09,
         FM_RADIO_ST            = 0x0A,
         NTSC_CODED_SIGNAL_ST   = 0x0B,
         DATA_BROADCAST_SERVICE = 0x0C,
         COMMON_INTERFACE_USAGE = 0x0D,
         RCS_MAP                = 0x0E,
         RCS_FLS                = 0x0F,
         DVB_MHP_SERVICE        = 0x10,
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
            UNDEF_MOD,
            QPSK_MOD,
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
   }

} // namespace

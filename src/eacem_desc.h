// Copyright 1999-2020 Ed Porras
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
// eacem_desc.h
// -----------------------------------

#pragma once

#include <list>
#include <vector>
#include "descriptor.h"
#include "dvb_desc.h"

namespace sigen {
   /*! \addtogroup descriptor
    *  @{
    */

   namespace EACEM {
      /*!
       * \defgroup eacem_d EACEM Descriptors
       * \addtogroup eacem_d
       *  @{
       */

      /*!
       * \brief EACEM Stream Identifier descriptor
       */
      struct PrivateDataSpecifierDesc : public sigen::PrivateDataSpecifierDesc
      {
         enum { EACEM_PDSD = 0x00000028 };

         /*!
          * \brief Constructor.
          */
         PrivateDataSpecifierDesc() : sigen::PrivateDataSpecifierDesc(EACEM_PDSD) {}
      };

      /*!
       * \brief EACEM Stream Identifier descriptor
       */
      struct StreamIdentifierDesc : public PrimitiveDatatypeDesc<ui8>
      {
         enum { TAG = 0x86 };

         /*!
          * \brief Constructor.
          * \param version Identifies the version of the specification.
          */
         StreamIdentifierDesc(ui8 version = 1) : PrimitiveDatatypeDesc<ui8>(TAG, version) {}

#ifdef ENABLE_DUMP
         virtual void dump(std::ostream& o) const {
            dumpData(o, EACEM_STREAM_IDENT_D_S, VER_NUM_S);
         }
#endif
      };


      /*!
       * \brief Logical Channel Desc
       */
      class LogicalChannelDesc : public Descriptor
      {
      public:
         enum { TAG = 0x83 };

         /*!
          * \brief Constructor.
          */
         LogicalChannelDesc() : Descriptor(TAG) {}

         /*!
          * \brief Add a logical channel entry to the loop.
          * \param sid Unique id of the service containing the announcements indicated.
          * \param logical_channel_num indicates the broadcaster preference for ordering services.
          * \param visible_service_flag Indicates that the service is normally visible and selectable.
          */
         bool addLogicalChan(ui16 sid, ui16 logical_channel_num = 0, bool visible_service_flag = true);

         virtual void buildSections(Section &s) const;

#ifdef ENABLE_DUMP
         virtual void dump(std::ostream& o) const;
#endif

      private:
         struct LogicalChan {
            enum { BASE_LEN = 4 };

            LogicalChan(ui16 service_id, bool visible_serv_flag, ui16 LCN)
               : sid(service_id), visible_service_flag(visible_serv_flag), logical_channel_number(LCN)
            {}

            ui16 sid;
            ui16 visible_service_flag : 1,
                 logical_channel_number : 10;
         };

         std::list<LogicalChan> channel_list;
      };

      //! @}
   }

   //! @}
} // sigen namespace

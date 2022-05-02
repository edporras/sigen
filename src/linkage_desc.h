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
// linkage_desc.h: class definition for subclasses of LinkageDesc
// -----------------------------------

#pragma once

#include <string>
#include <vector>
#include "descriptor.h"

namespace sigen {

   /*! \addtogroup descriptor
    *  @{
    */

   /*!
    * \defgroup linkage_d Linkage Descriptors
    * \addtogroup linkage_d
    *  @{
    */

   /*!
    * \brief Linkage Descriptor.
    */
   class LinkageDesc : public Descriptor
   {
   public:
      enum { TAG = 0x4a };

      /*!
       * \enum  Linkage_t
       *
       * \brief Linkage types.
       */
      enum Linkage_t {
         RESERVED            = 0x00,         //!< Reserved.
         INFO_SERV           = 0x01,         //!< Information service.
         EPG_SERV            = 0x02,         //!< EPG service.
         CA_REPL_SERV        = 0x03,         //!< CA replacement service.
         COMPL_NETBOUQ_SI    = 0x04,         //!< TS containing complete network/bouquet SI.
         SERV_REPLACE_SERV   = 0x05,         //!< Service replacement service.
         DATA_BROADCAST_SERV = 0x06,         //!< Data broadcast service.
         RCS_MAP             = 0x07,         //!< RCS map.
         MOBILE_HAND_OVER    = 0x08,         //!< Mobile hand-over. See MobileHandoverLinkageDesc.
         SSUS                = 0x09,         //!< System software update service. See SSUDesc.
         TS_SSU_BAT_OR_NIT   = 0x0A,         //!< TS containing SSU BAT or NIT.
         IPMAC_NOTIF_SERVICE = 0x0B,         //!< IP/MAC notification service.
         TS_INT_BAT_OR_NIT   = 0x0C,         //!< TS containing INT BAT or NIT.
         EVENT               = 0x0D,         //!< Event.
      };

      /*!
       * \brief constructor for generic linkage.
       * \param xs_id Unique id of the transport stream.
       * \param onid Id of the originating network.
       * \param sid Unique id of the service.
       * \param linkage_type Type of linkage descriptor.
       *
       * \warning
       * This class is set up to pass the descriptor private data if
       * it's hand-built.  For cleaner code, specific linkage types
       * should be derived from this class to implement the
       * content. Already some exist:
       *    1. 0x08: Use MobileHandoverLinkageDesc.
       *    2. 0x09: Use SSULinkageDesc.
       *    3. 0x0A: Use SSUScanLinkageDescr.
       * \warning
       * These types use a protected constructor of the class. If an
       * instance of LinkageDesc is created with any of the above
       * codes, a `std::domain_error()` exception is thrown.
       */
      LinkageDesc(ui16 xs_id, ui16 onid, ui16 sid, ui8 linkage_type);
      LinkageDesc() = delete;

      /*!
       * \brief Specify hand-build private data bytes.
       * \param data Byte vector containing the private data.
       */
      bool setPrivateData(const std::vector<ui8>& data);

      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif
   protected:
      LinkageDesc(Linkage_t lnkg_type, ui16 xsid, ui16 onid, ui16 sid) :
         Descriptor(TAG, 7),
         xport_stream_id(xsid),
         original_network_id(onid),
         service_id(sid),
         linkage_type(lnkg_type)
      {}

   private:
      // data
      ui16 xport_stream_id;
      ui16 original_network_id;
      ui16 service_id;
      ui8 linkage_type;
      std::vector<ui8> private_data;
   };


   /*!
    * \brief Mobile Handover Linkage Descriptor.
    */
   class MobileHandoverLinkageDesc : public LinkageDesc
   {
   public:
      /*!
       * \enum  Handover_t
       *
       * \brief Hand-over type.
       */
      enum Handover_t {
         HO_RESERVED            = 0x00,      //!< Reserved.
         HO_IDENTICAL_SERVICE   = 0x01,      //!< Hand-over to an identical service in a neighboring country.
         HO_LOCAL_VARIATION     = 0x02,      //!< Hand-over to a local variation of the same service.
         HO_ASSOCIATED_SERVICE  = 0x03,      //!< Hand-over to an associated service.
      };

      enum Origin_t {
         NIT                    = 0x0,
         SDT                    = 0x1,
      };

      /*!
       * \brief Constructor for descriptors with SDT MobileHandoverLinkageDesc::Origin_t.
       * \param xs_id Unique id of the transport stream.
       * \param onid Id of the originating network.
       * \param sid Unique id of the service.
       * \param hot Hand-over type, as per MobileHandoverLinkageDesc::Handover_t.
       * \param net_id Id of terrestrial network that supports the service.
       */
      MobileHandoverLinkageDesc(ui16 xs_id, ui16 onid, ui16 sid, Handover_t hot, ui16 net_id)
         : MobileHandoverLinkageDesc(xs_id, onid, sid, hot, MobileHandoverLinkageDesc::SDT, net_id, 0)
      {}
      /*!
       * \brief Constructor for descriptors with NIT MobileHandoverLinkageDesc::Origin_t.
       * \param xs_id Unique id of the transport stream.
       * \param onid Id of the originating network.
       * \param sid Unique id of the service.
       * \param hot Hand-over type, as per MobileHandoverLinkageDesc::Handover_t.
       * \param net_id Id of terrestrial network that supports the service.
       * \param init_serv_id Id of service for which the hand-over linkage is valid.
       */
      MobileHandoverLinkageDesc(ui16 xs_id, ui16 onid, ui16 sid, Handover_t hot, ui16 net_id, ui16 init_serv_id)
         : MobileHandoverLinkageDesc(xs_id, onid, sid, hot, MobileHandoverLinkageDesc::NIT, net_id, init_serv_id)
      {}
      MobileHandoverLinkageDesc() = delete;

      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif

   private:
      // added as of 300 468 v1.4.1
      ui8  hand_over_type : 4;
      bool origin_type;
      ui16 network_id;
      ui16 initial_service_id;
      std::vector<ui8> private_data;

      MobileHandoverLinkageDesc(ui16 xs_id, ui16 onid, ui16 sid, Handover_t hot, Origin_t ot,
                                ui16 net_id, ui16 init_serv_id);
   };
   //! @}
   //! @}
} // namespace

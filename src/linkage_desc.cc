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
// linkage_desc.cc: class definition for subclasses of LinkageDesc
// -----------------------------------

#include <iostream>
#include <sstream>
#include <list>
#include <stdexcept>
#include "descriptor.h"
#include "tstream.h"
#include "linkage_desc.h"

namespace sigen {

   // ---------------------------------------
   // linkage descriptor
   //
   LinkageDesc::LinkageDesc(ui16 xsid, ui16 onid, ui16 sid, ui8 lnkg_type) :
      Descriptor(TAG, 7),
      xport_stream_id(xsid),
      original_network_id(onid),
      service_id(sid),
      linkage_type(lnkg_type)
   {
      switch (linkage_type)
      {
        case MOBILE_HAND_OVER:
        case TS_SSU_BAT_OR_NIT:
        case SSUS:
           {
              std::stringstream msg;
              msg << "Attempt to create LinkageDesc with type " << std::hex << linkage_type;
              throw std::domain_error(msg.str());
           }
           break;
        default:
           break;
      }
   }

   bool LinkageDesc::setPrivateData(const std::vector<ui8>& data)
   {
      // check if we can fit it
      if ( !incLength( data.size()) )
         return false;

      // yep, add it then
      private_data = data;
      return true;
   }


   //
   // write to the section
   //
   void LinkageDesc::buildSections(Section &s) const
   {
      Descriptor::buildSections(s);

      s.set16Bits(xport_stream_id);
      s.set16Bits(original_network_id);
      s.set16Bits(service_id);
      s.set08Bits(linkage_type);

      s.setBits( private_data );
   }


#ifdef ENABLE_DUMP
   //
   // stdout
   void LinkageDesc::dump(std::ostream &o) const
   {
      dumpHeader(o, LINKAGE_D_S);

      identStr(o, XPORT_STREAM_ID_S, xport_stream_id, true);
      identStr(o, ORIG_NETWORK_ID_S, original_network_id, true);
      identStr(o, SERVICE_ID_S, service_id, true);
      identStr(o, LINKAGE_TYPE_S, linkage_type);
      identStr(o, PVT_DATA_S, private_data);
   }
#endif


   //
   // MobileHandoverLinkageDesc private constructor
   MobileHandoverLinkageDesc::MobileHandoverLinkageDesc(ui16 xs_id, ui16 onid, ui16 sid, Handover_t hot, Origin_t ot,
                                                        ui16 net_id, ui16 init_serv_id)
      : LinkageDesc(MOBILE_HAND_OVER, xs_id, onid, sid),
      hand_over_type(hot),
      origin_type(ot),
      network_id(net_id),
      initial_service_id(init_serv_id)
   {
      incLength(3);

      if (hand_over_type != MobileHandoverLinkageDesc::HO_RESERVED)
         incLength( sizeof(network_id) );

      if (origin_type == MobileHandoverLinkageDesc::NIT)
         incLength( sizeof(initial_service_id) );
   }

   //
   // write to the section
   //
   void MobileHandoverLinkageDesc::buildSections(Section &s) const
   {
      LinkageDesc::buildSections(s);

      s.set08Bits( hand_over_type << 4 |
                   rbits(0x7) << 1 |
                   origin_type );

      if (hand_over_type != MobileHandoverLinkageDesc::HO_RESERVED)
         s.set16Bits( network_id );

      if (origin_type == MobileHandoverLinkageDesc::NIT)
         s.set16Bits( initial_service_id );

      s.setBits( private_data );
   }


#ifdef ENABLE_DUMP
   //
   // stdout
   void MobileHandoverLinkageDesc::dump(std::ostream &o) const
   {
      LinkageDesc::dump(o);

      identStr( o, HAND_OVER_TYPE_S, hand_over_type );
      identStr( o, RESERVED_FU_S, rbits(0x7) );
      identStr( o, ORIGIN_TYPE_S, origin_type );

      if (hand_over_type != MobileHandoverLinkageDesc::HO_RESERVED)
         identStr( o, NETWORK_ID_S, network_id, true );

      if (origin_type == MobileHandoverLinkageDesc::NIT)
         identStr( o, INITIAL_SERV_ID_S, initial_service_id, true );

      identStr(o, PVT_DATA_S, private_data);
   }
#endif

} // namespace sigen

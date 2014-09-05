//
// linkage_desc.cc: class definition for subclasses of LinkageDesc
// -----------------------------------

#include <iostream>
#include <list>
#include "descriptor.h"
#include "tstream.h"
#include "linkage_desc.h"

namespace sigen {

   // ---------------------------------------
   // linkage descriptor
   //
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
   // MobileHandoverLinkageDesc constructor
   MobileHandoverLinkageDesc:: MobileHandoverLinkageDesc(ui16 xsid, ui16 onid, ui16 sid,
                                                         Handover_t hot, Origin_t ot, ui16 nid, ui16 isid, bool rsrv) :
      LinkageDesc(xsid, onid, sid, MOBILE_HAND_OVER),
      hand_over_type(hot),
      origin_type(ot),
      reserved(rsrv),
      network_id(nid),
      initial_service_id(isid)
   {
      incLength(MobileHandoverLinkageDesc::BASE_LEN);

      if (hand_over_type != MobileHandoverLinkageDesc::HO_RESERVED)
      {
         incLength( sizeof(network_id) );
      }

      if (origin_type == MobileHandoverLinkageDesc::NIT)
      {
         incLength( sizeof(initial_service_id) );
      }
   }

      //
      // write to the section
      //
      void MobileHandoverLinkageDesc::buildSections(Section &s) const
      {
         LinkageDesc::buildSections(s);

         s.set08Bits( hand_over_type << 4 |
                      rbits(reserved, 0x7) << 1 |
                      origin_type );

         if (hand_over_type != MobileHandoverLinkageDesc::HO_RESERVED) {
            s.set16Bits( network_id );
         }

         if (origin_type == MobileHandoverLinkageDesc::NIT) {
            s.set16Bits( initial_service_id );
         }

         s.setBits( private_data );
      }


#ifdef ENABLE_DUMP
      //
      // stdout
      void MobileHandoverLinkageDesc::dump(std::ostream &o) const
      {
         LinkageDesc::dump(o);

         identStr( o, HAND_OVER_TYPE_S, hand_over_type );
         identStr( o, RESERVED_FU_S, rbits(reserved, 0x7) );
         identStr( o, ORIGIN_TYPE_S, origin_type );

         if (hand_over_type != MobileHandoverLinkageDesc::HO_RESERVED) {
            identStr( o, NETWORK_ID_S, network_id, true );
         }

         if (origin_type == MobileHandoverLinkageDesc::NIT) {
            identStr( o, INITIAL_SERV_ID_S, initial_service_id, true );
         }

         identStr(o, PVT_DATA_S, private_data);
      }
#endif

} // namespace sigen
//
// linkage_desc.h: class definition for subclasses of LinkageDesc
// -----------------------------------

#ifndef SIGEN_LINKAGE_DESC_H
#define SIGEN_LINKAGE_DESC_H

#include <string>
#include <list>
#include <sigen/descriptor.h>

namespace sigen {

  // ---------------------------
  // Linkage Descriptor
  //
  class LinkageDesc : public Descriptor
  {
  private:
     // data
     ui16 xport_stream_id,
         original_network_id,
         service_id;
     ui8 linkage_type;
     std::vector<ui8> private_data;

  public:
     enum { TAG = 0x4a, BASE_LEN = 7 };

     // linkage types
     enum Linkage_t {
       RESERVED            = 0x00,
       INFO_SERV           = 0x01,
       EPG_SERV            = 0x02,
       CA_REPL_SERV        = 0x03,
       COMPL_NETBOUQ_SI    = 0x04,
       SERV_REPLACE_SERV   = 0x05,
       DATA_BROADCAST_SERV = 0x06,
       RCS_MAP             = 0x07,
       MOBILE_HAND_OVER    = 0x08,
       SSUS                = 0x09,
       TS_SSU_BAT_OR_NIT   = 0x0A,
       IPMAC_NOTIF_SERVICE = 0x0B,
       TS_INT_BAT_OR_NIT   = 0x0C,
     };

     // constructor for generic linkage
     //  - do not use this for linkage types:
     //    1. 0x08: Use the MobileHandoverLinkageDesc class
     //    2. 0x0A: Use the SSUScanLinkageDescr class
     LinkageDesc(ui16 xsid, ui16 onid, ui16 sid, ui8 lnkg_type) :
       Descriptor(TAG, BASE_LEN),
       xport_stream_id(xsid),
       original_network_id(onid),
       service_id(sid),
       linkage_type(lnkg_type)
     { }

     // utility
     virtual Descriptor *clone() const { return new LinkageDesc(*this); }
     virtual void buildSections(Section&) const;

     bool setPrivateData(const std::vector<ui8>& data);

#ifdef ENABLE_DUMP
     virtual void dump(std::ostream&) const;
#endif
  };


  // ---------------------------
  // Mobile HandOver Linkage Descriptor
  //
  class MobileHandoverLinkageDesc : public LinkageDesc
  {
  private:
     // added as of 300 468 v1.4.1
     ui8 hand_over_type : 4;
     bool origin_type,
          reserved;
     ui16 network_id,
          initial_service_id;

     std::vector<ui8> private_data;

  public:
     enum { BASE_LEN = 3 };

     enum Handover_t {
       HO_RESERVED            = 0x00,
       HO_IDENTICAL_SERVICE   = 0x01,
       HO_LOCAL_VARIATION     = 0x02,
       HO_ASSOCIATED_SERVICE  = 0x03,
     };

     enum Origin_t {
       NIT                 = 0x0,
       SDT                 = 0x1
     };

     // constructor for linkage types other than MOBILE_HAND_OVER (0x08)
     MobileHandoverLinkageDesc(ui16 xsid, ui16 onid, ui16 sid,
                               Handover_t hot, Origin_t ot, ui16 nid, ui16 isid,
                               bool rsrv = true);

     // utility
     virtual Descriptor *clone() const { return new MobileHandoverLinkageDesc(*this); }
     virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
     virtual void dump(std::ostream&) const;
#endif

  };

} // namespace

#endif

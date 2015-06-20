//
// nit.h: class definition for the NIT
// -----------------------------------

#ifndef SIGEN_NIT_H
#define SIGEN_NIT_H

#include <list>
#include <sigen/table.h>

namespace sigen {

   class Descriptor;

   //
   // the NIT
   //
   class NIT : public PSITable
   {
 private:
      enum { BASE_LEN = 9, MAX_SEC_LEN = 1024 };

      // the transport stream struct
      struct XportStream
      {
         enum { BASE_LEN = 6 };

         ui16 id,                    // transport stream id
            original_network_id,
            desc_length;
         std::list<PtrWrapper<Descriptor> > desc_list;  // descriptors

         // constructor
         XportStream(ui16 tsid, ui16 onid) :
            id(tsid), original_network_id(onid), desc_length(0) { }

         int length() const { return desc_length + BASE_LEN; }

         XportStream *clone() const { return new XportStream(*this); }
      };

      // NIT members
      ui16 network_desc_length,
         xport_stream_loop_length;

      std::list<PtrWrapper<Descriptor> > network_desc;   // network descriptors
      std::list<PtrWrapper<XportStream> > xport_streams; // transport streams

   public:
      enum Type { ACTUAL = 0x40, OTHER = 0x41 };
      enum { PID = 0x10 };

      // constructor - type refers to ACTUAL or OTHER, reserved is the
      // state of reserved bits
      NIT(ui16 network_id, NIT::Type type, ui8 ver, bool cni = true,
          bool rsrvd = true) :
       PSITable((ui8) type, network_id, BASE_LEN, MAX_SEC_LEN, ver, cni, rsrvd, rsrvd),
          network_desc_length(0),
          xport_stream_loop_length(0)
      { }

       // utility
       bool addNetworkDesc(Descriptor &);
       bool addXportStream(ui16 xs_id, ui16 on_id);
       bool addXportStreamDesc(ui16 xs_id, ui16 on_id, Descriptor &);
       bool addXportStreamDesc(Descriptor &); // adds it to the last stream added

       // dump to stdout routines
#ifdef ENABLE_DUMP
       virtual void dump(std::ostream &) const;
#endif

   protected:
       bool addXportStreamDesc(XportStream& , Descriptor &, ui16);
       virtual bool writeSection(Section& , ui8, ui16 &) const;
       bool writeXportStream(Section& , const XportStream& , ui16 &, ui16 &) const;

#ifdef ENABLE_DUMP
       void dumpXportStreams(std::ostream &) const;
#endif
   };


   struct NITActual : public NIT
   {
      NITActual(ui16 network_id, ui8 ver, bool cni = true, bool rsrvd = true) :
         NIT(network_id, NIT::ACTUAL, ver, cni, rsrvd)
      { }
   };

   struct NITOther : public NIT
   {
      NITOther(ui16 network_id, ui8 ver, bool cni = true, bool rsrvd = true) :
         NIT(network_id, NIT::OTHER, ver, cni, rsrvd)
      { }
   };

} // sigen namespace

#endif

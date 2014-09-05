//
// sdt.h: class definition for the SDT
// -----------------------------------

#ifndef SIGEN_SDT_H
#define SIGEN_SDT_H

#include <list>
#include <sigen/table.h>

namespace sigen {

   class Descriptor;

   //
   // the sdt class
   //
   class SDT : public PSITable
   {
   private:
      enum { BASE_LEN = 8, MAX_SEC_LEN = 1024 };

      // the service holder class - private to the sdt
      struct Service {
         enum { BASE_LEN = 5 };

         ui16 id,
            desc_loop_length;
         ui8 running_status : 3;
         bool eit_schedule,
            eit_present_following,
            free_ca_mode,
            reserved;

         std::list<PtrWrapper<Descriptor> > desc_list; // descriptor's list

         // constructor
         Service(ui16 sid, bool esf, bool epff, ui8 rs, bool fca, bool rsrvd) :
           id(sid), desc_loop_length(0), running_status(rs),
         eit_schedule(esf), eit_present_following(epff),
         free_ca_mode(fca), reserved(rsrvd) {}

         // utility
         void buildSections(Section &) const;

         Service *clone() const { return new Service(*this); }
      };

      // sdt data members begin here
      ui16 original_network_id;

      // the list of services
      std::list<PtrWrapper<Service> > service_list;

   public:
      enum Type { ACTUAL = 0x42, OTHER = 0x46 };
      enum { PID = 0x11 };

      // constructor
      SDT(ui16 xport_str_id, ui16 orig_network_id, SDT::Type type, ui8 ver,
          bool cni = true, bool rsrvd = true) :
        PSITable((ui8) type, xport_str_id, BASE_LEN, MAX_SEC_LEN, ver, cni, rsrvd, rsrvd),
        original_network_id(orig_network_id)
      { }

      // utility
      bool addService(ui16 sid, bool esf, bool epff, ui8 rs, bool fca);
      bool addServiceDesc(ui16 sid, Descriptor &d);
      bool addServiceDesc(Descriptor &d); // adds it to the last service added

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream &) const;
#endif

   protected:
      bool addServiceDesc(Service&, Descriptor &, ui16);
      virtual bool writeSection(Section&, ui8, ui16 &) const;
      bool writeService(Section&, const Service&, ui16 &) const;
   };


   struct SDTActual : public SDT {
      SDTActual(ui16 xport_str_id, ui16 orig_network_id, ui8 ver, bool cni = true, bool rsrvd = true) :
        SDT(xport_str_id, orig_network_id, SDT::ACTUAL, ver, cni, rsrvd) { }
   };

   struct SDTOther : public SDT {
      SDTOther(ui16 xport_str_id, ui16 orig_network_id, ui8 ver, bool cni = true, bool rsrvd = true) :
        SDT(xport_str_id, orig_network_id, SDT::OTHER, ver, cni, rsrvd) { }
   };

} // sigen namespace

#endif
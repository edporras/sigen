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
// sdt.h: class definition for the SDT
// -----------------------------------

#pragma once

#include <memory>
#include <list>
#include "table.h"

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

         ui16 id;
         ui8 running_status : 3;
         bool eit_schedule,
            eit_present_following,
            free_ca_mode,
            reserved;
         DescList descriptors;

         // constructor
         Service(ui16 sid, bool esf, bool epff, ui8 rs, bool fca, bool rsrvd) :
            id(sid), running_status(rs),
            eit_schedule(esf), eit_present_following(epff),
            free_ca_mode(fca), reserved(rsrvd) {}

         // utility
         void buildSections(Section &) const;
      };

      // sdt data members begin here
      ui16 original_network_id;

      // the list of services
      std::list<std::unique_ptr<Service> > service_list;

   public:
      enum Type { ACTUAL = 0x42, OTHER = 0x46 };
      enum { PID = 0x11 };

      // constructor
      SDT(ui16 xport_str_id, ui16 orig_network_id, SDT::Type type, ui8 ver,
          bool cni = true, bool rsrvd = true) :
         PSITable((ui8) type, xport_str_id, BASE_LEN, MAX_SEC_LEN, ver, cni, rsrvd, rsrvd),
         original_network_id(orig_network_id)
      { }
      SDT() = delete;

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

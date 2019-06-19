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
   public:
      enum { PID = 0x11 };
      enum { ACTUAL_TID = 0x42, OTHER_TID = 0x46 };

      bool addService(ui16 sid, bool esf, bool epff, ui8 rs, bool fca); // add a service
      bool addServiceDesc(ui16 sid, Descriptor &d);                     // add a descriptor to the service matching the sid
      bool addServiceDesc(Descriptor &d);                               // adds it to the last service added

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream &) const;
#endif

   private:
      enum { MAX_SEC_LEN = 1024 };

      // the service holder class - private to the sdt
      struct Service : public PSITable::ListItem {
         enum { BASE_LEN = 5 };

         ui16 id;
         ui8 running_status : 3;
         bool eit_schedule;
         bool eit_present_following;
         bool free_ca_mode;

         // constructor
         Service(ui16 sid, bool esf, bool epff, ui8 rs, bool fca) :
            id(sid), running_status(rs),
            eit_schedule(esf), eit_present_following(epff),
            free_ca_mode(fca) {}
         Service() = delete;

         // writes item header bytes, returns num bytes written
         virtual ui8 write_header(Section& sec) const;
         // writes the 2-byte desc loop len
         virtual void write_desc_loop_len(Section& sec, ui8* pos, ui16 len) const;
      };

      // sdt data members begin here
      ui16 original_network_id;

      // the list of services
      std::list<Service> service_list;

      enum State_t { INIT, WRITE_HEAD, GET_SERVICE, WRITE_SERVICE };
      mutable struct Context {
         Context() : op_state(INIT), serv(nullptr) {}
         
         State_t op_state;
         const Service *serv;
         std::list<Service>::const_iterator s_iter;
      } run;

   protected:
      // constructor
      SDT(ui8 type, ui16 xport_str_id, ui16 orig_network_id, ui8 ver,
          bool cni = true) :
         PSITable(type, xport_str_id, 8, MAX_SEC_LEN, ver, cni),
         original_network_id(orig_network_id)
      { }

      bool addServiceDesc(Service&, Descriptor &);
      virtual bool writeSection(Section&, ui8, ui16 &) const;
   };

   // public interface
   struct SDTActual : public SDT {
      SDTActual(ui16 xport_str_id, ui16 orig_network_id, ui8 ver, bool cni = true) :
         SDT(SDT::ACTUAL_TID, xport_str_id, orig_network_id, ver, cni) { }
   };

   struct SDTOther : public SDT {
      SDTOther(ui16 xport_str_id, ui16 orig_network_id, ui8 ver, bool cni = true) :
         SDT(SDT::OTHER_TID, xport_str_id, orig_network_id, ver, cni) { }
   };

} // sigen namespace

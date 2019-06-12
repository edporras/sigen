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
// eit.h: class definition for the EIT
// -----------------------------------

#pragma once

#include <memory>
#include <list>
#include "table.h"
#include "utc.h"
#include "util.h"

namespace sigen {

   class Descriptor;

   //
   // the eit class
   //
   class EIT : public PSITable
   {
   public:
      enum { PID = 0x12 };

      // utility
      virtual void buildSections(TStream &) const = 0;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream &) const;
#endif

   protected:
      enum { BASE_LEN = 11, MAX_SEC_LEN = 4096 };

      // only derived classes can build this type
      EIT(ui8 tid, ui16 sid, ui16 xsid, ui16 onid, ui8 ver, bool cni) :
         PSITable(tid, sid, BASE_LEN, MAX_SEC_LEN, ver, cni),
         xport_stream_id(xsid),
         original_network_id(onid)
      { }

      // the private event class
      struct Event {
         enum { BASE_LEN = 12 };

         // instance variables
         ui16 id;
         UTC utc;
         BCDTime duration;
         ui8 running_status : 3;
         bool free_CA_mode;
         DescList descriptors;

         // constructor
         Event(ui16 evid, const UTC& time, const BCDTime& dur, ui8 rs,
               bool fcam) :
            id(evid),
            utc(time),
            duration(dur),
            running_status(rs),
            free_CA_mode(fcam)
         { }

         // utility
         void buildSections(Section &) const;
      };

      // common EIT data members begin here
      ui16 xport_stream_id;
      ui16 original_network_id;

      // event/descriptor add routines
      bool addEvent(std::list<std::unique_ptr<Event> > &, ui16, UTC, BCDTime, ui8,
                    bool);
      bool addEventDesc(std::list<std::unique_ptr<Event> > &, ui16, Descriptor &);
      bool addEventDesc(std::list<std::unique_ptr<Event> > &, Descriptor &);
      bool addEventDesc(Event& , Descriptor &);

      // table builder routines
      bool writeSection(Section& s, const std::list<std::unique_ptr<Event> > &e_list,
                        ui8 last_tid,
                        ui8 cur_sec, ui8 last_sec_num, ui8 segm_last_sec_num,
                        ui16 &sec_bytes) const;
      bool writeEvent(Section& , const Event& , ui16 &) const;

#ifdef ENABLE_DUMP
      virtual void dumpHeader(std::ostream &) const = 0;
      virtual void dumpEvents(std::ostream &) const = 0;
      void dumpEventList(std::ostream &, const std::list<std::unique_ptr<Event> > &) const;
#endif

      // dummy function - we use a different writeSection for EIT's,
      // but we need this one to satisfy inheritance from PSITable..
      // this one is never called
      bool writeSection(Section& , ui8, ui16 &) const { return false; }
   };


   // ========================================================================
   //
   // the derived present/following eit classes
   class PF_EIT : public EIT
   {
   public:
      enum Type { ACTUAL = 0x4e, OTHER = 0x4f };

   private:
      enum { PRESENT, FOLLOWING };
      std::list<std::unique_ptr<Event> > event_list[2]; // present = 0, following = 1

   public:
      PF_EIT() = delete;

      // present event utility functions
      bool addPresentEvent(ui16 evid, UTC time, BCDTime dur, ui8 rs, bool fca) {
         return addEvent(event_list[PRESENT], evid, time, dur, rs, fca);
      }
      bool addPresentEventDesc(ui16 evid, Descriptor &d) {
         return addEventDesc(event_list[PRESENT], evid, d);
      }
      // adds it to the last event added
      bool addPresentEventDesc(Descriptor &d) {
         return addEventDesc(event_list[PRESENT], d);
      }
      // following event utility functions
      bool addFollowingEvent(ui16 evid, UTC time, BCDTime dur, ui8 rs, bool fca) {
         return addEvent(event_list[FOLLOWING], evid, time, dur, rs, fca);
      }
      bool addFollowingEventDesc(ui16 evid, Descriptor &d) {
         return addEventDesc(event_list[FOLLOWING], evid, d);
      }
      // adds it to the last event added
      bool addFollowingEventDesc(Descriptor &d) {
         return addEventDesc(event_list[FOLLOWING], d);
      }

      // top-level table builder
      void buildSections(TStream &) const;

   protected:
      // protected constructor
      PF_EIT(ui16 sid, ui16 xsid, ui16 onid, PF_EIT::Type type, ui8 ver,
             bool cni = true) :
         EIT(type, sid, xsid, onid, ver, cni)
      { }

#ifdef ENABLE_DUMP
      void dumpHeader(std::ostream &) const;
      void dumpEvents(std::ostream &) const;
#endif
   };

   // public interface classes
   struct PF_EITActual : public PF_EIT
   {
      PF_EITActual(ui16 sid, ui16 xsid, ui16 onid, ui8 ver, bool cni = true) :
         PF_EIT(sid, xsid, onid, PF_EIT::ACTUAL, ver, cni) { }
   };

   struct PF_EITOther : public PF_EIT
   {
      PF_EITOther(ui16 sid, ui16 xsid, ui16 onid, ui8 ver, bool cni = true) :
         PF_EIT(sid, xsid, onid, PF_EIT::OTHER, ver, cni) { }
   };


#if 0
   // ========================================================================
   //
   // event schedule EIT's

   // NOT YET IMPLEMENTED
   class ES_EIT : public EIT
   {
   private:
      enum { ACTUAL_BASE = 0x50, OTHER_BASE_0x60 };

      std::list<std::unique_ptr<Event> > event_list;

//   ES_EIT(ui16 sid, ui16 xsid, ui16 onid, ui8 ver, bool cni = true) :
//       EIT(TID, sid, xsid, onid, ver, cni) { }
   public:

#ifdef ENABLE_DUMP
      void dumpHeader(std::ostream &o) const;
#endif
   };
#endif // if 0

} // sigen namespace

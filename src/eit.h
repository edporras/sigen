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

namespace sigen {

   class Descriptor;

   /*! \addtogroup abstract
    *  @{
    */

   /*!
    * \brief Abstract Event Information %Table base class.
    */
   class EIT : public PSITable
   {
   public:
      enum {
         PID = 0x12                                    //!< Packet PID for transmission.
      };

      // utility
      virtual void buildSections(TStream &) const = 0;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream &) const;
#endif

   protected:
      enum { MAX_SEC_LEN = 4096 };

      // only derived classes can build this type
      EIT(ui8 tid, ui16 sid, ui16 xsid, ui16 onid, ui8 ver, bool cni) :
         PSITable(tid, sid, 11, MAX_SEC_LEN, ver, cni),
         xport_stream_id(xsid),
         original_network_id(onid)
      { }

      // the private event class
      struct Event : public PSITable::ListItem {
         enum { BASE_LEN = 12 };

         // instance variables
         ui16 id;
         UTC utc;
         BCDTime duration;
         ui8 running_status : 3;
         bool free_CA_mode;

         // constructor
         Event(ui16 evid, const UTC& time, const BCDTime& dur, ui8 rs,
               bool fcam) :
            id(evid),
            utc(time),
            duration(dur),
            running_status(rs),
            free_CA_mode(fcam)
         { }
         Event() = delete;

         bool equals(ui16 ev_id) const { return ev_id == id; }

         // writes item header bytes, returns num bytes written
         virtual ui8 write_header(Section& sec) const;
         // writes the 2-byte desc loop len
         virtual void write_desc_loop_len(Section& sec, ui8* pos, ui16 len) const;
      };

      // common EIT data members begin here
      ui16 xport_stream_id;
      ui16 original_network_id;

      // event/descriptor add routines
      bool addEvent(std::list<Event> &l, ui16 id, const UTC& st, const BCDTime& d, ui8 rs, bool fca);
      bool addEventDesc(std::list<Event> &, ui16, Descriptor &);
      bool addEventDesc(std::list<Event> &, Descriptor &);
      bool addEventDesc(Event& , Descriptor &);

      // table builder routines
      bool writeSection(Section& s, const std::list<Event> &e_list,
                        ui8 last_tid,
                        ui8 cur_sec, ui8 last_sec_num, ui8 segm_last_sec_num,
                        ui16 &sec_bytes) const;

#ifdef ENABLE_DUMP
      virtual void dumpHeader(std::ostream &) const = 0;
      virtual void dumpEvents(std::ostream &) const = 0;
      void dumpEventList(std::ostream &, const std::list<Event> &) const;
#endif

      // dummy function - we use a different writeSection for EIT's,
      // but we need this one to satisfy inheritance from PSITable..
      // this one is never called
      bool writeSection(Section& , ui8, ui16 &) const { return false; }

   private:
      enum State_t { INIT, WRITE_HEAD, GET_EVENT, WRITE_EVENT };
      mutable struct Context {
         Context() : op_state(INIT), event(nullptr) {}

         State_t op_state;
         const Event *event;
         std::list<Event>::const_iterator ev_iter;
      } run;
   };

   /*!
    * \brief Abstract base class for EIT-Present / -Following.
    */
   class PF_EIT : public EIT
   {
   public:
      enum Type { ACTUAL = 0x4e, OTHER = 0x4f };

   private:
      enum { PRESENT, FOLLOWING };
      std::list<Event> event_list[2]; // present = 0, following = 1

   public:
      /*!
       * \brief Add a present event.
       * \param ev_id Unique id of the event within the service.
       * \param start_time Start time of the event.
       * \param duration Duration of the event.
       * \param running_status Running status of the event. See sigen::Dvb::RunningStatus_t.
       * \param free_CA_mode `false`: no event components are scrambled; `true`: one ore more controlled by CA s
       */
      bool addPresentEvent(ui16 ev_id, const UTC& start_time, const BCDTime& duration,
                           ui8 running_status, bool free_CA_mode) {
         return addEvent(event_list[PRESENT], ev_id, start_time, duration, running_status, free_CA_mode);
      }
      /*!
       * \brief Add a Descriptor to the last added present event.
       * \param desc Descriptor to add.
       */
      bool addPresentEventDesc(Descriptor& desc) {
         return addEventDesc(event_list[PRESENT], desc);
      }
      /*!
       * \brief Add a Descriptor to the specified present event.
       * \param ev_id Id identifying the event.
       * \param desc Descriptor to add.
       */
      bool addPresentEventDesc(ui16 ev_id, Descriptor& desc) {
         return addEventDesc(event_list[PRESENT], ev_id, desc);
      }
      /*!
       * \brief Add a following event.
       * \param ev_id Unique id of the event within the service.
       * \param start_time Start time of the event.
       * \param duration Duration of the event.
       * \param running_status Running status of the event. See sigen::Dvb::RunningStatus_t.
       * \param free_CA_mode `false`: no event components are scrambled; `true`: one ore more controlled by CA s
       */
      bool addFollowingEvent(ui16 ev_id, const UTC& start_time, const BCDTime& duration,
                             ui8 running_status, bool free_CA_mode) {
         return addEvent(event_list[FOLLOWING], ev_id, start_time, duration, running_status, free_CA_mode);
      }
      /*!
       * \brief Add a Descriptor to the last added following event.
       * \param desc Descriptor to add.
       */
      bool addFollowingEventDesc(Descriptor& desc) {
         return addEventDesc(event_list[FOLLOWING], desc);
      }
      /*!
       * \brief Add a Descriptor to the specified following event.
       * \param ev_id Id identifying the event.
       * \param desc Descriptor to add.
       */
      bool addFollowingEventDesc(ui16 ev_id, Descriptor& desc) {
         return addEventDesc(event_list[FOLLOWING], ev_id, desc);
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
   //! @}

   /*! \addtogroup table
    *  @{
    */

   /*!
    * \brief Event Information %Table, Present / Following - Actual, as per ETSI EN 300 468.
    */
   struct PF_EITActual : public PF_EIT
   {
      /*!
       * \brief Constructor.
       * \param sid Id to identify the service.
       * \param xs_id Id to identify the transport stream.
       * \param on_id Id to identify the bouquet.
       * \param version_number Version number to use the subtable.
       * \param current_next_indicator `true`: version curently applicable, `false`: next applicable.
       */
      PF_EITActual(ui16 sid, ui16 xs_id, ui16 on_id, ui8 version_number, bool current_next_indicator = true)
         : PF_EIT(sid, xs_id, on_id, PF_EIT::ACTUAL, version_number, current_next_indicator) { }
   };

   /*!
    * \brief Event Information %Table, Present / Following - Other, as per ETSI EN 300 468.
    */
   struct PF_EITOther : public PF_EIT
   {
      /*!
       * \brief Constructor.
       * \param sid Id to identify the service.
       * \param xs_id Id to identify the transport stream.
       * \param on_id Id to identify the bouquet.
       * \param version_number Version number to use the subtable.
       * \param current_next_indicator `true`: version curently applicable, `false`: next applicable.
       */
      PF_EITOther(ui16 sid, ui16 xs_id, ui16 on_id, ui8 version_number, bool current_next_indicator = true)
         : PF_EIT(sid, xs_id, on_id, PF_EIT::OTHER, version_number, current_next_indicator) { }
   };
   //! @}

#if 0
   // ========================================================================
   //
   // event schedule EIT's

   // NOT YET IMPLEMENTED
   class ES_EIT : public EIT
   {
   private:
      enum { ACTUAL_BASE = 0x50, OTHER_BASE_0x60 };

      std::list<Event> event_list;

//   ES_EIT(ui16 sid, ui16 xsid, ui16 onid, ui8 ver, bool cni = true) :
//       EIT(TID, sid, xsid, onid, ver, cni) { }
   public:

#ifdef ENABLE_DUMP
      void dumpHeader(std::ostream &o) const;
#endif
   };
#endif // if 0

} // sigen namespace

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
// eit.cc: class definition for the EIT
// -----------------------------------

#include <iostream>
#include <algorithm>
#include <utility>
#include <list>
#include "table.h"
#include "descriptor.h"
#include "tstream.h"
#include "eit.h"

namespace sigen
{
   // ---------------------------
   // the abstract EIT base class
   //

   //
   // adds an event to the passed list...
   // protected function to be used by the derived classes
   //
   bool EIT::addEvent(std::list<Event> &e_list, ui16 evid, const UTC& time, const BCDTime& dur, ui8 rs, bool fca)
   {
      // make sure we can fit it
      if ( !incLength( Event::BASE_LEN) )
         return false;

      // add the event to the list
      e_list.emplace_back(evid, time, dur, rs, fca);
      return true;
   }



   //
   // adds a descriptor to event with the specified id..
   // protected function to be used by the derived classes
   //
   bool EIT::addEventDesc(std::list<Event> &e_list, ui16 evid, Descriptor &d)
   {
      // look for the id in the list.. if found, add the descriptor to it
      auto it = std::find_if(e_list.begin(), e_list.end(),
                             [=](const auto& e) { return e.equals(evid); });
      if (it == e_list.end())
         return false;

      return addEventDesc(*it, d);
   }



   //
   // adds the descriptor to the last event added
   // protected function to be used by the derived classes
   //
   bool EIT::addEventDesc(std::list<Event> &e_list, Descriptor &d)
   {
      if (e_list.empty())
         return false;

      return addEventDesc( e_list.back(), d );
   }


   //
   // actually adds the descriptor to an event..
   // protected function to be used by the derived classes
   //
   bool EIT::addEventDesc(Event& event, Descriptor &d)
   {
      ui16 d_len = d.length();

      // make sure we can add it (max desc loop len = 2^16 - 1)
      if ( !incLength(d_len) )
         return false;

      // we don't check if it can fit here since it should have
      // been done before we were called (protected function)
      event.descriptors.add(d, d_len);
      return true;
   }


#ifdef ENABLE_DUMP
   //
   // debug
   void EIT::dump(std::ostream &o) const
   {
      dumpHeader(o);

      identStr(o, XPORT_STREAM_ID_S, xport_stream_id);
      identStr(o, ORIG_NETWORK_ID_S, original_network_id);
      o << std::endl;

      // calls virtual function for event dumps
      dumpEvents(o);
   }


   //
   // dumps the passed event list
   //
   void EIT::dumpEventList(std::ostream &o, const std::list<Event> &e_list) const
   {
      // display the event list
      incOutLevel();

      for (const Event& event : e_list) {
         o << std::hex;
         identStr(o, EVENT_ID_S, event.id);
         identStr(o, START_TIME_S, event.utc);
         identStr(o, DURATION_S, event.duration);
         identStr(o, RUNNING_STATUS_S, event.running_status);
         identStr(o, FREE_CA_MODE_S, event.free_CA_mode);

         identStr(o, DESC_LOOP_LEN_S, event.descriptors.loop_length());
         o << std::endl;

         // display the descriptors
         event.descriptors.dump(o);

         o << std::endl;
      }
      decOutLevel();
   }
#endif


   //
   // writes the data to sections..
   // EIT's need a different writeSection() than other tables because of
   // rules applying to fields like section_number, last_section_number,
   // segment_last_section_number, etc.. because of this, the EIT base class
   // has a dummy writeSection() with arguments that match the virtual
   // one defined by PSITable.. because EIT also defines buildSections(),
   // we call this writeSection() from there and don't have to worry about
   // anybody calling the other one
   //
   bool EIT::writeSection(Section& section,
                          const std::list<Event> &event_list,
                          ui8 last_tid,
                          ui8 cur_sec, ui8 last_sec_num, ui8 segm_last_sec_num,
                          ui16 &sec_bytes) const
   {
      bool done = false, exit = false;

      while (!exit)
      {
         switch (run.op_state)
         {
           case INIT:
              run.ev_iter = event_list.begin();
              run.op_state = WRITE_HEAD;

           case WRITE_HEAD:
              // common data for every section
              // if table's length is > available space, we'll
              // overwrite the section length later on
              writeSectionHeader(section);

              // section count information
              section.set08Bits(cur_sec);

              // we don't know the last section number yet, so
              // for now set it to 0,
              section.set08Bits(last_sec_num);

              section.set16Bits(xport_stream_id);
              section.set16Bits(original_network_id);

              // standard calls for set values for these on PF
              section.set08Bits( segm_last_sec_num ); // segment last section num
              section.set08Bits( last_tid );          // last_table_id

              sec_bytes = BASE_LENGTH; // the minimum section size
              run.op_state = (!run.event ? GET_EVENT : WRITE_EVENT);
              break;

           case GET_EVENT:
              // fetch the next event
              if (run.ev_iter != event_list.end()) {
                 run.event = &(*run.ev_iter++);

                 if (!run.event->descriptors.list().empty()) {
                    const Descriptor *d = run.event->descriptors.front().get();

                    // check if we can fit it with at least one descriptor
                    if (sec_bytes + Event::BASE_LEN + d->length() >
                        getMaxDataLen()) {
                       // we can't, so let's get another section to write
                       // this event to
                       run.op_state = WRITE_HEAD;
                       exit = true;
                       break;
                    }
                 }
                 else {
                    // no descriptors.. can we add the empty event?
                    if ( (sec_bytes + Event::BASE_LEN) > getMaxDataLen() ) {
                       // no soup for you
                       run.op_state = WRITE_HEAD;
                       exit = true;
                       break;
                    }
                 }
                 // we can add it
                 run.op_state = WRITE_EVENT;
              }
              else {
                 // done with all services.. all sections are done!
                 run = Context();
                 exit = done = true;
                 break;
              }
              break;

           case WRITE_EVENT:
              // try to write it
              if (!(*run.event).write_section(section, getMaxDataLen(), sec_bytes)) {
                 run.op_state = WRITE_HEAD;
                 exit = true;
                 break;
              }
              run.op_state = GET_EVENT;
              break;
         }
      }
      return done;
   }


   //
   // data writers for EIT::Event
   //
   ui8 EIT::Event::write_header(Section& section) const
   {
      // write the event data
      section.set16Bits(id);

      // start utc
      section.set16Bits(utc.mjd);
      section.set08Bits(utc.time.getBCDHour());
      section.set08Bits(utc.time.getBCDMinute());
      section.set08Bits(utc.time.getBCDSecond());

      // duration
      section.set08Bits(duration.getBCDHour());
      section.set08Bits(duration.getBCDMinute());
      section.set08Bits(duration.getBCDSecond());

      return EIT::Event::BASE_LEN - 2;
   }

   void EIT::Event::write_desc_loop_len(Section& section, ui8* pos, ui16 len) const
   {
      section.set16Bits( pos,
                         (running_status << 13) |
                         (free_CA_mode << 12) |
                         (len & LEN_MASK) );
   }

   //
   // controls the table building.. eit's need special section builders
   // because of the DVB standard.. PF can't be sectionable and Present
   // sections are sent with a sec_num = 0, while Following sections are
   // sent with a sec_num = 1..
   // this function  simply allocates space, calls for the write, writes
   // the total length of the section (returned by writeSection() and crc's
   // it
   //
   void PF_EIT::buildSections(TStream &strm) const
   {
      ui16 sec_bytes;

      // build the present & following sections
      for (ui8 cur_sec = PRESENT, last_sec = FOLLOWING; cur_sec <= FOLLOWING; cur_sec++) {
         // allocate space for the section (use getMaxSectionLen() to include
         // room for CRC)
         Section *s = strm.getNewSection(getMaxSectionLen());

         // write the section
         writeSection(*s, event_list[cur_sec], getId(), // id is table_id
                      cur_sec, last_sec, last_sec, sec_bytes);

         // adjust the length, and calculate the crc
         s->set16Bits(1, buildLengthData(sec_bytes));
         s->calcCrc();
      }
   }



#ifdef ENABLE_DUMP
   //
   // p/f event dumps
   void PF_EIT::dumpEvents(std::ostream &o) const
   {
      // display the present event list
      incOutLevel();
      headerStr(o, P_EVENT_LIST_S, false);
      dumpEventList(o, event_list[PRESENT]);
      decOutLevel();

      // display the following event list
      incOutLevel();
      headerStr(o, F_EVENT_LIST_S, false);
      dumpEventList(o, event_list[FOLLOWING]);
      decOutLevel();
   }

   //
   // debug
   void PF_EIT::dumpHeader(std::ostream &o) const
   {
      PSITable::dumpHeader( o, ((getId() == ACTUAL) ?
                                EIT_PF_ACTUAL_S : EIT_PF_OTHER_S),
                            SERVICE_ID_S,
                            true );
   }
#endif

} // namespace sigen

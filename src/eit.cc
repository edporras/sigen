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
   bool EIT::addEvent(std::list<std::unique_ptr<Event> > &e_list, ui16 evid,
                      UTC time, BCDTime dur, ui8 rs, bool fca)
   {
      // make sure we can fit it
      if ( !incLength( Event::BASE_LEN) )
         return false;

      // add the event to the list
      e_list.push_back(std::make_unique<Event>(evid, time, dur, rs, fca));
      return true;
   }



   //
   // adds a descriptor to event with the specified id..
   // protected function to be used by the derived classes
   //
   bool EIT::addEventDesc(std::list<std::unique_ptr<Event> > &e_list, ui16 evid,
                          Descriptor &d)
   {
      ui16 d_len = d.length();

      // make sure we can add it (max desc loop len = 2^16 - 1)
      if ( !incLength(d_len) )
         return false;

      // look for the id in the list.. if found, add the descriptor to it
      for (std::unique_ptr<Event>& ep : e_list)
      {
         Event& event = *ep;
         if (event.id == evid)
            return addEventDesc(event, d, d_len);
      }
      return false;
   }



   //
   // adds the descriptor to the last event added
   // protected function to be used by the derived classes
   //
   bool EIT::addEventDesc(std::list<std::unique_ptr<Event> > &e_list, Descriptor &d)
   {
      ui16 d_len = d.length();

      // make sure we can add it
      if ( !incLength(d_len) )
         return false;

      // get the tail of the list, and add it to it
      if (!e_list.empty())
         return addEventDesc( *e_list.back(), d, d_len );
      return false;
   }


   //
   // actually adds the descriptor to an event..
   // protected function to be used by the derived classes
   //
   bool EIT::addEventDesc(Event& event, Descriptor &d, ui16 d_len)
   {
      // we don't check if it can fit here since it should have
      // been done before we were called (protected function)
      event.addDesc(d, d_len);
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
   void EIT::dumpEventList(std::ostream &o, const std::list<std::unique_ptr<Event> > &e_list) const
   {
      // display the event list
      incOutLevel();

      for (const std::unique_ptr<Event>& ep : e_list)
      {
         const Event& event = *ep;

         o << std::hex;
         identStr(o, EVENT_ID_S, event.id);
         identStr(o, START_TIME_S);
         o << event.utc << std::endl;

         identStr(o, DURATION_S);
         o << event.duration << std::endl;
         identStr(o, RUNNING_STATUS_S, event.running_status);
         identStr(o, FREE_CA_MODE_S, event.free_CA_mode);

         identStr(o, DESC_LOOP_LEN_S, event.desc_loop_length());
         o << std::endl;

         // display the descriptors
         dumpDescLoop( event.desc_list(), o );

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
                          const std::list<std::unique_ptr<Event> > &event_list,
                          ui8 last_tid,
                          ui8 cur_sec, ui8 last_sec_num, ui8 segm_last_sec_num,
                          ui16 &sec_bytes) const
   {
      enum State_t { WRITE_HEAD, GET_EVENT, WRITE_EVENT };

      bool done, exit;
      static State_t op_state = WRITE_HEAD;
      static const Event *event = nullptr;
      static std::list<std::unique_ptr<Event> >::const_iterator ev_iter = event_list.begin();

      // init
      exit = done = false;

      // buildSection() is defined in the derived EIT class (PF_EIT, or
      // ES_EIT).. when they call this, they will passed the event list we're
      // building sections on
      if (!event)
         ev_iter = event_list.begin();

      while (!exit)
      {
         switch (op_state)
         {
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

              sec_bytes = BASE_LEN; // the minimum section size
              op_state = (!event ? GET_EVENT : WRITE_EVENT);
              break;

           case GET_EVENT:
              // fetch the next event
              if (ev_iter != event_list.end())
              {
                 event = (*ev_iter++).get();

                 if (!event->desc_list().empty())
                 {
                    const Descriptor *d = event->desc_list().front().get();

                    // check if we can fit it with at least one descriptor
                    if (sec_bytes + Event::BASE_LEN + d->length() >
                        getMaxDataLen())
                    {
                       // we can't, so let's get another section to write
                       // this event to
                       op_state = WRITE_HEAD;
                       exit = true;
                       break;
                    }
                 }
                 else
                 {
                    // no descriptors.. can we add the empty event?
                    if ( (sec_bytes + Event::BASE_LEN) > getMaxDataLen() )
                    {
                       // no soup for you
                       op_state = WRITE_HEAD;
                       exit = true;
                       break;
                    }
                 }
                 // we can add it
                 op_state = WRITE_EVENT;
              }
              else
              {
                 // done with all services.. all sections are done!
                 op_state = WRITE_HEAD;
                 event = nullptr;
                 exit = done = true;
                 break;
              }
              break;

           case WRITE_EVENT:
              // try to write it
              if (!writeEvent(section, *event, sec_bytes))
              {
                 op_state = WRITE_HEAD;
                 exit = true;
                 break;
              }
              op_state = GET_EVENT;
              break;
         }
      }
      return done;
   }


   //
   // state machine for writing each event to the stream
   //
   bool EIT::writeEvent(Section& section, const Event& event,
                        ui16 &sec_bytes) const
   {
      enum State_t { WRITE_HEAD, GET_DESC, WRITE_DESC };

      ui8 *d_loop_len_pos = 0;
      ui16 desc_loop_len = 0;
      bool done, exit;
      static State_t op_state = WRITE_HEAD;
      static const Descriptor *d = nullptr;
      static std::list<std::unique_ptr<Descriptor> >::const_iterator d_iter;

      // set the descriptor list iterator to this event's
      // descriptor list
      if (!d)
         d_iter = event.desc_list().begin();

      done = exit = false;

      while (!exit)
      {
         switch (op_state)
         {
           case WRITE_HEAD:
              // write the event data
              section.set16Bits(event.id);

              // start utc
              section.set16Bits(event.utc.mjd);
              section.set08Bits(event.utc.time.getBCDHour());
              section.set08Bits(event.utc.time.getBCDMinute());
              section.set08Bits(event.utc.time.getBCDSecond());

              // duration
              section.set08Bits(event.duration.getBCDHour());
              section.set08Bits(event.duration.getBCDMinute());
              section.set08Bits(event.duration.getBCDSecond());

              // save the position of the desc loop length..
              // we'll set it when we're done
              d_loop_len_pos = section.getCurDataPosition();
              section.set16Bits( 0 );

              // increment the byte count
              sec_bytes += EIT::Event::BASE_LEN;

              op_state = (!d ? GET_DESC : WRITE_DESC);
              break;

           case GET_DESC:
              if (d_iter != event.desc_list().end())
              {
                 d = (*d_iter++).get();

                 // make sure we can fit it
                 if (sec_bytes + d->length() > getMaxDataLen())
                 {
                    // can't exit and wait to complete
                    op_state = WRITE_HEAD;
                    exit = true;
                    break;
                 }
                 op_state = WRITE_DESC;
              }
              else
              {
                 d = nullptr;
                 op_state = WRITE_HEAD;
                 exit = done = true;
                 break;
              }
              break;

           case WRITE_DESC:
              {
                 // the service descriptors
                 d->buildSections(section);

                 ui16 d_len = d->length();
                 sec_bytes += d_len;
                 desc_loop_len += d_len;

                 // try to get another one
                 op_state = GET_DESC;
              }
              break;
         }
      }

      // done with this service.. write the desc_loop_len
      section.set16Bits( d_loop_len_pos,
                         (event.running_status << 13) |
                         (event.free_CA_mode << 12) |
                         (desc_loop_len & LEN_MASK) );
      return done;
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
      ui8 cur_sec, last_sec;
      ui16 sec_bytes;
      Section *s;

      // build the present & following sections
      last_sec = FOLLOWING;
      for (cur_sec = PRESENT; cur_sec <= FOLLOWING; cur_sec++)
      {
         // allocate space for the section (use getMaxSectionLen() to include
         // room for CRC)
         s = strm.getNewSection(getMaxSectionLen());

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
#endif



   // =============================================
   // the private EIT::Event class

   // utility
   void EIT::Event::buildSections(Section &s) const
   {
      s.set16Bits(id);

      // date / time
      s.set16Bits(utc.mjd);
      s.set08Bits(utc.time.getBCDHour());
      s.set08Bits(utc.time.getBCDMinute());
      s.set08Bits(utc.time.getBCDSecond());

      // duration
      s.set08Bits( duration.getBCDHour() );
      s.set08Bits( duration.getBCDMinute() );
      s.set08Bits( duration.getBCDSecond() );

      s.set16Bits( (running_status << 13) |
                   (free_CA_mode << 12) |
                   desc_loop_length() );

      // descriptor loop
      for (const std::unique_ptr<Descriptor>& dp : desc_list())
         (*dp).buildSections(s);
   }


#ifdef ENABLE_DUMP
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

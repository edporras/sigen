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
// sdt.cc: class definition for the SDT
// -----------------------------------

#include <iostream>
#include <list>
#include <utility>
#include "table.h"
#include "descriptor.h"
#include "tstream.h"
#include "sdt.h"

namespace sigen
{
   //
   // adds a service to the linked list
   //
   bool SDT::addService(ui16 sid, bool esf, bool epff, ui8 rs, bool fca)
   {
      // make sure we can add it
      if ( !incLength( Service::BASE_LEN) )
         return false;

      service_list.push_back(std::make_unique<Service>(sid, esf, epff, rs, fca, reserved));
      return true;
   }


   //
   // adds a descriptor to the specified service
   //
   bool SDT::addServiceDesc(ui16 sid, Descriptor &d)
   {
      ui16 d_len = d.length();

      // make sure we can add it (max desc loop len = 2^16 - 1)
      if ( !incLength(d_len) )
         return false;

      // look for the id in the list
      for (std::unique_ptr<Service>& sp : service_list)
      {
         Service& service = *sp;
         if (service.id == sid)
            return addServiceDesc(service, d, d_len);
      }
      return false;
   }


   //
   // adds a descriptor to the last service that was added
   //
   bool SDT::addServiceDesc(Descriptor &d)
   {
      ui16 d_len = d.length();

      // make sure we can add it
      if ( !incLength(d_len) )
         return false;

      if (!service_list.empty())
         return addServiceDesc(*service_list.back(), d, d_len);
      return false;
   }


   //
   // actually adds the descriptor to the service's list
   //
   bool SDT::addServiceDesc(Service& serv, Descriptor &d, ui16 d_len)
   {
      serv.descriptors.add(d, d_len);
      return true;
   }


   //
   // write to the stream
   //
   bool SDT::writeSection(Section& section, ui8 cur_sec, ui16 &sec_bytes) const
   {
      enum State_t { WRITE_HEAD, GET_SERVICE, WRITE_SERVICE };

      bool done, exit;
      static State_t op_state = WRITE_HEAD;
      static const Service *serv = nullptr;
      static std::list<std::unique_ptr<Service> >::const_iterator s_iter = service_list.begin();

      // init
      done = exit = false;
      if (!serv)
         s_iter = service_list.begin();

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
              section.set08Bits(0);

              section.set16Bits(original_network_id);
              section.set08Bits( rbits( reserved, 0xff) ); // reserved (8)

              sec_bytes = BASE_LEN; // the minimum section size
              op_state = (!serv ? GET_SERVICE : WRITE_SERVICE);
              break;

           case GET_SERVICE:
              // fetch the next service
              if (s_iter != service_list.end())
              {
                 serv = (*s_iter++).get();

                 if (!serv->descriptors.empty())
                 {
                    const Descriptor *d = serv->descriptors.front().get();

                    // check if we can fit it with at least one descriptor
                    if (sec_bytes + Service::BASE_LEN + d->length() >
                        getMaxDataLen())
                    {
                       // we can't, so let's get another section to write
                       // this service to
                       op_state = WRITE_HEAD;
                       exit = true;
                       break;
                    }
                 }
                 else
                 {
                    // no descriptors.. can we add the empty service?
                    if ( (sec_bytes + Service::BASE_LEN) > getMaxDataLen() )
                    {
                       // no soup for you
                       op_state = WRITE_HEAD;
                       exit = true;
                       break;
                    }
                 }
                 // we can add it
                 op_state = WRITE_SERVICE;
              }
              else
              {
                 // done with all services.. all sections are done!
                 op_state = WRITE_HEAD;
                 serv = nullptr;
                 exit = done = true;
                 break;
              }
              break;

           case WRITE_SERVICE:
              // try to write it
              if (!writeService(section, *serv, sec_bytes))
              {
                 op_state = WRITE_HEAD;
                 exit = true;
                 break;
              }
              op_state = GET_SERVICE;
              break;
         }
      }
      return done;
   }


   //
   // state machine for writing each service to the stream
   //
   bool SDT::writeService(Section& section, const Service& service,
                          ui16 &sec_bytes) const
   {
      enum State_t { WRITE_HEAD, GET_DESC, WRITE_DESC };

      ui8 *d_loop_len_pos = 0;
      ui16 desc_loop_len = 0;
      bool done, exit;
      static State_t op_state = WRITE_HEAD;
      static const Descriptor *d = nullptr;
      static std::list<std::unique_ptr<Descriptor> >::const_iterator d_iter = service.descriptors.begin();

      // set the descriptor list iterator to this service's
      // descriptor list
      if (!d)
         d_iter = service.descriptors.begin();

      done = exit = false;

      while (!exit)
      {
         switch (op_state)
         {
           case WRITE_HEAD:
              // write the service data
              section.set16Bits(service.id);
              section.set08Bits( rbits(reserved, 0xfc) |
                                 service.eit_schedule |
                                 service.eit_present_following );

              // save the position of the desc loop length..
              // we'll set it when we're done
              d_loop_len_pos = section.getCurDataPosition();
              section.set16Bits( 0 );

              // increment the byte count
              sec_bytes += SDT::Service::BASE_LEN;

              op_state = (!d ? GET_DESC : WRITE_DESC);
              break;

           case GET_DESC:
              if (d_iter != service.descriptors.end())
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
                         (service.running_status << 13) |
                         (service.free_ca_mode << 12) |
                         (desc_loop_len & LEN_MASK) );
      return done;
   }

#ifdef ENABLE_DUMP
   //
   // dumps to stdout
   //
   void SDT::dump(std::ostream &o) const
   {
      // table header
      dumpHeader( o,
                  ((getId() == SDT::ACTUAL) ? SDT_DUMP_ACTUAL_S : SDT_DUMP_OTHER_S),
                  XPORT_STREAM_ID_S );

      // sdt-specific
      identStr(o, ORIG_NETWORK_ID_S, original_network_id);
      identStr(o, RESERVED_FU_S, rbits(reserved, 0xff));
      o << std::endl;

      // display the service list
      incOutLevel();
      headerStr(o, SERVICE_LIST_S, false);

      for (const std::unique_ptr<Service>& sp : service_list)
      {
         const Service& service =  *sp;

         o << std::hex;
         identStr(o, SERVICE_ID_S, service.id, true);
         identStr(o, RESERVED_FU_S, rbits(reserved, 0x3f)); // reserved future use
         identStr(o, EIT_SCHED_FLAG_S, service.eit_schedule);
         identStr(o, EIT_PF_F_S, service.eit_present_following);
         identStr(o, RUNNING_STATUS_S, service.running_status);
         identStr(o, FREE_CA_MODE_S, service.free_ca_mode);
         identStr(o, DESC_LOOP_LEN_S, service.descriptors.loop_length(), true);
         o << std::endl;

         // display the descriptors
         dumpDescLoop(service.descriptors.list(), o);

         o << std::endl;
      }
      decOutLevel();

      o << std::endl;
   }
#endif



   // =============================================
   // the private SDT::Service class

   //
   // write to the stream
   void SDT::Service::buildSections(Section &s) const
   {
      s.set16Bits( id );
      s.set08Bits( rbits(reserved, 0xfc) | (eit_schedule << 1) |
                   eit_present_following );
      s.set16Bits( (running_status << 13) | (free_ca_mode << 12) |
                   (descriptors.loop_length() & 0x0fff) );

      // TODO: refactor!!!
      for (const std::unique_ptr<Descriptor>& dp : descriptors.list())
         (*dp).buildSections(s);
   }

} // namespace sigen
